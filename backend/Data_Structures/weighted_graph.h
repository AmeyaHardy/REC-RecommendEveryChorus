#ifndef WEIGHTED_GRAPH_H
#define WEIGHTED_GRAPH_H

#include <unordered_map>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <cmath>
#include <set>

using namespace std;

struct Edge {
    string to;
    double weight;

    Edge(const string& t, double w) : to(t), weight(w) {}

    bool operator<(const Edge& other) const {
        return weight < other.weight; // For min-heap
    }
};

class WeightedGraph {
private:
    // Adjacency list representation
    unordered_map<string, vector<Edge>> adj_list;

public:
    
    void addVertex(const string& user_id) {
        if (adj_list.find(user_id) == adj_list.end()) {
            adj_list[user_id] = vector<Edge>();
        }
    }

    void addEdge(const string& user1, const string& user2, double weight) {
        addVertex(user1);
        addVertex(user2);

        adj_list[user1].push_back(Edge(user2, weight));
        adj_list[user2].push_back(Edge(user1, weight));
    }

    double getEdgeWeight(const string& user1, const string& user2) const {
        auto it = adj_list.find(user1);
        if (it == adj_list.end()) return 0.0;

        for (const auto& edge : it->second) {
            if (edge.to == user2) {
                return edge.weight;
            }
        }
        return 0.0;
    }

    vector<pair<string, double>> getTopKSimilarUsers(
        const string& user_id,
        int k
    ) const {
        auto it = adj_list.find(user_id);
        if (it == adj_list.end()) {
            return {};
        }

        // Use priority queue to get top K (max-heap)
        priority_queue<pair<double, string>> pq;

        for (const auto& edge : it->second) {
            pq.push({edge.weight, edge.to});
        }

        vector<pair<string, double>> result;
        while (!pq.empty() && result.size() < static_cast<size_t>(k)) {
            auto top = pq.top();
            pq.pop();
            result.push_back({top.second, top.first});
        }

        return result;
    }

    static double jaccardSimilarity(
        const set<string>& set1,
        const set<string>& set2
    ) {
        if (set1.empty() && set2.empty()) return 0.0;

        // Count intersection
        int intersection = 0;
        for (const auto& item : set1) {
            if (set2.find(item) != set2.end()) {
                intersection++;
            }
        }

        // Union size = size1 + size2 - intersection
        int union_size = set1.size() + set2.size() - intersection;

        return union_size > 0 ? static_cast<double>(intersection) / union_size : 0.0;
    }

    static double cosineSimilarity(
        const unordered_map<string, int>& vec1,
        const unordered_map<string, int>& vec2
    ) {
        if (vec1.empty() || vec2.empty()) return 0.0;

        double dot_product = 0.0;
        double norm1 = 0.0;
        double norm2 = 0.0;

        // Calculate dot product
        for (const auto& it1 : vec1) {
            auto& song_id = it1.first;
            auto& count1 = it1.second;
            auto it = vec2.find(song_id);
            if (it != vec2.end()) {
                dot_product += count1 * it->second;
            }
            norm1 += count1 * count1;
        }

        for (const auto& it : vec2) {
            auto& song_id = it.first;
            auto& count2 = it.second;
            norm2 += count2 * count2;
        }

        if (norm1 == 0.0 || norm2 == 0.0) return 0.0;

        return dot_product / (sqrt(norm1) * sqrt(norm2));
    }

    vector<pair<string, double>> getNeighbors(const string& user_id) const {
        auto it = adj_list.find(user_id);
        if (it == adj_list.end()) {
            return {};
        }

        vector<pair<string, double>> neighbors;
        for (const auto& edge : it->second) {
            neighbors.push_back({edge.to, edge.weight});
        }

        // Sort by weight descending
        sort(neighbors.begin(), neighbors.end(),
            [](const auto& a, const auto& b) {
                return a.second > b.second;
            });

        return neighbors;
    }

    int getVertexCount() const {
        return adj_list.size();
    }

    int getEdgeCount() const {
        int count = 0;
        for (const auto& it : adj_list) {
            auto& user = it.first;
            auto& edges = it.second;
            count += edges.size();
        }
        return count / 2; // Divide by 2 for undirected graph
    }

    bool hasVertex(const string& user_id) const {
        return adj_list.find(user_id) != adj_list.end();
    }

    vector<string> getAllVertices() const {
        vector<string> vertices;
        for (const auto& it : adj_list) {
            auto& user = it.first;
            auto& score = it.second;
            vertices.push_back(user);
        }
        return vertices;
    }

    void clear() {
        adj_list.clear();
    }
};

#endif
