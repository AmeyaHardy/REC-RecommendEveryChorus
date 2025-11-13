#ifndef BIPARTITE_GRAPH_H
#define BIPARTITE_GRAPH_H

#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

struct BipartiteEdge {
      string to;
    double weight; // Normalized play count

    BipartiteEdge(const   string& t, double w) : to(t), weight(w) {}
};

class BipartiteGraph {
private:
    // User -> Artists adjacency
      unordered_map<  string,   vector<BipartiteEdge>> user_to_artist;

    // Artist -> Users adjacency
      unordered_map<  string,   vector<BipartiteEdge>> artist_to_user;

    // Max play counts for normalization
      unordered_map<  string, int> max_plays_per_user;

public:
    
    void addEdge(const   string& user_id, const   string& artist_id, int play_count) {
        // Update max play count for user
        if (max_plays_per_user.find(user_id) == max_plays_per_user.end()) {
            max_plays_per_user[user_id] = play_count;
        } else {
            max_plays_per_user[user_id] = max(max_plays_per_user[user_id], play_count);
        }

        // Store raw play count (will normalize later)
        user_to_artist[user_id].push_back(BipartiteEdge(artist_id, play_count));
        artist_to_user[artist_id].push_back(BipartiteEdge(user_id, play_count));
    }

    void normalizeWeights() {
        // Normalize user->artist edges
        for (auto& it : user_to_artist) {
            auto& user_id = it.first;
            auto& edges = it.second;
            double max_plays = static_cast<double>(max_plays_per_user[user_id]);
            if (max_plays > 0) {
                for (auto& edge : edges) {
                    edge.weight = edge.weight / max_plays;
                }
            }
        }

        // Normalize artist->user edges
        for (auto& it : artist_to_user) {
            auto& artist_id = it.first;
            auto& edges = it.second;
            for (auto& edge : edges) {
                double max_plays = static_cast<double>(max_plays_per_user[edge.to]);
                if (max_plays > 0) {
                    edge.weight = edge.weight / max_plays;
                }
            }
        }
    }

    vector<pair<string, double>> getTopArtistsForUser(
        const string& user_id,
        int k
    ) const {
        auto it = user_to_artist.find(user_id);
        if (it == user_to_artist.end()) {
            return {};
        }

        vector<pair<string, double>> artists;
        for (const auto& edge : it->second) {
            artists.push_back({edge.to, edge.weight});
        }

        // Sort by weight descending
        sort(artists.begin(), artists.end(),
            [](const auto& a, const auto& b) {
                return a.second > b.second;
            });

        // Return top K
        if (artists.size() > static_cast<size_t>(k)) {
            artists.resize(k);
        }

        return artists;
    }

    vector<pair<string, double>> getArtistsForUser(
        const string& user_id
    ) const {
        auto it = user_to_artist.find(user_id);
        if (it == user_to_artist.end()) {
            return {};
        }

        vector<pair<string, double>> artists;
        for (const auto& edge : it->second) {
            artists.push_back({edge.to, edge.weight});
        }

        return artists;
    }

    vector<pair<string, double>> getUsersForArtist(
        const string& artist_id
    ) const {
        auto it = artist_to_user.find(artist_id);
        if (it == artist_to_user.end()) {
            return {};
        }

        vector<pair<string, double>> users;
        for (const auto& edge : it->second) {
            users.push_back({edge.to, edge.weight});
        }

        // Sort by weight descending
        sort(users.begin(), users.end(),
            [](const auto& a, const auto& b) {
                return a.second > b.second;
            });

        return users;
    }

    double getEdgeWeight(const string& user_id, const string& artist_id) const {
        auto it = user_to_artist.find(user_id);
        if (it == user_to_artist.end()) {
            return 0.0;
        }

        for (const auto& edge : it->second) {
            if (edge.to == artist_id) {
                return edge.weight;
            }
        }

        return 0.0;
    }

    vector<pair<string, double>> recommendFromTopArtists(
        const string& user_id,
        const unordered_map<string, vector<string>>& artist_songs,
        int top_artists = 5
    ) const {
        auto top_artists_list = getTopArtistsForUser(user_id, top_artists);

        unordered_map<string, double> song_scores;

        // Collect songs from top artists, weighted by artist preference
        for (const auto& it1 : top_artists_list) {
            auto& artist_id = it1.first;
            auto& weight = it1.second;
            auto it = artist_songs.find(artist_id);
            if (it != artist_songs.end()) {
                for (const auto& song_id : it->second) {
                    song_scores[song_id] += weight;
                }
            }
        }

        // Convert to vector and sort
        vector<pair<string, double>> recommendations;
        for (const auto& it : song_scores) {
            auto& song_id = it.first;
            auto& score = it.second;
            recommendations.push_back({song_id, score});
        }

        sort(recommendations.begin(), recommendations.end(),
            [](const auto& a, const auto& b) {
                return a.second > b.second;
            });

        return recommendations;
    }

    int getUserCount() const {
        return user_to_artist.size();
    }

    int getArtistCount() const {
        return artist_to_user.size();
    }

    int getEdgeCount() const {
        int count = 0;
        for (const auto& it : user_to_artist) {
            auto& user = it.first;
            auto& edges = it.second;
            count += edges.size();
        }
        return count;
    }

    bool hasUser(const   string& user_id) const {
        return user_to_artist.find(user_id) != user_to_artist.end();
    }

    bool hasArtist(const   string& artist_id) const {
        return artist_to_user.find(artist_id) != artist_to_user.end();
    }

    void clear() {
        user_to_artist.clear();
        artist_to_user.clear();
        max_plays_per_user.clear();
    }
};

#endif
