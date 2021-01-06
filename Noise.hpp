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
/*         void PopulatePermutations(){ //Stupid Idea AF
            vector<int> range = {};
            range.reserve(detail);
            for(int i = 0; i < detail; i++){
                range.emplace_back(i);
            }
            while(randPerm.size() != detail){
                int randIndex = rand()%detail;
                emplyToPerm(range, randIndex);
            }
        }

        bool emplyToPerm(vector<int>& range, int randIndex, int sign = 0){
            if(randIndex < 0 || randIndex > detail-1){
                return false;
            }

            if(temp.find(range[randIndex]) == temp.end()){
                randPerm.emplace_back(range[randIndex]);
                temp.insert(range[randIndex]);
                return true;
            }
            else{
                if(sign != 0){
                    if(emplyToPerm(range, randIndex+sign, sign)){
                        return true;
                    }
                    else{
                    return emplyToPerm(range, randIndex-sign, -sign);
                    }
                }
                else{
                    randIndex%2 == 0? sign = 1 : sign = -1;
                    return emplyToPerm(range, randIndex+sign, sign);
                }
            }
        } */
};