#ifndef BST_H_
#define BST_H_

#include <iostream>
#include <queue> // for level order traversal

namespace myDS {

template <typename T>
class BST {
private:
    struct Node {
        T m_val;
        Node* left {nullptr};
        Node* right {nullptr};

        Node(T val = T{}, Node* l = nullptr, Node* r = nullptr) : m_val{val}, left{l}, right{r} { }
    };

public:
    BST(T val) { root = new Node(val); } 
    
    void printPreorder() const { 
        printPreorderHelper(root); 
        std::cout << std::endl;
    }

    void printInorder() const {
        printInorderHelper(root);
        std::cout << std::endl;
    }

    void printPostorder() const {
        printPostorderHelper(root);
        std::cout << std::endl;
    }

    void printLevelOrder() const {
        printLevelOrderHelper(root);
    }

    int getHeight() const {
        return getHeightHelper(root);
    };

    Node* getSuccessor(Node* node) const {
        return getSuccessorHelper(node);
    }

    Node* getPredecessor(Node* node) const {
        return getPredecessorHelper(node);
    }

    T getNodeVal(Node* node) const {
        return node->m_val;
    }

    void insert(T val) {
        root = insertHelper(root, val);
    }

    bool search(T val) const {
        return searchHelper(root, val);
    }

    void remove(T val) {
        root = removeHelper(root, val);
    }

    ~BST() { clearBST(); }

private:
    Node* root;

    void printPreorderHelper(Node* node) const {
        if (!node) return;

        std::cout << node->m_val << " ";
        printPreorderHelper(node->left);
        printPreorderHelper(node->right);
    }

    void printInorderHelper(Node* node) const {
        if (!node) return;

        printInorderHelper(node->left);
        std::cout << node->m_val << " ";
        printInorderHelper(node->right);
    }

    void printPostorderHelper(Node* node) const {
        if (!node) return;

        printPostorderHelper(node->left);
        printPostorderHelper(node->right);
        std::cout << node->m_val << " ";
    }

    void printLevelOrderHelper(Node* node) const {
        if (!node) return;

        std::queue<Node*> q;
        q.push(node);

        while(!q.empty()) {
            int size = q.size();
            while (size--) {
                Node* tmp = q.front();
                q.pop();
                std::cout << tmp->m_val << " ";
                if (tmp->left) q.push(tmp->left);
                if (tmp->right) q.push(tmp->right);
            }
            std::cout << std::endl;
        }
    }

    int getHeightHelper(Node* node) const {
        if (!node) return -1;

        return std::max(getHeightHelper(node->left), getHeightHelper(node->right)) + 1;
    } 

    Node* getMin(Node* node) const {
        while (node && node->left) node = node->left;
        return node;
    }

    Node* getMax(Node* node) const {
        while (node && node->right) node = node->right;
        return node;
    }

    Node* getPredecessorHelper(Node* node) const {
        if (!node) return nullptr;
        if (node->left) return getMax(node->left);

        Node* predecessor = nullptr;
        Node* ancestor = root;

        while (ancestor != node) {
            if (node->m_val > ancestor->m_val) {
                predecessor = ancestor;
                ancestor = ancestor->right;
            } else {
                ancestor = ancestor->left;
            }
        }
        return predecessor;
    }

    Node* getSuccessorHelper(Node* node) const {
        if (!node) return nullptr;
        if (node->right) return getMin(node->right);

        Node* successor = nullptr;
        Node* ancestor = root;

        while (ancestor != node) {
            if (node->m_val < ancestor->m_val) {
                successor = ancestor;
                ancestor = ancestor->left;
            } else {
                ancestor = ancestor->right;
            }
        }
        return successor;
    }

    Node* insertHelper(Node* node, T val) {
        if (!node) return new Node(val);

        if (node->m_val > val) {
            node->left =  insertHelper(node->left, val);
        } else if (node->m_val < val) {
            node->right = insertHelper(node->right, val);
        }

        return node;
    }

    bool searchHelper(Node* node, T val) const {
        if (!node) return false;
        if (node->m_val == val) return true;

        if (node->m_val > val) return searchHelper(node->left, val);
        if (node->m_val < val) return searchHelper(node->right, val);

        return false;
    }

    Node* removeHelper(Node* node, T val) {
        if (!node) return nullptr;

        if (node->m_val > val) {
            node->left = removeHelper(node->left, val);
        } else if (node->m_val < val) {
            node->right = removeHelper(node->right, val);
        } else {
            if (!node->left) {
                Node* tmp = node->right;
                delete node;
                node = nullptr;
                return tmp;
            } else if (!node->right) {
                Node* tmp = node->left;
                delete node;
                node = nullptr;
                return tmp;
            }
            Node* tmp = getMin(node->right);
            std::swap(node->m_val, tmp->m_val);
            node->right = removeHelper(node->right, val);
        }

        return node;
    }

    void clearBST() {
        if (!root) return;

        std::queue<Node*> q;
        q.push(root);

        while(!q.empty()) {
            Node* node = q.front();
            q.pop();
            if (node->left) q.push(node->left);
            if (node->right) q.push(node->right);

            delete node;
            node = nullptr;
        }
    }
};

} // myDS

#endif