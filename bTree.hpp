#ifndef __BTREE_H__
#define __BTREE_H__

#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <queue>

template <typename T>
class BTree {
public:
    class BTreeNode : public std::enable_shared_from_this<BTreeNode> {
        public:
            BTreeNode(int t, bool isLeaf);
            void traverse() noexcept;            
            std::shared_ptr<BTreeNode> search(T key);
            
            // Inserts a new key in the subtree rooted with this node,
            // The node must be non-full when the function is called.
            void insertNonFull(T key);

            // Splits the child 'y' of this node.
            // 'i' is the index of 'y' in children array.
            // The child 'y' must be full when this function is called.
            void splitChild(int i, std::shared_ptr<BTreeNode> y);

            // returns the index of the first element >= key
            int findKey(T key);

            // wrapper function to remove the key in subtree rooted with this node
            void remove(T key);

            // removes the key present in the index-th position of this node, which is a leaf
            void removeFromLeaf(int index);

            // removes the key present in the index-th position of this node, which is NOT a leaf
            void removeFromNonLeaf(int index);

            // gets the predecessor of the key, where the key is
            // present in this node under index
            T getPredecessor(int index);

            // gets the successor of the key, where the key is
            // present in this node under index
            T getSuccessor(int index);

            // fills the child node present in the index-th pos
            // in the m_children if that child has less than t - 1 keys
            void fill(int index);

            // borrow a key from m_children[index - 1] node and
            // gives it to the node: m_children[index]
            void borrowFromPrev(int index);

            // borrow a key from m_children[index - 1] node and
            // gives it to the node: m_children[index]
            void borrowFromNext(int index);

            // merges the [index + 1]-th child with [index]-th child of this node
            void merge(int index);

            void print() const;
        public:
            int m_t; // min degree
            std::vector<T> m_keys;
            std::vector<std::shared_ptr<BTreeNode>> m_children;
            int m_size; // current number of keys
            bool m_isLeaf;
    };

public:
    std::shared_ptr<BTreeNode> m_root;
    int m_t; // min degree

    BTree(int t);
    void traverse() noexcept;
    std::shared_ptr<BTreeNode> search(T key);

    void insert(T key);
    void remove(T key);

    void print() noexcept;
};

// *******************   BTree functionality ********************

template <typename T>
BTree<T>::BTree(int t) : m_root{nullptr}, m_t{t}
{ }

template <typename T>
void
BTree<T>::traverse() noexcept
{
    if (m_root != nullptr) {
        m_root->traverse();
    }
}

template <typename T>
void
BTree<T>::print() noexcept
{
    std::cout << "\n\n";
    std::queue<std::shared_ptr<BTreeNode>> q;
    if (m_root) q.push(m_root);

    while (!q.empty()) {
        int size = q.size();
        while (size--) {
            auto top = q.front();
            q.pop();
            for (int i = 0; i < top->m_size; ++i) std::cout << top->m_keys[i] << " ";
            std::cout << "\t";

            for (int i = 0; i < top->m_size + 1; ++i) {
                if (top->m_children[i]) q.push(top->m_children[i]);
            }
        }

        std::cout << std::endl;
    }
}

template <typename T>
std::shared_ptr<typename BTree<T>::BTreeNode>
BTree<T>::search(T key)
{
    return m_root == nullptr ? nullptr : m_root->search(key);
}

template <typename T>
void
BTree<T>::insert(T key)
{
    if (m_root == nullptr) {
        m_root = std::make_shared<BTreeNode>(m_t, true);
        m_root->m_keys[0] = key;
        m_root->m_size = 1;
    } else {
        if (m_root->m_size == m_t * 2 - 1) {
            // creating a new root
            std::shared_ptr<BTreeNode> node =
                std::make_shared<BTreeNode>(m_t, false);

            // making the old root as a child of the new node
            node->m_children[0] = m_root;

            // split the old root and move 1 key to the new root
            node->splitChild(0, m_root);

            // new root has two children now.
            // decide which of the two children is going to have the new key
            int i = 0;
            if (node->m_keys[0] < key) {
                ++i;
            }
            node->m_children[i]->insertNonFull(key);
            m_root = node;
        } else {
            m_root->insertNonFull(key);
        }
    }
}

