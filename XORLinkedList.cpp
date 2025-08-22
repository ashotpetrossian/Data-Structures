/**
 * @file XORLinkedList.hpp
 * @brief Implementation of an XOR linked list (memory-efficient doubly linked list).
 *
 * XORLinkedList is a C++ template container that stores elements in a
 * doubly-linked list-like structure using XOR pointers. Each node stores
 * a single pointer that is the XOR of its previous and next node addresses,
 * which reduces memory usage compared to a classic doubly linked list.
 *
 * Main features implemented:
 *  - push_front / push_back, pop_front / pop_back
 *  - insert / erase by position
 *  - front() / back(), empty(), getSize()
 *  - copy constructor and copy assignment
 *  - move constructor and move assignment
 *  - full support for bidirectional iteration using a nested Iterator class
 *  - range-based for loop support
 *
 * Extensibility:
 *  - Can add random access operations (at, operator[]), find, reverse, sort
 *  - Can add serialization/deserialization (e.g., to/from std::vector)
 *
 * Note:
 *  - Iterators are bidirectional. They can traverse forward and backward.
 *  - Dereferencing an iterator at end() is undefined (similar to std::list).
 *  - XORLinkedList manages memory automatically but does not support concurrent
 *    modification during iteration.
 *
 * Example usage:
 * @code
 * XORLinkedList<int> ll;
 * ll.push_back(1);
 * ll.push_front(0);
 * for (auto& val : ll) std::cout << val << " ";
 * @endcode
 */



#include <iostream>
#include <cstdint> // for uintptr_t
#include <iterator>
#include <vector>
#include <algorithm>

template <typename T>
class XORLinkedList
{
	struct Node
	{
		T val;
		Node* both{};
	};

	Node* head{}, *tail{};
	std::size_t size{};

	static Node* XOR(Node* a, Node* b)
	{
		return reinterpret_cast<Node*>(reinterpret_cast<uintptr_t>(a) ^ reinterpret_cast<uintptr_t>(b));
	}

	class Iterator
	{
	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = T*;
		using reference = T&;

	private:
		Node* node{};
		Node* prev{};

	public:
		Iterator(Node* c, Node* p) : node{c}, prev{p} {}

		reference operator*() const { return node->val; }
		pointer operator->() const { return &(node->val); }

		Iterator& operator++()
		{
			Node* next = XOR(node->both, prev);
			prev = node;
			node = next;

			return *this;
		}

		Iterator operator++(int)
		{
			Iterator tmp{*this};
			++(*this);
			
			return tmp;
		}

		Iterator& operator--()
		{
			if (node == nullptr) {
				node = prev;
				prev = node ? XOR(node->both, nullptr) : nullptr;
			}
			else if (prev == nullptr) {
				node = nullptr;
			}
			else {
				Node* prevprev = XOR(prev->both, node);
				node = prev;
				prev = prevprev;
			}

			return *this;
		}

		Iterator operator--(int)
		{
			Iterator tmp{*this};
			--(*this);
			
			return tmp;
		}

		bool operator==(const Iterator& other) const { return node == other.node; }
		bool operator!=(const Iterator& other) const { return !(*this == other); }
	};

public:
	XORLinkedList() = default;

	XORLinkedList(const XORLinkedList& other) {
		head = tail = nullptr;
		size = 0;
		Node* prev = nullptr;
		for (Node* curr = other.head; curr != nullptr; ) {
			push_back(curr->val);
			Node* next = XOR(curr->both, prev);
			prev = curr;
			curr = next;
		}
	}

	XORLinkedList& operator=(const XORLinkedList& other) {
		if (this != &other) {
			clean();
			Node* prev{ nullptr };
			for (Node* curr = other.head; curr != nullptr; ) {
				push_back(curr->val);
				Node* next = XOR(curr->both, prev);
				prev = curr;
				curr = next;
			}
		}

		return *this;
	}

	XORLinkedList(XORLinkedList&& other) noexcept
		: head(other.head), tail(other.tail), size(other.size) 
	{
		other.head = other.tail = nullptr;
		other.size = 0;
	}

	XORLinkedList& operator=(XORLinkedList&& other) noexcept {
		if (this != &other) {
			clean();
			head = other.head;
			tail = other.tail;
			size = other.size;

			other.head = other.tail = nullptr;
			other.size = 0;
		}
		return *this;
	}

	~XORLinkedList()
	{
		clean();
	}

