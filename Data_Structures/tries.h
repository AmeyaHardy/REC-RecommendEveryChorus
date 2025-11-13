#ifndef TRIE_H
#define TRIE_H

#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

/**
 * Trie (Prefix Tree) Implementation for Search Autocomplete
 *
 * Time Complexities:
 * - Insert: O(m) where m is length of word
 * - Search: O(m)
 * - Prefix Search: O(m + k) where k is number of results
 * - Delete: O(m)
 *
 * Space Complexity: O(ALPHABET_SIZE * m * n) where n is number of words
 *
 * Used for autocomplete search of songs and artists
 */

struct TrieNode {
    std::unordered_map<char, std::shared_ptr<TrieNode>> children;
    bool is_end_of_word;
    std::string value; // Complete word/ID stored at leaf
    int frequency; // For ranking suggestions

    TrieNode() : is_end_of_word(false), frequency(0) {}
};

class Trie {
private:
    std::shared_ptr<TrieNode> root;

    /**
     * Helper function to collect all words from a given node
     * Time Complexity: O(k) where k is number of words
     */
    void collectAllWords(
        std::shared_ptr<TrieNode> node,
        std::string& current,
        std::vector<std::pair<std::string, int>>& results
    ) const {
        if (!node) return;

        if (node->is_end_of_word) {
            results.push_back({node->value, node->frequency});
        }

        for (auto& it : node->children) {
            auto& ch = it.first;
            auto& child = it.second;
            current.push_back(ch);
            collectAllWords(child, current, results);
            current.pop_back();
        }
    }

    /**
     * Helper for deletion
     * Time Complexity: O(m) where m is word length
     */
    bool deleteHelper(
        std::shared_ptr<TrieNode> node,
        const std::string& word,
        size_t index
    ) {
        if (!node) return false;

        // Base case: reached end of word
        if (index == word.length()) {
            if (!node->is_end_of_word) return false;
            node->is_end_of_word = false;
            node->value = "";
            node->frequency = 0;
            return node->children.empty();
        }

        char ch = std::tolower(word[index]);
        auto it = node->children.find(ch);
        if (it == node->children.end()) {
            return false;
        }

        bool should_delete_child = deleteHelper(it->second, word, index + 1);

        if (should_delete_child) {
            node->children.erase(it);
            return !node->is_end_of_word && node->children.empty();
        }

        return false;
    }

public:
    Trie() : root(std::make_shared<TrieNode>()) {}

    /**
     * Inserts a word into the trie
     * Time Complexity: O(m) where m is word length
     */
    void insert(const std::string& word, const std::string& value = "", int frequency = 1) {
        auto current = root;

        // Convert to lowercase for case-insensitive search
        std::string lower_word;
        for (char ch : word) {
            lower_word.push_back(std::tolower(ch));
        }

        for (char ch : lower_word) {
            if (current->children.find(ch) == current->children.end()) {
                current->children[ch] = std::make_shared<TrieNode>();
            }
            current = current->children[ch];
        }

        current->is_end_of_word = true;
        current->value = value.empty() ? word : value;
        current->frequency += frequency;
    }

    /**
     * Searches for exact word match
     * Time Complexity: O(m) where m is word length
     */
    bool search(const std::string& word) const {
        auto current = root;

        std::string lower_word;
        for (char ch : word) {
            lower_word.push_back(std::tolower(ch));
        }

        for (char ch : lower_word) {
            auto it = current->children.find(ch);
            if (it == current->children.end()) {
                return false;
            }
            current = it->second;
        }

        return current->is_end_of_word;
    }

    /**
     * Checks if any word starts with given prefix
     * Time Complexity: O(m) where m is prefix length
     */
    bool startsWith(const std::string& prefix) const {
        auto current = root;

        std::string lower_prefix;
        for (char ch : prefix) {
            lower_prefix.push_back(std::tolower(ch));
        }

        for (char ch : lower_prefix) {
            auto it = current->children.find(ch);
            if (it == current->children.end()) {
                return false;
            }
            current = it->second;
        }

        return true;
    }

    /**
     * Gets all words with given prefix (autocomplete)
     * Time Complexity: O(m + k) where m=prefix length, k=number of results
     */
    std::vector<std::string> autocomplete(const std::string& prefix, int max_results = 10) const {
        auto current = root;

        std::string lower_prefix;
        for (char ch : prefix) {
            lower_prefix.push_back(std::tolower(ch));
        }

        // Navigate to prefix node
        for (char ch : lower_prefix) {
            auto it = current->children.find(ch);
            if (it == current->children.end()) {
                return {}; // Prefix not found
            }
            current = it->second;
        }

        // Collect all words from this node
        std::vector<std::pair<std::string, int>> results;
        std::string temp = lower_prefix;
        collectAllWords(current, temp, results);

        // Sort by frequency (descending) and then alphabetically
        std::sort(results.begin(), results.end(),
            [](const auto& a, const auto& b) {
                if (a.second != b.second) {
                    return a.second > b.second;
                }
                return a.first < b.first;
            });

        // Extract just the words, limited to max_results
        std::vector<std::string> suggestions;
        for (size_t i = 0; i < results.size() && i < static_cast<size_t>(max_results); ++i) {
            suggestions.push_back(results[i].first);
        }

        return suggestions;
    }

    /**
     * Deletes a word from the trie
     * Time Complexity: O(m) where m is word length
     */
    bool remove(const std::string& word) {
        std::string lower_word;
        for (char ch : word) {
            lower_word.push_back(std::tolower(ch));
        }
        return deleteHelper(root, lower_word, 0);
    }

    /**
     * Increments the frequency count for a word
     * Time Complexity: O(m) where m is word length
     */
    void incrementFrequency(const std::string& word) {
        auto current = root;

        std::string lower_word;
        for (char ch : word) {
            lower_word.push_back(std::tolower(ch));
        }

        for (char ch : lower_word) {
            auto it = current->children.find(ch);
            if (it == current->children.end()) {
                return; // Word not found
            }
            current = it->second;
        }

        if (current->is_end_of_word) {
            current->frequency++;
        }
    }

    /**
     * Clears the entire trie
     * Time Complexity: O(1)
     */
    void clear() {
        root = std::make_shared<TrieNode>();
    }

    /**
     * Checks if trie is empty
     * Time Complexity: O(1)
     */
    bool isEmpty() const {
        return root->children.empty();
    }
};

#endif // TRIE_H
