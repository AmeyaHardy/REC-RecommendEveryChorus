#ifndef UNION_FIND_H
#define UNION_FIND_H

#include <unordered_map>
#include <string>
#include <vector>

class UnionFind {
private:
    std::unordered_map<std::string, std::string> parent;
    std::unordered_map<std::string, int> rank;
    std::unordered_map<std::string, int> set_size;

public:
    // Time Complexity: O(1)
    void makeSet(const std::string& user_id) {
        if (parent.find(user_id) == parent.end()) {
            parent[user_id] = user_id;
            rank[user_id] = 0;
            set_size[user_id] = 1;
        }
    }

    //Time Complexity: O(α(n)) ≈ O(1) amortized
    std::string find(const std::string& user_id) {
        if (parent.find(user_id) == parent.end()) {
            makeSet(user_id);
        }

        if (parent[user_id] != user_id) {
            parent[user_id] = find(parent[user_id]);
        }

        return parent[user_id];
    }

    //Time Complexity: O(α(n)) ≈ O(1) amortized
    bool unionSets(const std::string& user1, const std::string& user2) {
        std::string root1 = find(user1);
        std::string root2 = find(user2);

        // Already in same set
        if (root1 == root2) {
            return false;
        }

        // Union by rank
        if (rank[root1] < rank[root2]) {
            parent[root1] = root2;
            set_size[root2] += set_size[root1];
        } else if (rank[root1] > rank[root2]) {
            parent[root2] = root1;
            set_size[root1] += set_size[root2];
        } else {
            parent[root2] = root1;
            set_size[root1] += set_size[root2];
            rank[root1]++;
        }

        return true;
    }

    //Time Complexity: O(α(n)) ≈ O(1)
    bool connected(const std::string& user1, const std::string& user2) {
        return find(user1) == find(user2);
    }

    //Time Complexity: O(α(n)) ≈ O(1)
    std::string getCommunity(const std::string& user_id) {
        return find(user_id);
    }

    //Time Complexity: O(α(n)) ≈ O(1)
    int getCommunitySize(const std::string& user_id) {
        std::string root = find(user_id);
        return set_size[root];
    }

    //Time Complexity: O(n)
    std::vector<std::string> getCommunityMembers(const std::string& user_id) {
        std::string target_root = find(user_id);
        std::vector<std::string> members;

        for (const auto& [uid, _] : parent) {
            if (find(uid) == target_root) {
                members.push_back(uid);
            }
        }

        return members;
    }

    std::unordered_map<std::string, std::vector<std::string>> getAllCommunities() {
        std::unordered_map<std::string, std::vector<std::string>> communities;

        for (const auto& [uid, _] : parent) {
            std::string root = find(uid);
            communities[root].push_back(uid);
        }

        return communities;
    }

    int getNumberOfCommunities() {
        return getAllCommunities().size();
    }

    void clear() {
        parent.clear();
        rank.clear();
        set_size.clear();
    }
};

#endif // UNION_FIND_H
