#include "rbnode.hpp"
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
        return nullptr;
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
    RBTree() : latest_version{0}, heads(100, nullptr) {}

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
        if(node) {
            modfy_node(new_node, value);
        } else if (node->get_value(latest_version) > value) {
            node->modfy_left(new_node, latest_version);
        } else {
            node->modfy_right(new_node, latest_version);
        }

    }

    ~RBTree() {}

};



