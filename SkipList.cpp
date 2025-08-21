/**
 * @file SkipList.hpp
 * @brief Probabilistic, ordered list with O(log n) expected search/insert/erase.
 *
 * A **Skip List** maintains multiple levels of forward-linked lists. The bottom
 * level contains **all** keys in sorted order; each higher level contains a
 * sparser subset, acting like "express lanes" that let searches skip over many
 * items at once. Balancing is randomized: each inserted key is *promoted*
 * upward one level with probability p (here p = 0.5), then possibly again, etc.
 *
 * ---------------------------------------------------------------------------
 *  Layout & invariants (this implementation)
 * ---------------------------------------------------------------------------
 * - `levels[i]` is the **dummy head** of level i (0 = bottom, increasing upward).
 * - Every dummy head participates only in horizontal links at its level and
 *   vertical links to its neighbors' dummies (below/above).
 * - Each real key appears as a **tower** of nodes: always at level 0, and then
 *   at higher levels depending on coin flips during insertion.
 * - We always keep **at least** level 0 with a dummy head (created in the ctor).
 *   Empty top levels are trimmed automatically.
 *
 * ---------------------------------------------------------------------------
 *  Smart pointer ownership model
 * ---------------------------------------------------------------------------
 * - `next`  : std::shared_ptr  (owns the forward neighbor at the same level)
 * - `below` : std::shared_ptr  (owns the node in the level below)
 * - `prev`  : std::weak_ptr    (non-owning backward link; avoids cycles)
 * - `above` : std::weak_ptr    (non-owning upward link; avoids cycles)
 *
 * Using `weak_ptr` for back-references breaks reference cycles like
 *   a->next = b  and  b->prev = a,
 * which would otherwise keep both nodes alive forever. With this scheme,
 * nodes are freed automatically when disconnected from all owners.
 *
 * ---------------------------------------------------------------------------
 *  Randomized balancing & coin flips
 * ---------------------------------------------------------------------------
 * - `coinFlip()` ~ Bernoulli(p = 0.5). On insert, a new key is placed at level 0;
 *   while coinFlip() returns true, it is promoted one level higher (creating
 *   a new node linked via above/below).
 * - Probability a tower reaches height ≥ k (counting level 0 as height 1) is
 *      P[H ≥ k] = p^{k-1}.  With p = 0.5 this is 2^{-(k-1)}.
 * - Expected height of a *single* tower is 1/(1 - p). For p = 0.5 that is 2,
 *   so each insert creates, on average, a constant number of extra nodes.
 *
 * ---------------------------------------------------------------------------
 *  Why search is O(log n) expected
 * ---------------------------------------------------------------------------
 * - Expected number of levels (height of the tallest tower):
 *      H ≈ log_{1/p}(n).  With p = 0.5,  H ≈ log2(n).
 *   Sketch: the expected number of nodes at level k is ~ n * p^k. The top
 *   non-empty level K satisfies n * p^K ≈ 1 ⇒ K ≈ log_{1/p}(n).
 * - During search we start at the top-left dummy, move **right** while `next->val < x`,
 *   then **down** one level, and repeat. The expected number of right-steps per
 *   level is constant (≤ 1/p; with p=0.5, ≈ 2). Multiplying by H gives
 *      E[search cost] = O(H) = O(log n).
 *
 * ---------------------------------------------------------------------------
 *  Why space is O(n)
 * ---------------------------------------------------------------------------
 * Summing expected nodes across levels:
 *   Level 0: ~ n
 *   Level 1: ~ n * p
 *   Level 2: ~ n * p^2
 *   ...
 *   Total ≈ n * (1 + p + p^2 + …) = n / (1 - p).
 * With p = 0.5 this is ~ 2n. Thus auxiliary nodes add only a constant factor;
 * overall space is **O(n)**.
 *
 * ---------------------------------------------------------------------------
 *  API & behavior (high level)
 * ---------------------------------------------------------------------------
 * - `bool search(T) const`:
 *     Top-down walk (right, then down). Returns true iff the next node at level 0
 *     matches the value.
 *
 * - `void insert(T)`:
 *     Finds the predecessor at level 0, inserts the new node, then repeatedly
 *     promotes it upwards with probability p, creating vertical links and
 *     connecting at each higher level. Adds a new top dummy if we run out of path.
 *
 * - `void remove(T)`:
 *     Finds the node (if present) and disconnects the entire tower from bottom
 *     to top, then trims empty top levels (never removing level 0).
 *
 * - Utilities:
 *     `addNewTopLevel()` (internal), `trimEmptyTopLevels()` (internal),
 *     `connect()`/`disconnect()` maintain horizontal and vertical invariants.
 *
 * ---------------------------------------------------------------------------
 *  Requirements & notes
 * ---------------------------------------------------------------------------
 * - @tparam T must be default-constructible (for dummy heads) and have `operator<`
 *   for ordering; your `print()` uses `std::to_string(T)`, so T should also be
 *   printable that way (or adjust printing for general types).
 * - Duplicates: as written, duplicates are allowed (no equality check before insert).
 *   If you want a set-like structure, add `if (pred->next && pred->next->val == val) return;`
 *   before inserting.
 * - Thread safety: `coinFlip()` uses thread-local RNG for per-thread randomness,
 *   but the data structure itself is **not** synchronized and is not thread-safe.
 * - Invariant: constructor pre-creates level 0 dummy head, so `levels` is never empty.
 *
 * ---------------------------------------------------------------------------
 *  Complexity summary (expected)
 * ---------------------------------------------------------------------------
 * - Search:  O(log n)
 * - Insert:  O(log n)  (search + constant expected promotions)
 * - Remove:  O(log n)
 * - Space:   O(n)
 */


