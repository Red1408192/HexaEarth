#include "TileMap.h"
#include <iostream>
#include <chrono>
#include "ThreadPool.h"
using namespace std;

void DoCounting(){
    int result;
    for(int i = 0; i < INT32_MAX; i++){
        result ++;
    }
    for(int i = 0; i < INT32_MAX; i++){
        result --;
    }
    cout << result;
}

int main(){
    ThreadPool tP = ThreadPool();
    int input;
    cout << "number of jobs:";
    cin >> input;
    auto start = std::chrono::steady_clock::now();
    for(int i = 0; i < input; i++){
        tP.Add_Job(DoCounting);
    }
    while(tP.IsQueEmpty()){
        continue;
    }
    tP.shutdown();
    auto stop = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = stop-start;
    cout << "Done in "<< diff.count() << "sec" << '\n' << '\n';
    return 0;
};