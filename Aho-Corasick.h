/*
* The Aho-Corasick algorithm is a powerful string searching algorithm that efficiently matches multiple patterns in a given text.
* It combines the strengths of a Trie (prefix tree) for pattern storage and the Knuth-Morris-Pratt (KMP) algorithm
* for efficient pattern matching using failure links.
* The algorithm is widely used in scenarios where multiple keyword searches need to be performed simultaneously,
* such as detecting multiple patterns simultaneously in a large text, counting occurrences of patterns from a dictionary in a text.
* 
* Key Components:
* Trie Construction: A Trie is built where each node represents a prefix of one or more patterns.
* The Trie stores all the patterns, and leaf nodes indicate the end of a pattern.
* 
* Failure Links: Failure links are added to each node to indicate where the search should continue when a mismatch occurs.
* They ensure that the algorithm doesn’t need to restart from the root node but instead
* jumps to the longest suffix-prefix match in the Trie.
*
* Output Links: Output links connect nodes to other nodes representing patterns that are suffixes of the current pattern.
* These links are used to efficiently output all matched patterns, even overlapping ones.
*
*Text Processing: The text is traversed character by character. On a match, the algorithm follows the Trie.
* On a mismatch, it uses failure links to backtrack and continue the search.

Complexity:
Preprocessing (Trie and Failure Links): 𝑂(𝑛), where 𝑛 is the sum of the lengths of all patterns.
Search: 𝑂(𝑚+𝑘), where 𝑚 is the length of the text and 𝑘 is the number of matches found.
*/

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <sstream>

class Aho_Corasick
{
public:
	struct Node
	{
		// A map that stores child nodes for each character, representing the Trie structure.
		std::unordered_map<char, Node*> children;
		Node* parent{ nullptr };
		// Failure link used for backtracking in the Aho-Corasick algorithm.
		Node* failureLink{ nullptr };
		// Output link pointing to the next node with a valid word if the current node is part of multiple patterns.
		Node* outputLink{ nullptr };
		bool isWord{ false };
		char parentChar{ '*' };
		int id{ -1 };
		// The complete pattern (word) associated with this node, if it represents the end of a word.
		std::string pattern{ };

		// for Leetcode: 3213. Construct String with Minimum Cost
		// int len{ -1 };
		// int cost{ std::numeric_limits<int>::max() };


		friend std::ostream& operator<<(std::ostream& os, const Node* node)
		{
			std::string s{ "[" };
			for (auto [c, _] : node->children) {
				s.push_back(c);
				s.push_back(' ');
			}
			if (s.back() != '[') s.pop_back();
			s += "]";

			std::string parent = node->parent ? std::to_string(node->parent->id) + "(" + node->parentChar + ")" : "Null";
			std::string failure = node->failureLink ? std::to_string(node->failureLink->id) : "Null";
			std::string output = node->outputLink ? " outputLink: " + std::to_string(node->outputLink->id) : "";

			os << "Id: " << node->id << " " << s << ", p: " << parent << ", f: " << failure << output;

			return os;
		}
	};

	Aho_Corasick(std::string bigText, std::vector<std::string> listOfWords)
		: text{ std::move(bigText) },
		  words{ std::move(listOfWords) }
	{
		root = new Node{};
		buildTrieTree();
		fillFailureLinks();
	}

	~Aho_Corasick()
	{
		clear();
	}

	void buildTrieTree()
	{
		for (const std::string& word : words) {
			insert(word);
		}

		// Additional function to assign unique id to each node by level for debugging
		assignIds();
	}

	void insert(const std::string& s)
	{
		Node* node{ root };
		for (char c : s) {
			if (!node->children.contains(c)) {
				node->children[c] = new Node{};
			}

			node->children[c]->parent = node;
			node->children[c]->parentChar = c;
			node = node->children[c];
		}

		node->isWord = true;
		node->pattern = s;


		// node->len = s.size();
		// node->cost = std::min(node->cost, c);
	}

	void assignIds()
	{
		std::queue<Node*> q;
		q.push(root);
		int id{};

		while (!q.empty()) {
			auto size = q.size();

			while (size--) {
				Node* node{ q.front() };
				q.pop();

				node->id = id++;

				for (auto [c, nextNode] : node->children) {
					q.push(nextNode);
				}
			}
		}
	}

