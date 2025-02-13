#pragma once

#include <queue>
#include <functional>

namespace myDS {

    template <typename T>
    class AVL {
        struct Node {
            T m_val;
            Node* left{ nullptr };
            Node* right{ nullptr };

            Node(T val = T{}, Node* l = nullptr, Node* r = nullptr) :
                m_val{ val }, left{ l }, right{ r }
            { }
        };

    public:
        AVL() { }
        
        /*
        * Interface
        */

        int getHeight() const noexcept {
            return getHeightHelper(root);
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

        ~AVL() { cleanup(root); }

        /*
        * DFS and BFS traversals
        */
        void preorderTraverse(std::function<void(T)> func) const noexcept {
            preorderTraverseHelper(root, func);
        }

        void inorderTraverse(std::function<void(T)> func) const noexcept {
            inorderTraverseHelper(root, func);
        }

        void postorderTraverse(std::function<void(T)> func) const noexcept {
            postorderTraverseHelper(root, func);
        }

        void levelOrderTraverse(std::function<void(T)> func) const noexcept {
            levelOrderTraverseHelper(root, func);
        }

    private:
        Node* root{ nullptr };


    private:

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

        int getBF(Node* node) const {
            if (!node) return 0;
            return getHeightHelper(node->left) - getHeightHelper(node->right);
        }

        bool searchHelper(Node* node, T val) const {
            if (!node) return false;

            if (node->m_val == val) return true;
            if (node->m_val > val) return searchHelper(node->left, val);
            if (node->m_val < val) return searchHelper(node->right, val);

            return false;
        }

        Node* rightRotate(Node* y) {
            Node* x{ y->left };
            
            y->left = x->right;
            x->right = y;

            return x;
        }

        Node* leftRotate(Node* y) {
            Node* x{ y->right };

            y->right = x->left;
            x->left = y;

            return x;
        }

        Node* insertHelper(Node* node, T val) {
            if (!node) return new Node(val);

            if (val > node->m_val) {
                node->right = insertHelper(node->right, val);
            }
            else if (val < node->m_val) {
                node->left = insertHelper(node->left, val);
            }

            int bf{ getBF(node) };

            if (bf > 1 && val < node->left->m_val) {
                return rightRotate(node);
            }
            if (bf > 1 && val > node->left->m_val) {
                node->left = leftRotate(node->left);
                return rightRotate(node);
            }
            if (bf < -1 && val > node->right->m_val) {
                return leftRotate(node);
            }
            if (bf < -1 && val < node->right->m_val) {
                node->right = rightRotate(node->right);
                return leftRotate(node);
            }

            return node;
        }

        Node* removeHelper(Node* node, T val) {
            if (!node) return nullptr;

            if (val < node->m_val) {
                node->left = removeHelper(node->left, val);
            }
            else if (val > node->m_val) {
                node->right = removeHelper(node->right, val);
            }
            else {
                if (!node->left) {
                    Node* tmp = node->right;
                    delete node;
                    node = nullptr;
                    return tmp;
                }
                else if (!node->right) {
                    Node* tmp = node->left;
                    delete node;
                    node = nullptr;
                    return tmp;
                }

                Node* tmp{ getMin(node->right) };
                node->m_val = tmp->m_val;
                node->right = removeHelper(node->right, tmp->m_val);
            }

            int bf{ getBF(node) };

            if (bf > 1 && getBF(node->left) >= 0) {
                return rightRotate(node);
            }
            if (bf > 1 && getBF(node->left) < 0) {
                node->left = leftRotate(node->left);
                return rightRotate(node);
            }
            if (bf < -1 && getBF(node->right) <= 0) {
                return leftRotate(node);
            }
            if (bf < -1 && getBF(node->right) > 0) {
                node->right = rightRotate(node->right);
                return leftRotate(node);
            }

            return node;
        }

        void cleanup(Node* node) {
            if (!node) return;

            std::queue<Node*> q;
            q.push(node);

            while (!q.empty()) {
                Node* tmp{ q.front() };
                q.pop();

                if (tmp->left) q.push(tmp->left);
                if (tmp->right) q.push(tmp->right);
                
                delete tmp;
                tmp = nullptr;
            }
        }

        /*
        * DFS and BFS helpers
        */

        void preorderTraverseHelper(Node* node, std::function<void(T)> func) const noexcept {
            if (!node) return;

            func(node->m_val);
            preorderTraverseHelper(node->left, func);
            preorderTraverseHelper(node->right, func);
        }

        void inorderTraverseHelper(Node* node, std::function<void(T)> func) const noexcept {
            if (!node) return;

            inorderTraverseHelper(node->left, func);
            func(node->m_val);
            inorderTraverseHelper(node->right, func);
        }

        void postorderTraverseHelper(Node* node, std::function<void(T)> func) const noexcept {
            if (!node) return;

            postorderTraverseHelper(node->left, func);
            postorderTraverseHelper(node->right, func);
            func(node->m_val);
        }

        void levelOrderTraverseHelper(Node* node, std::function<void(T)> func) const noexcept {
            if (!node) return;

            std::queue<Node*> q;
            q.push(node);

            while (!q.empty()) {
                int size = q.size();
                while (size--) {
                    Node* tmp{ q.front() };
                    q.pop();

                    func(node->m_val);

                    if (tmp->left) q.push(tmp->left);
                    if (tmp->right) q.push(tmp->right);
                }
            }
        }
    };

} // myDS
