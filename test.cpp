#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "./include/skiplist.hpp"



using namespace std;



int main() {
    
    Skiplist sl;

    for(int i = 0; i < 10; i++){
        sl.add(rand(), "test");
    }
    sl.slprint();

    sl.serialize("testFile", 1024);
    sl.deserialize("testFile", 1024);

    sl.slprint();

    


    return 0;
}