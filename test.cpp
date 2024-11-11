#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "./include/skiplist.hpp"

using namespace std;



int main() {
    
    Skiplist sl;
    //生成大量随机数据
    for(int i = 0; i < 100; i++) {
        sl.add(rand(), to_string(rand()));
    }
    sl.slprint();
    
    //清空缓冲区
    sl.~Skiplist();

    return 0;
}