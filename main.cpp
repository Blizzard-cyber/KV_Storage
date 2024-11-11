#include <iostream>
#include <unordered_map>
#include <string>
#include <csignal>
#include <sstream>
#include <execinfo.h>
#include <unistd.h>

#include "./include/fileInfo.hpp"
#include "./include/cache.hpp"

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
        std::cout << "Data written to block 0." << std::endl;
    }
    cout << endl;
    cout << "Interrupt signal (" << signum << ") received." << endl;
    cout << "Program terminated..." << endl;
    exit(signum);
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
    ofstream file(FILE_PATH, ios::out | ios::binary);
    file.close();

    //KV

    //定义文件信息块
    FileInfo fileInfo(FILE_PATH);
    globalFileInfo = &fileInfo;

    //定义写缓冲区
    Cache writeCache(FILE_PATH);

    // //定义读缓冲区
    Cache readCache(FILE_PATH);
    
    fileInfo.printInfo();

    //从文件中读取文件信息块（第0块）
    fileInfo.readFromBlock0();
    // if(!fileInfo.readFromBlock0()) {
    //     return 1;
    // }



    cout << "Program starting..." << endl;

    //捕获中端信号
    signal(SIGINT, signalHandler);

    //捕获段错误
    signal(SIGSEGV, SigSegvProc);
    cout << "Enter command (PUT {key} {value}/GET {key}/DEL {key}): " << endl; 

    while (1) {
        
        getline(cin, line);
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
                        cout<<"[Main] Key not found!"<<endl;
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
                        cout<<"[Main] Key not found!"<<endl;
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