	void connectFailureLink(Node* node)
	{
		// If the node has no parent (i.e., it's the root), do nothing and return.
		if (!node->parent) return;
		// By default, set the failure link to the root (this applies when no proper suffix is found).
		node->failureLink = root;
		// If the node's parent is the root, there is no need to find a more specific failure link.
		if (node->parent == root) return;

		// Start with the failure link of the node's parent.
		Node* failureLink{ node->parent->failureLink };
		
		// Traverse up the failure links to find the longest suffix match for the current node's parentChar.
		while (failureLink) {
			// Check if the current failure link has a child that matches the parentChar of the node.
			if (failureLink->children.contains(node->parentChar)) {
				// Set the failure link of the node to the matching child node.
				node->failureLink = failureLink->children[node->parentChar];

				// If the failure link node represents a word, set it as the output link for the current node.
				if (node->failureLink->isWord == true) {
					node->outputLink = node->failureLink;
				}
				// Otherwise, inherit the output link from the failure link node.
				else {
					node->outputLink = node->failureLink->outputLink;
				}

				break;
			}

			// Move to the next failure link in the chain.
			failureLink = failureLink->failureLink;
		}
	}

	// This function initializes failure links for all nodes in the Trie using a BFS.
	void fillFailureLinks()
	{
		std::queue<Node*> q;
		q.push(root);

		while (!q.empty()) {
			auto size = q.size();

			while (size--) {
				Node* node{ q.front() };
				q.pop();

				connectFailureLink(node);

				for (auto [c, nextNode] : node->children) {
					q.push(nextNode);
				}
			}
		}
	}

	void findPatterns() // change the name
	{
		Node* node{ root };

		// Iterate over each character in the input text.
		for (int i{}; i < text.size(); ++i) {
			// Check if the current character exists as a child of the current node.
			if (node->children.contains(text[i])) {
				node = node->children[text[i]];
			}
			else {
				// If the character is not a child, follow failure links until a match is found or root is reached.
				while (node->failureLink && !node->children.contains(text[i])) {
					node = node->failureLink;
				}
				// If no match is found even after following failure links, continue with the next character.
				if (!node->children.contains(text[i])) {
					continue;
				}
				else {
					// If a match is found, move to the corresponding child node.
					node = node->children[text[i]];
				}
			}

			// Temporary pointer to traverse through the current node's output links.
			Node* curr{ node };
			// If the current node represents a word, output the word's pattern.
			if (node->isWord) {
				std::cout << "Found word: " << node->pattern << std::endl;
				/*
				int index = i - node->len + 1;
				if (dp[index] != std::numeric_limits<int>::max()) {
					dp[i + 1] = std::min(dp[i + 1], dp[index] + node->cost);
				}
				*/
			}
			// Traverse the output links to find additional patterns that end at the current position.
			while (curr->outputLink) {
				std::cout << "Found word using output link: " << curr->outputLink->pattern << std::endl;
				/*
				int index = i - curr->outputLink->len + 1;
				if (dp[index] != std::numeric_limits<int>::max()) {
					dp[i + 1] = std::min(dp[i + 1], dp[index] + curr->outputLink->cost);
				}
				*/
				
				curr = curr->outputLink;
			}
		}
	}

	void print()
	{
		std::queue<Node*> q;
		q.push(root);

		while (!q.empty()) {
			auto size = q.size();

			while (size--) {
				Node* node{ q.front() };
				q.pop();

				std::cout << node << "\t";

				for (auto [c, nextNode] : node->children) {
					q.push(nextNode);
				}
			}

			std::cout << std::endl;
		}
	}

private:
	void clear()
	{
		std::queue<Node*> q;
		q.push(root);

		while (!q.empty()) {
			Node* node{ q.front() };
			q.pop();

			for (auto [c, nextNode] : node->children) {
				q.push(nextNode);
			}

			node->failureLink = node->outputLink = node->parent = nullptr;
			delete node;
			node = nullptr;
		}
	}

private:
	std::string text;
	std::vector<std::string> words;
	Node* root{};
};
/*
int main()
{
	std::vector<std::string> listOfWords{ "meet", "meat", "eat", "eating", "tiny", "in" };
	std::string bigText{ "I am eating meat" };

	Aho_Corasick obj(bigText, listOfWords);
	obj.findPatterns();
}
*/
