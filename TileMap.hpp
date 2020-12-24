#pragma once
#include <cstring>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <string>
#include <cxxabi.h>
#include "MapObject.hpp"
#include "RedBlackTree.hpp"
typedef vector<Tile *> Row;
typedef vector<Row> SGTilesList;
using namespace std;
using namespace abi;

class Map{
    public:
        SGTilesList * map;
        void Linker(int fy, int fx, int dir, int ty, int tx, LinkerParam lp = Normal){
            dir = dir%6;
            if(lp == PolarSud){
                this->map->at(fy)[fx]->border[dir] = map->at(ty)[tx];
                this->map->at(ty)[tx]->border[dir-1] = map->at(fy)[fx];
            }
            else if(lp == PolarNord){
                this->map->at(fy)[fx]->border[dir] = map->at(ty)[tx];
                this->map->at(ty)[tx]->border[dir+1] = map->at(fy)[fx];
            }
            else{
                this->map->at(fy)[fx]->border[dir] = this->map->at(ty)[tx];
                this->map->at(ty)[tx]->border[dir + 3 == 6 ? 0 : dir] = this->map->at(fy)[fx];
            }
        }
    
};

class TileRowCreator : public Map{
    Tree tileList;
    int w;
    int dimension;

    int curRow;
    int curPos;

    bool inlac;
    public:
        TileRowCreator(int inW, int inDimension, int treeDimension, SGTilesList* context) : tileList(treeDimension){
            this->w = inW;
            this->dimension = inDimension;
            this->map = context;
        }
        void newRow(int row, bool inlac){
            this->inlac = inlac;
            this->curPos = 1;
            this->curRow = row;

            this->map->operator[](curRow).push_back(tileList.emplace_back(curRow, 0));
            if(!inlac){
                Map::Linker(curRow,0,1,curRow-1,0);
            }
            else{
                Map::Linker(curRow,0,2,curRow-1,0);
                Map::Linker(curRow,0,1,curRow-1,1);
            }
            emplaceSection(curPos+(w));
        }

        void emplaceSection(int target){
            for(int l = curPos; l < target; l++){
                curPos++;
                this->map->operator[](curRow).push_back(tileList.emplace_back(curRow,l));
                //linker
                Map::Linker(curRow,l,3,curRow,l-1);
                Map::Linker(curRow,l,2,curRow-1,l-1);
                Map::Linker(curRow,l,1,curRow-1,l);
                //sprintf_s(errorMessage, "colonna %d done", l);
                //puts(errorMessage);
                if(curPos == dimension){
                    //finalize the raw
                    Map::Linker(curRow,dimension-1,0,curRow,0);
                    if(inlac){
                        Linker(curRow,dimension-1,1,curRow-1,0);
                    }
                }
            }
        }
};


class TileBoard : public  MapObject, public Map{

    public:
        int len = 0;
        int hei = 0;
        Tree fullTileList;

        TileBoard(int dimension) : fullTileList(dimension*(dimension/2)){
            Naming(this);
            char errorMessage[50];
            puts("Naming done");
            //Variables initialization
            len = dimension;
            hei = dimension/2;
            map = new SGTilesList();
            map->reserve(hei);
            puts("Vectors initialization Done");
        }
};

