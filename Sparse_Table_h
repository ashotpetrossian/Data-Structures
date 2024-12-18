#pragma once

#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath> // Required for std::log2 and std::pow

/**
 * SparseTableRMQ class implements a sparse table data structure for efficient 
 * Range Minimum Query (RMQ) operations. The RMQ allows querying the minimum 
 * value in any subrange of a given array in O(1) time after O(n log n) preprocessing.
 *
 * Example:
 * Input array: [4, 2, 3, 1, 6, 5]
 * RMQ(1, 3) -> Minimum value between index 1 and 3 (inclusive) -> 1
 */
class SparseTableRMQ
{
public:
    /**
     * Constructor to initialize the SparseTableRMQ with a given vector of integers.
     * The constructor preprocesses the input array to build the sparse table.
     *
     * @param vec A vector of integers to perform RMQ operations on.
     */
    SparseTableRMQ(std::vector<int> vec) : nums{ std::move(vec) }
    {
        n = nums.size();
        preprocessRMQ();
    }

    /**
     * Preprocesses the input array to build the sparse table for RMQ.
     * This step is performed in O(n log n) time and involves filling a 2D table
     * where `table[i][j]` stores the minimum value in the range starting at index `i`
     * and covering `2^j` elements.
     */
    void preprocessRMQ()
    {
        // Resize the sparse table to have n rows and log2(n) + 1 columns
        table.resize(n);
        for (auto& column : table) column.resize(std::log2(n) + 1);

        // Initialize the first column of the table with the input array values
        for (int i{}; i < n; ++i) table[i][0] = nums[i];

        // Fill the rest of the sparse table using dynamic programming
        for (int j{ 1 }; (1 << j) <= n; ++j) {
            for (int i{ }; i + (1 << (j - 1)) < n; ++i) {
                // Compare two overlapping ranges of size 2^(j-1) and store the minimum
                table[i][j] = std::min(table[i][j - 1], table[i + (1 << (j - 1))][j - 1]);
            }
        }
    }

    /**
     * Queries the minimum value in the range [low, high] (inclusive) using the precomputed sparse table.
     * The query is answered in O(1) time by comparing two overlapping intervals.
     *
     * @param low The starting index of the range (0-based).
     * @param high The ending index of the range (0-based).
     * @return The minimum value in the range [low, high].
     */
    int query(int low, int high)
    {
        int len{ high - low + 1 }; // Length of the query range
        int k = std::log2(len);   // Maximum power of 2 that fits in the range
        // Compare two overlapping intervals of size 2^k to find the minimum
        return std::min(table[low][k], table[high - std::pow(2, k) + 1][k]);
    }

    /**
     * Prints the sparse table to the console for debugging or visualization purposes.
     */
    void print()
    {
        for (int i{}; i < n; ++i) {
            for (int j{}; (1 << j) <= n; ++j) {
                std::cout << std::setw(2) << table[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }

private:
    std::vector<int> nums;                  // Input array
    std::vector<std::vector<int>> table;    // Sparse table for RMQ
    int n{};                                // Size of the input array
};
