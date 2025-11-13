#ifndef KDTREE_H
#define KDTREE_H

#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include "../core/entities.h"

using namespace std;

struct KDNode {
    vector<double> point;
    string song_id;
    shared_ptr<KDNode> left;
    shared_ptr<KDNode> right;
    int depth;

    KDNode(const vector<double>& p, const string& id, int d)
        : point(p), song_id(id), depth(d), left(nullptr), right(nullptr) {}
};

class KDTree {
private:
    shared_ptr<KDNode> root;
    int dimensions; // 4 dimensions: [bpm, energy, danceability, valence]

    shared_ptr<KDNode> buildTree(
         vector< pair< vector<double>,  string>>& points,
        int depth
    ) {
        if (points.empty()) return nullptr;

        int axis = depth % dimensions;

        // Sort points by current axis
        // Time: O(n log n)
        sort(points.begin(), points.end(),
            [axis](const auto& a, const auto& b) {
                return a.first[axis] < b.first[axis];
            });

        // Select median as pivot
        size_t median = points.size() / 2;
        auto node =  make_shared<KDNode>(
            points[median].first,
            points[median].second,
            depth
        );

        // Recursively build left and right subtrees
        vector<pair<vector<double>, string>> left_points(
            points.begin(), points.begin() + median
        );
        vector<pair<vector<double>, string>> right_points(
            points.begin() + median + 1, points.end()
        );

        node->left = buildTree(left_points, depth + 1);
        node->right = buildTree(right_points, depth + 1);

        return node;
    }

    double distance(const vector<double>& a, const vector<double>& b) const {
        double sum = 0.0;
        for (size_t i = 0; i < a.size(); ++i) {
            double diff = a[i] - b[i];
            sum += diff * diff;
        }
        return sqrt(sum);
    }

    void knnSearch(
        const shared_ptr<KDNode>& node,
        const vector<double>& target,
        int k,
        vector<pair<double, string>>& best
    ) const {
        if (!node) return;

        double dist = distance(node->point, target);

        // Add current node to best list
        if (best.size() < static_cast<size_t>(k)) {
            best.push_back({dist, node->song_id});
            push_heap(best.begin(), best.end());
        } else if (dist < best.front().first) {
            pop_heap(best.begin(), best.end());
            best.pop_back();
            best.push_back({dist, node->song_id});
            push_heap(best.begin(), best.end());
        }

        int axis = node->depth % dimensions;
        double diff = target[axis] - node->point[axis];

        // Determine which subtree to explore first
        auto first = diff < 0 ? node->left : node->right;
        auto second = diff < 0 ? node->right : node->left;

        knnSearch(first, target, k, best);

        // Check if we need to explore the other subtree
        if (best.size() < static_cast<size_t>(k) ||
            abs(diff) < best.front().first) {
            knnSearch(second, target, k, best);
        }
    }

public:
    KDTree(int dims = 4) : root(nullptr), dimensions(dims) {}

    void build(const vector<Song>& songs) {
        vector<pair<vector<double>, string>> points;
        points.reserve(songs.size());

        for (const auto& song : songs) {
            points.push_back({song.getFeatureVector(), song.song_id});
        }

        root = buildTree(points, 0);
    }

     vector< pair<double,  string>> findKNearest(
        const  vector<double>& target_features,
        int k
    ) const {
         vector< pair<double,  string>> best;
        knnSearch(root, target_features, k, best);

        // Sort results by distance (ascending)
         sort(best.begin(), best.end());
        return best;
    }

     vector< string> findSimilarSongs(
        const Song& song,
        int k,
        const  string& exclude_song_id = ""
    ) const {
        auto features = song.getFeatureVector();
        auto nearest = findKNearest(features, k + 1);

         vector< string> similar;
        for (const auto& it : nearest) {
            auto& dist = it.first;
            auto& id = it.second;
            if (id != song.song_id && id != exclude_song_id) {
                similar.push_back(id);
                if (similar.size() >= static_cast<size_t>(k)) break;
            }
        }

        return similar;
    }

    bool isEmpty() const {
        return root == nullptr;
    }
};

#endif 
