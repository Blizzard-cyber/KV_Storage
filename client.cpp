#include <iostream>
#include <string>
#include <thread>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h> 
#include <cstring>
#include "global.h"

using namespace std;


string generateRandomString(size_t length) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    string result;
    result.resize(length);
    for (size_t i = 0; i < length; ++i) {
        result[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    return result;
}

void sendRequest(int clientSocket, const string& request) {
    write(clientSocket, request.c_str(), request.size());

    char response[1024];
    int bytesRead = read(clientSocket, response, sizeof(response));
    response[bytesRead] = '\0';

    //接受到暂停信号
    if (!strcmp(response, "PAUSE")) {
        cout << "Received PAUSE signal from server. Pausing production..." << endl;
        this_thread::sleep_for(chrono::milliseconds(5000)); // 暂停5秒
    } else if(strcmp(response, "OK")) {
        cout << "Received response: " << response << endl;
        
    }
}

void connectAndSendRequests(const string& serverIp, int port) {
    // 创建客户端 socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        cerr << "Error creating socket" << endl;
        return;
    }

    // 连接到服务器
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, serverIp.c_str(), &serverAddr.sin_addr);
    serverAddr.sin_port = htons(port);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Error connecting to server" << endl;
        close(clientSocket);
        return;
    }

    cout << "Connected to server:"<< serverIp << endl;

    int production = 0;
    while (true) {
        // 生成随机请求
        int requestType = rand() % 3;
        int key = rand() % 100000; // 随机生成一个0到99999之间的键
        size_t valueLength = rand() % 100 + 1; // 随机生成1到100之间的长度
        string value = generateRandomString(valueLength); // 生成随机长度的字符串

        string request;
       
        switch (requestType) {
            case 0: // PUT 请求
                request = "PUT " + to_string(key) + " " + value;
                break;
            case 1: // GET 请求
                request = "GET " + to_string(key);
                break;
            case 2: // DEL 请求
                request = "DEL " + to_string(key);
                break;
        }

        production++;
        if(production % 10000 == 0) {
            cout << "production number:" << production << endl;
        }

        sendRequest(clientSocket, request);
        

        
    }

    close(clientSocket);
    cout << "Connection closed" << endl;
}

int main() {

    connectAndSendRequests(SERVER_IP, SERVER_PORT);

    return 0;
}
