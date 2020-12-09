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

        Tile* first_emplace_back(int iy, int ix){
            tree.emplace_back(iy, ix);
            Insert(&tree.back(), &tree.front());
            tree.back().index = tree.size()-1;
            return tree.back().GetTilePtr();
        }

        Tile* emplace_back(int iy, int ix){
            tree.emplace_back(iy, ix);
            Insert(&tree.back(), treeRoot);
            CheckStability(&tree.back());
            tree.back().index = tree.size()-1;
            return tree.back().GetTilePtr();
        }

        Block* GetParent(Block* c){
            return c == nullptr? nullptr : c->parent == nullptr ? nullptr : c->parent;
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
                else{
                    Pivot->parent->childs[Major] = nH;
                }
            }
            else{
                this->treeRoot = nH;
            }
            if(nH->childs[way?Minor:Major] != nullptr){
                nH->childs[way?Minor:Major]->parent = Pivot;
            }
            Pivot->childs[!way?Minor:Major] = nH->childs[way?Minor:Major];
            nH->childs[way?Minor:Major] = Pivot;
            nH->parent = Pivot->parent;
            Pivot->parent = nH;
        }

        void Insert(Block* block, Block* root){
            if(block->id < root->id){
                if(root->childs[Minor] != nullptr){
                    Insert(block, root->childs[Minor]);
                }
                else{
                    root->childs[Minor] = block;
                    block->parent = root;
                }
                return;
            }
            else if(block->id > root->id){
                if(root->childs[Major] != nullptr){
                    Insert(block, root->childs[Major]);
                }
                else{
                    root->childs[Major] = block;
                    block->parent = root;
                }
                return;
            }
            else if(block->id = root->id){
                if(root == block){block->color = Black; treeRoot = block; return;}
                block->NewId();
                Insert(block, treeRoot);
            }
        }

        void CheckStability(Block* block){
            Block* GP = GetGrandParent(block);
            Block* U = GetUncle(block);
            Block* P = GetParent(block);
            if(P == nullptr){
                block->color = Black;
            }
            else if(P->color == Black){
                return;
            }
            else if(U != nullptr && U->color == Red){
                U->color = Black;
                block->parent->color = Black;
                GP->color = Red;
                CheckStability(GP);
            }
            else{
                if(GP->childs[Minor] == P && P->childs[Major] == block){
                    Rotate(P, Left);
                    block = P;
                    P = block->parent;
                }
                else if(GP->childs[Major] == P && P->childs[Minor] == block){
                    Rotate(P, Right);
                    block = P;
                    P = block->parent;
                }

                if(P->childs[Minor] == block){
                    Rotate(GP,Right);
                }
                else{
                    Rotate(GP, Left);
                }
                P->color = Black;
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
