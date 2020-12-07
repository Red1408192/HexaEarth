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

        Tile* emplace_back(int iy, int ix, bool first = false){
            tree.emplace_back(iy, ix);
            first? Insert(&tree.back(), &tree.front()) : Insert(&tree.back(), treeRoot);
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
            }
            else{
                this->treeRoot = nH;
            }
            nH->parent = Pivot->parent;
            Pivot->childs[!way?Minor:Major] = nH->childs[way?Minor:Major];
            nH->childs[way?Minor:Major] = Pivot;
            Pivot->parent = nH;

            if(Pivot->childs[!way?Minor:Major] != nullptr){
                Pivot->childs[!way?Minor:Major]->parent = Pivot; //was nH previusly
            }
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
                if(root == block){block->color = Black; treeRoot = block; return;}
                block->NewId();
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
                    Rotate(P, Left);
                    if(block->childs[Minor] == P){
                        Rotate(GP,Right);
                    }
                    else{
                        Rotate(GP, Left);
                    }
                }
                else if(GP->childs[Major] == P && P->childs[Minor] == block){
                    Rotate(P, Right);
                    if(block->childs[Major] == P){
                        Rotate(GP,Right);
                    }
                    else{
                        Rotate(GP, Left);
                    }
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
        Block* treeRoot = {nullptr};

};
