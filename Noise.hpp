#pragma once
#include <stdlib.h>
#include <set>
#include <memory>
#include <numeric>
#include <random>
#include <algorithm>

typedef uniform_int_distribution<int> uniform;
using namespace std;


class SimplexNoise{
    vector<int> randPerm = {};
    mt19937 generator; //higher quality random gen than rand()
    int detail = 0;
    public:
        SimplexNoise(int detail = 256, int iSeed = 0){
            this->generator = mt19937((unsigned int)iSeed);
            this->detail = detail;
            PopulatePermutations();
        }

        vector<int>& GetPerm(){
            return randPerm;
        }

    void PopulatePermutations(){
        vector<int> range = {};
        range.reserve(detail);
        for(int i = 0; i < detail; i++){
            range.emplace_back(i);
        }
        while(randPerm.size() != detail){
            auto uni = uniform(0, range.size()-1); // higher quality distribution of integers over random number
            int randIndex = uni(generator);
            randPerm.emplace_back(range[randIndex]);
            range.erase(next(range.begin() + (randIndex-1)));
        }
    }

    #pragma region currentIndex

    int CurrentTop(int y, int x, int w, bool last){
        int offset = 0;
        //converting to shadow indexes
        w = w/2;
        y = w*y;

        if(x%2==1){
            last? offset = -w : offset = 0;
            x = x/2+1;
        }
        else{
            offset = w;
            x = x/2;
        }

        return y+x+offset;
    }

    int CurrentLeft(int y, int x, int w, bool last){
        int offset = 0;
        //converting to shadow indexes
        w = w/2;
        y = w*y;

        if(x%2==1){
            offset = w-1;
            x = x/2+1;
        }
        else{
            offset = 0;
            x = x/2;
        }

        return y+x+offset;
    }

    int CurrentRight(int y, int x, int w, bool last){
        int offset = 0;
        //converting to shadow indexes
        w = w/2;
        y = w*y;

        if(x%2==1){
            last? offset = 0 : offset = w;
            x = x/2+1;
        }
        else{
            last? offset = -w+1 : offset = 1;
            x = x/2;
        }

        return y+x+offset;
    }

    #pragma endregion

    int ShadowIndex(int y, int x, int w, int st, int (SimplexNoise::*IndexFunction)(int, int, int, bool), SimplexNoise& instance, bool last){
        int i = ((instance.*IndexFunction)(y,x,w, last));
        float u = ((float)1/st)*((float)(i%st));
        int cVI = i/st;
        int cV = randPerm[cVI%detail];
        int nVI = i/st+1;
        int nV = randPerm[nV%detail];
        int intpol = (int)(u * (float)(nV - cV));

        return cV + intpol;
    }

    int NoiseMapInterrogation(int y, int x, int w,int steps, bool last = false){
        auto left = ShadowIndex(y, x, w, steps, &SimplexNoise::CurrentLeft, *this, x >= w-2);
        auto top = ShadowIndex(y, x, w, steps, &SimplexNoise::CurrentTop, *this, x >= w-2);        
        auto right = ShadowIndex(y, x, w, steps, &SimplexNoise::CurrentRight, *this, x >= w-2);
        return (top + left + right) / 3;
    }
};