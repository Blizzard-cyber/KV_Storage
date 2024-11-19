#include <stdlib.h>
#ifndef GLOBAL_H
#define GLOBAL_H

// 文件大小（以字节为单位）
const size_t FILE_SIZE = 1 * 1024 * 1024 ; // 1GB

// 块大小（以字节为单位）
const size_t BLOCK_SIZE = 64 * 1024 ; // 64MB

// 缓存大小（一块的大小）
const size_t CACHE_SIZE = 64 * 1024 ; // 64MB

// 块数
const size_t NUM_BLOCKS = FILE_SIZE / BLOCK_SIZE;

#endif // GLOBAL_H