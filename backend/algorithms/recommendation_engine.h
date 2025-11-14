#ifndef RECOMMENDATION_ENGINE_H
#define RECOMMENDATION_ENGINE_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <algorithm>
#include <iostream>
#include "../core/entities.h"
#include "../data_structures/kdtree.h"
#include "../data_structures/weighted_graph.h"
#include "../data_structures/bipartite_graph.h"
#include "../data_structures/union_find.h"
#include "../data_structures/trie.h"

/**
 * Recommendation Engine - Combines All 4 Pillars
 *
 * Pillar 1: Content-Based Filtering (K-D Tree)
 * Pillar 2: User-User Collaborative Filtering (Weighted Graph)
 * Pillar 3: Artist-Based Collaborative Filtering (Bipartite Graph)
 * Pillar 4: Taste Communities (Union-Find)
 *
 * Time Complexity for generateRecommendations:
 * O(k log n + E log k + A*S + C*U)
 * where n=songs, k=neighbors, E=user edges, A=artists, S=songs per artist, C=community, U=users
 */

class RecommendationEngine {
private:
    // Data structures for each pillar
    KDTree kdtree;
    WeightedGraph user_graph;
    BipartiteGraph bipartite_graph;
    UnionFind communities;
    Trie search_trie;

    // Data storage
    std::unordered_map<std::string, Song> song_map;
    std::unordered_map<std::string, Artist> artist_map;
    std::unordered_map<std::string, User> user_map;
    std::unordered_map<std::string, std::vector<std::string>> artist_songs;
    std::unordered_map<std::string, std::unordered_map<std::string, int>> user_song_plays;
    std::unordered_map<std::string, std::set<std::string>> user_liked_songs;

    // Configuration
    double similarity_threshold = 0.3; // For Union-Find merging
    int k_nearest = 10;
    int k_similar_users = 5;
    int top_artists = 5;


