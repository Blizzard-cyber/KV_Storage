#ifndef CACHE_HPP
#define CACHE_HPP

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <shared_mutex>
#include "skiplist.hpp" 
#include "IOpool.hpp"
#include "global.h"
using namespace std;



//读写缓冲类
class Cache {
public:

    size_t currentSize;
    IOThreadPool& ioPool;
    
    Cache(string filename, IOThreadPool& ioPool) : ioPool(ioPool) {
        currentSize = 0;
        FILENAME = filename;
    }

    void put(FileInfo& fileInfo,int key, const string& value) {
        unique_lock<shared_mutex> lock(WRmutex);
        size_t nodesize = RWcache.add(key, value);  // 向SkipList中插入键值对

        currentSize += nodesize;
        //cout << "[WRcache] PUT: Key = " << key << ", Value = " << value <<endl;
        //cout << "[WRcache] PUTsize: "<<nodesize<<endl;
        //cout << "[WRcache] TotalSize:"<<currentSize  <<endl;
       
        if (currentSize > CACHE_SIZE ) {
            size_t freeSpace = (CACHE_SIZE - currentSize + nodesize);
            size_t freeBlock = fileInfo.getFreeBlock();
            //lock.unlock(); // 释放当前写缓冲的锁

            // 提交 flushToDisk 任务到 I/O 线程池，并传递写锁
            auto lockPtr = make_shared<unique_lock<shared_mutex>>(move(lock));
            ioPool.submit(freeBlock-1, [this, &fileInfo, freeSpace, freeBlock, lockPtr] {
                flushToDisk(fileInfo, FILENAME, freeSpace, freeBlock); // 交由IOpool的线程接管
            });
            
        }
        
       
    }

    //flag为0表示查写缓冲，flag为1表示需要从磁盘读取
    string  get(FileInfo& fileInfo,int key,int flag) {
        //读缓冲
        if(flag){
            size_t blockNumber = fileInfo.getBlockNumber(key);  // 获取键值对所在的块号
            if(blockNumber != -1){
                if(blockNumber == currentReadBlock){   //读缓冲中已经有了该块
                    shared_lock<shared_mutex> lock(RDmutex); // 对读缓冲区加锁，获得进入读缓冲区的条件
                    return RWcache.search(key); // 从SkipList中查找键值对
                }
                else{   //读缓冲中没有该块
                    unique_lock<shared_mutex> lock(RDmutex); // 独占锁，只有当不存在get和del时才能获取到
                    currentReadBlock = blockNumber;
                    lock.unlock(); // 解锁并当前线程执行结束

                    ioPool.submit(blockNumber-1, [this, &fileInfo, blockNumber, key] {
                        loadFromDisk(fileInfo, FILENAME, blockNumber);
                        shared_lock<shared_mutex> readLock(RDmutex); // 读取锁
                        return RWcache.search(key); // 从SkipList中查找键值对
                    });
                    return ""; // 返回空字符串，实际结果由IO线程池的线程返回
                }
            }
            else{
                return ""; 
            }
        }
        //写缓冲
        else{
            shared_lock<shared_mutex> lock(WRmutex); // 对写缓冲区加锁
            string value = RWcache.search(key); // 从SkipList中查找键值对
            if(value == "") {
                //cout << "[RWcache] GET: Key: "<< key << "    not found." << endl;  
            }
            else{
                //cout << "[RWcache] GET: Key = " << key << ", Value = " << value <<endl;
                
            }
            return value; 
        }
    }

