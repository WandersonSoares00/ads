#include <iostream>
#include "rbnode.hpp"
#include <limits>
#include <vector>
#include <queue>
#include <sstream>
#include <unordered_map>


using namespace RBNode;

class RBTree {
    
    int latest_version;
    std::vector <Node*> heads;

    Node *get_head(int version) {
        int i = version;
        if (version > latest_version)   i = latest_version;
        for (; i >= 0; --i){
            if(heads[i]) {
                return heads[i];
            }
        }
        return nullptr;
    }

    inline void modfy_value(Node *&node, int value) {
        Node* temp = node->modfy(value, latest_version);
        node = node->new_splitted;

        if (temp) {
            heads[latest_version] = temp;
        }
    }

    inline void modfy_left(Node *&node, Node *new_node, bool root=true) {
        Node *temp = node->modfy_left(new_node, latest_version);
        node = node->new_splitted;

        if(root and temp) {
            heads[latest_version] = temp;
        }
    }

    inline void modfy_right(Node *&node, Node *new_node, bool root=true) {
        Node *temp = node->modfy_right(new_node, latest_version);
        node = node->new_splitted;
        
        if(root and temp) {
            heads[latest_version] = temp;
        }
    }

    inline void modfy_color(Node *&node, bool color, bool root=true) {
        Node *temp = node->modfy(color, latest_version);
        node = node->new_splitted;
        
        if(root and temp) {
            heads[latest_version] = temp;
        }
    }


