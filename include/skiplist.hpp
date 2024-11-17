#pragma once

#include <vector>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>

#include "../proto/sl.pb.h"

using namespace std;

class Skiplist {
public:
    static const int level = 8; // 层数，经验值 8，太大浪费空间，因为每一个节点都要存在每一层的 next，层数越多节点数越多

    // 定义跳表节点
    struct Node {
        int key; // 节点key
        string value;  // 节点value
        vector<Node*> next; // 记录节点在每一层的 next，next[i] 表示当前节点第 i 层的 next

        // 构造函数
        Node(int _key, string _value) : key(_key), value(_value) {
            next.resize(level, nullptr); // 初始化 next 数组的大小和层数 level 相同，初始值都指向 NULL
        }
        
    }*head,*tail; 

    Skiplist() {
        head = new Node(-1,""); // 初始化一个不存在的节点值 -1
        tail = head;  //初始化尾指针
    }

    // 析构函数，释放内存，保留头节点
    ~Skiplist(){
        Node* p = head->next[0];
        while (p) {
            Node* nextNode = p->next[0];
            delete p;
            p = nextNode;
        }
        // 保留头节点
        head->next.assign(level, nullptr);
        //slprint();
        
    }

    // 辅助函数：找到每一层 i 小于目标值 target 的最大节点 pre[i]，最后 pre 中存的就是每一层小于 target 的最大节点
    void find(int target, vector<Node*>& pre) {
        auto p = head; // 从头节点开始遍历每一层
        for (int i = level - 1; i >= 0; i -- ) { // 从上层往下层找
            while (p->next[i] && p->next[i]->key < target) p = p->next[i]; // 如果当前层 i 的 next 不为空，且它的值小于 target，则 p 往后走指向这一层 p 的 next
            pre[i] = p; // 退出 while 时说明找到了第 i 层小于 target 的最大节点就是 p
        }
    }
    
    // 从跳表中查找 target
    string search(int target) {
        vector<Node*> pre(level);
        find(target, pre); // 先找到每一层 i 小于目标值 target 的最大节点 pre[i]
        
        auto p = pre[0]->next[0]; // 因为最下层【0】的节点是全的，所以只需要判断 target 是否在第 0 层即可，而 pre[0] 正好就是小于 target 的最大节点，如果 pre[0]->next[0] 的值不是 target 说明没有这个元素
        return  ( p && p->key == target) ? p->value : "";
    }
    
    // 向跳表中插入元素 num,flag为1表示顺序插入（key已经有序）
    size_t add(int num, string value, int flag = 0) {

        auto node = new Node(num, value); // 创建要插入的新节点
        size_t nodeSize = sizeof(int) + value.capacity() + sizeof(Node*) * level; // 计算节点所占用的空间大小
        

        if(!flag){
        vector<Node*> pre(level);
        find(num, pre); // 先找到每一层 i 小于目标值 target 的最大节点 pre[i]

            //如果存在相同的key，则更新value
            if(pre[0]->next[0] && pre[0]->next[0]->key == num){
                pre[0]->next[0]->value = value;
                return nodeSize;
            }
        
        for (int i = 0; i < level; i++) { // 遍历每一层，从下往上插入新节点
            node->next[i] = pre[i]->next[i]; // 这两步就是单链表的插入
            pre[i]->next[i] = node;

            
            if (rand() %2) break; // 每一层有 50% 的概率不插入新节点
            }
        }
        else{
            //顺序插入，key已经有序
            
            for (int i = 0; i < level; i++) { 
                //使用尾插法
               
                node->next[i] = tail->next[i];
                tail->next[i] = node;

                //cout << rand() %2 << endl;
                if (rand() %2) break; // 每一层有 50% 的概率不插入新节点
            }
             tail = node;
        }
        return nodeSize;
    }
    
    // 从跳表中删除 num
    bool erase(int num , size_t& nodeSize) {
        vector<Node*> pre(level);
        find(num, pre); // 先找到每一层 i 小于目标值 target 的最大节点 pre[i]

        // 先判断 num 是否存在，不存在直接返回 false
        // 第 0 层存储的是全部节点，所以只需要判断 pre[0]->next[0]（第 0 层小于 num 的最大节点的在第 0 层的 next） 是不是 num 即可
        auto p = pre[0]->next[0];
        if (!p || p->key != num) return false;

        nodeSize = sizeof(int) + p->value.capacity() + sizeof(Node*) * level; // 计算节点所占用的空间大小
       
        // 否则删除每一层的 num，如果 pre[i]->next[i] == p 说明第 i 层存在 p
        for (int i = 0; i < level && pre[i]->next[i] == p; i ++ ) {
            pre[i]->next[i] = p->next[i]; // 单链表删除
        }

        delete p; // 删除节点 p，防止内存泄漏

        return true;
    }

    // 打印跳表
    void slprint(){
        for(int i = 0; i < level; i++){
            auto p = head; // 从头节点开始遍历每一层
            cout << "level " << i << ": ";
            while(p->next[i]){
                cout << p->next[i]->key << " ";
                p = p->next[i];
            }
            cout << endl;
        }
       
    }

