#include <iostream>
#include <string>
#include <csignal>
#include <execinfo.h>
#include <unistd.h>
#include <thread>
#include <condition_variable>
#include <mutex>

#include "fileInfo.hpp"
#include "cache.hpp"
#include "thread_pool.hpp"
#include "IOpool.hpp"
#include "global.h"

using namespace std;


FileInfo* globalFileInfo = nullptr; // 全局指针，用于在信号处理程序中访问 FileInfo 对象
thread_utils::thread_pool* globalThreadPool = nullptr; // 全局指针，用于在信号处理程序中访问线程池对象
Cache* globalCache = nullptr; // 全局指针，用于在信号处理程序中访问 Cache 对象

condition_variable cv;  //
mutex cv_m;
bool ready = false;  //是否程序准备好结束（信号处理函数是否执行完毕）
bool producing = true; //是否满足生产条件

//信号处理函数
void signalHandler(int signum) {
    //生产停止
    producing = false;

    cout << "Interrupt signal (" << signum << ") received." << endl;
    //等待线程执行完毕
    if (globalThreadPool) {
        globalThreadPool->shutdown();
        cerr << "Thread pool shutdown." << endl;
    }

    //程序结束时将缓冲区数据写入到文件
    if (globalCache) {
        
        globalCache->flushToDisk(*globalFileInfo, FILE_PATH, CACHE_SIZE - globalCache->currentSize ,globalFileInfo -> getFreeBlock());
        cout << "WriteCache flush to disk." <<endl;
    }
    //将文件信息块写入到文件
    if (globalFileInfo) {
        globalFileInfo->writeToBlock0();
       cout << "FileInfo written to block 0." <<endl;
    }
    // 通知主线程清理工作完成
    {
        lock_guard<mutex> lk(cv_m);
        ready = true;
    }
    cv.notify_all();
}

void processPutRequests( FileInfo& fileInfo, Cache& writeCache, int key, string value) {
     writeCache.put(fileInfo, key, value);
}

void processGetRequests(FileInfo& fileInfo,Cache& writeCache,Cache& readCache ,int key) {
    if(writeCache.get(fileInfo, key, 0)==""){  //如果写缓冲区没有找到
        readCache.get(fileInfo, key, 1);
    }
       
}

void processDelRequests(FileInfo& fileInfo, Cache& writeCache, Cache& readCache ,int key) {
    if(!writeCache.del(fileInfo, key, 0)){
        readCache.del(fileInfo, key, 1);
     }
}

// 生成随机长度的字符串
string generateRandomString(size_t length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    string result;
    result.resize(length);

    for (size_t i = 0; i < length; ++i) {
        result[i] = charset[rand() % (sizeof(charset) - 1)];
    }

    return result;
}

// 请求生产者
void requestProducer(thread_utils::thread_pool& pool, FileInfo& fileInfo, Cache& writeCache, Cache& readCache) {
    int count=0;
    while (producing) {
        try{
        // 随机生成请求类型
        int requestType = rand() % 3;
        int key = rand() % 100000; // 随机生成一个0到99999之间的键
        size_t valueLength = rand() % 100 + 1; // 随机生成1到100之间的长度
        string value = generateRandomString(valueLength); // 生成随机长度的字符串

        switch (requestType) {
            case 0: // PUT 请求
                //cout<< "PUT " << key << " " << value << endl;
                pool.submit(processPutRequests, ref(fileInfo), ref(writeCache), key, value);//提交任务
                break;
            case 1: // GET 请求
                //cout << "GET " << key << endl;
                pool.submit(processGetRequests, ref(fileInfo), ref(writeCache), ref(readCache), key);  //提交任务
                break;
            case 2: // DEL 请求
                //cout << "DEL " << key << endl;
                pool.submit(processDelRequests, ref(fileInfo), ref(writeCache), ref(readCache), key);  //提交任务
                break;
            }
        //this_thread::sleep_for(chrono::milliseconds(10)); // 模拟请求间隔
            count++;
            //每一万个请求输出一次
            if(count%10000==0){
                cout<<"[Main] "<<count<<" requests have been produced :"<< count <<endl;
            }
        }
        catch (const runtime_error& e) {
            // 捕获到 runtime_error 异常，说明线程池满，等待一段时间再生产请求
            cerr << "Caught runtime_error: " << e.what() << ". Waiting before retrying..." << endl;
            this_thread::sleep_for(chrono::milliseconds(100)); // 等待100毫秒
        }
    }

}



