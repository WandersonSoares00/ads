#include <iostream>
#include "rbnode.hpp"
#include <vector>


using namespace RBNode;

class RBTree {
    
    int latest_version;
    std::vector <Node*> heads;

    Node *get_head(int version) {
        for (int i = version; i >= 0; --i){
            if(heads.at(i)) {
                return heads[i];
            }
        }
        return nullptr;
    }

    void modfy_node(Node *&node, int value) {
        Node* temp = node->modfy(value, latest_version);
        //node = node->new_splitted;

        if (temp) { 
            heads[latest_version] = temp;
        }
        else if (!node->get_back()) {
            heads[latest_version] = node;
        }
        //++latest_version;
    }

    inline void modfy_left(Node *&node, Node *new_node, bool root=true) {
        Node *temp = node->modfy_left(new_node, latest_version);
        node = node->new_splitted;

        if(root and temp) {
            heads[latest_version] = temp;
        }
        //++latest_version;
    }

    inline void modfy_right(Node *&node, Node *new_node, bool root=true) {
        Node *temp = node->modfy_right(new_node, latest_version);
        node = node->new_splitted;
        
        if(root and temp) {
            heads[latest_version] = temp;
        }
        //++latest_version;
    }

    inline void modfy_color(Node *&node, bool color, bool root=true) {
        Node *temp = node->modfy(color, latest_version);
        node = node->new_splitted;
        
        if(root and temp) {
            heads[latest_version] = temp;
        }
        //++latest_version;
    }


    inline void rrotate(Node *&y) {
        Node *x = y->get_left(latest_version);
        Node *rx = x->get_right(latest_version);
    /*      y                   x
           / \                 / \
          x   ry   ---->      lx  y
         / \                     / \
        lx  rx                 rx   ry
    */
        bool left = y->is_left_child();
        Node *yparent = y->get_back();
        x->set_back(nullptr);
        y->set_back(nullptr);
        modfy_left(y, rx, !yparent);
        modfy_right(x, y, !yparent);
        x->set_back(yparent);
        y->set_back(x);

        if (yparent) {
            if (left)   modfy_left(yparent, x);
            else        modfy_right(yparent, x);
            x->set_back(yparent);
        }
    }

    inline void lrotate(Node *&y){
        Node *x = y->get_right(latest_version);
        Node *lx = x->get_left(latest_version);
    /*      y                   x
           / \                 / \
         ly   x   ---->       y  rx
             / \             / \
            lx rx           ly  lx
    */
        bool left = y->is_left_child();
        Node *yparent = y->get_back();
        x->set_back(nullptr);
        y->set_back(nullptr);
        modfy_right(y, lx, !yparent);
        modfy_left(x, y, !yparent);
        x->set_back(yparent);
        y->set_back(x);

        if (yparent) {
            if (left)   modfy_left(yparent, x);
            else        modfy_right(yparent, x);
            x->set_back(yparent);
        }
    }

    void fixtree(Node *new_node) {
        Node *node = new_node;
        Node *nodep = node->get_back();
        Node* nodegp { nullptr };

        while (nodep and nodep->is_red(latest_version)) {
            nodegp = nodep->get_back();
            if (nodep == nodegp->get_right(latest_version)) { // node's parent is a right child
                Node *uncle = nodegp->get_left(latest_version);
                if (uncle and uncle->is_red(latest_version)) {
                    modfy_color(uncle, BLACK);
                    modfy_color(nodep, BLACK);
                    nodegp = nodep->get_back();
                    if (nodegp->get_back()) modfy_color(nodegp, RED); // root needs to be black
                } else { // node's uncle is black
                    if (node == nodep->get_left(latest_version)) { //right left case
                        rrotate(nodep);
                        nodep = node;
                        nodegp = nodep->get_back();
                    }
                    lrotate(nodegp);
                    modfy_color(nodegp, RED);
                    nodep = nodegp->get_back();
                    modfy_color(nodep, BLACK);
                }
            } else {    // node's parent is a left child
                Node *uncle = nodegp->get_right(latest_version);
                if (uncle and uncle->is_red(latest_version)) {
                    modfy_color(uncle, BLACK);
                    modfy_color(nodep, BLACK);
                    nodegp = nodep->get_back();
                    if (nodegp->get_back()) modfy_color(nodegp, RED); // root needs to be black
                } else { // node's uncle is black
                    if (node == nodep->get_right(latest_version)) { //left right case
                        lrotate(nodep);
                        nodep = node;
                        nodegp = nodep->get_back();
                    }
                    rrotate(nodegp);
                    modfy_color(nodegp, RED);
                    nodep = nodegp->get_back();
                    modfy_color(nodep, BLACK);
                }
            }
            node  = nodegp;
            nodep = node ? node->get_back() : nullptr;
        }
    }

    public:
    RBTree() : latest_version{0}, heads(100, nullptr) {}


    void insert(int value) {
        
        Node *node {nullptr};
        Node *temp = get_head(latest_version);

        while(temp){
            node = temp;
            if(temp->get_value(latest_version) > value){
                temp = temp->get_left(latest_version);
            } else {
                temp = temp->get_right(latest_version);
            }
        }
        
        if(!node) {  // root node
            Node *new_node{ new Node(value, BLACK) };
            ++latest_version;
            heads[latest_version] = new_node;
        } else {
            Node *new_node{ new Node(value, RED) };
            ++latest_version;
            
            if (node->get_value(latest_version) > value)
                modfy_left(node, new_node);
            else
                modfy_right(node, new_node);

            fixtree(new_node);
            
        }

    }

    void printtree(int version) {
        printtree(get_head(version), version, 0);
    }

    void printtree(Node *node, int v, int h) {
        if (!node)      return;

        h += 5;
        printtree(node->get_right(v), v, h);

        std::cout << '\n';
        for (int i = 5; i < h; ++i)     std::cout << ' ';
        
        if (node->is_red(v))
            std::cout << "\033[1;31m " << node->vdebug(v) <<"\033[0m\n";
        else
            std::cout << node->vdebug(v) << '\n';
                
        printtree(node->get_left(v), v, h);
    }

    void print(int version){
        Node *node = get_head(version);
        inorder(node, version);
        std::cout << '\n';
    }

    void inorder(Node *node, int version){
        if(!node)
            return;
        inorder(node->get_left(version), version);
        std::cout << node->get_value(version) << ' ';
        inorder(node->get_right(version), version);
    }

    ~RBTree() {
    }

};


int main() {
    RBTree tree;
    int nodes[14] = {10,85,90,70,20,60,30,50,65,80,90,40,5,55};
    
    for (int i = 0; i < 14; ++i){
        tree.insert(nodes[i]);
        std::cout << "---------------------------------------------------\n";
        tree.print(i+1);
    }

}


