#include <iostream>
#include "rbnode.hpp"
#include <iterator>
#include <memory>
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
            ++latest_version;
            heads[latest_version] = node;
            return;
        }
        auto temp = node->modfy(value, latest_version);
        ++latest_version;
        
        if (temp) { 
            heads[latest_version] = temp;
        }
        else if (!node->get_back()) {
            heads[latest_version] = node;
        }
    }

    void modfy_left(Node *node, std::shared_ptr<Node> &new_node) {
        auto temp = node->modfy_left(new_node, latest_version);
        ++latest_version;

        if(temp) {
            heads[latest_version] = temp;
        }
    }

    void modfy_right(Node *node, std::shared_ptr<Node> &new_node) {
        auto temp = node->modfy_right(new_node, latest_version);
        ++latest_version;

        if(temp) {
            heads[latest_version] = temp;
        }
    }


    public:
    RBTree() : latest_version{-1}, heads(100, nullptr) {}


    void insert(int value) {
        std::shared_ptr<Node> node;
        auto temp = get_head(latest_version);
        while(temp){
            node = temp;
            if(temp->get_value(latest_version) > value){
                temp = temp->get_left(latest_version);
            } else {
                temp = temp->get_right(latest_version);
            }
        }
        
        std::shared_ptr<Node> new_node{ new Node(value) };
        if(!node) {
            modfy_node(new_node, value);
        } else if (node->get_value(latest_version) > value) {
            modfy_left(node.get(), new_node);
        } else {
            modfy_right(node.get(), new_node);
        }

    }

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
    
    tree.insert(20);
    tree.insert(10);
    tree.insert(30);
    tree.insert(25);
    tree.insert(35);
    
    for (int i = 0; i < 10; ++i){
        tree.print(i);
    }

}