	void clean()
	{
		Node* prev{ nullptr };
		while (head != nullptr) {
			auto next = XOR(head->both, prev);
			prev = head;
			delete head;
			head = next;
		}

		head = tail = nullptr;
		size = 0;
	}

	
	// Iterator related
	Iterator begin() { return Iterator{head, nullptr}; }
	Iterator end() { return Iterator{nullptr, tail}; }
	Iterator rbegin() { return std::prev(end()); }
	Iterator rend() { return std::prev(begin()); }

	// Non-Modifiers
	std::size_t getSize() const { return size; }

	friend std::ostream& operator<<(std::ostream& os, const XORLinkedList& list)
	{
		Node* curr{ list.head };
		Node* prev{ nullptr };

		while (curr != nullptr) {
			os << curr->val << " ";
			auto next = XOR(curr->both, prev);
			prev = curr;
			curr = next;
		}

		return os;
	}

	T front() const {
		if (size == 0) throw std::out_of_range("XORLinkedList is empty.");
		return head->val;
	}
	
	T back() const {
		if (size == 0) throw std::out_of_range("XORLinkedList is empty.");
		return tail->val;
	}

	bool empty() const { return size == 0; }

	// Modifiers
	void push_back(T val)
	{
		auto node = new Node{ val };
		if (head == nullptr) {
			head = tail = node;
		}
		else {
			node->both = tail;
			tail->both = XOR(tail->both, node);
			tail = node;
		}

		++size;
	}

	void push_front(T val)
	{
		auto node = new Node{ val };
		if (head == nullptr) {
			head = tail = node;
		}
		else {
			node->both = head;
			head->both = XOR(head->both, node);
			head = node;
		}

		++size;
	}

	void pop_back()
	{
		if (head == nullptr) {
			throw std::out_of_range("XORLinkedList is empty.");
		}

		if (head == tail) {
			delete tail;
			head = tail = nullptr;
		}
		else {
			auto prev = tail->both;
			prev->both = XOR(XOR(prev->both, tail), nullptr);
			delete tail;
			tail = prev;
		}

		--size;
	}

	void pop_front()
	{
		if (head == nullptr) {
			throw std::out_of_range("XORLinkedList is empty.");
		}

		if (head == tail) {
			delete tail;
			head = tail = nullptr;
		}
		else {
			auto next = head->both;
			next->both = XOR(XOR(next->both, head), nullptr);
			delete head;
			head = next;
		}

		--size;
	}

	void insert(T val, int pos)
	{
		if (pos < 0 || pos > size) {
			throw std::out_of_range("Invalid position for insertion.");
		}

		if (pos == 0) {
			push_front(val);
			return;
		}
		else if (pos == size) {
			push_back(val);
			return;
		}
		Node* prev{ nullptr };
		Node* curr{ head };
		while (pos--) {
			auto next = XOR(curr->both, prev);
			prev = curr;
			curr = next;
		}

		auto node = new Node{ val };
		node->both = XOR(curr, prev);
		
		prev->both = XOR(XOR(prev->both, curr), node);
		curr->both = XOR(XOR(curr->both, prev), node);

		++size;
	}

	void erase(int pos)
	{
		if (pos < 0 || pos >= size) {
			throw std::out_of_range("Invalid position for insertion.");
		}

		if (pos == 0) {
			pop_front();
			return;
		}
		else if (pos == size - 1) {
			pop_back();
			return;
		}

		Node* prev{ nullptr };
		Node* curr{ head };
		while (pos--) {
			auto next = XOR(curr->both, prev);
			prev = curr;
			curr = next;
		}

		auto next = XOR(curr->both, prev);
		next->both = XOR(XOR(next->both, curr), prev);
		prev->both = XOR(XOR(prev->both, curr), next);

		delete curr;
		--size;
	}

	void clear() { clean(); }

	// Extandable. Can be added: at, operator[], assign, resize, swap, find, reverse, sort. Seriliazation like to vector, etc..
};

int main()
{
	XORLinkedList<int> ll;
	for (int i : {1, 2, 3}) ll.push_back(i);
	for (int i : {6, 5}) ll.push_front(i);

	std::cout << ll << std::endl;

	for (int i : ll) std::cout << i << " ";
	std::cout << std::endl;
	
	
	for (auto it = ll.rbegin(); it != ll.rend(); --it) {
		*it += 10;
		std::cout << *it << " ";
	}
	std::cout << std::endl;


	auto l = [](int val) {
		std::cout << val << " ";
	};

	std::for_each(ll.begin(), ll.end(), l);

	std::cout<< "\nmoving\n";
	XORLinkedList<int> mv{};
	mv = std::move(ll);
	std::cout << mv << std::endl;
	std::cout << ll << std::endl;
}