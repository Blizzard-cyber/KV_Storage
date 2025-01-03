#include <iostream>
#include <string>
#include <csignal>
#include <execinfo.h>
#include <unistd.h>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sstream>

#include "fileInfo.hpp"
#include "cache.hpp"
#include "thread_pool.hpp"
#include "IOpool.hpp"
#include "global.h"

using namespace std;


FileInfo* globalFileInfo = nullptr; // 全局指针，用于在信号处理程序中访问 FileInfo 对象
thread_utils::thread_pool* globalThreadPool = nullptr; // 全局指针，用于在信号处理程序中访问线程池对象
Cache* globalWRCache = nullptr; // 全局指针，用于在信号处理程序中访问 写Cache 对象
Cache* globalRDCache = nullptr; // 全局指针，用于在信号处理程序中访问 读Cache 对象

condition_variable cv;  
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
    if (globalWRCache) {
        
        globalWRCache->flushToDisk(*globalFileInfo, FILE_PATH, CACHE_SIZE - globalWRCache->currentSize ,globalFileInfo -> getFreeBlock());
        cout << "WriteCache flush to disk." <<endl;
    }
    //将文件信息块写入到文件
    if (globalFileInfo) {
        globalFileInfo->writeToBlock0();
       cout << "FileInfo written to block 0." <<endl;
    }

    // 通知主线程清理工作完成   
    lock_guard<mutex> lk(cv_m);
    ready = true;
    cv.notify_all();
   
}

void processPutRequests(int key, string value) {
    globalWRCache->put(*globalFileInfo, key, value);
    //  writeCache.put(fileInfo, key, value);
}

void processGetRequests(int key) {
    if(globalWRCache->get(*globalFileInfo, key, 0)==""){  //如果写缓冲区没有找到
         globalRDCache->get(*globalFileInfo, key, 1);
    }

    // if(writeCache.get(fileInfo, key, 0)==""){  //如果写缓冲区没有找到
    //     readCache.get(fileInfo, key, 1);
    // }
       
}

void processDelRequests(int key) {
    if(!globalWRCache->del(*globalFileInfo, key, 0)){
        globalRDCache->del(*globalFileInfo, key, 1);
    }
    // if(!writeCache.del(fileInfo, key, 0)){
    //     readCache.del(fileInfo, key, 1);
    //  }
}


void handleClient(int clientSocket) {
    char buffer[1024];
    int getCount = 0;
    int putCount = 0;
    int delCount = 0;
    while (producing) {
        try {
            int bytesRead = read(clientSocket, buffer, sizeof(buffer) - 1);
            if (bytesRead <= 0) {
                cerr << "Error reading from socket or Client disconnected" << endl;
                break;
            }
            buffer[bytesRead] = '\0';
            //cout << "Received request: " << buffer << endl;

            // 解析请求
            istringstream iss(buffer);
            string result = "OK";
            string command;
            iss >> command;

            if (command == "PUT") {
                int key;
                string value;
                iss >> key >> value;
                globalThreadPool->submit(processPutRequests, key, value);  // 提交PUT任务
                putCount++;
            } else if (command == "GET") {
                int key;
                iss >> key;
                globalThreadPool->submit(processGetRequests, key);  // 提交GET任务
                getCount++;
            } else if (command == "DEL") {
                int key;
                iss >> key;
                globalThreadPool->submit(processDelRequests, key);  // 提交DEL任务
                delCount++;
            } else {
                result = "Invalid command!";
            }

            if((getCount + putCount + delCount) % 10000 == 0) {
                cout << "submit task: " << getCount << " GET, " << putCount << " PUT, " << delCount << " DEL" << endl;
                result = "submit task: " + to_string(getCount) + " GET, " + to_string(putCount) + " PUT, " + to_string(delCount) + " DEL";
            }
            write(clientSocket, result.c_str(), result.size());
        } catch (const runtime_error& e) {
            // 线程池满，向客户端发送暂停生产信号
            cerr << "Caught runtime_error: " << e.what() << ". Sending pause signal to client..." << endl;
            string pauseSignal = "PAUSE";
            write(clientSocket, pauseSignal.c_str(), pauseSignal.size());
            //this_thread::sleep_for(chrono::milliseconds(100)); // 等待100毫秒
        }
    }
    // 服务端已经关闭,先停止读，后停止写，最后关闭socket
    // 关闭后客户端再读写会触发SIGPIPE信号，默认终止程序
    shutdown(clientSocket, SHUT_RD);
    shutdown(clientSocket, SHUT_WR);
    close(clientSocket);
}

void startServer() {
    // 服务器 socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        cerr << "Error creating socket" << endl;
        return;
    }

    // 设置服务器套接字为非阻塞模式
    int flags = fcntl(serverSocket, F_GETFL, 0);
    fcntl(serverSocket, F_SETFL, flags | O_NONBLOCK);

    // 绑定到指定端口
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(SERVER_PORT);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Error binding socket" << endl;
        close(serverSocket);
        return;
    }

    // 监听连接，队列长度为3
    if (listen(serverSocket, SERVER_LISTEN_QUEUE) < 0) {
        cerr << "Error listening on socket" << endl;
        close(serverSocket);
        return;
    }

    cout << "Server listening on port: " << SERVER_PORT << endl;

    while (producing) {
        // 初始化文件描述符集
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(serverSocket, &readfds);

        // 设置超时时间
        timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int activity = select(serverSocket + 1, &readfds, nullptr, nullptr, &timeout);

        if (activity < 0 && errno != EINTR) {
            cerr << "Error in select" << endl;
            break;
        }

        // 处理新连接
        if (activity > 0 && FD_ISSET(serverSocket, &readfds)) {
            int clientSocket = accept(serverSocket, nullptr, nullptr);
            if (clientSocket < 0) {
                if (errno != EWOULDBLOCK) {
                    cerr << "Error accepting connection" << endl;
                }
                continue;
            }
            thread(handleClient, clientSocket).detach();
        }
    }

    cout << "Server shutting down..." << endl;
    close(serverSocket);
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



    //创建一个线程池，初始包8个线程，任务队列最大容量为100000
    thread_utils::thread_pool pool(8,100000);
    globalThreadPool = &pool; // 将线程池对象的地址赋值给全局指针


    // 创建一个 I/O 线程池，初始包含15个线程
    IOThreadPool ioPool(15);

    //定义写缓冲区
    Cache writeCache(FILE_PATH,ioPool);
    globalWRCache = &writeCache;

    //定义读缓冲区
    Cache readCache(FILE_PATH,ioPool);
    globalRDCache = &readCache;

    //主线程 接收请求并向线程池提交任务
    cout << "Program starting..." << endl;

    //捕获中端信号
    signal(SIGINT, signalHandler);

    //捕获段错误
    signal(SIGSEGV, SigSegvProc);

    //启动服务器接受请求并提交处理
    startServer();

    
    // 等待 signalHandler 完成清理工作
    unique_lock<mutex> lk(cv_m);
    cv.wait(lk, []{ return ready; });

    cout << "Program terminated..." << endl;
    return 0;
}