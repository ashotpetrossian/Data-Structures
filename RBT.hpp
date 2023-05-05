#include <iostream>
#include <queue>

namespace myDS {

template <typename T>
class RBT {
    enum class COLOR { RED, BLACK };
    struct Node {
        T m_val;
        COLOR m_color;
        Node* left;
        Node* right;
        Node* parent;

        Node(T val): m_val(val), m_color{COLOR::RED}, left{nullptr}, right{nullptr}, parent{nullptr} { } 
    };

public:
    RBT() {
        Tnil = new Node(T{});
        Tnil->m_color = COLOR::BLACK;
        root = Tnil;
    }

    ~RBT() {
        cleaner();
    }

    void printPreorder() {
        printPreorderHelper(root);
        std::cout << std::endl;
    }

    void printInorder() {
        printInorderHelper(root);
        std::cout << std::endl;
    }

    void printPostorder() {
        printPostorderHelper(root);
        std::cout << std::endl;
    }

    void printLevelOrder() {
        printLevelOrderHelper(root);
    }

    bool search(T val) {
        return searchHelper(root, val);
    }

    void insert(T val) {
        insertHelper(val);
    }

    void remove(T val) {
        removeHelper(val);
    }

private:
    void printPreorderHelper(Node* node) {
        if (node == Tnil) return;

        printPreorderHelper(node->left);
        printPreorderHelper(node->right);

        std::string m_color = (node->m_color == COLOR::BLACK) ? "(B)" : "(R)"; 
        std::cout << node->m_val << "_" << m_color << "  ";
    }

    void printInorderHelper(Node* node) {
        if (node == Tnil) return;

        printInorderHelper(node->left);

        std::string m_color = (node->m_color == COLOR::BLACK) ? "(B)" : "(R)"; 
        std::cout << node->m_val << "_" << m_color << "  ";

        printInorderHelper(node->right);
    }

    void printPostorderHelper(Node* node) {
        if (node == Tnil) return;

        std::string m_color = (node->m_color == COLOR::BLACK) ? "(B)" : "(R)"; 
        std::cout << node->m_val << "_" << m_color << "  ";

        printPostorderHelper(node->left);
        printPostorderHelper(node->right);
    }

    void printLevelOrderHelper(Node* node) {
        if (node == Tnil) return;

        std::queue<Node*> q;
        q.push(node);

        while (!q.empty()) {
            int size = q.size();
            while (size--) {
                Node* tmp = q.front();
                q.pop();

                if (tmp->left != Tnil) q.push(tmp->left);
                if (tmp->right != Tnil) q.push(tmp->right);

                std::string m_color = (tmp->m_color == COLOR::BLACK) ? "(B)" : "(R)"; 
                std::cout << tmp->m_val << "_" << m_color << "  ";
            }
            std::cout << std::endl;
        }
    }

    bool searchHelper(Node* node, T val) {
        if (node == Tnil) return false;

        if (node->m_val == val) return true;
        if (val > node->m_val) return searchHelper(node->right, val);
        if (val < node->m_val) return searchHelper(node->left, val);

        return false;
    }

    Node* getMin(Node* node) {
        while (node && node->left && node->left != Tnil) node = node->left;
        return node;
    }

    Node* getMax(Node* node) {
        while (node && node->right && node->right != Tnil) node = node->right;
        return node;
    }

    // v takes u's place, connections fix only parents.
    void transplant(Node* u, Node* v) {
        if (u->parent == Tnil) {
            root = v;
        } else if (u == u->parent->left) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }
        v->parent = u->parent;
    }

    void leftRotate(Node* y) {
        Node* x = y->right;
        y->right = x->left;

        if (x->left != Tnil) {
            x->left->parent = y;
        }
        x->parent = y->parent;
        if (y->parent == Tnil) {
            root = x;
        } else if (y == y->parent->left) {
            y->parent->left = x;
        } else {
            y->parent->right = x;
        }
        x->left = y;
        y->parent = x;
    }

    void rightRotate(Node* y) {
        Node* x = y->left;
        y->left = x->right;

        if (x->right != Tnil) {
            x->right->parent = y;
        }

        x->parent = y->parent;
        if (y->parent == Tnil) {
            root = x;
        } else if (y == y->parent->left) {
            y->parent->left = x;
        } else  {
            y->parent->right = x;
        }

        x->right = y;
        y->parent = x;
    }

    void insertHelper(T val) {
        Node* z = new Node(val);
        z->left = z->right = z->parent = Tnil;
        z->m_color = COLOR::RED;

        Node* y = Tnil;
        Node* x = root;

        while (x != Tnil) {
            y = x;
            if (z->m_val < x->m_val) {
                x = x->left;
            } else {
                x = x->right;
            }
        }

        z->parent = y;
        if (y == Tnil) { // if z has no parent, than z is the root
            z->m_color = COLOR::BLACK; // root should always be black
            root = z;
            return;
        } else if (z->m_val < y->m_val) {
            y->left = z;
        } else {
            y->right = z;
        }

        // if z's grandparent is Tnil than z id the root's child, nothing should be fixed then
        if (z->parent->parent == Tnil) return;

        insertFixUp(z);
    }

