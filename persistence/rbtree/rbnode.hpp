#ifndef NODE_HPP
#define NODE_HPP

#include <memory>
#include <iostream>

class Node {

    int content;
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;

    std::shared_ptr<Node> back;
    
    enum Field { Content, Left, Right, None };

    struct Change {
        Field field;
        char  version;
        int new_content;
        std::shared_ptr<Node> new_left;
        std::shared_ptr<Node> new_right;
    };
    
    int i;
    Change changes[2];

    std::shared_ptr<Node> &latest_left() {
        if(changes[1].field == Left)    
            return changes[1].new_left;
        if (changes->field == Left)
            return changes->new_left;
        else
            return left;
    }

    std::shared_ptr<Node> &latest_right() {
        if(changes[1].field == Right)    
            return changes[1].new_right;
        if (changes->field == Right)
            return changes->new_right;
        else
            return right;
    }

    int latest_content() {
        if(changes[1].field == Content)
            return changes[1].new_content;
        if(changes->field == Content)
            return changes->new_content;
        else
            return content;
    }
    
    // precondition: The node is a child
    bool is_left_child(){
        if (left.get() == this)
            return true;

        for (int v = 0; v < 2; ++v){
            if (back->changes->new_left.get() == this)
                return true;
        }

        return false;
    }

    public:

    Node(int content) : content{content}, left{nullptr}, right{nullptr},
                        back{nullptr}, i{0}
    {
        changes[0].field = None;
        changes[1].field = None;
    }

    std::shared_ptr<Node> get_back() {
        return back;
    }

    int get_value(int version) {
        int value = content;
        for (int v = 0; v < 2; ++v){
            if (changes[v].field == Content and changes[v].version <= version){
            value = changes[v].new_content;
            }
        }
        
        return value;
    }

    std::shared_ptr<Node> get_left(int version) {
        auto left_node = left;

        for (int v = 0; v < 2; ++v){
            if (changes[v].field == Left and changes[v].version <= version){
                left_node = changes[v].new_left;
            }
        }
        
        return left_node;
    }
   
    std::shared_ptr<Node> get_right(int version) {
        auto right_node = right;

        for (int v = 0; v < 2; ++v){
            if (changes[v].field == Right and changes[v].version <= version){
                right_node = changes[v].new_right;
            }
        }
        
        return right_node;
    }
    

     std::shared_ptr<Node> modfy(int content, int latest_version) {
        if (i < 2){
            changes[i].field = Content;
            changes[i].new_content = content;
            changes[i].version = latest_version+1;
            ++i;
        } else {
            // create a new node with all modifications of fields updated, and all back pointers copied
            std::shared_ptr<Node> new_node {new Node(latest_content())};
            auto lleft = latest_left();
            auto lright = latest_right();
            new_node->left = lleft;
            new_node->right = lright;
            new_node-> back = this->back;
            // update back pointers of child nodes to the latest version
            if(lleft){
                lleft->back = new_node; // copy
            }
            if(lright){
                lright->back = new_node;
            }
            // apply new modifications, always returns nullptr
            new_node->modfy(content, latest_version);

            if (new_node->back){
                // update pointer of the node that points to this node
                if (new_node->is_left_child())
                    return new_node->back->modfy_left(new_node, latest_version);
                else
                    return new_node->back->modfy_right(new_node, latest_version);
            } else {
                // new root in version latest_version
                return new_node;
            }

        }

        return nullptr;
    }

    std::shared_ptr<Node> modfy_left(std::shared_ptr<Node> &left_node, int latest_version) {
        if (i < 2){
            changes[i].field = Left;
            changes[i].new_left = left_node;
            //left_node->back = this;
            changes[i].version = latest_version+1;
            ++i;
        } else {
            // create a new node with all modifications of fields updated, and all back pointers copied
            std::shared_ptr<Node> new_node {new Node(latest_content())};
            auto lleft = latest_left();
            auto lright = latest_right();
            new_node->left = lleft;
            new_node->right = lright;
            new_node-> back = this->back;
            // update back pointers of child nodes to the latest version
            if(lleft){
                lleft->back = new_node; // copy
            }
            if(lright){
                lright->back = new_node;
            }
            // apply new modifications, always returns nullptr
            new_node->modfy_left(left_node, latest_version);
            if (new_node->back){
                // update pointer of the node that points to this node
                if (new_node->is_left_child())
                    return new_node->back->modfy_left(new_node, latest_version);
                else
                    return new_node->back->modfy_right(new_node, latest_version);
            } else {
                // new root in version latest_version
                return new_node;
            }

        }

        return nullptr;
    }

    std::shared_ptr<Node> modfy_right(std::shared_ptr<Node> &right_node, int latest_version) {
        if (i < 2){
            changes[i].field = Right;
            changes[i].new_right = right_node;
            changes[i].version = latest_version+1;
            ++i;
        } else {
            // create a new node with all modifications of fields updated, and all back pointers copied
            std::shared_ptr<Node> new_node {new Node(latest_content())};
            auto lleft = latest_left();
            auto lright = latest_right();
            new_node->left = lleft;
            new_node->right = lright;
            new_node-> back = this->back;
            // update back pointers of child nodes to the latest version
            if(lleft){
                lleft->back = new_node; // copy
            }
            if(lright){
                lright->back = new_node;
            }
            // apply new modifications, always returns nullptr
            new_node->modfy_right(right_node, latest_version);
            if (new_node->back){
                // update pointer of the node that points to this node
                if (new_node->is_left_child())
                    return new_node->back->modfy_left(new_node, latest_version);
                else
                    return new_node->back->modfy_right(new_node, latest_version);
            } else {
                // new root in version latest_version
                return new_node;
            }

        }

        return nullptr;
    }

};


#endif




