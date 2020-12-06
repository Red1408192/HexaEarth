#pragma once
#include <cstring>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <string>
#include <cxxabi.h>
#include "Native.h"
using namespace std;
using namespace abi;

class MapObject{
    public:
        char name [20];
        int id = 0;
        MapObject(){
            Naming(this);
        }
        template <typename T> 
        void Naming(T object){
            object->id = (const int)rand();
            char hexa[12] = "0x";
            int* status = 0;
            strcpy(name, __cxa_demangle(typeid(object).name(), 0, 0, status));
            strcat(object->name, " - ");
            strcat(object->name, To_hex(object->id, hexa));
        }
};

class Tile : public MapObject{
    Position * position;

    //Mass
    int land{};
    int water{};

    public:
        Tile *border[6];
        Tile(int iy, int ix){
            Naming(this);
            position = new Position(iy, ix);
        }
        int GetX(){
            return this->position->x;
        }
        int GetY(){
            return this->position->y;
        }
};

class Plate : public MapObject{
    public:
        Plate(){
            Naming(this);
        }
    Cardinals direction;
    int size;
    vector<Tile*> tiles;
};

class Block{
    public:
        int id;
        int index;
        Color color = Red;
        Tile tile;

        Block* childs[2] = {nullptr, nullptr};
        Block* parent = nullptr;

        Block(int iy, int ix) : tile(iy, ix) {
            id = tile.id;
        }

        Tile* GetTilePtr(){
            return &tile;
        }

        void NewId(){
            tile.id = rand();
            id = tile.id;
        }
};