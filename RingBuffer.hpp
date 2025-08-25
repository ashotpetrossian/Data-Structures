#pragma once

#include <memory>
#include <stdexcept>
#include <iostream>

/**
 * @brief A dynamically resizable Ring Buffer (Circular Buffer) implementation.
 *
 * A ring buffer is a contiguous block of memory treated as circular.
 * When the end is reached, it wraps around to the beginning.
 *
 * This implementation supports **dynamic resizing**, meaning:
 * - When the buffer becomes full, the capacity automatically doubles.
 * - Unlike traditional fixed-size ring buffers, it can grow to accommodate more elements.
 *
 * Why we need it:
 * - Efficient push/pop operations at both ends (O(1) amortized).
 * - Reduces frequent memory allocations by reusing a preallocated array.
 *
 * Real-life applications (simple words):
 * - Audio/video streaming: storing a fixed-size window of samples.
 * - Logging: keeping the last N messages in memory.
 * - Producer-consumer queues: for multithreaded pipelines.
 *
 * Operations and their time complexity:
 * - push_back(T val): Add element to the end, O(1) amortized (O(n) when resizing)
 * - push_front(T val): Add element to the front, O(1) amortized (O(n) when resizing)
 * - pop_back(): Remove element from the end, O(1)
 * - pop_front(): Remove element from the front, O(1)
 * - front(): Access first element, O(1)
 * - back(): Access last element, O(1)
 * - size(): Return number of elements, O(1)
 * - empty(): Check if buffer is empty, O(1)
 */


template <typename T>
class RingBuffer
{
public:
	RingBuffer(long long cap = 0);

	void push_back(T val);
	void push_front(T val);
	void pop_back();
	void pop_front();

	T& front();
	T& back();

	std::size_t size() const { return len; }
	bool empty() const { return len == 0; }

	template <typename U>
	friend std::ostream& operator<<(std::ostream& os, const RingBuffer<U>& buffer);

private:
	void resize();
	std::size_t next(std::size_t index) const { return (index + 1) % capacity; }
	std::size_t prev(std::size_t index) const { return (index + capacity - 1) % capacity; }

private:
	std::size_t len{};
	std::size_t capacity{};
	std::size_t head{};
	std::size_t tail{};

	std::unique_ptr<T[]> data;
};

template <typename T>
RingBuffer<T>::RingBuffer(long long cap)
{
	if (cap < 0) {
		throw std::invalid_argument("Capacity cannot be negative.");
	}

	if (cap > 0) {
		data = std::make_unique<T[]>(cap);
	}

	capacity = cap;
}

template <typename T>
void RingBuffer<T>::resize()
{
	auto newCap = capacity == 0 ? 1 : capacity * 2;
	auto newData = std::make_unique<T[]>(newCap);

	std::size_t ptr{ head };

	for (std::size_t i{}; i < len; ++i) {
		newData[i] = std::move(data[ptr]);
		ptr = next(ptr);
	}

	data = std::move(newData);
	capacity = newCap;
	head = 0;
	tail = len;
}

template <typename T>
void RingBuffer<T>::push_back(T val)
{
	if (capacity == len) resize();

	data[tail] = val;
	tail = next(tail);
	++len;
}

template <typename T>
void RingBuffer<T>::push_front(T val)
{
	if (capacity == len) resize();
	head = prev(head);
	data[head] = val;
	++len;
}

template <typename T>
void RingBuffer<T>::pop_back()
{
	if (empty()) throw std::out_of_range("The buffer is empty.");

	tail = prev(tail);
	--len;
}

template <typename T>
void RingBuffer<T>::pop_front()
{
	if (empty()) throw std::out_of_range("The buffer is empty.");

	head = next(head);
	--len;
}

template <typename T>
T& RingBuffer<T>::front()
{
	if (empty()) throw std::out_of_range("The buffer is empty.");

	return data[head];
}

template <typename T>
T& RingBuffer<T>::back()
{
	if (empty()) throw std::out_of_range("The buffer is empty.");

	return data[prev(tail)];
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const RingBuffer<T>& buffer)
{
	os << "Data (size: " << buffer.size() << ") -> ";
	std::size_t index{ buffer.head };
	for (std::size_t i{}; i < buffer.size(); ++i) {
		os << buffer.data[index] << " ";
		index = buffer.next(index);
	}
	os << "\n";

	return os;
}