template <typename T>
void
BTree<T>::remove(T key)
{
    if (!m_root) return;

    m_root->remove(key);

    // If after the remove the root has no keys
    // make his first child as the new root
    // if it has no children, set the root null

    if (m_root->m_size == 0) {
        if (m_root->m_isLeaf) {
            m_root = nullptr;
        } else {
            m_root = m_root->m_children[0];
        }
    }
}


// ***************************    BTreeNode functionality  *****************************

template <typename T>
BTree<T>::BTreeNode::BTreeNode(int t, bool isLeaf)
    : m_t{t}, m_isLeaf{isLeaf}, m_size{}
{
    m_keys.resize(2 * m_t - 1); // keys should be one less than the order
    m_children.resize(2 * m_t);
}

template <typename T>
void
BTree<T>::BTreeNode::traverse() noexcept
{
    int i = 0;
    for (; i < m_size; ++i) {
        if (m_isLeaf == false) {
            m_children[i]->traverse();
        }
        std::cout << " " << m_keys[i];
    }

    // printing the leaf child
    if (m_isLeaf == false) {
        m_children[i]->traverse();
    }
}

template <typename T>
void
BTree<T>::BTreeNode::print() const
{
    for (int i = 0; i < m_size; ++i) std::cout << m_keys[i] << " ";
    std::cout << std::endl;
}

template <typename T>
std::shared_ptr<typename BTree<T>::BTreeNode>
BTree<T>::BTreeNode::search(T key)
{
    int i = 0;
    while (i < m_size && key > m_keys[i]) {
        ++i;
    }

    if (m_keys[i] == key) {
        return this->shared_from_this();
    }

    if (m_isLeaf == true) {
        return nullptr;
    }

    return m_children[i]->search(key);
}

template <typename T>
void
BTree<T>::BTreeNode::insertNonFull(T key)
{
    int i = m_size - 1;
    if (m_isLeaf == true) {
        // find the location of the new key
        // move all greater elems to the right
        while (i >= 0 && m_keys[i] > key) {
            m_keys[i + 1] = m_keys[i];
            --i;
        }
        m_keys[i + 1] = key;
        ++m_size;
    } else {
        while (i >= 0 && m_keys[i] > key) {
            --i;
        }


        // find the child which is going to have the new key
        if (m_children[i + 1]->m_size == 2 * m_t - 1) {
            splitChild(i + 1, m_children[i + 1]); // solves the problem of overflow

            // as after the split the middle key of m_children[i] goes up
            // and children[i] is splitted into two.
            if (m_keys[i + 1] < key) ++i;
        }

        m_children[i + 1]->insertNonFull(key);
    }
}

template <typename T>
void
BTree<T>::BTreeNode::splitChild(int index, std::shared_ptr<BTreeNode> y)
{
    std::shared_ptr<BTreeNode> z = std::make_shared<BTreeNode>(y->m_t, y->m_isLeaf);
    z->m_size = m_t - 1;

    // copy the last (t - 1) keys of y to z
    for (int i = 0; i < m_t - 1; ++i) {
        z->m_keys[i] = y->m_keys[i + m_t];
    }

    // copy the last t children of y to z
    if (y->m_isLeaf == false) {
        for (int i = 0; i < m_t; ++i) {
            z->m_children[i] = y->m_children[i + m_t];
        }
    }

    // reduce the number of keys in y
    y->m_size = m_t - 1;

    // since 'this' node is going to have a new child
    // we need a space for that
    for (int i = m_size; i > index; --i) {
        m_children[i + 1] = m_children[i];
    }

    m_children[index + 1] = z;
    for (int i = int(m_size); i >= int(index); --i) {
        m_keys[i + 1] = m_keys[i];
    }

    // copy the middle node of y to 'this' node
    m_keys[index] = y->m_keys[m_t - 1];
    ++m_size;
}

template <typename T>
int
BTree<T>::BTreeNode::findKey(T key)
{
    int index = 0;
    while (index < m_size && m_keys[index] < key) {
        ++index;
    }

    return index;
}

