#include <vector>
#include <iostream>

/*
* Based on the LeetCode 1756. Design Most Recently Used Queue
* Design a queue-like data structure that moves the most recently used element to the end of the queue.
* 
* Implement the MRUQueue class:
* 
* MRUQueue(int n) constructs the MRUQueue with n elements: [1,2,3,...,n].
* int fetch(int k) moves the kth element (1-indexed) to the end of the queue and returns it.
* 
* Constraints: 
* 1 <= n <= 2000
* 1 <= k <= n
* At most 2000 calls will be made to fetch.
*/

/*
* Fenwick Tree:
* 
* Purpose: The Fenwick Tree (Fenwick class) is used to efficiently track the frequency of elements in the queue.
* By maintaining a frequency count for each element, we can efficiently determine the position of the k-th element
* using prefix sums and update the tree as we modify the queue.
* Methods:
* sum(index): This function returns the prefix sum up to the specified index. It helps find the position of the k-th element based on the frequency counts.
* update(i, val): This function updates the frequency of an element at index i. It modifies the Fenwick Tree by adding val at index i.
* This is used when an element is moved within the queue, either being fetched or inserted at the end.
* print(n): This helper function prints the current state of the Fenwick Tree for debugging.
*
* MRUQueue:
* 
* Purpose: The MRUQueue class manages a queue where the most recently used (MRU) element is moved to the end of the queue every time it is accessed.
* The queue is initialized with n elements and each element is initially stored at its respective index. When the k-th element is fetched,
* it is moved to the end of the queue.
* Methods:
* MRUQueue(int n): The constructor initializes the queue with n elements (from 1 to n). It also initializes the Fenwick Tree
* and sets all the frequencies to 1, indicating that all elements are initially present in the queue.
* fetch(int k): This function finds the k-th element in the queue using a binary search over the indices,
* aided by the Fenwick Tree for efficient prefix sum queries. Once the element is found, it is moved to the end of the queue by:
* Decreasing the frequency of the element at index l-1.
* Increasing the frequency at the end of the queue (size).
* The element is then moved to the end of the nums array and the queue size is incremented.
* The function returns the value of the element that was moved to the end.
* 
* Key Points:
* The Fenwick Tree is used to track the frequency counts of each element in the queue and helps us efficiently find the position of the k-th element by querying the prefix sums.
* The binary search is used to identify the position of the k-th element based on these frequency counts.
* After fetching the element, it is moved to the end of the queue by adjusting the frequencies and updating the nums array.
* 
* Time Complexity:
* fetch(k):
* Binary search: O(log n) for finding the k-th element.
* Fenwick Tree updates: Each update (insert()) takes O(log n), so there are two updates per fetch(k) operation.
* Overall, the time complexity of fetch(k) is O(log n).
*/

// Fenwick Tree class for frequency counting
class Fenwick
{
public:
    // Constructor initializes the Fenwick Tree with 'n' elements
    Fenwick(int n) : tree(n + 1) {}

    // Returns the prefix sum up to the given index
    int sum(int index)
    {
        int res{};  // Store the result of the sum
        while (index > 0) {
            res += tree[index];  // Add the value at the current index
            index &= index - 1;   // Move to the parent index
        }
        return res;  // Return the accumulated sum
    }

    // Updates the Fenwick Tree at index 'i' by adding 'val'
    void update(int i, int val)
    {
        ++i;  // Convert to 1-based index
        while (i < tree.size()) {
            tree[i] += val;  // Add 'val' at the current index
            i += (i & -i);   // Move to the next index in the tree
        }
    }

    // Prints the tree values for debugging
    void print(int n)
    {
        for (int i{ 1 }; i < n; ++i) {
            std::cout << tree[i] << " ";  // Print each tree value
        }
        std::cout << std::endl;  // Print a newline at the end
    }

private:
    std::vector<int> tree;  // Vector to hold the Fenwick Tree values
};

// MRUQueue class that uses Fenwick Tree to maintain order
class MRUQueue {
    int size{};  // Size of the current queue
    Fenwick tree;  // Fenwick Tree to track frequencies of elements
    std::vector<int> nums;  // Vector to store the actual queue elements
public:
    // Constructor initializes the queue with 'n' elements
    MRUQueue(int n) : size{ n }, tree(n + 2000), nums(n + 2000) {
        // Initialize the Fenwick Tree and 'nums' array with elements [1, 2, ..., n]
        for (int i{}; i < n; ++i) {
            tree.update(i, 1);  // Each element appears once initially
            nums[i] = i + 1;  // Store the elements in the 'nums' array
        }

        // tree.print(n + 1);  // Print the Fenwick Tree for debugging
    }

    // Fetches the 'k'-th element and moves it to the end of the queue
    int fetch(int k) {
        int l{}, r{ size }, mid{};  // Binary search bounds for finding the 'k'-th element

        // Perform binary search to find the position of the k-th element
        while (l < r) {
            mid = (l + r) / 2;  // Calculate the midpoint
            if (tree.sum(mid) < k) {
                l = mid + 1;  // Move to the right half if the sum is less than 'k'
            }
            else {
                r = mid;  // Move to the left half otherwise
            }
        }

        // Update the Fenwick Tree: Decrease the frequency of the element at index 'l-1'
        tree.update(l - 1, -1);
        // Increase the frequency at the end of the queue (position 'size')
        tree.update(size, 1);

        // Move the found element to the end of the queue
        nums[size] = nums[l - 1];  // Place the element at the end of the 'nums' array
        ++size;  // Increment the size of the queue as an element has been added at the end

        return nums[l - 1];  // Return the fetched element
    }
};

int main()
{
    MRUQueue q(8);
    std::cout << q.fetch(3) << std::endl;
    std::cout << q.fetch(5) << std::endl;
    std::cout << q.fetch(2) << std::endl;
    std::cout << q.fetch(8) << std::endl;
}