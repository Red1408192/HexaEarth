#pragma once
#include <stdlib.h>
#include <set>
#include <memory>
#include <numeric>
#include <random>
#include <tuple>
#include <algorithm>
typedef tuple<int, int> inmap;
typedef uniform_int_distribution<int> uniform;
using namespace std;


class RedNoise{ // I don't what this is, but it's definetevly not a simplex noise
    vector<int> randPerm = {};
    mt19937 generator; //higher quality random gen than rand()
    int detail = 0;
    public:
        RedNoise(int detail = 256, int iSeed = 0){
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

    #pragma region shadow index

    inmap CurrentTop(int y, int x, bool last){
        //converting to shadow indexes the triangle top/bottom angle

        if(x%2==1){
            x = x/2+1;
            last? y-- : y;
        }
        else{
            x = x/2;
            y++;            
        }

        return inmap(y,x);
    }

    inmap CurrentLeft(int y, int x, bool last){
        //converting to shadow indexes the triangle left angle

        if(x%2==1){
            x = x/2+1;
            y++, x--;            
        }
        else{
            x = x/2;
        }

        return inmap(y,x);
    }

    inmap CurrentRight(int y, int x, bool last){
        //converting to shadow indexes the triangle right angle

        if(x%2==1){
            x = x/2+1;
            last? y : y++;            
        }
        else{
            x = x/2;
            last? y--, x++ : x++;            
        }

        return inmap(y,x);
    }

    int Nearhood(inmap pos,int w, int st, int deepNees, float gravity){
        if (deepNees == 0){
            return Evaluate(pos, w, st);
        }
        deepNees--;

        int y = get<0>(pos);
        int x = get<1>(pos);

        int cur = Evaluate(pos, w, st);
        int near = 0;

        if(y == 0){
            near = (
            Nearhood(inmap(y++, w-1), w, st, deepNees, gravity)+
            Nearhood(inmap(y, w-1), w, st, deepNees, gravity)+
            Nearhood(inmap(y--, x), w, st, deepNees, gravity)+
            Nearhood(inmap(y++, x), w, st, deepNees, gravity)+
            Nearhood(inmap(y, x++), w, st, deepNees, gravity)+
            Nearhood(inmap(y--, x++), w, st, deepNees, gravity))/6;
        }
        else if(x == w-1){
            near = (
            Nearhood(inmap(y++, x--), w, st, deepNees, gravity)+
            Nearhood(inmap(y, x--), w, st, deepNees, gravity)+
            Nearhood(inmap(y--, x--), w, st, deepNees, gravity)+
            Nearhood(inmap(y++, 0), w, st, deepNees, gravity)+
            Nearhood(inmap(y, 0), w, st, deepNees, gravity)+
            Nearhood(inmap(y--, 0), w, st, deepNees, gravity))/6;
        }
        else{
            near = (
            Nearhood(inmap(y++, x--), w, st, deepNees, gravity)+
            Nearhood(inmap(y, x--), w, st, deepNees, gravity)+
            Nearhood(inmap(y--, x), w, st, deepNees, gravity)+
            Nearhood(inmap(y++, x), w, st, deepNees, gravity)+
            Nearhood(inmap(y, x++), w, st, deepNees, gravity)+
            Nearhood(inmap(y--, x++), w, st, deepNees, gravity))/6;
        }
        return cur + (near-cur)*gravity;
    }

    #pragma endregion

    int Evaluate(inmap pos, int w, int st){
        int i = get<0>(pos)*w + get<1>(pos);

        float u = ((float)1/st)*((float)(i%st));
        int cVI = i<0? detail-(i/st%detail) : i/st%detail;
        int nVI = i<0? detail-(i/st%detail)+1 : i/st%detail+1;
        int cV = randPerm[cVI];
        int nV = randPerm[nVI];


        int intpol = (int)(u * (float)(nV - cV));
        return cV + intpol;
    }

    int ShadowIndex(int y, int x, int w, int st, inmap (RedNoise::*IndexFunction)(int, int, bool), RedNoise& instance, bool last, int deepNees, float gravity){
        inmap im = ((instance.*IndexFunction)(y,x, last));
        return Nearhood(im, w/2, st, deepNees, gravity);
    }

    int NoiseMapInterrogation(int y, int x, int w,int steps, int deepNees, float gravity){
        auto left = ShadowIndex(y, x, w, steps, &RedNoise::CurrentLeft, *this, x >= w-2, deepNees, gravity);
        auto top = ShadowIndex(y, x, w, steps, &RedNoise::CurrentTop, *this, x >= w-2, deepNees, gravity);        
        auto right = ShadowIndex(y, x, w, steps, &RedNoise::CurrentRight, *this, x >= w-2, deepNees, gravity);
        return (top + left + right) / 3;
    }
};