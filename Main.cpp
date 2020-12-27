#include "TileMap.hpp"
#include <iostream>
#include <chrono>
#include "ThreadPool.hpp"
using namespace std;

void DoCounting(int& input){
    int result = input;
    for(int i = 0; i < INT32_MAX; i++){
        result ++;
    }
    for(int i = 0; i < INT32_MAX; i++){
        result --;
    }
    cout << "result:"<< result<<"\n";
    cout << "result pointer:"<< &result<<"\n";
}

int main(){
    ThreadPool<void, int&> tP = ThreadPool<void, int&>(); //personal threadpool implementation
    int size = tP.ThreadsCount();
    int input;
    cout << "number of jobs:";
    cin >> input;
    auto start = std::chrono::steady_clock::now();
    int results[50] = {}; //create a pool of results
    for(int i = 0; i < input; i++){
        tP.AddJob(DoCounting, results[i]);
    }
    while(!tP.IsQueEmpty()){
        continue;
    }
    cout << "calling shutdown\n";
    tP.shutdown();
    auto stop = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = stop-start;
    cout << "Done in "<< diff.count() << "sec" << '\n' << '\n';
    return 0;
}

/*     
    int input;
    cout << "number of jobs:";
    cin >> input;
    auto start = std::chrono::steady_clock::now();
    for(int i = 0; i < input; i++){
        DoCounting();
    }
    auto stop = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = stop-start;
    cout << "Done in "<< diff.count() << "sec" << '\n' << '\n';
*/