    //flag为0表示查写缓冲，flag为1表示需要从磁盘读取
    bool del(FileInfo& fileInfo,int key,int flag) {
        //读缓冲
        if(flag){           
            size_t blockNumber = fileInfo.getBlockNumber(key);
            if(blockNumber != -1){
                unique_lock<shared_mutex> lock(RDmutex); // 对读缓冲区加锁，获得进入读缓冲区的条件
                if(blockNumber != currentReadBlock){  //读缓冲中没有该块
                    currentReadBlock = blockNumber;
                    lock.unlock(); // 释放当前读缓冲的锁
                   // 提交 I/O 任务到 I/O 线程池
                    ioPool.submit(blockNumber-1, [this, &fileInfo, blockNumber, key,flag] {
                        loadFromDisk(fileInfo, FILENAME, blockNumber);
                        unique_lock<shared_mutex> writeLock(RDmutex);
                        delhander(key,flag,fileInfo,blockNumber);
                    });

                    // loadFromDisk(fileInfo, FILENAME,blockNumber);
                    // delhander(key,flag,fileInfo,blockNumber);
                }
                delhander(key,flag,fileInfo,blockNumber);               
            }
            else{
                return false; 
            }
        }
        //写缓冲
        else{
            unique_lock<shared_mutex> lock(WRmutex); // 对写缓冲区加锁
            delhander(key,flag,fileInfo);
        }
        return true;
       
    }
    

    void delhander(int key,int flag,FileInfo& fileInfo,size_t blockNumber=-1){
        size_t nodeSize=0;
        if(RWcache.erase(key,nodeSize)){
            if(flag){
            fileInfo.updateBlockInfo(blockNumber,nodeSize);
            }
            //cout << "[RWcache] DEL: Key = " << key <<endl;
            currentSize -= nodeSize;
            
        }
        else{
            //cout << "[RWcache] DEL: Key: "<< key << "    not found." << endl;
            
         }
    }

     //写磁盘
    bool  flushToDisk(FileInfo& fileInfo, const string& filename,size_t freeSpace,size_t freeBlock) {
       
        //size_t freeBlock=fileInfo.getFreeBlock();
        if(freeBlock==-1){
            //cout<<"[WRcache] No free block to write!"<<endl;
            return false;
        }
        else{
            //cout<<"[WRcache] Write to block: "<<freeBlock<<endl;
            
            //写入key与块号的map关系
            auto p = RWcache.head;
            while(p->next[0]){
                fileInfo.addKeyBlockMapping(p->next[0]->key,freeBlock);
                p = p->next[0];
                ////cout<<"[WRcache] Add key: "<<p->next[0]->next[0]<<" to block: "<<freeBlock<<endl;
            }
        }
        //unique_lock<shared_mutex> lock(mutex);
        size_t offset = freeBlock * CACHE_SIZE;
        ////cout << "offset: "<<offset<<endl;
        RWcache.serialize(filename,offset);  // 序列化SkipList到文件

        //更新块的剩余空间信息
        fileInfo.updateBlockInfo(freeBlock, freeSpace);

        //打印文件信息
        //fileInfo.printInfo();

        //清空缓冲区
        RWcache.~Skiplist();

        
        
        currentSize = 0;
        return true;
    }

    //从磁盘读取
    bool loadFromDisk(FileInfo& fileInfo, const string& filename , size_t blockNumber=-1) {
        //unique_lock<shared_mutex> lock(mutex);
        //size_t blockNumber = fileInfo.getBlockNumber(key);
        if(blockNumber==-1){
            //cout<<"[Readcache] Key not found!"<<endl;
            return false;
        }
        else{
            RWcache.~Skiplist();//清空读缓冲
            //cout<<"[Readcache] Read from block: "<<blockNumber<<endl;
            currentSize = RWcache.deserialize(filename,blockNumber*CACHE_SIZE);  // 从文件中反序列化到SkipList
            return true;
        }

        
    }

    

private:

    Skiplist RWcache;
    //size_t currentSize;
    string FILENAME;
    shared_mutex WRmutex; //写缓冲的共享互斥锁，put为写，get为读,del为写
    shared_mutex RDmutex; //读缓冲的共享互斥锁,get为读，del为写
    size_t currentReadBlock = -1; // 当前读缓冲的块号
    

   
    
};




#endif // CACHE_HPP