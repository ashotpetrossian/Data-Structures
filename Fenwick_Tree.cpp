#include <vector>
#include <iostream>
#include <exception>

/*
* Fenwick Tree (Binary Indexed Tree) Overview
* A Fenwick Tree (BIT), also known as a Binary Indexed Tree, is a powerful data structure designed for efficiently querying prefix sums
* and performing point updates in an array.
* It operates in O(log n) time for both updates and queries, making it highly suitable for dynamic cumulative frequency tables,
* range queries, and other cumulative operations. The Fenwick Tree is particularly useful when you need
* to perform operations on an array that involves frequent updates and range queries.
*
* Key Operations:
* 
* Prefix Sum Query:
* Compute the sum of elements from the start (index 0) to a specified index i (inclusive).
* The operation is performed in O(log n) time.
* 
* Range Sum Query:
* Efficiently compute the sum of elements between indices l and r by using the difference between two prefix sums.
* This operation also takes O(log n) time.
* 
* Point Update:
* Update a specific element in the array, and propagate the change throughout the Fenwick Tree to ensure the correct cumulative values are maintained.
* Point updates also take O(log n) time.
* 
* Versatility for Range Queries:
* 
* The Fenwick Tree can be adapted to support various range queries beyond just summation. For example, it can efficiently handle:
* XOR Queries: Compute the XOR of elements in a specified range.
* Range Minimum/Maximum Queries: Extend the Fenwick Tree to support minimum or maximum values in a range,
* though this requires extra storage or modifications.
* With small adaptations, you can handle multiplicative or additive operations as well.
* 
* How It Works:
* The Fenwick Tree is implemented using a 1-based index array. Each element at index i stores a cumulative value for a range of elements,
* and the tree is structured such that the parent-child relationships follow specific rules based on the binary representation of the indices.
* During a query operation, the tree moves from index i to its parent node by the operation i -= (i & -i), summing the appropriate values along the way.
* For an update operation, the index i is incremented by i += (i & -i) to propagate the change through the tree.
* 
* Advantages:
* Time Efficiency: Both update and query operations are executed in O(log n) time.
* Space Efficiency: The Fenwick Tree only requires an array of size n+1 (for a 1-based index), which means it’s more space-efficient
* than other data structures like segment trees.
* Flexibility: While it's commonly used for range sum queries, it can be easily extended for other operations, such as XOR, min/max,
* and even product operations, depending on the problem requirements.
* Applications:
* Range Sum Queries: Fenwick Trees are widely used in competitive programming and real-time applications where you need to frequently query
* and update cumulative sums.
* Frequency Counting: Frequently used for counting occurrences or maintaining frequency distributions.
* Cumulative Distribution Functions (CDFs): Ideal for maintaining cumulative probability distributions or cumulative sums in data analysis.
* XOR Queries: By modifying the tree structure, it can be adapted for efficient XOR queries, where the query computes the XOR of elements within a range.
* Dynamic Range Queries: Supports a wide variety of dynamic range queries, such as computing the minimum or maximum of a range, with some modifications.
*
*/

/*
* Add operation:
* 
* Each time flip the last one bit, one - indexed
* sum(7) = sum(00111) =
*	T[00111] + T[00110] + T[00100] =
*	T[7] + T[6] + T[4] = 
*	range(7, 7) + range(5, 6) + range(1, 4)
* 
* Explanation: i -= (i & -i)
* 
* 7 = (00111)
* -7 = (11001) // 2's compliment (flip all bits and add one)
* 
* 00111 & 11001 = 00001
* 00111 - 00001 = 00110 
* as a result 7 became 6
* 
* next: 6 -> 4
* 
* 6 = (00110)
* -6 = (11010)
*
* 00110 & 11010 = 00010
* 00110 - 00010 = 00100
* 6 became 4 | the right most bit flipped
* 
* Update operation:
* let's say update is made on 2nd index (0 - indexed) with diff + 10
* 
* => the i-th index for tree will be 3 = 00011
* T[00011] = 9 + 10
* move one bit right: i += (i & -i);
* 
* 3 = 00011
* -3 = 11101
* 
* 00011 & 11101 = 00001
* 00011 + 00001 = 00100
* 3 became 4 | the bit moved left
* 
*/

class FenwickTree
{
public:
	FenwickTree(const std::vector<int>& data)
	{
		size = data.size();
		m_data.resize(size + 1);

		// Initialize Fenwick Tree with the given data
		for (int i{ }; i < size; ++i) {
			update(i, data[i]);
		}
	}

	int getRangeSum(int l, int r) const
	{
		if (l < 0 || r >= size || l > r) throw std::out_of_range("Invalid range");

		return getPrefixSum(r) - (l > 0 ? getPrefixSum(l - 1) : 0);
	}

	int getPrefixSum(int i) const
	{
		if (i < 0 || i >= size) throw std::out_of_range("Invalid index");
		
		int res{};
		++i; // Conver to 1 - indexed

		while (i > 0) {
			res += m_data[i];
			i -= (i & -i); // Move to the parent index
		}

		return res;
	}

	void update(int i, int diff)
	{
		if (i < 0 || i >= size) throw std::out_of_range("Invalid index");

		++i; // Convert to one-indexed
		while (i <= size) {
			m_data[i] += diff;
			i += (i & -i); // Move to the next affected index
		}
	}

	void print() const noexcept
	{
		for (int i{ 1 }; i <= size; ++i) std::cout << m_data[i] << " ";
		std::cout << std::endl;
	}
private:
	std::vector<int> m_data;
	int size{};
};

int main()
{
	try {
		std::vector<int> data{ 5, 2, 9, -3, 5, 20, 10, -7, 2, 3, -4, 0, -2, 15, 5 };
		FenwickTree f(data);

		f.print();

		std::cout << "till 4 = " << f.getPrefixSum(4) << std::endl;
		std::cout << "till 14 = " << f.getPrefixSum(14) << std::endl;

		std::cout << "range: 1 - 7 = " << f.getRangeSum(1, 7) << std::endl;
		std::cout << "range: 0 - 7 = " << f.getRangeSum(0, 7) << std::endl;
		std::cout << "range: 4 - 7 = " << f.getRangeSum(4, 7) << std::endl;
		std::cout << "range: 5 - 14 = " << f.getRangeSum(5, 14) << std::endl;

		f.update(2, 10);
		f.print();

		std::cout << "range: 1 - 7 = " << f.getRangeSum(1, 7) << std::endl;
		std::cout << "range: 0 - 7 = " << f.getRangeSum(0, 7) << std::endl;
		std::cout << "range: 4 - 7 = " << f.getRangeSum(4, 7) << std::endl;
		std::cout << "range: 5 - 14 = " << f.getRangeSum(5, 14) << std::endl;
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
}