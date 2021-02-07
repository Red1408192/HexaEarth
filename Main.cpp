#include "TileMap.hpp"
#include <iostream>
#include <chrono>
#include <set>
#include "ThreadPool.hpp"
#include "Noise.hpp"
#include "BitmapMaker.hpp"
using namespace std;

void MapTest(int size, float simplexSize, int definition, float hOffset = 0.,float vOffset = 0., float slope = 6.){
    auto noise = RedNoise(definition, 476473642);
    NoiseParameter p = NoiseParameter{size,simplexSize, hOffset, vOffset, slope};
    for(int y = size-1; y >= 0; y--){
        for(int x = 0; x < size; x++){
            cout << noise.NoiseMapInterrogation(y, x, p).t << '\t';
        }
        cout << '\n';
        cout << '\n';
        cout << '\n';
    }
}

void PrintTestMap(int size, float simplexSize, int definition = 256, float hOffset = 0.,float vOffset = 0., float slope = 6.){
    auto noise = RedNoise(definition, 476473642);
    OutputBitmap op = OutputBitmap();
    op.beginMap(size);
    NoiseParameter p = NoiseParameter{size,simplexSize, hOffset, vOffset, slope};
    for(int y = size-1; y >= 0; y--){
        for(int x = 0; x < size; x++){
            auto value = noise.NoiseMapInterrogation(y, x, p);
            op.InsertMap(value.t, value.q, value.p);
        }
    }
    op.CloseMap();
}

void loadTest(int definition, int maxHeight, float maxSimplex, float vOffset = 0.,float hOffset = 0., float startSimplex = 1., float slope = 6.){
    auto noise = RedNoise(definition, 476473642);
    cout << "[";
    for(int r : noise.GetPerm()){
        cout << r << ", ";
    }
    cout << "]\n";
    cout << "permutations size: " << noise.GetPerm().size() << '\n';

    NoiseParameter p = NoiseParameter{0,0.f, hOffset, vOffset, slope};
    for (float simplexSize = startSimplex; simplexSize < maxSimplex; simplexSize += simplexSize*1.1){
        p.simplexWidth = simplexSize;
        for (int size = 8; size < maxHeight; size=size*2){
            p.w = size;
            auto start = std::chrono::steady_clock::now();
            for(int y = size-1; y >= 0; y--){
                for(int x = 0; x < size*2; x++){
                    noise.NoiseMapInterrogation(y, x, p);
                }
            }
            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsed_seconds = end-start;
            cout << "size: " << size << ", simplex: " << simplexSize << ", time elapsed: " << elapsed_seconds.count()<< '\n';
        }
    }
}

int main(){
    float h = (64.f*0.866025f);
    PrintTestMap(512, 64.f, 256, h/1.3, 0.0, 0.1);
    auto noise = RedNoise(256, 476473642);
    return 0;
}

