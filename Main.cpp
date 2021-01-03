#include "TileMap.hpp"
#include <iostream>
#include <chrono>
#include <set>
#include "ThreadPool.hpp"
using namespace std;

int DoCounting(){
    int result = 0;
    for(int i = 0; i < INT32_MAX; i++){
        result ++;
    }
    for(int i = 0; i < INT32_MAX; i++){
        result --;
    }
    return result;
}

int main(){
    ThreadPool<int> tP = ThreadPool<int>(); //personal threadpool implementation
    int input;
    cout << "number of jobs:";
    cin >> input;
    auto start = std::chrono::steady_clock::now();
    set<int> toCheck;
    auto& subscriber =  tP.Subscribe();
    for(int i = 0; i < input; i++){
        toCheck.emplace(tP.AddJob(DoCounting));
    }
    while(true){
        for(int i : toCheck){
            int result;
            if(subscriber.await(i, result)){
                cout<< i <<"th job result is: " << result <<'\n';
                toCheck.erase(i);
                break;
            }
        }
        if(toCheck.empty()){
            break;
        }
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