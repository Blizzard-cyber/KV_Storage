#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "./include/skiplist.hpp"
#include "./include/fileInfo.hpp"


using namespace std;



int main() {
    // Skiplist sl;
    // for(int i = 0; i < 10; i++) {
    //     sl.add(i, to_string(i));
    // }

    // sl.serialize("testFile", 1024*16);
    // sl.slprint();

    // sl.deserialize("testFile", 1024*16);
    // sl.slprint();



     FileInfo info("testFile");
    // info.addKeyBlockMapping(1, 1);
    // info.addKeyBlockMapping(2, 2);
    // info.addKeyBlockMapping(3, 3);

    // info.writeToBlock0();

    info.readFromBlock0();


    return 0;
}