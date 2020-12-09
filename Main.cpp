#include "TileMap.h"
#include <iostream>
#include <chrono>
using namespace std;


int main(){
    cout << "tile map width:\n";
    int testnum;
    cin >> testnum;
    testnum = testnum%2==0? testnum : testnum - 1;
    auto start = std::chrono::steady_clock::now();
    TileBoard tileboard = TileBoard(testnum);
    auto stop = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = stop-start;
    cout << "tile map of " << testnum*(testnum/2) << " tiles \n";
    cout << "Done in "<< diff.count() << "sec" << '\n' << '\n';
    double res;
    cout << "executing " << testnum/4 << " random tile search test \n";
    for(int i = 0; i < (testnum/4)+1; i++){
        int idToFind = tileboard.fullTileList[rand()%testnum+1].id;
        auto start = std::chrono::steady_clock::now();
        tileboard.fullTileList.GetIndex(idToFind);
        auto stop = std::chrono::steady_clock::now();
        std::chrono::duration<double> diff = stop-start;
        res += diff.count();
    }
    cout << "mean time to find a random tile:\n";
    cout << res/(testnum/4) << "sec" << '\n';
    return 0;
};