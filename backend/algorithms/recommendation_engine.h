#ifndef RECOMMENDATION_ENGINE_H
#define RECOMMENDATION_ENGINE_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <algorithm>
#include <iostream>
#include "../core/entities.h"
#include "../Data_Structures/union_find.h"
#include "../Data_Structures/weighted_graph.h"
#include "../Data_Structures/bipartite_graph.h"
#include "../Data_Structures/tries.h"
#include "../Data_Structures/kdtree.h"

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
    double similarity_threshold = 0.3;
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

        for (size_t i = 0; i < users.size(); ++i) {
            for (size_t j = i + 1; j < users.size(); ++j) {
                const string& u1 = users[i];
                const string& u2 = users[j];

                double similarity = WeightedGraph::jaccardSimilarity(
                    user_liked_songs[u1],
                    user_liked_songs[u2]
                );

                if (similarity > 0.0) {
                    user_graph.addEdge(u1, u2, similarity);

                    if (similarity >= similarity_threshold) {
                        communities.unionSets(u1, u2);
                    }
                }
            }
        }

        cout << "Built user similarity graph with "
             << user_graph.getEdgeCount()
             << " edges across " << users.size() << " users." << endl;

        cout << "Formed "
             << communities.getNumberOfCommunities()
             << " taste communities." << endl;
    }



