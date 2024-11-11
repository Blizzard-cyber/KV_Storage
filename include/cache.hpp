#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <fstream>
//#include <mutex>
#include <shared_mutex>
#include "skiplist.hpp" 

using namespace std;
//const size_t CACHE_SIZE = 64 * 1024 * 1024; // 64MB

//test
const size_t CACHE_SIZE = 4 * 1024 ; // 4kB


//读写缓冲类
class Cache {
public:
    Cache(string filename) {
        currentSize = 0;
        FILENAME = filename;
    }

    void put(FileInfo& fileInfo,int key, const string& value) {
        //unique_lock<shared_mutex> lock(mutex);
        size_t nodesize = RWcache.add(key, value);  // 向SkipList中插入键值对
        cout << "PUTsize: "<<nodesize<<endl;
        if (currentSize + nodesize > CACHE_SIZE) {
            size_t freeSpace =CACHE_SIZE- currentSize;
            flushToDisk(fileInfo, FILENAME,freeSpace);  // 缓冲区满了，将数据写入磁盘
        }
        currentSize += nodesize;
        cout << "[WRcache] Size:"<<currentSize  <<endl;
        cout << "[WRcache] PUT: Key = " << key << ", Value = " << value <<endl;
    }

    //flag为0表示查写缓冲，flag为1表示需要从磁盘读取
    string  get(FileInfo& fileInfo,int key,int flag) {
        if(flag==1){
            if(!loadFromDisk(fileInfo, FILENAME,key)){
                return ""; 
            }; 
        }

            string value = RWcache.search(key); // 从SkipList中查找键值对
            
            if(value == "") {
                cout << "[RWcache] GET: Key: "<< key << "    not found." << endl;
                
            }
            else{
                cout << "[RWcache] GET: Key = " << key << ", Value = " << value <<endl;
                
            }
            return value; 
    }

    //flag为0表示查写缓冲，flag为1表示需要从磁盘读取
    bool del(FileInfo& fileInfo,int key,int flag) {
        if(flag){
            if(!loadFromDisk(fileInfo, FILENAME,key)){
                return false; 
            }; 
        }
        size_t nodeSize =0;
        bool result = RWcache.erase(key,nodeSize); // 从SkipList中删除键值对
        cout << "DELsize: "<<nodeSize<<endl;
        if(result){
        //WRcache.slprint();
            cout << "[RWcache] DEL: Key = " << key <<endl;
            currentSize -= nodeSize;
            if(flag){
                //删除key与块号的map关系
                fileInfo.delKeyBlockMapping(key);

                //更新对应块的剩余空间信息
                fileInfo.updateBlockInfo(fileInfo.getBlockNumber(key),nodeSize);

                //清空读缓冲区
                RWcache.~Skiplist();
            }
            return true;
        }
        else{
             cout << "[RWcache] DEL: Key: "<< key << "    not found." << endl;
             return false;
        }
       
    }
    

    

    

    

private:

    Skiplist RWcache;
    size_t currentSize;
    string FILENAME;
    //shared_mutex mutex;

    //写磁盘
    void flushToDisk(FileInfo& fileInfo, const string& filename,size_t freeSpace) {
       
        size_t freeBlock=fileInfo.getFreeBlock();
        if(freeBlock==-1){
            cout<<"[WRcache] No free block to write!"<<endl;
            return;
        }
        else{
            cout<<"[WRcache] Write to block: "<<freeBlock<<endl;
            
            //写入key与块号的map关系
            auto p = RWcache.head;
            while(p->next[0]){
                fileInfo.addKeyBlockMapping(p->next[0]->key,freeBlock);
                p = p->next[0];
                //cout<<"[WRcache] Add key: "<<p->next[0]->next[0]<<" to block: "<<freeBlock<<endl;
            }
        }
        //unique_lock<shared_mutex> lock(mutex);
        size_t offset = freeBlock * CACHE_SIZE;
        //cout << "offset: "<<offset<<endl;
        RWcache.serialize(filename,offset);  // 序列化SkipList到文件

        //更新块的剩余空间信息
        fileInfo.updateBlockInfo(freeBlock, freeSpace);

        //打印文件信息
        fileInfo.printInfo();

        //清空缓冲区
        RWcache.~Skiplist();
        
        currentSize = 0;
    }

    //从磁盘读取
    bool loadFromDisk(FileInfo& fileInfo, const string& filename ,int key) {
        //unique_lock<shared_mutex> lock(mutex);
        size_t blockNumber = fileInfo.getBlockNumber(key);
        if(blockNumber==-1){
            cout<<"[Readcache] Key not found!"<<endl;
            return false;
        }
        else{
            cout<<"[Readcache] Read from block: "<<blockNumber<<endl;
            currentSize = RWcache.deserialize(filename,blockNumber*CACHE_SIZE);  // 从文件中反序列化到SkipList
            return true;
        }

        
    }
    
};



// int main() {
//     WriteCache writeCache;
//     ReadCache readCache;

//     writeCache.put("key1", "value1");
//     string value = readCache.get("key1");

//     cout << "Read value: " << value << endl;

//     return 0;
// }