    void insertFixUp(Node* z) {
        while (z->parent->m_color == COLOR::RED) {
            if (z->parent == z->parent->parent->left) {
                Node* y = z->parent->parent->right; // setting uncle
                if (y->m_color == COLOR::RED) {
                    z->parent->m_color = COLOR::BLACK;
                    y->m_color = COLOR::BLACK;
                    z->parent->parent->m_color = COLOR::RED;
                    z = z->parent->parent;
                } else { 
                    if (z == z->parent->right) {
                        z = z->parent;
                        leftRotate(z);
                    }
                    
                    z->parent->m_color = COLOR::BLACK;
                    z->parent->parent->m_color = COLOR::RED;
                    rightRotate(z->parent->parent);
                }
            } else { // mirror version
                Node* y = z->parent->parent->left;
                if (y->m_color == COLOR::RED) {
                    z->parent->m_color = COLOR::BLACK;
                    y->m_color = COLOR::BLACK;
                    z->parent->parent->m_color = COLOR::RED;
                    z = z->parent->parent;
                } else { 
                    if (z == z->parent->left) {
                        z = z->parent;
                        rightRotate(z);
                    }

                    z->parent->m_color = COLOR::BLACK;
                    z->parent->parent->m_color = COLOR::RED;
                    leftRotate(z->parent->parent);
                }
            }

            if (z == root) break;
        }

        root->m_color = COLOR::BLACK;
    }

    void removeHelper(T val) {
        Node* z = Tnil;
        Node* node = root;

        while (node != Tnil) {
            if (node->m_val == val) {
                z = node;
                break;
            } else if (node->m_val > val) {
                node = node->left;
            } else {
                node = node->right;
            }
        }

        if (z == Tnil) {
            std::cout << "WARNING: no key: " << val << " was found in the tree" << std::endl;
        }

        Node* y = z;
        Node* x = Tnil;
        COLOR originalColor = y->m_color;

        if (z->left == Tnil) {
            x = z->right;
            transplant(z, z->right);
        } else if (z->right == Tnil) {
            x = z->right;
            transplant(z, z->left);
        } else {
            y = getMin(z->right);
            originalColor = y->m_color;

            x = y->right;

            if (y->parent == z) {
                x->parent = y;
            } else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }

            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->m_color = z->m_color;

            z = nullptr;

            if (originalColor == COLOR::BLACK) {
                removeFixUp(x);
            }
        }
    }

    void removeFixUp(Node* x) {
        Node* w = Tnil; // declaring uncle

        while (x != root && x->m_color == COLOR::BLACK) {
            if (x == x->parent->left) {
                w = x->parent->right;
                if (w->m_color == COLOR::RED) { // case 1
                    w->m_color = COLOR::BLACK;
                    x->parent->m_color = COLOR::RED;
                    leftRotate(x->parent);
                    w = x->parent->right;
                }
        
                if (w->left->m_color == COLOR::BLACK && w->right->m_color == COLOR::BLACK) { // case 2
                    w->m_color = COLOR::RED;
                    x = x->parent;
                } else {
                    if (w->right->m_color == COLOR::BLACK) { // case 3
                        w->left->m_color = COLOR::BLACK;
                        w->m_color = COLOR::RED;
                        rightRotate(w);
                        w = x->parent->right;
                    }

                    w->m_color = x->parent->m_color; // case 4
                    x->parent->m_color = COLOR::BLACK;
                    w->right->m_color = COLOR::BLACK;
                    leftRotate(x->parent);
                    x = root; // nothing more to be done
                }
            } else { // mirror version (x == x->parent->right)
                w = x->parent->left;
                if (w->m_color == COLOR::RED) {
                    w->m_color = COLOR::BLACK;
                    x->parent->m_color = COLOR::RED;
                    rightRotate(x->parent);
                    w = x->parent->left;
                }

                if (w->right->m_color == COLOR::BLACK && w->left->m_color == COLOR::BLACK) {
                    w->m_color = COLOR::RED;
                    x = x->parent;
                } else {
                    if (w->left->m_color == COLOR::BLACK) {
                        w->right->m_color = COLOR::BLACK;
                        w->m_color = COLOR::RED;
                        leftRotate(w);
                        w = x->parent->left;
                    }

                    w->m_color = x->parent->m_color;
                    x->parent->m_color = COLOR::BLACK;
                    w->left->m_color = COLOR::BLACK;
                    rightRotate(x->parent);
                    x = root;
                }
            }
        }

        x->m_color = COLOR::BLACK;
    }

    void cleaner() {
        if (root == Tnil) return;

        std::queue<Node*> q;
        q.push(root);

        while (!q.empty()) {
            Node* tmp = q.front();
            q.pop();
            if (tmp->left != Tnil) q.push(tmp->left);
            if (tmp->right != Tnil) q.push(tmp->right);
            delete tmp;
            tmp = nullptr;
        }

        delete Tnil;
        Tnil = nullptr;
    }




private:
    Node* root;
    Node* Tnil; // due to Cormen
};

} // myDS