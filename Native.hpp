#pragma once
#include <cstring>
#include <bit>


char* To_hex(int intToHex, char* str = new char[9], int count = 0){
    const char heximals[17] = "0123456789ABCDEF";
    int result = (int)intToHex/16;
    int remainder = (int)intToHex%16;
    count += 1;
    if (result >= 1 or count<8){
        return strncat(To_hex(result, str, count), &heximals[remainder], 1);
    }
    strncat(str, &heximals[remainder], 1);
    return str;
}

class Position{
    public:
        int y;
        int x;
        Position(int iy, int ix){
            y = iy,x = ix;
        }
};

enum Cardinals{
    e = 0,
    sE = 60,
    sW = 120,
    w = 180,
    nW = 240,
    nE = 300 
};

enum Color : bool{
    Red = true,
    Black = false
};

enum LinkerParam{
    PolarNord = 1,
    PolarSud = 2,
    Normal = 0
};

enum Way : bool{
    Left = true,
    Right = false
};

enum Sibling : int8_t{
    Minor = 0,
    Major = 1
};

int MapInterpolation(int left, int right, int up){
    return (((left+right)/2)+up)/2;
}