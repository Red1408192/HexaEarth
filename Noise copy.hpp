#pragma once
#include <stdlib.h>
#include <set>
#include <memory>
#include <numeric>
#include <map>
#include <random>
#include <tuple>
#include <cmath>
#include <algorithm>
typedef struct{int y; int x;} inmap;
typedef uniform_int_distribution<int> uniform;
using namespace std;

struct NoiseParameter{
    int w; //map width
    float simplexWidth = 1.; //single triangle width
    float vOffset = 0; // vertical offset
    float hOffset = 0; // horizontal offset
    float slope = 6; // fader function's slope parameter
};

class RedNoise{
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
            auto uni = uniform(0, range.size()-1); // better than modulo
            int randIndex = uni(generator);
            randPerm.emplace_back(range[randIndex]);
            range.erase(next(range.begin() + (randIndex-1)));
        }
    }

    #pragma region triagle index

    int CurrentTop(int y, int x, int w, bool last){
        //converting to shadow indexes
        if(x<0) {x=w+x;last=x>w-2;}; 
        int offset = 0;
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
        //converting to shadow indexes
        if(x<0) {x=w+x;last=x>w-2;}; 
        int offset = 0;
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
        //converting to shadow indexes
        if(x<0) {x=w+x;last=x>w-2;}; 
        int offset = 0;
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
    #pragma region gradient and dotproduct

    float DotProduct(int index, float y, float x){
        //y,x coordinate are the distance from the point and the
        int hash = randPerm[index%detail];
        switch (hash&0b111)
        {
        case 0: return -x;
        case 1: return -y;
        case 2: return x;
        case 3: return y;
        case 4: return -x-y;
        case 5: return x+y;
        case 6: return -x+y;
        case 7: return x-y;
        default: return 0.;
        };
    }

    float lerp(float a, float b, float f){
        return a + f * (b - a);
    }


    float SimplexProduct(float y, float x, float o, int w){
        //y,x coordinate are from the interrogated point
        float r = 0.866025f*o;
        float s = o/2;
        int b = x/s;
        int a = y/r;
        int sl = w/s;

        float xi = x-(s*b);
        float yi = y-(r*a);

        float t = 0;
        float q = 0;
        float p = 0;

        float f = 0.2;

        if((a%2==0)==(b%2==0)){
            if(yi>=xi*r/s){         //0
                float ty = yi;
                float tx = xi;
                float qy = r-yi;
                float qx = s-xi;
                float py = r-yi;
                float px = xi+s;
                //t = DotProduct(CurrentTop(a, b-1, w, (a>sl-2)), yi, xi);
                t = sqrt((ty*ty)+(tx*tx))/o;
                //q = DotProduct(CurrentLeft(a, b-1, w, (a>sl-2)), yi, xi);
                q = sqrt((qy*qy)+(qx*qx))/o;
                //p = DotProduct(CurrentRight(a, b-1, w, (a>sl-2)), yi, xi);
                p = sqrt((py*py)+(px*px))/o;
            }
            else{                   //1
                float ty = r-yi;
                float tx = s-xi;
                float qy = yi;
                float qx = o-xi;
                float py = yi;
                float px = xi;
                //t = DotProduct(CurrentTop(a, b, w, (a>sl-2)), yi, xi);
                t = sqrt((ty*ty)+(tx*tx))/o;
                //q = DotProduct(CurrentLeft(a,b, w, (a>sl-2)), yi, xi);
                q = sqrt((qy*qy)+(qx*qx))/o;
                //p = DotProduct(CurrentRight(a,b, w, (a>sl-2)), yi, xi);
                p = sqrt((py*py)+(px*px))/o;
            }
        }
        else{
            if(yi>r-xi*(r/s)){      //2
                float ty = yi;
                float tx = s-xi;
                float qy = r-yi;
                float qx = o-xi;
                float py = r-yi;
                float px = xi;
                //t = DotProduct(CurrentTop(a, b, w, (a>sl-2)), yi, xi);
                t = sqrt((ty*ty)+(tx*tx))/o;
                //q = DotProduct(CurrentLeft(a, b, w, (a>sl-2)), yi, xi);
                q = sqrt((qy*qy)+(qx*qx))/o;
                //p = DotProduct(CurrentRight(a,b, w, (a>sl-2)), yi, xi);
                p = sqrt((py*py)+(px*px))/o;
            }
            else{                   //3
                float ty = r-yi;
                float tx = xi;
                float qy = yi;
                float qx = s-xi;
                float py = yi;
                float px = xi+s;
                //t = DotProduct(CurrentTop(a, b-1, w, (a>sl-2)), yi, xi);
                t = sqrt((ty*ty)+(tx*tx))/o;
                //q = DotProduct(CurrentLeft(a, b-1, w, (a>sl-2)), yi, xi);
                q = sqrt((qy*qy)+(qx*qx))/o;
                //p = DotProduct(CurrentRight(a, b-1, w, (a>sl-2)), yi, xi);
                p = sqrt((py*py)+(px*px))/o;
            }
        }
        return Fade(t,f)+Fade(p,f)+Fade(q,f);
    }
    #pragma endregion
    
    void printTestIndexesMap(){
        for (int y = 23; y>-1; y--){
            for (int x = 0; x<24; x++){
                auto p = CurrentLeft(y,x,24,(x>22));
                cout << p << '\t';
            }
            cout<<'\n';
        }
        cout<<'\n';
        cout<<'\n';
        for (int y = 23; y>-1; y--){
            for (int x = 0; x<24; x++){
                auto p = CurrentRight(y,x,24,(x>21));
                cout << p << '\t';
            }
            cout<<'\n';
        }
        cout<<'\n';
        cout<<'\n';
        for (int y = 23; y>-1; y--){
            for (int x = 0; x<24; x++){
                auto p = CurrentTop(y,x,24,(x>21));
                cout << p << '\t';
            }
            cout<<'\n';
        }

    }

    float Fade(float x, float slope){
        if (x==0) return 0;
        float d = 1/(exp(slope/2)-1);
        return (1.0+d*2.0)/(1+exp(-slope*(x-1./2.)))-d;
    }

    float NoiseMapInterrogation(int y, int x, NoiseParameter p){
        float t = SimplexProduct(p.vOffset+y,p.hOffset+x, p.simplexWidth, p.w);
        return Fade(t, p.slope);
    }
};