template <typename T>
void
BTree<T>::BTreeNode::remove(T key)
{
    int index = findKey(key);
    // the key to be removed is in this node

    /* Cormen - Cases 1 and 2 */
    if (index < m_size && m_keys[index] == key) {
        // if the node is a leaf node
        if (m_isLeaf) {
            removeFromLeaf(index);
        } else {
            removeFromNonLeaf(index);
        }
    } else { /* Cormen - Case 3: search arrives at an internal node x that does not contain key. */
        // if this node is a leaf node, then the key is not present in the tree
        if (m_isLeaf) {
            return;
        }
        // At this point the key to be removed is present in the subtree rooted with this node

        bool flag = index == m_size;
        // If the child where the key is supposed to exist has less than t keys, then we fill the child.
        // This is done because if the key exists in the m_children[index] node, 
        // and we remove from that node, we end up with a node which has ( < t - 1) keys
        if (m_children[index]->m_size < m_t) {
            fill(index); // solves the problem of underflow
        }

        // if the LAST child has been merged(flag), it must have been merged with the previous 
        // child and so we recurse on the [index - 1]-th child. 
        // Else we recurse on the [index]-th child which now has at least t keys, as we've already filled it
        if (flag && index > m_size) { // (index == m_size) means whether the key present in the subtree rooted with the last child of this node
            m_children[index - 1]->remove(key);
        } else {
            m_children[index]->remove(key);
        }
    }
}

/* CORMEN - Case 1: The search arrives at a leaf node x. */
template <typename T>
void
BTree<T>::BTreeNode::removeFromLeaf(int index)
{
    // Moving all keys after the index-th position one place backward
    for (int i = index + 1; i < m_size; ++i) {
        m_keys[i - 1] = m_keys[i];
    }

    --m_size;
}

/* Cormen - Case 2: The search arrives at an internal node x that contains the key. */
template <typename T>
void
BTree<T>::BTreeNode::removeFromNonLeaf(int index)
{
    T key = m_keys[index];

    // If the child that precedes key(m_children[index]) has at least t keys,
    // find the predecessor of key in the subtree rooted at m_children[index].
    // Replace key by predecessor and recursively delete the predecessor in m_children[index]
    
    /* Cormen - Case 2.a: x.c[i] has at least t keys */
    if (m_children[index]->m_size >= m_t) {
        T predecessor = getPredecessor(index);
        m_keys[index] = predecessor;
        m_children[index]->remove(predecessor);
    }

    // If the child m_children[index] has less than k keys, examine m_children[index + 1]
    // If m_children[index + 1] has at least t keys, find the successor of key
    // in the subtree rooted at m_children[index + 1].
    // Replace key by successor and recursively delete successor in m_chilren[index + 1].

    /* Cormen - Case 2.b: x.c[i] has t - 1 keys and x.c[i + 1] has at least t keys */
    else if (m_children[index + 1]->m_size >= m_t) {
        T successor = getSuccessor(index);
        m_keys[index] = successor;
        m_children[index + 1]->remove(successor);
    }
    
    // If both m_children[index] and m_children[index + 1] have less than t keys,
    // merge key and all of m_children[index + 1] into m_children[index]
    // After merging m_children[index] contains 2 * t - 1 keys
    // Then free m_children[index + 1] and recursively delete key from m_children[index]

    /* Cormen - Case 2.c: x.c[i] and x.c[i + 1] have t - 1 keys */
    else {
        merge(index);
        m_children[index]->remove(key);
    }
}

template <typename T>
T
BTree<T>::BTreeNode::getPredecessor(int index)
{
    std::shared_ptr<BTreeNode> curr = m_children[index];
    while (!curr->m_isLeaf) {
        curr = curr->m_children[m_size];
    }

    return curr->m_keys[curr->m_size - 1];
}

template <typename T>
T
BTree<T>::BTreeNode::getSuccessor(int index)
{
    std::shared_ptr<BTreeNode> curr = m_children[index + 1];
    while (!curr->m_isLeaf) {
        curr = curr->m_children[0];
    }

    return curr->m_keys[0];
}

/* Cormen - Case 3 ... */
template <typename T>
void
BTree<T>::BTreeNode::fill(int index)
{
    // if the previous child (m_children[index - 1]) has more than t - 1 keys
    // borrow a key from that child

    /* Cormen - Case 3.a: x.c[i] has only t - 1 keys but has an immediate sibling with at least t keys */
    if (index != 0 && m_children[index - 1]->m_size >= m_t) {
        borrowFromPrev(index);
    }
    // if the next child (m_children[index + 1]) has more than t - 1 keys
    // borrow a key from that child

    /* Cormen - Case 3.a: the same as above */
    else if (index != m_size && m_children[index + 1]->m_size >= m_t) {
        borrowFromNext(index);
    }

    // Now merge m_children[index] with its sibling
    // if the m_children[index] is the last child, merge it with its prev sibling
    // otherwise merge it with its next sibling

    /* Cormen - 3.b: x.c[i] and each of x.c[i]'s siblings have t - 1 keys */
    else {
        if (index != m_size) {
            merge(index);
        } else {
            merge(index - 1);
        }
    }
}

