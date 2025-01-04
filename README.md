# KV存储引擎

 [![License](https://img.shields.io/github/license/Razirp/ThreadPool)](https://github.com/Razirp/ThreadPool/blob/main/LICENSE) [![Language: C++20](https://img.shields.io/badge/Language-C%2B%2B20-blue)](https://cppreference.com/)  

众所周知，非关系型数据库`redis`，以及`levedb`，`rockdb`其核心存储引擎的数据结构就是跳表。

本项目就是基于跳表实现的轻量级键值型存储引擎，使用`C++`实现。插入数据、删除数据、查询数据、数据落盘、文件加载数据。

# 项目中文件

```shell
.
├── bin         //生成可执行文件
├── build        //cmake build
│  
├── CMakeLists.txt  
├── command.sh        //测试脚本
├── include          //头文件
│   ├── cache.hpp      //读写缓冲类
│   ├── fileInfo.hpp   //文件管理信息类
│   ├── global.h       //全局配置信息
│   ├── skiplist.hpp   //跳表结构类
│   └── thread_pool.hpp  //线程池
├── lib               //库文件
├── LICENSE           
├── server.cpp           //服务端入口（处理请求）
├── client.cpp           //客户端入口（生产请求）
├── proto              //protobuf定义
│   ├── info.pb.cc
│   ├── info.pb.h
│   ├── info.proto      //文件信息
│   ├── sl.pb.cc
│   ├── sl.pb.h
│   └── sl.proto        //跳表
├── README.md
├── src               //源文件
│   ├── thread_pool.cpp       //线程池
│   └── worker_thread.cpp     //工作线程
└── test.cpp            //测试文件
```

# 食用指南

### 从源代码编译

1. 克隆仓库（取得源代码）：

   ```shell
   git clone https://github.com/Blizzard-cyber/KV_Storage.git
   ```

   > 亦可通过你所熟悉的任何可行的方式（如ssh/GitHub CLI进行克隆、直接下载zip压缩包等）获得源代码。

2. 进入项目目录：

   ```shell
   cd KV_Storage
   ```

   > 目录名取决于你的命名。

3. 创建并进入构建目录：

   ```shell
   mkdir build
   cd build
   ```

   > 推荐，但非必需。创建专门的构建目录的目的是为了更方便地管理构建/编译过程中生成的文件。

4. 运行`cmake`生成构建文件：

   ```shell
   cmake ..
   ```

   > `cmake` 后的路径为 CMakeLists.txt 所在目录。

5. 编译库文件：

   ```
   cmake --build .
   ```

   > 或使用 GNU make 在 Makefile 所在目录下直接使用 `make` 命令进行编译：
   >
   > **note: 项目使用到C++ 20标准库特性**
   >
   > ```shell
   > make
   > ```

6. 运行程序:

   ```shell
   ./server
   ./client
   ```
   
   

# TODO

- [x] **单线程版本**(`singalThread`分支）

> - [x] `GET PUT DEL` 请求的生产及处理
> - [x] 文件信息类的存储
> - [x] `key` `value` 跳表结构存储
> - [x] 文件信息类以及跳表的序列化和反序列化
> - [ ] .......

- [x] **多线程版本** （`mutiThread`分支）**【待完善】**

> - [x] 线程池设计
> - [x] **多线程访问cache的锁的设计（锁的粒度）**
> - [x] **`接口层 ——> 内存 ——> 磁盘` 分层线程工作流**
> - [x] **请求生产者速度与线程处理请求的速度匹配问题**
> - [ ] **当前CPU占用率低的问题**
> - [ ] **其他提高并发读的措施**
> - [ ] **压力测试**

- [x] **网络通信版本**（`socket`分支）

> - [x] 将原程序入口`main`分割为`server`和`client`两部分
> - [x] `server`继承了原版本的主要功能，主要用户接收并将请求提交给线程池处理
> - [x] `client`主要作为请求的生产者并将请求发送到`server`
> - [x] **使用`select`编写非阻塞式`accept`**
> - [x] 客户端或服务端单方面断开情况处理
> - [x] **流量控制**（服务器线程池满，通知客户端暂停生产）
> - [ ] 网络中的出错处理以及多客户机连接并发写

- [ ] **最新版本**（`main`分支）
   > **本项目在不断更新中。。。。**

参考
---

- 线程池参考：[ThreadPool](https://github.com/Razirp/ThreadPool)

许可证
---

本项目采用MIT许可协议 – 详情请参阅 [LICENSE](LICENSE) 文件。