#include <iostream>
#include <vector>
#include <memory>
#include <random>
#include <string>
#include <cassert>

template <typename T>
struct Node
{
    T val;
    std::shared_ptr<Node> next{}, below{};
    std::weak_ptr<Node> above{}, prev{};

    explicit Node(T data) : val{data} { }
};

template <typename T>
class SkipList
{
private:
    std::vector<std::shared_ptr<Node<T>>> levels;

    bool coinFlip()
    {
        static thread_local std::mt19937 rng{std::random_device{}()};
        static thread_local std::bernoulli_distribution coin{0.5};
        return coin(rng);
    }
    
    void addNewTopLevel()
    {
        assert(!levels.empty()); // internal: base dummy must exist

        auto newHead = std::make_shared<Node<T>>(T{});
    
        auto prevTop{ levels.back() };
        newHead->below = prevTop;
        prevTop->above = newHead;
    
        levels.push_back(newHead);
    }
    
    void trimEmptyTopLevels()
    {
        while (levels.size() > 1 && !levels.back()->next) {
            auto top{ levels.back() };
            levels.pop_back();
    
            if (auto below = top->below) below->above.reset();
            top->below.reset();
        }
    }

    void connect(std::shared_ptr<Node<T>> a, std::shared_ptr<Node<T>> b)
    {
        b->next = a->next;
        if (a->next) a->next->prev = b; // assign weak ptr from shared
        b->prev = a;                    // assign weak ptr from shared
        a->next = b;
    }
    
    void disconnect(std::shared_ptr<Node<T>> node)
    {
        if (auto p = node->prev.lock()) p->next = node->next;
        if (node->next) node->next->prev = node->prev;
    
        if (auto b = node->below) b->above.reset();
        if (auto a = node->above.lock()) a->below.reset();
        
        node->next.reset();
        node->below.reset();
        node->prev.reset();
        node->above.reset();
    }

    auto searchHelper(T val, bool needThePath = false) const -> std::pair<std::shared_ptr<Node<T>>, std::vector<std::shared_ptr<Node<T>>>>
    {
        if (levels.empty()) return {nullptr, {}};
    
        auto curr = levels.back();
        std::vector<std::shared_ptr<Node<T>>> path;
        path.reserve(levels.size());

        while (curr->below) {
            while (curr->next && curr->next->val < val) {
                curr = curr->next;
            }
    
            if (needThePath) path.push_back(curr);
            curr = curr->below;
        }
    
        while (curr->next && curr->next->val < val) curr = curr->next;

        return std::make_pair(curr, path);
    }

public:
    SkipList()
    {
        auto dummy = std::make_shared<Node<T>>(T{});
        levels.push_back(dummy);
    }

    SkipList(const SkipList&) = delete;
    SkipList& operator=(const SkipList&) = delete;

    bool search(T val) const
    {
        auto [pred, _] = searchHelper(val);
        return pred && pred->next && pred->next->val == val;
    }

    void insert(T val)
    {
        auto [pred, path] = searchHelper(val, true);
        auto node = std::make_shared<Node<T>>(val);
        connect(pred, node);

        auto below = node;

        while (coinFlip()) {
            if (path.empty()) {
                addNewTopLevel();
                path.push_back(levels.back());
            }

            auto upPred = path.back();
            path.pop_back();

            auto up = std::make_shared<Node<T>>(val);
            up->below = below;
            below->above = up;

            connect(upPred, up);
            below = up;
        }
    }


