#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <ctime>
#include <fstream> 

#include "../proto/info.pb.h"

//#include <mutex>
using namespace std;

// const size_t BLOCK_SIZE = 64 * 1024 * 1024; // 64MB
// const size_t FILE_SIZE = 1 * 1024 * 1024 * 1024; // 1GB
// const size_t NUM_BLOCKS = FILE_SIZE / BLOCK_SIZE;

//test
const size_t BLOCK_SIZE = 4 * 1024 ; // 4kB
const size_t FILE_SIZE = 64 * 1024; // 64kB
const size_t NUM_BLOCKS = FILE_SIZE / BLOCK_SIZE;


struct SuperBlock {
    string fileName;  //文件名
    size_t fileSize; //文件大小
    size_t blockSize; //块大小
    int blockNum; //块数
    string createTime; //创建时间
};

struct BlockIndexTable {
    size_t blockNumber;  //块号
    size_t offset;       //偏移量
    size_t freeSpace;       //剩余空间
};

class FileInfo {
public:
    FileInfo(string fileName) {
        // Initialize superblock
        superBlock.fileName = fileName;
        superBlock.fileSize = FILE_SIZE;
        superBlock.blockSize = BLOCK_SIZE;
        superBlock.blockNum = NUM_BLOCKS;
        superBlock.createTime = getCurrentTime();


        blockUsageTable.resize(NUM_BLOCKS, false);
        blockUsageTable[0] = true; // First block is used

        blockIndexTable.resize(NUM_BLOCKS);
        for(size_t i = 0; i < NUM_BLOCKS; i++) {
            blockIndexTable[i].blockNumber = i;
            blockIndexTable[i].offset = i*BLOCK_SIZE;
            blockIndexTable[i].freeSpace = BLOCK_SIZE;
        }

        //lockQueue.resize(NUM_BLOCKS);


    }

