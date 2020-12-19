#include "TileMap.h"
#include <iostream>
#include <chrono>
#include "ThreadPool.h"
using namespace std;

void DoCounting(){
    int result;
    for(int i = 0; i < INT32_MAX; i++){
        if(i % 1000000000 == 0){
            cout << "progress...\n";
        }
        result ++;
    }
    for(int i = 0; i < INT32_MAX; i++){
        if(i % 1000000000 == 0){
            cout << "progress...\n";
        }
        result --;
    }
    cout << "Done\n"<< "result:"<< result<<"\n";
}

int main(){
    ThreadPool tP = ThreadPool();
    int input;
    cout << "number of jobs:";
    cin >> input;
    auto start = std::chrono::steady_clock::now();
    for(int i = 0; i < input; i++){
        tP.AddJob(DoCounting);
    }
    while(tP.IsQueEmpty()){
        cout << "working\n";
    }
    tP.shutdown();
    auto stop = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = stop-start;
    cout << "Done in "<< diff.count() << "sec" << '\n' << '\n';
    return 0;
};

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