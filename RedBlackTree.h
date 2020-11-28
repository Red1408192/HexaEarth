#include "TileMap.h"
#include "Native.h"
#include <vector>
#include <bitset>

struct Block{
    char id[9];
    Color color = Red;
    Tile tile;

    Block* childs[2] = {nullptr, nullptr};
    Block* parent = nullptr;
};

class Tree{
    vector<Block> tree;
    public:
        Tree(int dimension){
        tree.reserve(dimension);
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

        void Rotate(Way way){
            
        }
};