//段错误调用栈打印
void ShowStack()
{
    int i;
    void *buffer[1024];
    int n = backtrace(buffer, 1024);
    char **symbols = backtrace_symbols(buffer, n);
    for (i = 0; i < n; i++) {
        printf("%s\n", symbols[i]);
    }
}

//段错误处理函数
void SigSegvProc(int signo) {
    if (signo == SIGSEGV) {
        printf("Receive SIGSEGV signal\n");
        printf("-----call stack-----\n");
        ShowStack();
        exit(-1);
    } else {
        printf("this is sig %d", signo);
    }
}




int main() {
    string command, line;
    int key;
    string value;

   
    //判断一个文件是否存在，不存在则创建
    if(access(FILE_PATH.c_str(), F_OK) == -1) {
        ofstream file(FILE_PATH.c_str(), ios::out | ios::binary);
        file.close();
    }

    //定义文件信息块
    FileInfo fileInfo(FILE_PATH);
    globalFileInfo = &fileInfo;
    //从文件中读取文件信息块（第0块）
    fileInfo.readFromBlock0();


   
    


    //创建一个线程池，初始包含10个线程，任务队列最大容量为100000
    thread_utils::thread_pool pool(10,100000);
    globalThreadPool = &pool; // 将线程池对象的地址赋值给全局指针


    // 创建一个 I/O 线程池，初始包含15个线程
    IOThreadPool ioPool(15);

    //定义写缓冲区
    Cache writeCache(FILE_PATH,ioPool);
    globalCache = &writeCache;

    //定义读缓冲区
    Cache readCache(FILE_PATH,ioPool);

    //主线程 接收请求并向线程池提交任务
    cout << "Program starting..." << endl;

    //捕获中端信号
    signal(SIGINT, signalHandler);

    //捕获段错误
    signal(SIGSEGV, SigSegvProc);

    //cout << "Enter command (PUT {key} {value}/GET {key}/DEL {key}): " << endl; 

    //请求生产者
    requestProducer(pool, fileInfo, writeCache, readCache);

    //用户输入场景
    // while (1) {   
    //     getline(cin, line);
    //     cout <<"command:"<<line<<endl;
    //     istringstream iss(line);
    //     iss >> command;
    //     if (command == "PUT") {
    //         string key_str;
    //         iss >> key_str >> value;
    //         if (is_integer(key_str) && !value.empty()) {
    //             key = stoi(key_str);
    //             //writeCache.put(fileInfo, key, value);
    //             //putQueue.addTask({TaskType::PUT, key, value});
    //             auto furture = pool.submit(processPutRequests,ref(fileInfo),ref(writeCache),key,value);               
    //         } else {
    //             cout << "Invalid PUT command format" << endl;
    //         }
    //     } else if (command == "GET") {
    //         string key_str;
    //         iss >> key_str;
    //         if (is_integer(key_str)) {
    //             key = stoi(key_str);
    //             // if(writeCache.get(fileInfo, key, 0)==""){  //如果写缓冲区没有找到
    //             //     if(readCache.get(fileInfo, key, 1)=="" ){  //如果读缓冲区也没有找到
    //             //         cout<<"[Main] Key: "<< key << "   not found!"<<endl;
    //             //     }
    //             // }
    //             //getQueue.addTask({TaskType::GET, key, ""});
    //             auto furture = pool.submit(processGetRequests,ref(fileInfo),ref(writeCache),ref(readCache),key);
    //         } else {
    //             cout << "Invalid GET command format" << endl;
    //         }
    //     } else if (command == "DEL") {
    //         string key_str;
    //         iss >> key_str;
    //         if (is_integer(key_str)) {
    //             key = stoi(key_str);
    //             // if(!writeCache.del(fileInfo, key, 0)){
    //             //     if(!readCache.del(fileInfo, key, 1)){
    //             //        cout<<"[Main] Key: "<< key << "  not found!"<<endl;
    //             //     }
    //             // }
    //             //delQueue.addTask({TaskType::DEL, key, ""});
    //             auto furture = pool.submit(processDelRequests,ref(fileInfo),ref(writeCache),ref(readCache),key);
    //         } else {
    //             cout << "Invalid DEL command format" << endl;
    //         }
    //     } else {
    //         cout << "Unknown command" << endl;
    //     }
    // }

    
    // 等待 signalHandler 完成清理工作
    {
        unique_lock<mutex> lk(cv_m);
        cv.wait(lk, []{ return ready; });
    }

    cout << "Program terminated..." << endl;
    return 0;
}