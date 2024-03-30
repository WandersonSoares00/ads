#include <iostream>
#include <vector>

using namespace std;


struct Node {
    int value;
    Node *next;
    
    Node *back;

    enum Field { Value, Next, None };

    struct Change{
        char version;
        Field field;

        int new_value;
        Node *new_next;
    };

    Change changes[2];
    int i;

    Node(int value) {
        changes[0].field = None;
        changes[1].field = None;
        i = 0;
        next = back = nullptr;
        this->value = value;
    }

    public:
    
    int latest_value() {
        if (changes[1].field == Value)
            return changes[1].new_value;
        if (changes->field == Value)
            return changes->new_value;
        return value;
    }

    Node* latest_next() {
        if (changes[1].field == Next)
            return changes[1].new_next;
        if (changes->field == Next)
            return changes->new_next;
        return next;
    }

    void set_latest_next(Node *new_next) {
        if (changes[1].field == Next)
           changes[1].new_next = new_next;
        if (changes->field == Next)
           changes->new_next = new_next;
        else
            next = new_next;
    }

    // vesion > 0
    int get_value(int version){
        int val = value;
        for (int v = 0; v < 2; ++v) {
            if (changes[v].field == Value && changes[v].version <= version)
                val = changes[v].new_value;
        }
        
        return val;
    }

    Node *get_next(int version){
        Node *next_node = next;
    
        for (int v = 0; v < 2; ++v) {
            if (changes[v].field == Next && changes[v].version <= version)
                next_node = changes[v].new_next;
        }
        
        return next_node;
    }

    Node *get_back() {
        return back;
    }

    Node *modfy(int value, int latest_version) {
        if (i < 2) {
            changes[i].field   = Value;
            changes[i].new_value = value;
            changes[i].version   = ++latest_version;
            //next->latest_version=latest_version;
            ++i;
        } else {
            Node *new_node = new Node(latest_value());
            Node *lnext = latest_next();
            new_node->next = lnext;
            new_node->back = this->back;
            if (lnext) {
                lnext->back = new_node;
            }
            new_node->modfy(value, latest_version); // return null
            if (new_node->back){
                return new_node->back->modfy(new_node, latest_version);
            }
            else{
                return new_node;
            }
        }
        return nullptr;
    }

    Node *modfy(Node *next, int latest_version) {
        if (i < 2) {
            changes[i].field  = Next;
            changes[i].new_next = next;
            changes[i].version  = ++latest_version;
            next->back = this;
            //next->latest_version=latest_version;
            ++i;
        } else {
            Node *new_node = new Node(latest_value());
            Node *lnext = latest_next();
            new_node->next = lnext;
            new_node->back = this->back;
            if (lnext) {
                lnext->back = new_node;
            }
            new_node->modfy(next, latest_version);
            if (new_node->back){
                // quem apontava para mim, deve apontar para new_node
                return new_node->back->modfy(new_node, latest_version);
            }
            else {
                return new_node;
            }
        }
        return nullptr;
    }
    
    ~Node() {}

    void _delete() {}
    
};

struct List{
    Node *tail;
    int latest_version;

    //std::map<int, Node*> heads;

    struct head_version{
        Node *head;
        int version;
    };
    
    std::vector <head_version> heads;
    
    
    void reg_head (Node *node) {
        struct head_version head = {node, latest_version};
        heads.push_back(head);
    }

    void modfy_node (Node *node, int value) {
        Node *temp = node->modfy(value, latest_version);
        ++latest_version;
        if (temp) {
            reg_head (temp);
        }
        else if (!node->get_back()){
            reg_head(node);
        }
        if (node == tail){
            temp = get_head(latest_version);
            while(temp){
                tail = temp;
                temp = temp->get_next(latest_version);
            }
        }
    }

    void modfy_node (Node *node, Node *new_node) {
        Node *temp = node->modfy(new_node, latest_version);
        ++latest_version;
        if (temp) {
            reg_head (temp);
        }
        if (node == tail) {
            tail = new_node;
        }
    }

    Node *get_head(int version) {
        
        Node *head_prev {nullptr};

        for (auto & hv : heads) {
            if (hv.version == version){
                return hv.head;
            } else if (hv.version > version) {
                break;
            }
            head_prev = hv.head;
        }

        return head_prev;
    }

    public:

    class Iterator {
        Node *node;
        int v;

        friend List;

        Iterator(Node *n, int version) : node{n}, v{version} {}

        public:
        
        Iterator() : node {nullptr} {}
        
        int operator * (){
            return node->get_value(v);
        }

        void operator = (Iterator i){
            node = i.node;
            v = i.v;
        }

        bool operator == (Iterator i){
            return node == i.node;
        }

        bool operator != (Iterator i){
            return node != i.node;
        }

        Iterator& operator ++ (){
            node = node->get_next(v);
            return *this;
        }

    };
    
    List() : tail{nullptr}, latest_version{0} {}

    
    Iterator end(int version){
        return Iterator(nullptr, version);
    }

    Iterator begin(int version){
        if (version > 0 and version <= latest_version){
            return Iterator(get_head(version), version);
        }
        return end(version);
    }

    void push_back(int value) {
        Node *new_node = new Node(value);
        Node *head = get_head(latest_version);
        //cerr << new_node->get_value(latest_version) << '\n';

        if (!head) {
            tail = new_node;
            ++latest_version;
            reg_head(new_node);
            return;
        }
        
        modfy_node(tail, new_node);
        //++latest_version;
    }

    void insert_after(Iterator i, int value){
        if (!i.node or i.v != latest_version){
            return;
        }

        Node *new_node = new Node(value);
        modfy_node(i.node, new_node);
        //++latest_version;
    }
    
    void replace_value(int position, int value){
        if (position < 0)
            return;

        Node *temp = get_head(latest_version);
        int i = 0;
        
        while(temp) {
            if (i == position){
                modfy_node(temp, value);
                //++latest_version;
                break;
            }
            ++i;
            temp = temp->get_next(latest_version);
        }
    }

    void show(int version){
        //int i = 0;
        if (version > 0 && version <= latest_version) {
            Node *temp = get_head(version);
        
            while(temp){
                std::cout << temp->get_value(version) << " -> ";
                temp = temp->get_next(version);
            }
        }

        std::cout << "null";
    }

};

void show_list(List l, int version){
    List::Iterator it;
    for (it = l.begin(version); it != l.end(version); ++it){
        std::cout << *it << " -> ";
    }

    std::cout << "null";
}

int main(){
    List list;
    
    /*
    for (int i = 10; i < 20; ++i) {
        list.insert(i);
        list.show(i-10);
        std::cout << '\n';
    }
    */

    list.push_back(10);
    list.push_back(20);
    list.push_back(30);
    
    list.replace_value(0, 11);
    list.replace_value(0, 12);
    list.replace_value(1, 21);
    list.replace_value(1, 22);


    for (int i = 0; i < 10; ++i) {
        std::cout << "Versão " << i << ": ";
        show_list(list, i);
        std::cout << '\n';
    }
}

