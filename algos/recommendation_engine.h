#ifndef RECOMMENDATION_ENGINE_H
#define RECOMMENDATION_ENGINE_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <algorithm>
#include <iostream>
#include "../core/entities.h"
#include "../data_structures/union_find.h"

using namespace std;

class RecommendationEngine {
private:
    // Data structures for each pillar
    KDTree kdtree;
    WeightedGraph user_graph;
    BipartiteGraph bipartite_graph;
    UnionFind communities;
    Trie search_trie;

    // Data storage
    unordered_map<string, Song> song_map;
    unordered_map<string, Artist> artist_map;
    unordered_map<string, User> user_map;
    unordered_map<string, vector<string>> artist_songs;
    unordered_map<string, unordered_map<string, int>> user_song_plays;
    unordered_map<string, set<string>> user_liked_songs;

    // Configuration
    double similarity_threshold = 0.3; // For Union-Find merging
    int k_nearest = 10;
    int k_similar_users = 5;
    int top_artists = 5;

    
    void buildUserSimilarities() {
        vector<string> users;
        for (const auto& it : user_map) {
            auto & uid = it.first;
            users.push_back(uid);
            communities.makeSet(uid);
        }

        // Build weighted graph based on Jaccard similarity
        for (size_t i = 0; i < users.size(); ++i) {
            for (size_t j = i + 1; j < users.size(); ++j) {
                const string& u1 = users[i];
                const string& u2 = users[j];

                // Calculate Jaccard similarity on liked songs
                double similarity = WeightedGraph::jaccardSimilarity(
                    user_liked_songs[u1],
                    user_liked_songs[u2]
                );

                if (similarity > 0.0) {
                    user_graph.addEdge(u1, u2, similarity);

                    // Merge into same community if highly similar
                    if (similarity >= similarity_threshold) {
                        communities.unionSets(u1, u2);
                    }
                }
            }
        }

        cout << "Built user similarity graph with " << user_graph.getEdgeCount()
                  << " edges across " << users.size() << " users." << endl;
        cout << "Formed " << communities.getNumberOfCommunities()
                  << " taste communities." << endl;
    }

public:
    RecommendationEngine() {}

    void initialize(
        const vector<Song>& songs,
        const vector<Artist>& artists,
        const vector<User>& users,
        const vector<UserSongInteraction>& song_interactions,
        const vector<UserArtistInteraction>& artist_interactions
    ) {
        cout << "\n=== Initializing Recommendation Engine ===" << endl;

        // Build maps
        for (const auto& song : songs) {
    song_map[song.song_id] = song;
    artist_songs[song.artist_id].push_back(song.song_id);

    // Insert ONLY the title into the trie
    search_trie.insert(song.title);
}

for (const auto& artist : artists) {
    artist_map[artist.artist_id] = artist;

    // Insert ONLY the artist name
    search_trie.insert(artist.artist_name);
}


        for (const auto& user : users) {
            user_map[user.user_id] = user;
        }

        // Process interactions
        for (const auto& interaction : song_interactions) {
            user_song_plays[interaction.user_id][interaction.song_id] = interaction.play_count;

            if (interaction.liked) {
                user_liked_songs[interaction.user_id].insert(interaction.song_id);
                user_map[interaction.user_id].liked_songs.push_back(interaction.song_id);
            }

            // Increment search frequency for popular songs
            if (interaction.play_count > 10) {
                auto it = song_map.find(interaction.song_id);
                if (it != song_map.end()) {
                    search_trie.incrementFrequency(it->second.title);
                }
            }
        }

      cout << "\n[Pillar 2 & 4] Building user similarity graph and taste communities..." << endl;
        buildUserSimilarities();

        
//Pillar 2
    vector<Recommendation> getUserCollaborativeRecommendations(
        const string& user_id,
        int num_recommendations = 10
    ) {
        vector<Recommendation> recommendations;

        // Get top K similar users
        auto similar_users = user_graph.getTopKSimilarUsers(user_id, k_similar_users);

        if (similar_users.empty()) {
            return recommendations;
        }

        // Aggregate songs liked by similar users
        unordered_map<string, double> song_scores;
        auto& current_user_likes = user_liked_songs[user_id];

        for (const auto& it : similar_users) {
            auto& similar_user_id = it.first;
            auto& similarity = it.second;
            auto& similar_user_likes = user_liked_songs[similar_user_id];

            for (const auto& song_id : similar_user_likes) {
                // Don't recommend already liked songs
                if (current_user_likes.find(song_id) == current_user_likes.end()) {
                    song_scores[song_id] += similarity;
                }
            }
        }

        // Convert to recommendations
        for (const auto& it : song_scores) {
            auto& song_id = it.first;
            auto& score = it.second;
            auto song_it = song_map.find(song_id);
            if (song_it != song_map.end()) {
                recommendations.push_back(
                    Recommendation(song_id, song_it->second.title, score, "user-collab")
                );
            }
        }

        // Sort by score
        sort(recommendations.begin(), recommendations.end());

        if (recommendations.size() > static_cast<size_t>(num_recommendations)) {
            recommendations.resize(num_recommendations);
        }

        return recommendations;
    }
        

//Pillar 4
      vector<Recommendation> getCommunityBasedRecommendations(
        const string& user_id,
        int num_recommendations = 10
    ) {
        vector<Recommendation> recommendations;

        // Get community members
        auto community_members = communities.getCommunityMembers(user_id);

        if (community_members.size() <= 1) {
            return recommendations; // No other members
        }

        // Aggregate popular songs in community
        unordered_map<string, double> song_scores;
        auto& current_user_likes = user_liked_songs[user_id];

        for (const auto& member_id : community_members) {
            if (member_id == user_id) continue;

            auto& member_likes = user_liked_songs[member_id];

            for (const auto& song_id : member_likes) {
                if (current_user_likes.find(song_id) == current_user_likes.end()) {
                    song_scores[song_id] += 1.0;
                }
            }
        }

        // Convert to recommendations
        for (const auto& it : song_scores) {
            auto& song_id = it.first;
            auto& score = it.second;
            auto song_it = song_map.find(song_id);
            if (song_it != song_map.end()) {
                recommendations.push_back(
                    Recommendation(song_id, song_it->second.title, score, "community")
                );
            }
        }

        // Sort by score
        sort(recommendations.begin(), recommendations.end());

        if (recommendations.size() > static_cast<size_t>(num_recommendations)) {
            recommendations.resize(num_recommendations);
        }

        return recommendations;
    }

    string getUserCommunity(const string& user_id) {
        return communities.getCommunity(user_id);
    }

    int getCommunitySize(const string& user_id) {
        return communities.getCommunitySize(user_id);
    }

    UnionFind& getCommunities() { return communities; }
};

  #endif
