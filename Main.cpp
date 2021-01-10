#include "TileMap.hpp"
#include <iostream>
#include <chrono>
#include <set>
#include "ThreadPool.hpp"
#include "Noise.hpp"
using namespace std;

int main(){
    auto noise = SimplexNoise(512, 476473642);
    cout << "[";
    for(int r : noise.GetPerm()){
        cout << r << ", ";
    }
    cout << "]\n";
    cout << "permutations size: " << noise.GetPerm().size() << '\n';

    int size = 8;

    for(int y = size-1; y >= 0; y--){
        for(int x = 0; x < size; x++){
            cout << noise.NoiseMapInterrogation(y, x, size, 32) << '\t';
        }
        cout <<'\n';
        cout <<'\n';
        cout <<'\n';
        cout <<'\n';
    }
    return 0;
}