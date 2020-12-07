#pragma once
#include <cstring>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <string>
#include <cxxabi.h>
#include "MapObject.h"
#include "RedBlackTree.h"

using namespace std;
using namespace abi;

class TileBoard : public  MapObject{
    public:
        int len = 0;
        int hei = 0;
        Tree fullTileList;
        vector<vector<Tile *>> map;

        TileBoard(int dimension) : fullTileList(dimension*(dimension/2)){
            Naming(this);
            char errorMessage[50];
            puts("Naming done");
            //Variables initialization
            len = dimension;
            hei = dimension/2;
            map.reserve(hei);
            puts("Vectors initialization Done");

            //
            //Map creation - Polar Sud Row
            //
            vector<Tile *> sudLine;
            sudLine.reserve(len);
            map.push_back(sudLine);
            map[0].push_back(fullTileList.first_emplace_back(0,0));
            puts("First tile done");

            //first raw
            for (int l = 1; l < len; l++){
                map[0].push_back(fullTileList.emplace_back(0,l));

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

                map[h].push_back(fullTileList.emplace_back(h, 0));
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
                    map[h].push_back(fullTileList.emplace_back(h,l));
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

            map[h].push_back(fullTileList.emplace_back(h, 0));
            if(inlac){
                Linker(h,0,1,h-1,0);
            }
            else{
                Linker(h,0,2,h-1,0);
                Linker(h,0,1,h-1,1);
            }
            puts("North Pole row initialized");
            for(int pl = 1; pl < len; pl++){
                
                map[h].push_back(fullTileList.emplace_back(h,pl));
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

