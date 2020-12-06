#pragma once
#include "TileMap.h"
#include "MapObject.h"
#include "Native.h"
#include <vector>
#include <bitset>
using namespace std;

class Tree{
    public:
        Tree(int dimension){
            tree.reserve(dimension);
        }

        Tile* emplace_back(int iy, int ix){
            tree.emplace_back(iy, ix);
            Insert(&tree.back(), &tree.front());
            tree.back().index = tree.size()-1;
            return tree.back().GetTilePtr();
        }

        Block* GetParent(Block* c){
            return c->parent == nullptr ? nullptr : c->parent;
        }

        Block* GetGrandParent(Block* gC){
            return GetParent(GetParent(gC));
        }

        Block* GetSibling(Block* s){
            Block* p = GetParent(s);
            if (p == nullptr){ return nullptr; };
            return p->childs[Minor]==s? p->childs[Major] : p->childs[Minor];
        }

        Block* GetUncle(Block* n){
            Block* p = GetParent(n);
            if (p == nullptr){ return nullptr; };

            return GetSibling(p);
        }

        void Rotate(Block* Pivot, Way way){
            Block* nH = Pivot->childs[!way?Minor:Major];

            if (Pivot->parent != nullptr){
                if(Pivot->parent->childs[Minor] == Pivot){
                    Pivot->parent->childs[Minor] = nH;
                }
                else if(Pivot->parent->childs[Major] == Pivot){
                    Pivot->parent->childs[Major] = nH;
                }
                nH->parent = Pivot->parent;
            }

            Pivot->childs[!way?Minor:Major] = nH->childs[way?Minor:Major];
            if(Pivot->childs[!way?Minor:Major] == nullptr){
                Pivot->childs[!way?Minor:Major]->parent = Pivot; //was nH previusly
            }

            nH->childs[way?Minor:Major] = Pivot;
            Pivot->parent = nH;
        }

        void Insert(Block* block, Block* root){
            if(block->id < root->id){
                if(root->childs[Minor] != nullptr){
                    Insert(block, root->childs[Minor]);
                }
                root->childs[Minor] = block;
                block->parent = root;
                if(root->color != Black){
                CheckStability(block);
                }
                return;
            }
            else if(block->id > root->id){
                if(root->childs[Major] != nullptr){
                    Insert(block, root->childs[Major]);
                }
                root->childs[Major] = block;
                block->parent = root;
                if(root->color != Black){
                CheckStability(block);
                }
                return;
            }
            else if(block->id = root->id){
                if(block == root){block->color = Black; return;}
                Insert(block, &tree.front());
            }
        }

        void CheckStability(Block* block){
            Block* GP = GetGrandParent(block);
            Block* U = GetUncle(block);
            Block* P = GetParent(block);
            
            if(U != nullptr && U->color == Red){
                U->color = Black;
                block->parent->color = Black;
                if(GP != &tree.front()){
                    GP->color = Red;
                    if(GP->parent != nullptr && GP->parent->color != Black){
                        CheckStability(GP);
                    }
                }
            }
            else{
                if(GP->childs[Minor] == P && P->childs[Major] == block){
                    Rotate(block, Left);
                }
                else if(GP->childs[Major] == P && P->childs[Minor] == block){
                    Rotate(block, Right);
                }
            
                if(P == block->childs[Minor]){
                    Rotate(GP,Left);
                }
                else{
                    Rotate(GP, Right);
                }
                block->color = Black;
                GP->color = Red;
            }
        }

        int GetIndex(int tileId, Block* currentBlock = nullptr){
            if(currentBlock->id == tileId){
                return currentBlock->index;
            }
            else if(currentBlock->id < tileId){
                currentBlock->childs[Minor] == nullptr? throw : GetIndex(tileId, currentBlock->childs[Minor]);
            }
            else if(currentBlock->id > tileId){
                currentBlock->childs[Major] == nullptr? throw : GetIndex(tileId, currentBlock->childs[Major]);
            }
            else{
                GetIndex(tileId, &tree[0]);
            }
        }

        Tile operator [](int index){
            return tree[index].tile;
        }

        Block GetBlock(int index){
            return tree[index];
        }
    private:
        vector<Block> tree;

};
