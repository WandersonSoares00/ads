#ifndef NODE_HPP
#define NODE_HPP

#include <iostream>
#include <sstream>

namespace RBNode {
    constexpr bool RED = true;
    constexpr bool BLACK = false;
    constexpr int MAX_MODS = 2;

    class Node {
        int content;
        bool color;
        Node *left;
        Node *right;
        Node *back;
        
        enum Field { Content, Left, Right, Color, None };
        
        struct Change {
            Field field;
            char  version;
            int   new_content;
            bool  new_color;
            Node *new_child;
        };
       

        int i;
        Change changes[MAX_MODS];

        Node *latest_left() {
            for (int y = MAX_MODS-1; y >= 0; --y){
                if(changes[y].field == Left)    
                    return changes[y].new_child;
            }
            
            return left;
        }

        Node *latest_right() {
            for (int y = MAX_MODS-1; y >= 0; --y){
                if(changes[y].field == Right)    
                    return changes[y].new_child;
            }

            return right;
        }

        int latest_content() {
            for (int y = MAX_MODS-1; y >= 0; --y){
                if(changes[y].field == Content)
                    return changes[y].new_content;
            }
            
            return content;
        }

        bool latest_color() {
            for (int y = MAX_MODS-1; y >= 0; --y){
                if(changes[y].field == Color)
                    return changes[y].new_color;
            }
            
            return color;
        }
       
        public:

        bool is_left_child(){
            if (!back)
                return false;

            if (back->left == this)
                return true;
        
            for (int v = 0; v < MAX_MODS; ++v){
                if (back->changes[v].field == Left and back->changes[v].new_child == this)
                    return true;
            }

            return false;
        }


        Node(int content, bool color) : content{content}, color{color}, left{nullptr}, right{nullptr}, back{nullptr}, i{0}, new_splitted{this}
        {
            for (int y = 0; y < MAX_MODS; ++y){
                changes[y].field = None;
            }
        }

        Node *get_back() {
            return back;
        }

        void set_back(Node *node) {
            back = node;
        }

        bool is_red(int version) {
            return get_color(version) == RED;
        }

        int get_value(int version) {
            int value = content;
            for (int v = 0; v < MAX_MODS; ++v){
                if (changes[v].field == Content and changes[v].version <= version){
                    value = changes[v].new_content;
                }
            }
            
            return value;
        }

        bool get_color(int version) {
            bool vcolor = color;
            for (int v = 0; v < MAX_MODS; ++v){
                if (changes[v].field == Color and changes[v].version <= version){
                    vcolor = changes[v].new_color;
                }
            }
            
            return vcolor;
        }

        Node *get_left(int version) {
            Node *left_node = left;

            for (int v = 0; v < MAX_MODS; ++v){
                if (changes[v].field == Left and changes[v].version <= version){
                    left_node = changes[v].new_child;
                }
            }

            return left_node;
        }
       
        Node *get_right(int version) {
            Node* right_node = right;

            for (int v = 0; v < MAX_MODS; ++v){
                if (changes[v].field == Right and changes[v].version <= version){
                    right_node = changes[v].new_child;
                }
            }
            
            return right_node;
        }
        
        Node *modfy_helper(Node *new_node, int latest_version) {
            Node *lleft = latest_left();
            Node *lright = latest_right();
            new_node->left  = lleft;
            new_node->right = lright;
            new_node->back  = this->back;
            // update back pointers of child nodes to the latest version
            if(lleft and lleft->back == this){
                lleft->back = new_node;
            }
            if(lright and lright->back == this){
                lright->back = new_node;
            }

            if (new_node->back){
                // update pointer of the node that points to this node
                if (is_left_child()){
                    return new_node->back->modfy_left(new_node, latest_version);
                }
                else {
                     return new_node->back->modfy_right(new_node, latest_version);
                }
           } else {
                // new root in version latest_version
                return new_node;
            }
           
        }
        
         Node *modfy(int content, int latest_version) {
            if (i < MAX_MODS){
                changes[i].field = Content;
                changes[i].new_content = content;
                changes[i].version = latest_version;
                new_splitted = this;
                ++i;
            } else {
                // create a new node with all modifications of fields updated
                Node *new_node {new Node(latest_content(), latest_color())};
                new_splitted = new_node;
                // apply new modifications, always returns nullnew_child
                new_node->modfy(content, latest_version);
                return modfy_helper(new_node, latest_version);
            }

            return nullptr;
        }

         Node *modfy(bool color, int latest_version) {
            if (i < MAX_MODS){
                changes[i].field = Color;
                changes[i].new_color = color;
                changes[i].version = latest_version;
                new_splitted = this;
                ++i;
            } else {
                // create a new node with all modifications of fields updated
                Node *new_node {new Node(latest_content(), latest_color())};
                new_splitted = new_node;
                // apply new modifications, always returns nullnew_child
                new_node->modfy(color, latest_version);
                return modfy_helper(new_node, latest_version);
            }

            return nullptr;
        }



        Node *modfy_left(Node *left_node, int latest_version) {
            if (i < MAX_MODS){
                changes[i].field = Left;
                changes[i].new_child = left_node;
                if (left_node)
                    left_node->back = this;
                changes[i].version = latest_version;
                new_splitted = this;
                ++i;
            } else {
                // create a new node with all modifications of fields updated
                Node *new_node {new Node(latest_content(), latest_color())};
                new_splitted = new_node;
                // apply new modifications, always returns nullnew_child
                new_node->modfy_left(left_node, latest_version);
                return modfy_helper(new_node, latest_version);
            }

            return nullptr;
        }

        Node *modfy_right(Node *right_node, int latest_version) {
            if (i < MAX_MODS){
                changes[i].field = Right;
                changes[i].new_child = right_node;
                if (right_node)
                    right_node->back = this;
                changes[i].version = latest_version;
                new_splitted = this;
                ++i;
            } else {
                // create a new node with all modifications of fields updated
                Node *new_node {new Node(latest_content(), latest_color())};
                new_splitted = new_node;
                // apply new modifications, always returns nullnew_child
                new_node->modfy_right(right_node, latest_version);
                return modfy_helper(new_node, latest_version);
            }

            return nullptr;
        }
        
        ~Node() {
        }

        public:
        Node *new_splitted;
    };
}

#endif



