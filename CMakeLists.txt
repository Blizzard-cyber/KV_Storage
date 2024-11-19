cmake_minimum_required(VERSION 3.20)
project(KVstorage)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 设置 C 编译器的优化等级
set(CMAKE_C_FLAGS_RELEASE "-O2") # 为 Release 类型设置 -O2 优化
set(CMAKE_C_FLAGS_DEBUG "-g")    # 为 Debug 类型设置 -g（生成调试信息），不设置优化

# 设置 C++ 编译器的优化等级
set(CMAKE_CXX_FLAGS_RELEASE "-O2") # 为 Release 类型设置 -O2 优化
set(CMAKE_CXX_FLAGS_DEBUG "-g")    # 为 Debug 类型设置 -g（生成调试信息），不设置优化

# Set the include directories
include_directories(include proto)

# Add the source files
set(SOURCES 
    src/thread_pool.cpp 
    src/worker_thread.cpp 
    proto/info.pb.cc 
    proto/sl.pb.cc 
    main.cpp)


# 生成可执行文件
add_executable(main ${SOURCES})

# 链接所需的库
find_package(Protobuf REQUIRED)
target_link_libraries(main PRIVATE ${Protobuf_LIBRARIES} pthread)

# 设置输出目录
set_target_properties(main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)


# 打印编译器信息
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")