public:
    // ----- Accessors (read-only) -----
    const unordered_map<string, User>& getUserMap() const { return user_map; }
    const unordered_map<string, Song>& getSongMap() const { return song_map; }
    const WeightedGraph& getUserGraph() const { return user_graph; }
    const BipartiteGraph& getBipartiteGraph() const { return bipartite_graph; }
    UnionFind& getCommunities() { return communities; }

    // initialize: populate maps, trie, interactions and build similarities
    void initialize(
        const vector<Song>& songs,
        const vector<Artist>& artists,
        const vector<User>& users,
        const vector<UserSongInteraction>& song_interactions,
        const vector<UserArtistInteraction>& artist_interactions
    ) {
        cout << "\n=== Initializing Recommendation Engine ===" << endl;

        // build maps and trie
        for (const auto &s : songs) {
            song_map[s.song_id] = s;
            artist_songs[s.artist_id].push_back(s.song_id);
            search_trie.insert(s.title);
        }
        for (const auto &a : artists) {
            artist_map[a.artist_id] = a;
            search_trie.insert(a.artist_name);
        }
        for (const auto &u : users) {
            user_map[u.user_id] = u;
        }

        // interactions
        for (const auto &it : song_interactions) {
            user_song_plays[it.user_id][it.song_id] = it.play_count;
            if (it.liked) {
                user_liked_songs[it.user_id].insert(it.song_id);
                user_map[it.user_id].liked_songs.push_back(it.song_id);
            }
            if (it.play_count > 10) {
                auto sit = song_map.find(it.song_id);
                if (sit != song_map.end()) search_trie.incrementFrequency(sit->second.title);
            }
        }

        // artist interactions can be used to weight bipartite graph (if implemented)
        for (const auto &it : artist_interactions) {
            // placeholder: bipartite_graph.addPlay(it.user_id, it.artist_id, it.play_count);
            (void)it;
        }

        // build user similarities / communities
        buildUserSimilarities();

        // build KD-tree from songs (if KDTree has such API)
        try {
            vector<Song> song_list;
            for (const auto &p : song_map) song_list.push_back(p.second);
            kdtree.build(song_list);
        } catch (...) {
            // ignore if kdtree implementation differs
        }
    }

    // PILLAR 1: Content-based via KDTree
    vector<Recommendation> getContentBasedRecommendations(const string& user_id, int num_recommendations = 10) {
        vector<Recommendation> recommendations;
        auto it = user_liked_songs.find(user_id);
        if (it == user_liked_songs.end() || it->second.empty()) return recommendations;

        unordered_map<string, double> scores;
        for (const auto &liked : it->second) {
            auto sit = song_map.find(liked);
            if (sit == song_map.end()) continue;
            vector<string> neighbors;
            try {
                neighbors = kdtree.findSimilarSongs(sit->second, k_nearest);
            } catch(...) { continue; }
            for (const auto &nid : neighbors) {
                if (it->second.find(nid) == it->second.end()) scores[nid] += 1.0;
            }
        }

        for (const auto &p : scores) {
            auto sit = song_map.find(p.first);
            if (sit != song_map.end()) recommendations.emplace_back(p.first, sit->second.title, p.second, "content");
        }
        sort(recommendations.begin(), recommendations.end());
        if ((int)recommendations.size() > num_recommendations) recommendations.resize(num_recommendations);
        return recommendations;
    }

    // PILLAR 2: User collaborative
    vector<Recommendation> getUserCollaborativeRecommendations(const string& user_id, int num_recommendations = 10) {
        vector<Recommendation> recommendations;
        auto similar = user_graph.getTopKSimilarUsers(user_id, k_similar_users);
        if (similar.empty()) return recommendations;
        unordered_map<string, double> scores;
        auto &cur = user_liked_songs[user_id];
        for (const auto &p : similar) {
            const string &other = p.first;
            double sim = p.second;
            for (const auto &sid : user_liked_songs[other]) {
                if (cur.find(sid) == cur.end()) scores[sid] += sim;
            }
        }
        for (const auto &q : scores) {
            auto sit = song_map.find(q.first);
            if (sit != song_map.end()) recommendations.emplace_back(q.first, sit->second.title, q.second, "user-collab");
        }
        sort(recommendations.begin(), recommendations.end());
        if ((int)recommendations.size() > num_recommendations) recommendations.resize(num_recommendations);
        return recommendations;
    }

    // PILLAR 3: Artist-based via bipartite graph
    vector<Recommendation> getArtistBasedRecommendations(const string& user_id, int num_recommendations = 10) {
        vector<Recommendation> recommendations;
        try {
            auto song_scores = bipartite_graph.recommendFromTopArtists(user_id, artist_songs, top_artists);
            auto &user_likes = user_liked_songs[user_id];
            for (const auto &p : song_scores) {
                if (user_likes.find(p.first) == user_likes.end()) {
                    auto sit = song_map.find(p.first);
                    if (sit != song_map.end()) recommendations.emplace_back(p.first, sit->second.title, p.second, "artist-collab");
                }
            }
        } catch(...) {
            // fallback empty
        }
        sort(recommendations.begin(), recommendations.end());
        if ((int)recommendations.size() > num_recommendations) recommendations.resize(num_recommendations);
        return recommendations;
    }

    // PILLAR 4: Community-based
    vector<Recommendation> getCommunityBasedRecommendations(const string& user_id, int num_recommendations = 10) {
        vector<Recommendation> recommendations;
        auto members = communities.getCommunityMembers(user_id);
        if (members.size() <= 1) return recommendations;
        unordered_map<string, double> song_scores;
        auto &cur = user_liked_songs[user_id];
        for (const auto &m : members) {
            if (m == user_id) continue;
            for (const auto &sid : user_liked_songs[m]) {
                if (cur.find(sid) == cur.end()) song_scores[sid] += 1.0;
            }
        }
        for (const auto &p : song_scores) {
            auto sit = song_map.find(p.first);
            if (sit != song_map.end()) recommendations.emplace_back(p.first, sit->second.title, p.second, "community");
        }
        sort(recommendations.begin(), recommendations.end());
        if ((int)recommendations.size() > num_recommendations) recommendations.resize(num_recommendations);
        return recommendations;
    }

    // community helpers
    string getUserCommunity(const string& user_id) { return communities.getCommunity(user_id); }
    int getCommunitySize(const string& user_id) { return communities.getCommunitySize(user_id); }

    // safe search fallback
    vector<string> searchAutocomplete(const string& prefix, int limit = 10) {
        vector<string> results;
        if (prefix.empty()) return results;
        for (const auto &p : song_map) {
            const auto &title = p.second.title;
            if (title.size() >= prefix.size() &&
                equal(prefix.begin(), prefix.end(), title.begin(),
                      [](char a, char b){ return tolower(a) == tolower(b); })) {
                results.push_back(title);
                if ((int)results.size() >= limit) return results;
            }
        }
        for (const auto &p : artist_map) {
            const auto &name = p.second.artist_name;
            if (name.size() >= prefix.size() &&
                equal(prefix.begin(), prefix.end(), name.begin(),
                      [](char a, char b){ return tolower(a) == tolower(b); })) {
                results.push_back(name);
                if ((int)results.size() >= limit) return results;
            }
        }
        return results;
    }

    // fused recommendations (keeps same semantics)
    vector<Recommendation> generateRecommendations(const string& user_id, int num_recs = 10) {
        unordered_map<string,double> score_map;
        auto addWeighted = [&](const vector<Recommendation>& recs, double w){
            for (const auto &r : recs) {
                // resolve id via title
                string found;
                for (const auto &kv : song_map) {
                    if (kv.second.title == r.title) { found = kv.first; break; }
                }
                if (!found.empty()) score_map[found] += r.score * w;
            }
        };

        addWeighted(getContentBasedRecommendations(user_id,num_recs), 1.0);
        addWeighted(getUserCollaborativeRecommendations(user_id,num_recs), 1.2);
        addWeighted(getArtistBasedRecommendations(user_id,num_recs), 0.9);
        addWeighted(getCommunityBasedRecommendations(user_id,num_recs), 0.8);

        vector<Recommendation> out;
        for (const auto &p : score_map) {
            auto it = song_map.find(p.first);
            string title = (it != song_map.end()) ? it->second.title : string("<unknown>");
            out.emplace_back(p.first, title, p.second, "combined");
        }
        sort(out.begin(), out.end());
        if ((int)out.size() > num_recs) out.resize(num_recs);
        return out;
    }
};

#endif
