#include <iostream>
#include <string>
#include <iomanip>
#include "core/entities.h"
#include "core/data_loader.h"
#include "algorithms/recommendation_engine.h"


void printHeader() {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║            MUSIC RECOMMENDATION SYSTEM (DSA PROJECT)          ║\n";
    std::cout << "║                 Academic Implementation in C++                ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";
}

void printRecommendations(const std::vector<Recommendation>& recommendations) {
    std::cout << "\n┌─────────────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│                     TOP RECOMMENDATIONS                     │" << std::endl;
    std::cout << "├──────┬─────────────────────────────┬──────────┬─────────────┤" << std::endl;
    std::cout << "│ Rank │ Song Title                  │  Score   │   Source    │" << std::endl;
    std::cout << "├──────┼─────────────────────────────┼──────────┼─────────────┤" << std::endl;

    int rank = 1;
    for (const auto& rec : recommendations) {
        std::string title = rec.title;
        if (title.length() > 27) {
            title = title.substr(0, 24) + "...";
        }

        std::cout << "│ " << std::setw(4) << rank << " │ "
                  << std::left << std::setw(27) << title << " │ "
                  << std::right << std::setw(8) << std::fixed << std::setprecision(2) << rec.score << " │ "
                  << std::left << std::setw(11) << rec.source << " │" << std::endl;
        rank++;
    }

    std::cout << "└──────┴─────────────────────────────┴──────────┴─────────────┘" << std::endl;
}

void printPillarRecommendations(
    const std::string& pillar_name,
    const std::vector<Recommendation>& recommendations,
    int display_count = 5
) {
    std::cout << "\n[" << pillar_name << "]" << std::endl;
    std::cout << "────────────────────────────────────────" << std::endl;

    int count = 0;
    for (const auto& rec : recommendations) {
        if (count >= display_count) break;

        std::cout << "  " << (count + 1) << ". " << rec.title
                  << " (Score: " << std::fixed << std::setprecision(2) << rec.score << ")" << std::endl;
        count++;
    }

    if (recommendations.empty()) {
        std::cout << "  No recommendations available." << std::endl;
    }
}

void demonstrateForUser(RecommendationEngine& engine, const std::string& user_id) {
    const auto& user_map = engine.getUserMap();
    auto user_it = user_map.find(user_id);

    if (user_it == user_map.end()) {
        std::cout << "User not found: " << user_id << std::endl;
        return;
    }

    const User& user = user_it->second;

    std::cout << "\n\n";
    std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;
    std::cout << " USER PROFILE: " << user.username << " (" << user_id << ")" << std::endl;
    std::cout << "═══════════════════════════════════════════════════════════════" << std::endl;

    // User stats
    std::cout << "\nUser Statistics:" << std::endl;
    std::cout << "  • Liked Songs: " << user.liked_songs.size() << std::endl;
    std::cout << "  • Community ID: " << engine.getUserCommunity(user_id) << std::endl;
    std::cout << "  • Community Size: " << engine.getCommunitySize(user_id) << " users" << std::endl;

    // Show some liked songs
    std::cout << "\nLiked Songs (sample):" << std::endl;
    const auto& song_map = engine.getSongMap();
    int shown = 0;
    for (const auto& song_id : user.liked_songs) {
        if (shown >= 5) break;
        auto it = song_map.find(song_id);
        if (it != song_map.end()) {
            std::cout << "  • " << it->second.title << " [" << it->second.genre << "]" << std::endl;
            shown++;
        }
    }

    // Get recommendations from each pillar separately
    std::cout << "\n\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    std::cout << " RECOMMENDATIONS BY PILLAR" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;

    auto content_recs = engine.getContentBasedRecommendations(user_id, 5);
    printPillarRecommendations("PILLAR 1: Content-Based Filtering (K-D Tree)", content_recs);

    auto user_collab_recs = engine.getUserCollaborativeRecommendations(user_id, 5);
    printPillarRecommendations("PILLAR 2: User-User Collaborative Filtering (Weighted Graph)", user_collab_recs);

    auto artist_recs = engine.getArtistBasedRecommendations(user_id, 5);
    printPillarRecommendations("PILLAR 3: Artist-Based Collaborative Filtering (Bipartite Graph)", artist_recs);

    auto community_recs = engine.getCommunityBasedRecommendations(user_id, 5);
    printPillarRecommendations("PILLAR 4: Community-Based Recommendations (Union-Find)", community_recs);

    // Combined recommendations
    std::cout << "\n\n";
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;
    std::cout << " FINAL COMBINED RECOMMENDATIONS" << std::endl;
    std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" << std::endl;

    auto combined_recs = engine.generateRecommendations(user_id, 15);
    printRecommendations(combined_recs);
}

