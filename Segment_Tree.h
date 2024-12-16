// Header guard to prevent multiple inclusions
#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include <cmath> // For pow and log2 functions

// Class for implementing a Segment Tree for Range Minimum Query (RMQ)
class SegmentTreeRMQ
{
public:
    // Constructor to initialize the segment tree with the input data
    // @param data: A vector of integers representing the input array
    SegmentTreeRMQ(const std::vector<int>& data) : m_data{ data }
    {
        // Calculate the size of the segment tree array, ensuring enough space for the tree
        size_t treeSize{ getNextPowerOf2(data.size()) * 2 - 1 };
        m_segTreeArray.resize(treeSize);
    }

    // Function to construct the segment tree
    // @param low: Starting index of the current range in the input array
    // @param high: Ending index of the current range in the input array
    // @param pos: Current index in the segment tree array
    void constructSegTree(int low, int high, int pos)
    {
        // Base case: If the range is invalid, return
        if (low > high) return;

        // Base case: If the range represents a single element, store it in the tree
        if (low == high) {
            m_segTreeArray[pos] = m_data[low];
            return;
        }

        // Recursive case: Divide the range into two halves
        int mid{ (high - low) / 2 + low };
        constructSegTree(low, mid, 2 * pos + 1);    // Left child
        constructSegTree(mid + 1, high, 2 * pos + 2); // Right child

        // Store the minimum of the two child nodes in the current node
        m_segTreeArray[pos] = std::min(m_segTreeArray[2 * pos + 1], m_segTreeArray[2 * pos + 2]);
    }

    // Function to perform a range minimum query
    // @param qLow: Query range start index
    // @param qHigh: Query range end index
    // @param low: Current range start index in the segment tree
    // @param high: Current range end index in the segment tree
    // @param pos: Current index in the segment tree array
    // @return The minimum value in the specified query range
    int rangeMinQuery(int qLow, int qHigh, int low, int high, int pos)
    {
        // Case 1: Complete overlap (query range completely covers the current range)
        if (qLow <= low && qHigh >= high) {
            return m_segTreeArray[pos];
        }
        // Case 2: No overlap (query range is outside the current range)
        else if (qLow > high || qHigh < low) {
            return std::numeric_limits<int>::max(); // Return maximum value as neutral element for min
        }

        // Case 3: Partial overlap
        int mid{ (high - low) / 2 + low };
        int left{ rangeMinQuery(qLow, qHigh, low, mid, 2 * pos + 1) };
        int right{ rangeMinQuery(qLow, qHigh, mid + 1, high, 2 * pos + 2) };

        return std::min(left, right);
    }

    // Function to update an element in the segment tree
    // @param start: Start index of the current range in the input tree
    // @param end: End index of the current range in the input tree
    // @param pos: Current index in the segment tree array
    // @param index: Index of the element to be updated in the input array
    // @param val: New value to update
    void update(int start, int end, int pos, int index, int val)
    {
        // If the index is out of the current range, return
        if (index < start || index > end) return;

        // Base case: If the current range represents a single element, update it
        if (start == end) {
            m_segTreeArray[pos] = m_data[index] = val;
            return;
        }

        // Recursive case: Update the left or right child
        int mid{ (end - start) / 2 + start };
        update(start, mid, 2 * pos + 1, index, val);    // Left child
        update(mid + 1, end, 2 * pos + 2, index, val); // Right child

        // Recalculate the current node's value after the update
        m_segTreeArray[pos] = std::min(m_segTreeArray[2 * pos + 1], m_segTreeArray[2 * pos + 2]);
    }

    // Function to print the input data and segment tree
    void print() const noexcept
    {
        // Print the input data
        for (int i : m_data) std::cout << i << " ";
        std::cout << std::endl;

        // Print the segment tree level by level using a queue
        int n = m_segTreeArray.size();
        std::queue<int> q;
        q.push(0);

        while (!q.empty()) {
            int size = q.size();

            while (size--) {
                auto index{ q.front() };
                q.pop();

                std::cout << m_segTreeArray[index] << " ";

                if (index * 2 + 1 < n) q.push(index * 2 + 1); // Left child
                if (index * 2 + 2 < n) q.push(index * 2 + 2); // Right child
            }

            std::cout << std::endl;
        }
    }

private:
    // Helper function to calculate the next power of 2 greater than or equal to n
    // @param n: Input number
    // @return The smallest power of 2 greater than or equal to n
    size_t getNextPowerOf2(size_t n) const noexcept
    {
        return pow(2, std::ceil(std::log2(n + 1)));
    }

private:
    std::vector<int> m_data;         // Input data array
    std::vector<int> m_segTreeArray; // Segment tree array
};
