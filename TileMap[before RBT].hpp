#pragma once
#include <cstring>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <string>
#include <cxxabi.h>
#include "Native.hpp"

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

class TileBoard : public  MapObject{
    public:
        int len = 0;
        int hei = 0;
        vector<Tile> fullTileList;
        vector<vector<Tile *>> map;

        TileBoard(int dimension){
            Naming(this);
            char errorMessage[50];
            puts("Naming done");

            //Variables initialization
            len = dimension;
            hei = dimension/2;
            fullTileList.reserve(len*hei);
            map.reserve(hei);
            int index = 0;
            puts("Vectors initialization Done");

            //
            //Map creation - Polar Sud Row
            //
            fullTileList.emplace_back(0,0);
            vector<Tile *> sudLine;
            sudLine.reserve(len);
            map.push_back(sudLine);
            map[0].push_back(&fullTileList[index]);
            index++;
            puts("First tile done");

            //first raw
            for (int l = 1; l < len; l++){
                fullTileList.emplace_back(0,l);
                map[0].push_back(&fullTileList[index]);
                index++;

                //linker
                Linker(0,l,3,0,l-1);
                if(l>=len/2){ //might be improvable
                    Linker(0,l,2,0,l-len/2, PolarSud);
                    Linker(0,l-len/2,1,0,l, PolarNord); //We use polarNord only for to use the correct link direction
                    if(l == len-1){
                        Linker(0,l,3,0,0);
                    }
                }
            }
            puts("First Row Linker Done");
            //
            //main tile map
            //
            bool inlac;
            for(int h = 1; h < hei-1; h++){
                inlac = h%2==0;
                vector<Tile *> newLine;
                newLine.reserve(len);
                map.push_back(newLine);

                fullTileList.emplace_back(h, 0);
                map[h].push_back(&fullTileList[index]);
                index++;
                if(!inlac){
                    Linker(h,0,1,h-1,0);
                }
                else{
                    Linker(h,0,2,h-1,0);
                    Linker(h,0,1,h-1,1);
                }
                sprintf(errorMessage, "row %d initialized", h);
                puts(errorMessage);
                for (int l = 1; l < len; l++){
                    fullTileList.emplace_back(h,l);
                    map[h].push_back(&fullTileList[index]);
                    index++;
                    //linker
                    Linker(h,l,3,h,l-1);
                    Linker(h,l,2,h-1,l-1);
                    Linker(h,l,1,h-1,l);
                    //sprintf_s(errorMessage, "colonna %d done", l);
                    //puts(errorMessage);
                }
                //finalize the raw
                Linker(h,len-1,0,h,0);
                if(inlac){
                    Linker(h,len-1,1,h-1,0);
                }
                sprintf(errorMessage, "row %d done", h);
                puts(errorMessage);
            }
            //
            // PolarNord row
            //
            int h = hei-1; //last raw
            vector<Tile *> nordLine;
            nordLine.reserve(len);
            map.push_back(nordLine);

            fullTileList.emplace_back(h, 0);
            map[h].push_back(&fullTileList[index]);
            index++;
            if(inlac){
                Linker(h,0,1,h-1,0);
            }
            else{
                Linker(h,0,2,h-1,0);
                Linker(h,0,1,h-1,1);
            }
            puts("North Pole row initialized");
            for(int pl = 1; pl < len; pl++){
                fullTileList.emplace_back(h,pl);
                map[h].push_back(&fullTileList[index]);
                index++;
                //linker
                Linker(h,pl,3,h,pl-1);
                Linker(h,pl,2,h-1,pl-1);
                Linker(h,pl,1,h-1,pl);
                if(pl >= len/2){
                    Linker(h,pl,4,h,pl-len/2, PolarNord);
                    Linker(h,pl-len/2,5,h,pl, PolarSud); //We use PolarSud only for to use the correct link direction
                    if(pl == len-1){
                        Linker(h, pl, 3, h, 0);
                    }
                }
            }
            sprintf(errorMessage, "Last row done", h);
            puts(errorMessage);
        }

    void Linker(int fy, int fx, int dir, int ty, int tx, LinkerParam lp = Normal){
        dir >= 6 ? 0 : dir;
        if(lp == PolarSud){
            map[fy][fx]->border[dir] = map[ty][tx];
            map[ty][tx]->border[dir-1] = map[fy][fx];
        }
        else if(lp == PolarNord){
            map[fy][fx]->border[dir] = map[ty][tx];
            map[ty][tx]->border[dir+1] = map[fy][fx];
        }
        else{
            map[fy][fx]->border[dir] = map[ty][tx];
            map[ty][tx]->border[dir + 3 == 6 ? 0 : dir] = map[fy][fx];
        }
    }
};

