#include "TileMap.h"
#include "Native.h"
#include <vector>
#include <bitset>

struct Block{
    int id;
    Color color = Red;
    Tile * tile;

    Block* childs[2] = {nullptr, nullptr};
    Block* parent = nullptr;
    public:
        Block(int iy, int ix){
            tile = new Tile(iy, ix);
            id = tile->id;
        }

        Tile* GetTilePtr(){
            return tile;
        }

        void NewId(){
            tile->id = rand();
            id = tile->id;
        }
};

class Tree{
    vector<Block> tree;
    public:
        Tree(int dimension){
            tree.reserve(dimension);
        }

        Tile* emplace_back(int iy, int ix){
            Insert(&tree.emplace_back(iy, ix), &tree.front());
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
            return p->childs[Minor]==s? p->childs[1] : p->childs[Major];
        }

        Block* GetUncle(Block* n){
            Block* p = GetParent(n);
            if (p == nullptr){ return nullptr; };

            return GetSibling(p);
        }

        void Rotate(Block* Pivot, Way way){
            Block* nH = Pivot->childs[!way?Left:Right];

            if (Pivot->parent != nullptr){
                if(Pivot->parent->childs[Left] == Pivot){
                    Pivot->parent->childs[Left] = nH;
                }
                else if(Pivot->parent->childs[Right] == Pivot){
                    Pivot->parent->childs[Right] = nH;
                }
                nH->parent = Pivot->parent;
            }

            Pivot->childs[!way?Left:Right] = nH->childs[way?Left:Right];
            if(Pivot->childs[!way?Left:Right] == nullptr){
                Pivot->childs[!way?Left:Right]->parent = Pivot; //was nH previusly
            }

            nH->childs[way?Left:Right] = Pivot;
            Pivot->parent = nH;            
        }

        void Insert(Block* block, Block* root){
            if(block->id < root->id){
                if(root->childs[Left] != nullptr){
                    Insert(block, root->childs[Left]);
                }
                root->childs[Left] = block;
                block->parent = root;
                if(root->color != Black){
                CheckStability(block);
                }
                return;
            }
            else if(block->id > root->id){
                if(root->childs[Right] != nullptr){
                    Insert(block, root->childs[Right]);
                }
                root->childs[Right] = block;
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
                if(GP->childs[Left] == P && P->childs[Right] == block){
                    Rotate(block, Left);
                }
                else if(GP->childs[Right] == P && P->childs[Left] == block){
                    Rotate(block, Right);
                }
            
                if(P == block->childs[Left]){
                    Rotate(GP,Right);
                }
                else{
                    Rotate(GP, Left);
                }
                block->color = Black;
                GP->color = Red;
            }
        }

};
