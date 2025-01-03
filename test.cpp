#include <iostream>
#include <csignal>
#include <mutex>
#include <condition_variable>

using namespace std;

mutex cv_m;
condition_variable cv;
bool ready = false;

//信号处理函数
void signalHandler(int signum) {
    cout << "Interrupt signal (" << signum << ") received." << endl;
    sleep(5);
    lock_guard<mutex> lk(cv_m);
    ready = true;
    cv.notify_all();
    cout << "Signal handler done." << endl;
}


int main() {
  //捕获中端信号
    signal(SIGINT, signalHandler);


    unique_lock<mutex> lk(cv_m);
    cv.wait(lk, []{ return ready; });
    
    cout << "Main thread is done." << endl;
    return 0;
}