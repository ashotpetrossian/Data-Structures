#include <iostream>
#include <vector>
#include <queue>

class SegmentTreeRMQ
{
public:
	SegmentTreeRMQ(const std::vector<int>& data) : m_data{data}
	{
		size_t treeSize{ getNextPowerOf2(data.size()) * 2 - 1 };

		std::cout << "initial size: " << data.size() << ", next: " << treeSize << std::endl;

		m_segTreeArray.resize(treeSize);
	}

	void constructSegTree(int low, int high, int pos)
	{
		if (low > high) return;
		if (low == high) {
			m_segTreeArray[pos] = m_data[low];
			return;
		}

		int mid{ (high - low) / 2 + low };
		constructSegTree(low, mid, 2 * pos + 1);
		constructSegTree(mid + 1, high, 2 * pos + 2);

		m_segTreeArray[pos] = std::min(m_segTreeArray[2 * pos + 1], m_segTreeArray[2 * pos + 2]);
	}

	void print() const noexcept
	{
		//for (int i : m_segTreeArray) std::cout << i << " ";
		//std::cout << std::endl;

		int n = m_segTreeArray.size();
		std::queue<int> q;
		q.push(0);

		while (!q.empty()) {
			int size = q.size();

			while (size--) {
				auto index{ q.front() };
				q.pop();

				std::cout << m_segTreeArray[index] << " ";

				if (index * 2 + 1 < n) q.push(index * 2 + 1);
				if (index * 2 + 2 < n) q.push(index * 2 + 2);
			}

			std::cout << std::endl;
		}
	}

private:
	size_t getNextPowerOf2(size_t n) const noexcept
	{
		return pow(2, std::ceil(std::log2(n + 1)));
	}

private:
	std::vector<int> m_data;
	std::vector<int> m_segTreeArray;
};

int main()
{
	std::vector<int> nums{ 1, 3, 5, 7, 9, 11 };
	SegmentTreeRMQ segTree(nums);

	segTree.constructSegTree(0, nums.size() - 1, 0);
	segTree.print();
}
