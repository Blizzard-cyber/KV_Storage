#include <iostream>
#include <unordered_map>
#include <string>
#include <csignal>
#include <sstream>
#include <execinfo.h>
#include <unistd.h>
#include <thread>

#include "./include/fileInfo.hpp"
#include "./include/cache.hpp"
#include "./include/tasks.hpp"

//#include "./proto/info.pb.h"

using namespace std;

#define FILE_PATH "dumpFile"

FileInfo* globalFileInfo = nullptr; // 全局指针，用于在信号处理程序中访问 FileInfo 对象

bool is_integer(const string& s) {
    return !s.empty() && s.find_first_not_of("0123456789") == string::npos;
}

//信号处理函数
void signalHandler(int signum) {
    
    if (globalFileInfo) {
        globalFileInfo->writeToBlock0();
       cout << "Data written to block 0." <<endl;
    }
    cout << endl;
    cout << "Interrupt signal (" << signum << ") received." << endl;
    cout << "Program terminated..." << endl;
    exit(signum);
}

void processPutRequests(TaskQueue& putQueue, FileInfo& fileInfo) {
    while (true) {
        Task task = putQueue.getTask();
        fileInfo.addKeyBlockMapping(std::stoi(task.key),stoull(task.value));
       cout << "Processed PUT request: key = " << task.key << ", value = " << task.value <<endl;
    }
}

void processGetRequests(TaskQueue& getQueue, FileInfo& fileInfo) {
    while (true) {
        Task task = getQueue.getTask();
        size_t blockNumber = fileInfo.getBlockNumber(std::stoi(task.key));
       cout << "Processed GET request: key = " << task.key << ", block number = " << blockNumber <<endl;
    }
}

void processDelRequests(TaskQueue& delQueue, FileInfo& fileInfo) {
    while (true) {
        Task task = delQueue.getTask();
        // Implement the deletion logic if needed
       cout << "Processed DEL request: key = " << task.key <<endl;
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

   
    //创建一个文件
    // ofstream file(FILE_PATH, ios::out | ios::binary);
    // file.close();

    //判断一个文件是否存在，不存在则创建
    if(access(FILE_PATH, F_OK) == -1) {
        ofstream file(FILE_PATH, ios::out | ios::binary);
        file.close();
    }

    //定义文件信息块
    FileInfo fileInfo(FILE_PATH);
    globalFileInfo = &fileInfo;
    //从文件中读取文件信息块（第0块）
    fileInfo.readFromBlock0();


    //定义写缓冲区
    Cache writeCache(FILE_PATH);

    //定义读缓冲区
    Cache readCache(FILE_PATH);
    

    //定义任务队列
    // TaskQueue putQueue;
    // TaskQueue getQueue;
    // TaskQueue delQueue;

    // 启动处理请求的线程
//    thread putThread(processPutRequests,ref(putQueue),ref(fileInfo));
//    thread getThread(processGetRequests,ref(getQueue),ref(fileInfo));
//    thread delThread(processDelRequests,ref(delQueue),ref(fileInfo));


    cout << "Program starting..." << endl;

    //捕获中端信号
    signal(SIGINT, signalHandler);

    //捕获段错误
    signal(SIGSEGV, SigSegvProc);
    cout << "Enter command (PUT {key} {value}/GET {key}/DEL {key}): " << endl; 

    while (1) {
        
        getline(cin, line);
        cout <<"command:"<<line<<endl;
        istringstream iss(line);
        iss >> command;

        if (command == "PUT") {
            string key_str;
            iss >> key_str >> value;
            if (is_integer(key_str) && !value.empty()) {
                key = stoi(key_str);
                writeCache.put(fileInfo, key, value);
                
            } else {
                cout << "Invalid PUT command format" << endl;
            }
        } else if (command == "GET") {
            string key_str;
            iss >> key_str;
            if (is_integer(key_str)) {
                key = stoi(key_str);
                if(writeCache.get(fileInfo, key, 0)==""){  //如果写缓冲区没有找到
                    if(readCache.get(fileInfo, key, 1)=="" ){  //如果读缓冲区也没有找到
                        cout<<"[Main] Key: "<< key << "   not found!"<<endl;
                    }
                }
            } else {
                cout << "Invalid GET command format" << endl;
            }
        } else if (command == "DEL") {
            string key_str;
            iss >> key_str;
            if (is_integer(key_str)) {
                key = stoi(key_str);
                if(!writeCache.del(fileInfo, key, 0)){
                    if(!readCache.del(fileInfo, key, 1)){
                       cout<<"[Main] Key: "<< key << "  not found!"<<endl;
                    }

                }
            } else {
                cout << "Invalid DEL command format" << endl;
            }
        } else {
            cout << "Unknown command" << endl;
        }
    }

    //cout << "Program terminated..." << endl;
    return 0;
}