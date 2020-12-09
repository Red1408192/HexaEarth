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
    double res;
    for(int i = 0; i < 1001; i++){
        int idToFind = tileboard.fullTileList[rand()%4801].id;
        auto start = std::chrono::steady_clock::now();
        tileboard.fullTileList.GetIndex(idToFind);
        auto stop = std::chrono::steady_clock::now();
        std::chrono::duration<double> diff = stop-start;
        res += diff.count();
    }
    cout << res/1000;
    return 0;
};