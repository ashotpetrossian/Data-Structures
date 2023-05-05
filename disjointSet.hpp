#include <iostream>
#include <vector>

class UnionFind {
public:
    UnionFind(std::size_t size) : m_size { size }, m_components { size } {
        m_ids.resize(size);
        m_sizes.resize(size, 1);
        for (int i = 0; i < size; ++i) {
            m_ids[i] = i;
        }
    }

    // find to which component p node belongs to
    std::size_t find(std::size_t p) const noexcept {
        std::size_t root = p;
        while (root != m_ids[root]) root = m_ids[root];

        // compress the path leading back to the root.
        while (p != root) {
            std::size_t next = m_ids[p];
            m_ids[p] = root;
            p = next;
        }

        return root;
    }

    bool areConnected(std::size_t p, std::size_t q) const noexcept {
        return find(p) == find(q);
    }

    std::size_t getComponentSize(std::size_t p) const noexcept {
        return m_sizes[find(p)];
    }

    constexpr std::size_t getSize() const noexcept { return m_size; }

    constexpr std::size_t getNumComponents() const noexcept { return m_components; }

    void unify(std::size_t p, std::size_t q) noexcept {
        int root1 = find(p);
        int root2 = find(q);

        if (root1  == root2) return;

        if (m_sizes[root1] >= m_sizes[root2]) {
            m_sizes[root1] += m_sizes[root2];
            m_ids[root2] = m_ids[root1];
        } else {
            m_sizes[root2] += m_sizes[root1];
            m_ids[root1] = m_ids[root2];
        }

        --m_components;
    }

    void display() {
        for (int i = 0; i < m_size; ++i) {
            std::cout << "Index: " << i << ", id: " << m_ids[i] << ", size = " << m_sizes[i] << std::endl;
        }
    }

private:
    const std::size_t m_size; // number of all elemets
    mutable std::vector<std::size_t> m_ids; // id[i] is the index of parent of i, if id[i] == i => i is a root
    std::vector<std::size_t> m_sizes; // size of each component
    std::size_t m_components; // components' count
};