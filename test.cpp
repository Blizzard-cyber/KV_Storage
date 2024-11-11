#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "./include/skiplist.hpp"
#include "./include/fileInfo.hpp"

using namespace std;



int main() {
    
    FileInfo fileInfo("dumpFile");
    //fileInfo.printInfo();

    //读取磁盘
    fileInfo.readFromBlock0();


    return 0;
}