template <typename T>
void
BTree<T>::BTreeNode::borrowFromPrev(int index)
{
    std::shared_ptr<BTreeNode>& child = m_children[index];
    std::shared_ptr<BTreeNode>& sibling = m_children[index - 1];

    // The last key from m_children[index - 1] goes up to the parent 
    // and m_keys[index - 1] from parent is inserted as the first key in m_children[index].
    // Thus the sibling loses one key and the child gains one

    // Moving all keys in m_children[index] one step ahead
    for (int i = child->m_size - 1; i >= 0; --i) {
        child->m_keys[i + 1] = child->m_keys[i];
    }

    // If the m_children[index] is not a leaf, move all its child pointers one step ahead
    if (!child->m_isLeaf) {
        for (int i = child->m_size; i >= 0; --i) {
            child->m_children[i + 1] = child->m_children[i];
        }
    }
    
    // Setting child's first key equal to this node's m_keys[index - 1]
    child->m_keys[0] = m_keys[index - 1];

    // Move sibling's last child as m_children[index]'s first child
    if (!child->m_isLeaf) {
        child->m_children[0] = sibling->m_children[sibling->m_size];
    }

    // Move the key from the sibling to the parent 
    // This reduces the number of keys in the sibling
    m_keys[index - 1] = sibling->m_keys[sibling->m_size - 1];

    ++child->m_size;
    --sibling->m_size;
}

template <typename T>
void
BTree<T>::BTreeNode::borrowFromNext(int index)
{
    std::shared_ptr<BTreeNode>& child = m_children[index];
    std::shared_ptr<BTreeNode>& sibling = m_children[index + 1];

    // m_keys[index] is inserted as the last key in m_children[index]
    child->m_keys[child->m_size] = m_keys[index];

    // Sibling's first child is inserted as a last child into m_children[index]
    if (!child->m_isLeaf) {
        child->m_children[child->m_size + 1] = sibling->m_children[0];
    }

    // The first key from sibling is inserted into m_keys[index]
    m_keys[index] = sibling->m_keys[0];

    // Moving all keys in sibling one step behind
    for (int i = 1; i < sibling->m_size; ++i) {
        sibling->m_keys[i - 1] = sibling->m_keys[i];
    }

    // Moving the child pointers one step behind
    if (!sibling->m_isLeaf) {
        for (int i = 1; i <= sibling->m_size; ++i) {
            sibling->m_children[i - 1] = sibling->m_children[i];
        }
    }

    // increasing and decreasing the sizes
    ++child->m_size;
    --sibling->m_size;
}

template <typename T>
void 
BTree<T>::BTreeNode::merge(int index) 
{
    std::shared_ptr<BTreeNode>& child = m_children[index];
    std::shared_ptr<BTreeNode>& sibling = m_children[index + 1];

    // Pull a key from this node and insert it into [t - 1]-th pos of m_children[index]
    child->m_keys[m_t - 1] = m_keys[index];

    // Copy the keys from m_children[index + 1] to m_children[index] at the end
    for (int i = 0; i < sibling->m_size; ++i) {
        child->m_keys[m_t + i] = sibling->m_keys[i];
    }

    // Copy the child pointers
    if (!child->m_isLeaf) {
        for (int i = 0; i <= sibling->m_size; ++i) {
            child->m_children[m_t + i] = sibling->m_children[i];
        }
    }

    // Move all keys after index int this node one step back,
    // as we've created a gap when gave m_keys[index] to the child
    for (int i = index + 1; i < m_size; ++i) {
        m_keys[i - 1] = m_keys[i];
    }

    // The same for the children pointers after [index + 1] in this node one step back
    // no need to check if this node is a leaf, as it is impossible
    for (int i = index + 2; i <= m_size; ++i) {
        m_children[i - 1] = m_children[i];
    }

    // So the child took one key from its parent(from this) and all keys from its sibling
    child->m_size += sibling->m_size + 1;
    --m_size;
}

#endif