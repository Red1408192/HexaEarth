#include "TileMap.h"
#include <iostream>
#include <chrono>
using namespace std;


int main(){
    auto start = std::chrono::steady_clock::now();
    TileBoard tileboard = TileBoard(4800);
    auto stop = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = stop-start;
    cout << "Done in "<< diff.count() << "sec" << '\n';
    return 0;
};
