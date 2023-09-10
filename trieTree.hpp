#include <string>
#include <iostream>
#include <unordered_map>
#include <queue>

class Trie {
private:
    struct Node {
        std::unordered_map<char, Node*> m_map;
        bool isWord {false};
    };

    Node* root {nullptr};

public:
    void print() {
        std::queue<Node*> q;
        q.push(root);
        while (!q.empty()) {
            int size = q.size();
            while (size--) {
                auto node = q.front();
                q.pop();
                // std::cout << "node is a word = " << std::boolalpha << node->isWord << std::endl;
                // std::cout << "node's chars: ";
                std::string word = (node->isWord) ? "T" : "F";
                std::cout << "Node(" << word << "): ";
                for (const auto& p : node->m_map) {
                    std::cout << p.first << " ";
                    q.push(p.second);
                }
                std::cout << ", ";
            }
            std::cout << std::endl;
        }
    }
public:
    Trie() {
        root = new Node();
    }
    
    void insert(const std::string& word) {
        if (word.empty()) return;
        Node* curr = root;
        int i = 0;
        for (; i < word.size(); ++i) {
            char c = word[i];
            if (curr->m_map.find(c) == curr->m_map.end()) {
                Node* node = new Node();
                curr->m_map[c] = node;
            }
            curr = curr->m_map[c];
        }
        curr->isWord = true;
    }

    void insertRecursive(const std::string& word) {
        insertRecursiveHelper(root, word, 0);
    }

    void insertRecursiveHelper(Node* node, const std::string& word, int i) {
        if (i == word.size()) {
            node->isWord = true;
            return;
        }

        Node* tmp = node->m_map[word[i]];
        if (!tmp) {
            tmp = new Node();
            node->m_map[word[i]] = tmp;
        }

        insertRecursiveHelper(tmp, word, i + 1);
    }
    
    bool search(const std::string& word) {
        if (word.empty() || !root) return false;
        Node* node = root;
        for (char c : word) {
            if (node->m_map.find(c) == node->m_map.end()) {
                return false;
            }
            node = node->m_map[c];
        }
        return node->isWord;
    }

    bool searchRecursive(const std::string& word) {
        return searchRecursiveHelper(root, word, 0);
    }

    bool searchRecursiveHelper(Node* node, const std::string& word, int i) {
        if (i == word.size()) {
            return node->isWord;
        }

        Node* tmp = node->m_map[word[i]];
        if (!tmp) return false;

        return searchRecursiveHelper(tmp, word, i + 1);
    }
    
    bool startsWith(const std::string& prefix) {
        if (prefix.empty() || !root) return false;
        Node* node = root;
        for (char c : prefix) {
            if (node->m_map.find(c) == node->m_map.end()) {
                return false;
            }
            node = node->m_map[c];
        }
        return true;
    }

    void removeWord(const std::string& word) {
        removeWordHelper(root, word, 0);
    }

    bool removeWordHelper(Node* node, const std::string& word, int i) {
        if (i == word.size()) {
            if (!node->isWord) {
                return false;
            }
            node->isWord = false;
            return node->m_map.empty();
        }

        Node* tmp = node->m_map[word[i]];
        if (!tmp) return false;

        bool shouldDeleteCurrNode = removeWordHelper(tmp, word, i + 1);
        if (shouldDeleteCurrNode) {
            node->m_map.erase(word[i]);
            if (node->m_map.empty()) {
                delete node; node = nullptr;
                return true;
            }
            return false;
        }
        return false;
    }
};