    //获取当前时间并格式化输出
    string getCurrentTime() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        return asctime(ltm);
        
    }
    //添加key和block的映射
    void addKeyBlockMapping(int key, size_t blockNumber) {
        //lock_guard<mutex> lock(mutex);
        if(keyBlockMap.find(key) != keyBlockMap.end()) {
            cout << "Key already exists! update key: "<<key<<" to block:"<<blockNumber << endl;
            return;
        }
        keyBlockMap[key] = blockNumber;
        blockIndexTable[0].freeSpace -= (sizeof(int) + sizeof(size_t));
        //cout << "Add key: " << key << " to block: " << blockNumber << endl;
    }

    //获取key和block的映射
    size_t getBlockNumber(int key) {
        //lock_guard<mutex> lock(mutex);
        if(keyBlockMap.find(key) == keyBlockMap.end()) {
            cout << "Key not found!" << endl;
            return -1;
        }
        return keyBlockMap[key];
    }

    //删除key和block的映射
    void delKeyBlockMapping(int key) {
        //lock_guard<mutex> lock(mutex);
        if(keyBlockMap.find(key) == keyBlockMap.end()) {
            cout << "Key not found!" << endl;
            return;
        }
        keyBlockMap.erase(key);
        blockIndexTable[0].freeSpace += (sizeof(int) + sizeof(size_t));
    }

    //获取空闲的block号并标记为已使用
    size_t getFreeBlock() {
        //lock_guard<mutex> lock(mutex);
        
        for(size_t i = 0; i < NUM_BLOCKS; i++) {
            if(blockUsageTable[i] == false) {
                blockUsageTable[i] = true;
                return i;
                cout << "Free block: " << i << endl;
            }
        }
        cout << "No free block!" << endl;
        return -1;
    }


    //获取块的信息
    BlockIndexTable getBlockInfo(size_t blockNumber) {
        //lock_guard<mutex> lock(mutex);
        if(blockNumber >= NUM_BLOCKS) {
            cout << "Block number out of range!" << endl;
            return blockIndexTable[0];
        }
        return blockIndexTable[blockNumber];
    }

    //更新块的剩余空间信息
    void updateBlockInfo(size_t blockNumber, size_t freeSpace) {
        //lock_guard<mutex> lock(mutex);
        if(blockNumber >= NUM_BLOCKS) {
            cout << "Block number out of range!" << endl;
            return;
        }
        blockIndexTable[blockNumber].freeSpace = freeSpace;
    }

    //计算这个类中的数据结构所占的大小
    size_t getSize() {
        size_t size = 0;
        size += sizeof(SuperBlock);
        size += blockUsageTable.size() * sizeof(bool);
        size += blockIndexTable.size() * sizeof(BlockIndexTable);
        size += keyBlockMap.size() * (sizeof(int) + sizeof(size_t));
        return size;
    }

    //打印文件信息
    void printInfo(){
        cout << "SuperBlock: " << endl;
        cout << "------------------------------" << endl;
        cout << "File Name: " << superBlock.fileName << endl;
        cout << "File Size: " << superBlock.fileSize << endl;
        cout << "Block Size: " << superBlock.blockSize << endl;
        cout << "Block Number: " << superBlock.blockNum << endl;
        cout << "Create Time: " << superBlock.createTime << endl;
        

        cout << "Block Usage Table: " << endl;
        cout << "------------------------------" << endl;
        for(size_t i = 0; i < NUM_BLOCKS; i++) {
            cout << blockUsageTable[i] << " ";
        }
        cout << endl;

        cout << "Block Index Table: " << endl;
        cout << "------------------------------" << endl;
        for(size_t i = 0; i < NUM_BLOCKS; i++) {
            cout << "Block Number: " << blockIndexTable[i].blockNumber << " ";
            cout << "Offset: " << blockIndexTable[i].offset << " ";
            cout << "Free Space: " << blockIndexTable[i].freeSpace << endl;
        }

        cout << "Key Block Map: " << endl;
        cout << "------------------------------" << endl;
        for(auto it = keyBlockMap.begin(); it != keyBlockMap.end(); it++) {
            cout << "Key: " << it->first << " ->";
            cout << "Block: " << it->second << endl;
        }
    }


    void testset() {
        //lock_guard<mutex> lock(mutex);
       addKeyBlockMapping(1, 1);
        addKeyBlockMapping(2, 2);
        addKeyBlockMapping(3, 3);
    }

    //将数据写入到磁盘文件的第一块中
    void writeToBlock0() {
        //lock_guard<mutex> lock(mutex);
        ofstream file(superBlock.fileName, ios::out | ios::binary);
        if(!file.is_open()) {
            cout << "Failed to open file: " << superBlock.fileName << endl;
            return;
        }
        file.seekp(0, ios::beg);

        //计算第0块剩余的大小
        size_t alloc = getSize();
        blockIndexTable[0].freeSpace = BLOCK_SIZE - alloc;
        
       proto::FileInfo info;

       //superblock
       info.mutable_superblock()->set_filename(superBlock.fileName);
       info.mutable_superblock()->set_filesize(superBlock.fileSize);
       info.mutable_superblock()->set_blocksize(superBlock.blockSize);
       info.mutable_superblock()->set_blocknum(superBlock.blockNum);
       info.mutable_superblock()->set_createtime(superBlock.createTime);

        //blockUsageTable  
        for(size_t i = 0; i < NUM_BLOCKS; i++) {
            info.add_blockusagetable(blockUsageTable[i]);
        }

        //blockIndexTable
        for(size_t i = 0; i < NUM_BLOCKS; i++) {
            proto::BlockIndexTable* blockIndex = info.add_blockindextables();
            blockIndex->set_blocknumber(blockIndexTable[i].blockNumber);
            blockIndex->set_offset(blockIndexTable[i].offset);
            if(i == 0) {
                //blockIndex->set_freespace(BLOCK_SIZE);
            } 
            else {
            blockIndex->set_freespace(blockIndexTable[i].freeSpace);
            }
        }

        //keyBlockMap
        for(auto it = keyBlockMap.begin(); it != keyBlockMap.end(); it++) {
            info.mutable_keyblockmap()->insert({it->first, it->second});
        }

        //序列化为文件流
        string str;
        info.SerializeToString(&str);
       
        file.write(str.c_str(), str.size());

        file.close();
        printInfo();
    }

    //从文件的第0块中读取数据
    bool readFromBlock0() {
        //lock_guard<mutex> lock(mutex);
        ifstream file(superBlock.fileName, ios::in | ios::binary);
        if(!file.is_open()) {
            cout << "Failed to open file: " << superBlock.fileName << endl;
            file.close();
            return false;
        }

        // 检查文件是否为空
        // char c;
        // file >> c;
        // if (file.eof()) {
        //     cerr << "File is empty: " << superBlock.fileName << endl;
        //     file.close();
        //     return false;
        // }

        // file.seekg(0, ios::end);
        // if (file.tellg() == 0) {
        //     cerr << "File is empty: " << superBlock.fileName << endl;
        //     file.close();
        //     return false;
        // }

        file.seekg(0, ios::beg);

        proto::FileInfo info;
       
        string str((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        if (str.empty()) {
            cerr << "Failed to parse file: " << superBlock.fileName << endl;
            return false;
         }

        info.ParseFromString(str);
        //cout << "Parse file: " << str << endl;
        file.close();

        //superblock
        superBlock.fileName = info.superblock().filename();
        cout << "File Name: " << info.superblock().filename() << endl;
        superBlock.fileSize = info.superblock().filesize();
        superBlock.blockSize = info.superblock().blocksize();
        superBlock.blockNum = info.superblock().blocknum();
        superBlock.createTime = info.superblock().createtime();

        //blockUsageTable
        for(int i = 0; i < info.blockusagetable_size(); i++) {
            blockUsageTable[i] = info.blockusagetable(i);
            cout << info.blockusagetable(i) << " ";
        }

        //blockIndexTable
        for(int i = 0; i < info.blockindextables_size(); i++) {
            blockIndexTable[i].blockNumber = info.blockindextables(i).blocknumber();
            blockIndexTable[i].offset = info.blockindextables(i).offset();
            blockIndexTable[i].freeSpace = info.blockindextables(i).freespace();
        }

        //keyBlockMap
        for(auto it = info.keyblockmap().begin(); it != info.keyblockmap().end(); it++) {
            keyBlockMap[it->first] = it->second;
        }

        printInfo();
        

        return true;


        
    }

private:
    SuperBlock superBlock;
    vector<bool> blockUsageTable;
    vector<BlockIndexTable> blockIndexTable;
    //vector<mutex> lockQueue;
    map<int, size_t> keyBlockMap;
    
};

// int main() {
//     FileInfo fileInfo;

//     //将信息存入到文件的第一块中
//     return 0;
// }