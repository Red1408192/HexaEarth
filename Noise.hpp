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


    int CurrentTop(int y, int x, int w){
        //converting to shadow indexes
        if(x<0) {x=w+x;};
        bool last=x>=w-2;
        int offset = 0;
        w = w/2;
        y = w*y;

        if(x%2==1){
            last? offset = -w : offset = 0;
            x = x/2+1;
        }
        else{
            offset = w;
            x = (x/2);
        }

        return y+x+offset;
    }

    int CurrentLeft(int y, int x, int w){
        //converting to shadow indexes
        if(x<0) {x=w+x;};
        bool last=x>=w-2;
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

    int CurrentRight(int y, int x, int w){
        //converting to shadow indexes
        if(x<0) {x=w+x;};
        bool last=x>=w-2;
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

    float DotProduct(int index, float r, float dy, float dx){
        //y,x coordinate are the distance from the point and the
        int hash = randPerm[index%detail];
        float gy, gx = 0;
        switch (hash&0b111)
        {
            case 0: gy = 0; gx = r;
            case 1: gy = r*sin(0.706858f); gx = r*cos(0.706858f);
            case 2: gy = r; gx = 0;
            case 3: gy = (r*sin(0.706858f)); gx = -(r*cos(0.706858f));
            case 4: gy = 0; gx = -r;
            case 5: gy = -(r*sin(0.706858f)); gx = -(r*cos(0.706858f));
            case 6: gy = -r; gx = 0;
            case 7: gy = -(r*sin(0.706858f)); gx = (r*cos(0.706858f));
            default: break;
        }
        float upper = dy*gy + dx*gx;
        float bottom = sqrt((gy*gy)+(gx*gx))*sqrt((dy*dy)+(dx*dx));
        return acos(upper/bottom);
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
        float ty,tx,qy,qx,py,px = 0;
        int G = 0;
        if((a%2==0)==(b%2==0)){
            if(yi>=xi*r/s){         //0
                ty = yi;
                tx = xi;

                qy = r-yi;
                qx = s-xi;

                py = r-yi;
                px = xi+s;

                G = b/2;
            }
            else{                   //1
                ty = r-yi;
                tx = s-xi;

                qy = yi;
                qx = o-xi;

                py = yi;
                px = xi;

                G = (b/2)-1;
            }
        }
        else{
            if(yi>r-xi*(r/s)){      //2
                ty = yi;
                tx = s-xi;

                qy = r-yi;
                qx = o-xi;

                py = r-yi;
                px = xi;

                G = b/2;
            }
            else{                   //3
                ty = r-yi;
                tx = xi;

                qy = yi;
                qx = s-xi;

                py = yi;
                px = xi+s;

                G = (b/2)-1;
            }
        }
        auto cT = CurrentTop(a, G, sl);
        t = DotProduct(cT, o, ty, tx); //CurrentTop(a, G, w, (b>sl-2)
        //t = sqrt((ty*ty)+(tx*tx))/o;
        auto cQ = CurrentLeft(a, G, sl);
        q = DotProduct(cQ ,o, qy, qy);
        //q = sqrt((qy*qy)+(qx*qx))/o;
        auto cP = CurrentRight(a, G, sl);
        p = DotProduct(cP , o, py, px);
        //p = sqrt((py*py)+(px*px))/o;
        return t;
    }
    #pragma endregion
    
    void printTestIndexesMap(){
        for (int y = 23; y>-1; y--){
            for (int x = 0; x<24; x++){
                auto p = CurrentLeft(y,x,24);
                cout << p << '\t';
            }
            cout<<'\n';
        }
        cout<<'\n';
        cout<<'\n';
        for (int y = 23; y>-1; y--){
            for (int x = 0; x<24; x++){
                auto p = CurrentRight(y,x,24);
                cout << p << '\t';
            }
            cout<<'\n';
        }
        cout<<'\n';
        cout<<'\n';
        for (int y = 23; y>-1; y--){
            for (int x = 0; x<24; x++){
                auto p = CurrentTop(y,x,24);
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