#ifndef UNION_FIND_H
#define UNION_FIND_H

#include <unordered_map>
#include <string>
#include <vector>

using namespace std;

class UnionFind {
private:
    unordered_map<string, string> parent;
    unordered_map<string, int> rank;
    unordered_map<string, int> set_size;

public:
    string findConst(const string& user_id) const {
        auto it = parent.find(user_id);
        if (it == parent.end()) return string();
        if (it->second == user_id) return it->second;
        return findConst(it->second);
    }

    void makeSet(const string& user_id) {
        if (parent.find(user_id) == parent.end()) {
            parent[user_id] = user_id;
            rank[user_id] = 0;
            set_size[user_id] = 1;
        }
    }

    string find(const string& user_id) {
        if (parent.find(user_id) == parent.end()) {
            makeSet(user_id);
        }

        // Path compression: make every node point directly to root
        if (parent[user_id] != user_id) {
            parent[user_id] = find(parent[user_id]);
        }

        return parent[user_id];
    }

    bool unionSets(const string& user1, const string& user2) {
        string root1 = find(user1);
        string root2 = find(user2);

        // Already in same set
        if (root1 == root2) {
            return false;
        }

        // Union by rank: attach smaller tree under larger tree
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


    bool connected(const string& user1, const string& user2) {
        return find(user1) == find(user2);
    }

    string getCommunity(const string& user_id) {
        return find(user_id);
    }

    int getCommunitySize(const string& user_id) {
        string root = find(user_id);
        return set_size[root];
    }

    vector<string> getCommunityMembers(const string& user_id) {
        string target_root = find(user_id);
        vector<string> members;

        for (const auto& it : parent) {
            auto & uid = it.first;
            if (find(uid) == target_root) {
                members.push_back(uid);
            }
        }

        return members;
    }

    unordered_map<string, vector<string>> getAllCommunities() const{
        unordered_map<string, vector<string>> communities;

        for (const auto& p : parent) {
        string root = findConst(p.first);
        communities[root].push_back(p.first);
        }

        return communities;
    }


    int getNumberOfCommunities() const{
        return getAllCommunities().size();
    }

    void clear() {
        parent.clear();
        rank.clear();
        set_size.clear();
    }
};

#endif