    /**
     * Builds similarity between all users and merges communities
     * Time Complexity: O(U^2 * S) where U=users, S=songs per user
     */
    void buildUserSimilarities() {
        std::vector<std::string> users;
        for (const auto& [uid, _] : user_map) {
            users.push_back(uid);
            communities.makeSet(uid);
        }

        // Build weighted graph based on Jaccard similarity
        for (size_t i = 0; i < users.size(); ++i) {
            for (size_t j = i + 1; j < users.size(); ++j) {
                const std::string& u1 = users[i];
                const std::string& u2 = users[j];

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

        std::cout << "Built user similarity graph with " << user_graph.getEdgeCount()
                  << " edges across " << users.size() << " users." << std::endl;
        std::cout << "Formed " << communities.getNumberOfCommunities()
                  << " taste communities." << std::endl;
    }

public:
    RecommendationEngine() {}

    /**
     * Initializes the recommendation engine with all data
     * Time Complexity: O(n log n + U^2*S + A*U)
     */

     // in RecommendationEngine (public)
    int getKNearest() const { return k_nearest; }
    int getKSimilarUsers() const { return k_similar_users; }


    void normalizePillarScores(std::vector<Recommendation>& recs, double pillarMax) {
        if (recs.empty()) return;

        if (pillarMax <= 0.0) return;

        for (auto &r : recs) {
            r.score = (r.score / pillarMax) * 10.0;
            if (r.score > 10.0) r.score = 10.0;   // clamp upper bound
        }
    }

    void initialize(
        const std::vector<Song>& songs,
        const std::vector<Artist>& artists,
        const std::vector<User>& users,
        const std::vector<UserSongInteraction>& song_interactions,
        const std::vector<UserArtistInteraction>& artist_interactions
    ) {
        std::cout << "\n=== Initializing Recommendation Engine ===" << std::endl;

        // Build maps
        for (const auto& song : songs) {
            song_map[song.song_id] = song;
            artist_songs[song.artist_id].push_back(song.song_id);

            // Add to search trie
            search_trie.insert(song.title, song.song_id, 1);
        }

        for (const auto& artist : artists) {
            artist_map[artist.artist_id] = artist;
            search_trie.insert(artist.artist_name, artist.artist_id, 1);
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

        // PILLAR 1: Build K-D Tree for content-based filtering
        std::cout << "\n[Pillar 1] Building K-D Tree for content-based filtering..." << std::endl;
        kdtree.build(songs);
        std::cout << "K-D Tree built with " << songs.size() << " songs." << std::endl;

        // PILLAR 2 & 4: Build user-user graph and taste communities
        std::cout << "\n[Pillar 2 & 4] Building user similarity graph and taste communities..." << std::endl;
        buildUserSimilarities();

        // PILLAR 3: Build bipartite user-artist graph
        std::cout << "\n[Pillar 3] Building bipartite user-artist graph..." << std::endl;
        for (const auto& interaction : artist_interactions) {
            bipartite_graph.addEdge(
                interaction.user_id,
                interaction.artist_id,
                interaction.play_count
            );
        }
        bipartite_graph.normalizeWeights();
        std::cout << "Bipartite graph built with " << bipartite_graph.getUserCount()
                  << " users and " << bipartite_graph.getArtistCount() << " artists." << std::endl;

        std::cout << "\n=== Initialization Complete ===" << std::endl;
    }

    /**
     * PILLAR 1: Content-Based Recommendations using K-D Tree
     * Time Complexity: O(k log n)
     */
    std::vector<Recommendation> getContentBasedRecommendations(
        const std::string& user_id,
        int num_recommendations = 10
    ) {
        std::vector<Recommendation> recommendations;

        // Get user's liked songs
        auto it = user_liked_songs.find(user_id);
        if (it == user_liked_songs.end() || it->second.empty()) {
            return recommendations;
        }

        // Find similar songs for each liked song
        std::unordered_map<std::string, double> song_scores;

        for (const auto& liked_song_id : it->second) {
            auto song_it = song_map.find(liked_song_id);
            if (song_it == song_map.end()) continue;

            auto similar = kdtree.findSimilarSongs(song_it->second, k_nearest);

            for (const auto& similar_id : similar) {
                // Don't recommend already liked songs
                if (it->second.find(similar_id) == it->second.end()) {
                    song_scores[similar_id] += 1.0;
                }
            }
        }

        // Convert to recommendations
        for (const auto& [song_id, score] : song_scores) {
            auto song_it = song_map.find(song_id);
            if (song_it != song_map.end()) {
                recommendations.push_back(
                    Recommendation(song_id, song_it->second.title, score, "content")
                );
            }
        }

        // Sort by score
        std::sort(recommendations.begin(), recommendations.end());

        if (recommendations.size() > static_cast<size_t>(num_recommendations)) {
            recommendations.resize(num_recommendations);
        }

        return recommendations;
    }

    /**
     * PILLAR 2: User-User Collaborative Filtering using Weighted Graph
     * Time Complexity: O(E log k + k*S) where E=edges, k=similar users, S=songs
     */
    std::vector<Recommendation> getUserCollaborativeRecommendations(
        const std::string& user_id,
        int num_recommendations = 10
    ) {
        std::vector<Recommendation> recommendations;

        // Get top K similar users
        auto similar_users = user_graph.getTopKSimilarUsers(user_id, k_similar_users);

        if (similar_users.empty()) {
            return recommendations;
        }

        // Aggregate songs liked by similar users
        std::unordered_map<std::string, double> song_scores;
        auto& current_user_likes = user_liked_songs[user_id];

        for (const auto& [similar_user_id, similarity] : similar_users) {
            auto& similar_user_likes = user_liked_songs[similar_user_id];

            for (const auto& song_id : similar_user_likes) {
                // Don't recommend already liked songs
                if (current_user_likes.find(song_id) == current_user_likes.end()) {
                    song_scores[song_id] += similarity;
                }
            }
        }

        // Convert to recommendations
        for (const auto& [song_id, score] : song_scores) {
            auto song_it = song_map.find(song_id);
            if (song_it != song_map.end()) {
                recommendations.push_back(
                    Recommendation(song_id, song_it->second.title, score, "user-collab")
                );
            }
        }

        // Sort by score
        std::sort(recommendations.begin(), recommendations.end());

        if (recommendations.size() > static_cast<size_t>(num_recommendations)) {
            recommendations.resize(num_recommendations);
        }

        return recommendations;
    }

    /**
     * PILLAR 3: Artist-Based Collaborative Filtering using Bipartite Graph
     * Time Complexity: O(A*S) where A=top artists, S=songs per artist
     */
    std::vector<Recommendation> getArtistBasedRecommendations(
        const std::string& user_id,
        int num_recommendations = 10
    ) {
        std::vector<Recommendation> recommendations;

        auto song_scores = bipartite_graph.recommendFromTopArtists(
            user_id,
            artist_songs,
            top_artists
        );

        // Filter out already liked songs
        auto& user_likes = user_liked_songs[user_id];

        for (const auto& [song_id, score] : song_scores) {
            if (user_likes.find(song_id) == user_likes.end()) {
                auto song_it = song_map.find(song_id);
                if (song_it != song_map.end()) {
                    recommendations.push_back(
                        Recommendation(song_id, song_it->second.title, score, "artist-collab")
                    );
                }
            }
        }

        // Sort by score
        std::sort(recommendations.begin(), recommendations.end());

        if (recommendations.size() > static_cast<size_t>(num_recommendations)) {
            recommendations.resize(num_recommendations);
        }

        return recommendations;
    }

    /**
     * PILLAR 4: Community-Based Recommendations using Union-Find
     * Time Complexity: O(C*S) where C=community size, S=songs
     */
    std::vector<Recommendation> getCommunityBasedRecommendations(
        const std::string& user_id,
        int num_recommendations = 10
    ) {
        std::vector<Recommendation> recommendations;

        // Get community members
        auto community_members = communities.getCommunityMembers(user_id);

        if (community_members.size() <= 1) {
            return recommendations; // No other members
        }

        // Aggregate popular songs in community
        std::unordered_map<std::string, double> song_scores;
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
        for (const auto& [song_id, score] : song_scores) {
            auto song_it = song_map.find(song_id);
            if (song_it != song_map.end()) {
                recommendations.push_back(
                    Recommendation(song_id, song_it->second.title, score, "community")
                );
            }
        }

        // Sort by score
        std::sort(recommendations.begin(), recommendations.end());

        if (recommendations.size() > static_cast<size_t>(num_recommendations)) {
            recommendations.resize(num_recommendations);
        }

        return recommendations;
    }

    /**
     * Combined recommendations from all 4 pillars
     * Time Complexity: O(k log n + E log k + A*S + C*U)
     */
    std::vector<Recommendation> generateRecommendations(
        const std::string& user_id,
        int num_recommendations = 20
    ) {
        std::cout << "\n=== Generating Recommendations for User: " << user_id << " ===" << std::endl;

        // Get recommendations from each pillar
        auto content_recs = getContentBasedRecommendations(user_id, num_recommendations);
        auto user_collab_recs = getUserCollaborativeRecommendations(user_id, num_recommendations);
        auto artist_recs = getArtistBasedRecommendations(user_id, num_recommendations);
        auto community_recs = getCommunityBasedRecommendations(user_id, num_recommendations);

        std::cout << "[Pillar 1] Content-based: " << content_recs.size() << " recommendations" << std::endl;
        std::cout << "[Pillar 2] User-collab: " << user_collab_recs.size() << " recommendations" << std::endl;
        std::cout << "[Pillar 3] Artist-based: " << artist_recs.size() << " recommendations" << std::endl;
        std::cout << "[Pillar 4] Community: " << community_recs.size() << " recommendations" << std::endl;

        // Combine all recommendations with weighted scores
        std::unordered_map<std::string, Recommendation> combined;

        auto addRecommendations = [&combined](const std::vector<Recommendation>& recs, double weight) {
            for (const auto& rec : recs) {
                if (combined.find(rec.song_id) == combined.end()) {
                    combined[rec.song_id] = rec;
                    combined[rec.song_id].score *= weight;
                } else {
                    combined[rec.song_id].score += rec.score * weight;
                }
            }
        };

        // Weight different pillars
        addRecommendations(content_recs, 1.0);
        addRecommendations(user_collab_recs, 1.2);
        addRecommendations(artist_recs, 1.1);
        addRecommendations(community_recs, 0.8);

        // Convert to vector and sort
        std::vector<Recommendation> final_recs;
        for (const auto& [id, rec] : combined) {
            final_recs.push_back(rec);
        }

        std::sort(final_recs.begin(), final_recs.end());

        if (final_recs.size() > static_cast<size_t>(num_recommendations)) {
            final_recs.resize(num_recommendations);
        }

        return final_recs;
    }

    /**
     * Search autocomplete using Trie
     * Time Complexity: O(m + k) where m=prefix length, k=results
     */
    std::vector<std::string> searchAutocomplete(const std::string& prefix, int max_results = 10) {
        return search_trie.autocomplete(prefix, max_results);
    }

    /**
     * Gets user's community information
     * Time Complexity: O(α(n)) ≈ O(1)
     */
    std::string getUserCommunity(const std::string& user_id) {
        return communities.getCommunity(user_id);
    }

    int getCommunitySize(const std::string& user_id) {
        return communities.getCommunitySize(user_id);
    }

    const std::unordered_map<std::string, Song>& getSongMap() const { return song_map; }
    const std::unordered_map<std::string, Artist>& getArtistMap() const { return artist_map; }
    const std::unordered_map<std::string, User>& getUserMap() const { return user_map; }
    const WeightedGraph& getUserGraph() const { return user_graph; }
    const BipartiteGraph& getBipartiteGraph() const { return bipartite_graph; }
    const UnionFind& getCommunities() const { return communities; }
};

#endif // RECOMMENDATION_ENGINE_H
