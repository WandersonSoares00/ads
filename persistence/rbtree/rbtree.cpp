#include <iostream>
#include "rbnode.hpp"
#include <iterator>
#include <memory>
#include <type_traits>
#include <vector>


class RBTree {
    
    int latest_version;
    std::vector <std::shared_ptr<Node>> heads;

    std::shared_ptr<Node> get_head(int version) {
        for (int i = version; i >= 0; --i){
            if(heads.at(i)) {
                return heads[i];
            }
        }
        return std::shared_ptr<Node> {nullptr};
    }

    void modfy_node(std::shared_ptr<Node> &node, int value) {
        if (latest_version == 0) {
            heads[latest_version] = node;
            return;
        }
        auto temp = node->modfy(value, latest_version);
        
        if (temp) { 
            heads[latest_version+1] = temp;
        }
        else if (!node->get_back()) {
            heads[latest_version+1] = node;
        }
    }

    inline void modfy_left(Node *node, std::shared_ptr<Node> &new_node) {
        auto temp = node->modfy_left(new_node, latest_version);
        if(temp) {
            heads[latest_version+1] = temp;
        }
    }

    inline void modfy_right(Node *node, std::shared_ptr<Node> &new_node) {
        auto temp = node->modfy_right(new_node, latest_version);
        if(temp) {
            heads[latest_version+1] = temp;
        }
    }

    inline void modfy_color(Node *node, char color) {
        auto temp = node->modfy(color, latest_version);
        if(temp) {
            heads[latest_version+1] = temp;
        }
    }


    inline Node *rrotate(Node *y) {
        auto node = std::make_shared<Node>(y);
        Node *x = node->get_left(latest_version).get();
        auto rx = x->get_right(latest_version);
        modfy_right(x, node);
        modfy_left(y, rx);
        return x;
    }

    inline Node *lrotate(Node *y){
        auto node = std::make_shared<Node>(y);
        Node *x = node->get_right(latest_version).get();
        auto lx = x->get_left(latest_version);
        modfy_left(x, node);
        modfy_right(y, lx);
        return x;
    }

    // the path have more than one node
    void fixtree(Node **path, int n) {
        Node *node = path[n];
        Node *nodep = path[n-1];
        int i = n;

        while (nodep != *path and nodep->get_color(latest_version) != 'r') {
            if (nodep == path[i-2]->get_right(latest_version).get()) { // node's parent is a right child
                Node *uncle = path[i-2]->get_left(latest_version).get();
                if (uncle and uncle->get_color(latest_version) == 'r') {
                    modfy_color(uncle, 'b');
                    modfy_color(nodep, 'b');
                    if (path[i-2] != *path) modfy_color(path[i-2], 'r'); // root needs to be black
                } else { // node's uncle is black
                    if (node == nodep->get_left(latest_version).get()) { //right left case
                        rrotate(nodep);
                        std::swap(node, nodep);
                    }
                    lrotate(path[i-2]);
                    modfy_color(nodep, 'b');
                    modfy_color(path[i-2], 'r');
                }
            } else {    // node's parent is a left child
                Node *uncle = path[i-2]->get_right(latest_version).get();
                if (uncle and uncle->get_color(latest_version) == 'r') {
                    modfy_color(uncle, 'b');
                    modfy_color(nodep, 'b');
                    if (path[i-2] != *path) modfy_color(path[i-2], 'r'); // root needs to be black
                } else { // node's uncle is black
                    if (node == nodep->get_right(latest_version).get()) { //left right case
                        lrotate(nodep);
                        std::swap(node, nodep);
                    }
                    rrotate(path[i-2]);
                    modfy_color(nodep, 'b');
                    modfy_color(path[i-2], 'r');
                }
               
            }
            i -= 2;
            node = path[i];
            if (node == *path)    break;
            nodep = path[i-1];
        }
    }

    public:
    RBTree() : latest_version{-1}, heads(100, nullptr) {}


    void insert(int value) {
        
        Node *node = nullptr;
        auto temp = get_head(latest_version);
        
        Node *parents[20];
        int k = -1;

        while(temp){
            node = temp.get();
            parents[++k] = node;
            if(temp->get_value(latest_version) > value){
                temp = temp->get_left(latest_version);
            } else {
                temp = temp->get_right(latest_version);
            }
        }
        

        if(!node) {  // root node
            std::shared_ptr<Node> new_node{ new Node(value, 'b') };
            modfy_node(new_node, value);
            ++latest_version;
        } else {
            std::shared_ptr<Node> new_node{ new Node(value, 'r') };
            
            if (node->get_value(latest_version) > value)
                modfy_left(node, new_node);
            else
                modfy_right(node, new_node);
            
            parents[++k] = new_node.get();

            fixtree(parents, k);
            
            ++latest_version;
        }

    }

    /*
    Node* _insert(Node *head, Node *new_node) {
        if (!head) {
            return new_node;
        }
        
        if (head->get_value(latest_version) > new_node->get_value(latest_version)) {
            Node *tmp = _insert(head->get_left(latest_version).get(), new_node);
            if(tmp)     modfy_left(head, new_node);
        }
        else {
            Node *tmp = _insert(head->get_right(latest_version).get(), new_node);
             if(tmp)    modfy_right(head, new_node);
        }

        return nullptr;
    }
    */

    void print(int version){
        inorder(get_head(version).get(), version);
        std::cout << '\n';
    }

    void inorder(Node *node, int version){
        if(!node){
            return;
        }
        inorder(node->get_left(version).get(), version);
        std::cout << node->get_value(version) << ' ';
        inorder(node->get_right(version).get(), version);
    }

    ~RBTree() {}

};


int main() {
    RBTree tree;
    // 10,85,15,70,20,60,30,50,65,80,90,40,5 and 55    
    tree.insert(10);
    tree.insert(85);
    tree.insert(15);
    tree.insert(70);
    tree.insert(20);
    tree.insert(60);
    tree.insert(30);
    tree.insert(50);
    tree.insert(65);
    tree.insert(80);
    tree.insert(90);
    tree.insert(40);
    tree.insert(5);
    tree.insert(55);
    
    for (int i = 0; i < 15; ++i){
        tree.print(i);
    }

}