    void rrotate(Node *&y) {
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

    void lrotate(Node *&y){
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

    void fixInsert(Node *new_node) {
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
                        nodep = nodep->get_back();
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
                        nodep = nodep->get_back();
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
    
    // v is the node that was deleted and u is the child that replaces v
    void fixDelete(Node *v, Node *u) {
        auto vcolor = v->get_color(latest_version);
        auto ucolor = u ? u->get_color(latest_version) : BLACK;

        if (ucolor == RED and vcolor == BLACK) {
            modfy_color(u, BLACK);
            return;
        }
        if (vcolor == RED) {
            return;
        }

        // now, both u and v are black
        Node *s;
        u = v;
        Node *uparent = u->get_back();
        while (ucolor == BLACK and uparent) {
            if (u->is_left_child()) {
                s = uparent->get_right(latest_version);
                if (s->is_red(latest_version)) {
                    lrotate(uparent);
                    modfy_color(uparent, RED);
                    s = uparent->get_back();
                    modfy_color(s, BLACK);
                }
                else { // s is black
                    Node *rs = s->get_right(latest_version);
                    Node *ls = s->get_left(latest_version);
                    auto rscolor = rs ? rs->get_color(latest_version) : BLACK;
                    auto lscolor = ls ? ls->get_color(latest_version) : BLACK;

                    if (rscolor == BLACK and lscolor == BLACK) {
                        modfy_color(s, RED);
                        u = s->get_back();
                        if (u->is_red(latest_version)) {
                            modfy_color(u, BLACK);
                            break;
                        }
                    }
                    else {
                        if (rscolor == BLACK){ //right left case
                            rrotate(s);
                            modfy_color(s, RED);
                            ls = s->get_back();
                            modfy_color(ls, BLACK);
                            s = ls;
                            uparent = s->get_back();
                        }
                        lrotate(uparent);
                        s = uparent->get_back();
                        modfy_color(s, uparent->get_color(latest_version));
                        rs = s->get_right(latest_version);
                        modfy_color(rs, BLACK);
                        if (uparent->is_red(latest_version))   modfy_color(uparent, BLACK);
                        break;
                    }
                }
            }
            else {
                s = uparent->get_left(latest_version);
                if (s->is_red(latest_version)) {
                    rrotate(uparent);
                    modfy_color(uparent, RED);
                    s = uparent->get_back();
                    modfy_color(s, BLACK);
                }
                else { // s is black
                    Node *rs = s->get_right(latest_version);
                    Node *ls = s->get_left(latest_version);
                    auto rscolor = rs ? rs->get_color(latest_version) : BLACK;
                    auto lscolor = ls ? ls->get_color(latest_version) : BLACK;

                    if (rscolor == BLACK and lscolor == BLACK) {
                        modfy_color(s, RED);
                        u = s->get_back();
                         if (u->is_red(latest_version)) {
                            modfy_color(u, BLACK);
                            break;
                        }
                    }
                    else {
                        if (lscolor == BLACK){ //left right case
                            lrotate(s);
                            modfy_color(s, RED);
                            rs = s->get_back();
                            modfy_color(rs, BLACK);
                            s = rs;
                            uparent = s->get_back();
                        }
                        rrotate(uparent);
                        s = uparent->get_back();
                        modfy_color(s, uparent->get_color(latest_version));
                        ls = s->get_left(latest_version);
                        modfy_color(ls, BLACK);
                        if (uparent->is_red(latest_version))   modfy_color(uparent, BLACK);
                        break;
                    }
                }
            }
            ucolor = u->get_color(latest_version);
            uparent = u->get_back();
        }

        if (!(u->get_back()) and u->is_red(latest_version))     modfy_color(u, BLACK);
    }

    public:
    RBTree() : latest_version{0}, heads(100, nullptr) {}

    bool search(int value, int version, Node *&node) {
        Node *temp = get_head(version);
        bool founded { false };
        Node *snode { nullptr };

        while(temp){
            node = temp;
            if(temp->get_value(version) > value){
                temp = temp->get_left(version);
            }
            else if (temp->get_value(version) < value) {
                temp = temp->get_right(version);
            }
            else {
                founded = true;
                snode = temp;
                temp = temp->get_right(version);
            }
        }
        
        if (founded)    node = snode;

        return founded;
    }

    void insert(int value) {
        
        Node *parent {nullptr};

        search (value, latest_version, parent);
        
        if(!parent) {  // root node
            Node *new_node{ new Node(value, BLACK) };
            ++latest_version;
            heads[latest_version] = new_node;
        } else {
            Node *new_node{ new Node(value, RED) };
            ++latest_version;
            
            if (parent->get_value(latest_version) < value)
                modfy_right(parent, new_node);
            else
                modfy_left(parent, new_node);

            fixInsert(new_node);
        }
    }
    
    int successor(int value, int version) {
        Node *node = successor(get_head(version), value, version);
        if (!node)
            return std::numeric_limits<int>::lowest();
        if (node->get_value(version) == value)
            return std::numeric_limits<int>::max(); // or infinity();
        return node->get_value(version);
    }

    Node *successor(Node *root, int value, int version) {
        if (!root)
            return nullptr;

        int root_value = root->get_value(version);
        Node *suc;

        if (root_value == value)
            suc = successor(root->get_right(version), value, version);
        else if (root_value > value)
            suc = successor(root->get_left(version), value, version);
        else
            return successor(root->get_right(version), value, version);

        if (!suc)   return root;
        if (suc->get_value(version) == value)   return root;
        return suc;
    }

    Node *minValue(Node *node, int version) {
        Node *suc { node };
        Node *tmp { suc };
        while (tmp) {
            suc = tmp;
            tmp = suc->get_left(version);
        }
        return suc;
    }

    void remove(int value) {
        Node *node { nullptr };
        if (!search (value, latest_version, node))
            return;

        Node *parent = node->get_back();        
        Node *rnode = node->get_right(latest_version);
        Node *lnode = node->get_left(latest_version);
        ++latest_version;

        if (!lnode) {
            if (!parent)                    heads[latest_version] = rnode;
            else if(node->is_left_child())  modfy_left(parent, rnode);
            else                            modfy_right(parent, rnode);

            if (rnode)      rnode->set_back(parent);
            fixDelete(node, rnode);
        }
        else if (!rnode) {
            if (!parent)                    heads[latest_version] = lnode;
            else if(node->is_left_child())  modfy_left(parent, lnode);
            else                            modfy_right(parent, lnode);

            lnode->set_back(parent);
            fixDelete(node, lnode);
        }
        else if (lnode and rnode) {
            Node *suc = minValue(node->get_right(latest_version), latest_version);
            modfy_value(node, suc->get_value(latest_version));

            Node *rsuc = suc->get_right(latest_version);
            
            fixDelete(suc, rsuc);

            Node *bsuc = suc->get_back();
            if (suc->is_left_child())
                modfy_left(bsuc, rsuc);
            else
                modfy_right(bsuc, rsuc);
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
            std::cout << "\033[1;31m " << node->get_value(v) <<"\033[0m\n";
        else
            std::cout << node->get_value(v) << '\n';
                
        printtree(node->get_left(v), v, h);
    }

    std::string inorder(int version){
        Node *node = get_head(version);
        std::ostringstream out;
        _inorder(node, version, 0, out);
        return out.str();
    }

    void _inorder(Node *node, int version, int h, std::ostringstream &str){
        if(!node)
            return;
        _inorder(node->get_left(version), version, h + 1, str);
        if (node->is_red(version))
            str << node->get_value(version) << ',' << h << ',' << "R" << ' ';
        else
            str << node->get_value(version) << ',' << h << ',' << "N" << ' ';
        _inorder(node->get_right(version), version, h + 1, str);
    }

    ~RBTree() {

        std::queue <Node*> queue;
        std::unordered_map<Node*,bool> visited;
        
        Node *node, *lnode, *rnode;
        visited[nullptr] = true;

        for (int v = 1; v < latest_version; ++v) {
            node = get_head(v);
            if (node) {
                queue.push(node);
                visited[node] = true;
            }
            while (!queue.empty()) {
                node = queue.front();
                queue.pop();
                
                lnode = node->get_left(v);
                rnode = node->get_right(v);
                if (!visited[lnode]) {
                    queue.push(lnode);
                    visited[lnode] = true;
                }
                if (!visited[rnode]) {
                    queue.push(rnode);
                    visited[rnode] = true;
                }
            }
        }
        
        std::unordered_map<Node*, bool>::iterator it;
        for (it = visited.begin(); it != visited.end(); ++it) {
            delete it->first;
        }
    }

};

