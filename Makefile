# 编译器
CXX = g++

# 编译选项
CXXFLAGS = -std=c++11 -g -rdynamic

# 链接选项
LDFLAGS = -lprotobuf

# 目标文件
TARGET = main

# 源文件
SRCS = main.cpp proto/info.pb.cc



# 链接目标文件生成可执行文件
$(TARGET):
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)


# 测试
test:
	$(CXX)  -o test test.cpp 
# 清理生成的文件
clean:
	rm -f $(TARGET)  test