    void remove(T val)
    {
        auto [pred, _] = searchHelper(val, false);
        if (!pred || !pred->next || pred->next->val != val) return;

        auto curr = pred->next;

        while (curr) {
            auto above = curr->above.lock();
            disconnect(curr);
            curr = above;
        }

        trimEmptyTopLevels();
    }

    void checkPrinter() const
    {
        for (int i = static_cast<int>(levels.size()) - 1; i >= 0; --i) {
            auto curr = levels[i]->next;
            std::cout << "[L" << i << "] ";
            while (curr) {
                std::cout << curr->val << " -> ";
                curr = curr->next;
            }
            std::cout << std::endl;
        }
    }

    void print() const
    {
        if (levels.empty()) return;
        auto curr = levels[0];
        int size = 0;

        while (curr) {
            ++size;
            curr = curr->next;
        }

        std::vector<std::vector<std::string>> mat(levels.size() * 2, std::vector<std::string>(size, " "));
        curr = levels[0];
        int j{};
        while (curr) {
            int i = levels.size() * 2 - 1;

            auto node = curr;
            mat[i][j] = std::to_string(node->val);

            while (node->above.lock()) {
                --i;
                mat[i][j] = "|";
                
                --i;
                mat[i][j] = std::to_string(node->val);
                
                node = node->above.lock();
            }

            ++j;

            curr = curr->next;
        }

        for (auto& vec : mat) {
            if (vec.empty()) break;
            for (auto& s : vec) std::cout << s << " ";
            std::cout << std::endl;
        }
    }
};

int main()
{
    SkipList<int> sl;
    std::cout << "Insert: " << 5 << std::endl;
    sl.insert(5);
    sl.print();
    sl.checkPrinter();
    std::cout << std::endl;

    std::cout << "Insert: " << 3 << std::endl;
    sl.insert(3);
        
    sl.checkPrinter();
    std::cout << std::endl;
    sl.print();

    std::cout << "Insert: " << 9 << std::endl;
    sl.insert(9);
    sl.print();
    sl.checkPrinter();
    std::cout << std::endl;

    std::cout << "Insert: 8\n";
    sl.insert(8);
    sl.print();
    sl.checkPrinter();
    std::cout << std::endl;

    std::cout << "Insert: 1\n";
    sl.insert(1);    
    sl.print();
    sl.checkPrinter();
    std::cout << std::endl;


    //std::cout << "Searching for: " << 1 << " = " << sl.search(1) << std::endl;
    //std::cout << "Searching for: " << 0 << " = " << sl.search(0) << std::endl;
    //std::cout << "Searching for: " << 5 << " = " << sl.search(5) << std::endl;
    //std::cout << "Searching for: " << 9 << " = " << sl.search(9) << std::endl;
    //std::cout << "Searching for: " << 2 << " = " << sl.search(2) << std::endl;
    //std::cout << "Searching for: " << -1 << " = " << sl.search(-1) << std::endl;

    std::cout << "Delete: " << 5 << std::endl;
    sl.remove(5);
    sl.print();
    sl.checkPrinter();
    std::cout << std::endl;

    std::cout << "Delete: " << 9 << std::endl;
    sl.remove(9);
    sl.print();
    sl.checkPrinter();
    std::cout << std::endl;

    std::cout << "Inserting 2\n";
    sl.insert(2);
    sl.print();
    sl.checkPrinter();
    std::cout << std::endl;


    std::cout << "Removing 1\n";
    sl.remove(1);
    sl.print();
    sl.checkPrinter();
    std::cout << std::endl;


    std::cout << "Removing 2\n";
    sl.remove(2);
    sl.print();
    sl.checkPrinter();
    std::cout << std::endl;

    std::cout << "Removing 2\n";
    sl.remove(2);
    sl.print();
    sl.checkPrinter();
    std::cout << std::endl;

    std::cout << "Removing 3\n";
    sl.remove(3);
    sl.print();
    sl.checkPrinter();
    std::cout << std::endl;

    std::cout << "Removing 8\n";
    sl.remove(8);
    sl.print();
    sl.checkPrinter();
    std::cout << std::endl;

    std::cout << "Insert: 1\n";
    sl.insert(1);    
    sl.print();
    sl.checkPrinter();
    std::cout << std::endl;
}