    //序列化到文件中
    void serialize(const string& filename, size_t offset) {
        fstream outFile(filename, ios::binary | ios::out | ios::in);
        if (!outFile.is_open()) {
            cerr << "Failed to open file for writing: " << filename << endl;
            return;
        }


        proto::Skiplist sl;
        Node* p = head; //将头结点序列化，便于后续建立头结点的next指针
        while (p) {
            proto::Node* nodeProto = sl.add_nodes();
            nodeProto->set_key(p->key);
            nodeProto->set_value(p->value);
            for (int i = 0; i < level; ++i) {
                if (p->next[i]) {
                    nodeProto->add_next(p->next[i]->key);
                } else {
                    nodeProto->add_next(-2); // -1表示头指针，-2表示空指针
                }
             }
                p = p->next[0];
            }

        
        serializedSize = sl.ByteSizeLong();

        //将序列化数据写入文件
        vector<char> buffer(serializedSize);
        if (!sl.SerializeToArray(buffer.data(), serializedSize)) {
            cerr << "Failed to serialize data." << endl;
        }

        // 将文件指针移动到指定的偏移量位置
        outFile.seekp(offset, ios::beg);
        outFile.write(reinterpret_cast<const char*>(&serializedSize), sizeof(size_t));

        outFile.seekp(offset + sizeof(size_t), ios::beg);
        outFile.write(buffer.data(), serializedSize);
        // if (!sl.SerializeToOstream(&outFile)) {
        //     cerr << "Failed to serialize data." << endl;
        // }

        outFile.close();
        //cout<< "ByteSizeLong:" <<sl.ByteSizeLong() << endl;
        //slprint();
    }

    // 从文件中反序列化到跳表中
    size_t deserialize(const string& filename, size_t offset) {
        fstream inFile(filename, ios::binary | ios::in | ios::out);
        if (!inFile.is_open()) {
            cerr << "Failed to open file for reading: " << filename << endl;
            return 0;
        }

        // 将文件指针移动到指定的偏移量位置
        inFile.seekg(offset, ios::beg);
        inFile.read(reinterpret_cast<char*>(&serializedSize), sizeof(size_t));
        
        inFile.seekg(offset + sizeof(size_t), ios::beg);
        vector<char> buffer(serializedSize);
        inFile.read(buffer.data(), serializedSize);
        streamsize bytesRead = inFile.gcount();
        if (bytesRead == 0) {
            cerr << "Failed to read data from file: " << filename << endl;
            inFile.close();
            return 0;
        }
       proto::Skiplist sl;

        if (!sl.ParseFromArray(buffer.data(), serializedSize)) {
            cerr << "Failed to parse file: " << filename << endl;
            return 0;
        }
        // if (!sl.ParseFromIstream(&inFile)) {
        //     cerr << "Failed to parse file: " << filename << endl;
        //     return 0;
        // }

        inFile.close();

        // 清空当前跳表
        
        Node* p = head->next[0];
        while (p) {
            Node* nextNode = p->next[0];
            delete p;
            p = nextNode;
        }
        head->next.assign(level, nullptr);

        // 反序列化数据并计算跳表所占空间
        size_t totalSize = 0;
        map<int, Node*> nodeMap;  //key和节点指针的映射
        for (const auto& nodeProto : sl.nodes()) {
            Node* node = new Node(nodeProto.key(), nodeProto.value());
            nodeMap[nodeProto.key()] = node;
            totalSize += sizeof(int) + nodeProto.value().capacity() + sizeof(Node*) * level;
        }
        for (const auto& nodeProto : sl.nodes()) {
            Node* node = nodeMap[nodeProto.key()];           
            for (int i = 0; i < nodeProto.next_size(); ++i) {
                int nextKey = nodeProto.next(i);
                if (nextKey != -2) {
                    //头结点的next指针需要特殊处理
                    if(nodeProto.key() == -1){
                        head->next[i] = nodeMap[nextKey];
                    }
                    node->next[i] = nodeMap[nextKey];
                   
                }
            }
        }

        // //连接头结点
        // if (!sl.nodes().empty()) {
        //     for(int i = 0; i < level; i++){
        //         head->next[i] = nodeMap[sl.nodes(0).key()];
        //     }
           
        // }
        slprint();

        return totalSize;
    }



private:
 size_t serializedSize;















    //跳表序列化到文件中
    // void serialize(const string& filename, size_t offset){
    //     ofstream file_writer(filename, ios::out | ios::binary);
    //     //file_writer.open(filename);
    //     if(!file_writer.is_open()){
    //         cerr << "Failed to open file: " << filename << endl;
    //         return;
    //     }
    //     //从块的偏移量开始写入
    //     file_writer.seekp(offset, ios::beg);
    //     auto p = head;
    //     while(p->next[0]){
    //         file_writer << p->next[0]->key << ":" << p->next[0]->value << endl;
    //         p = p->next[0];
    //     }
    //     file_writer.flush();
    //     file_writer.close();
        


    // }

    //从文件中反序列化到跳表中
    // size_t deserialize(const string& filename,size_t offset){
    //     ifstream file_reader(filename, ios::in | ios::binary);
    //     //file_reader.open(filename);
    //     if(!file_reader.is_open()){
    //         cerr << "Failed to open file: " << filename << endl;
    //         return -1;
    //     }
    //     file_reader.seekg(offset, ios::beg);
    //     string line;

    //     size_t listSize = 0;
    //     while(getline(file_reader,line)){
    //         cout << "line: " << line << endl;
    //         //捕获stoi异常
    //         try {
    //             int key = stoi(line.substr(0, line.find(":")));
    //             cout << "Key: " << key << endl;

    //             string value = line.substr(line.find(":")+1);
            
    //             //顺序插入并每次返回最后一个节点的指针
    //             listSize += add(key,value,1);
    //         } 
    //         catch (const invalid_argument& e) {
    //             cout << "line: " << line << endl;
    //             cerr << "Invalid argument: " << e.what() << endl;
    //         } 
    //         catch (const out_of_range& e) {
    //             cout << "line: " << line << endl;
    //             cerr << "Out of range: " << e.what() << endl;
    //         }
    //         //int key = stoi(line.substr(0,line.find(":")));
            

            
    //     }
    //     slprint();  
    //     file_reader.close();

    //     return listSize; 
    // }
};
    
