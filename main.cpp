#include <iostream>
#include <string>
#include <iomanip>
#include "core/entities.h"
#include "core/data_loader.h"
#include "algorithms/recommendation_engine.h"


void printHeader() {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║     SPOTIFY-LIKE MUSIC RECOMMENDATION SYSTEM (DSA PROJECT)    ║\n";
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

