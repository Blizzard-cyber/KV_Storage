#include <stdlib.h>

#ifndef GLOBAL_H
#define GLOBAL_H

// 服务器IP
const std::string SERVER_IP =  "127.0.0.1";   // 本地回环地址
// 服务器端口
const int SERVER_PORT = 12345;
// 服务器监听队列长度
const int SERVER_LISTEN_QUEUE = 3;

// 文件名
const std::string FILE_PATH = "dumpFile";
// 文件大小（以字节为单位）
const size_t FILE_SIZE = 1 *1024 * 1024 * 1024 ; // 1GB

// 块大小（以字节为单位）
const size_t BLOCK_SIZE = 64 * 1024 * 1024; // 64MB

// 缓存大小（一块的大小）
const size_t CACHE_SIZE = 64 * 1024 * 1024 ; // 64MB

// 块数
const size_t NUM_BLOCKS = FILE_SIZE / BLOCK_SIZE;

#endif // GLOBAL_H