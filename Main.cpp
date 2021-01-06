#include "TileMap.hpp"
#include <iostream>
#include <chrono>
#include <set>
#include "ThreadPool.hpp"
#include "Noise.hpp"
using namespace std;

int main(){
    auto noise = SimplexNoise(256, random_device{}());
    cout << "[";
    for(int r : noise.GetPerm()){
        cout << r << ", ";
    }
    cout << "]\n";
    cout << "permutations size: " << noise.GetPerm().size();
    return 0;
}