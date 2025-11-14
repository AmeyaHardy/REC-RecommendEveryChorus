# REC-RecommendEveryChorus

Music Recommendation System


Academic Project — Data Structures & Algorithms (C++17)
This project implements a miniature music recommendation engine inspired by Spotify, built entirely using core Data Structures and Algorithms in C++17.
All algorithms and data structures are implemented from scratch for academic learning.


Project Overview

This system demonstrates how classical DSA concepts can be applied to real-world recommendation engines.

1.Core Objectives

Apply advanced DSA in a real application
Implement both content-based and collaborative filtering
Build everything without external ML libraries
Provide clean, documented, easily compilable C++17 code

2. System Capabilities
The system supports the following high-level features:

  2.1 Content-Based Filtering (Song-to-Song Similarity)
  Uses a 4-dimensional K-D Tree to store songs based on audio features:
  energy
  danceability
  valence
  tempo
  The K-D Tree enables efficient retrieval of nearest neighbors in feature space.

  2.2 User-User Collaborative Filtering
  Users are represented as nodes in a weighted graph, where edges represent similarity scores based on:
  overlap in liked songs
  listening behaviour
  artist preferences
  A priority queue retrieves top neighbors for recommendation generation.

  2.3 Artist-Based Collaborative Filtering
  A bipartite graph between user nodes and artist nodes is constructed.
  Weights on edges represent user–artist interaction strength (play counts, likes, etc.).
  Recommendations are generated based on the most relevant artists for a user.

  2.4 Taste Community Detection (Union-Find)
  Users who share similar listening habits are grouped via a disjoint-set union structure.
  This allows the system to provide community-based recommendations:
  common songs within the community
  shared artists
  trending preferences



3.Features
Sr.No  | Pillar                               | Data Structure Used | Algorithm                   | Complexity |
------ | ------------------------------------ | ------------------- | --------------------------- | ---------- |
  1    | Content-Based Filtering              | K-D Tree            | K-NN                        | O(k log n) |
  2    | User-User Collaborative Filtering    | Weighted Graph      | Similarity + Priority Queue | O(E log k) |
  3    | Artist-Based Collaborative Filtering | Bipartite Graph     | Weighted aggregation        | O(A × S)   |
  4    | Community-Based Filtering            | Union-Find          | Community detection         | O(α(n))    |




4.Data Structures Implemented

K-D Tree – multi-dimensional nearest neighbor search

Weighted Graph – models user similarity

Bipartite Graph – user–artist relationships

Union-Find – clustering into taste communities

Trie – prefix-based search and autocomplete

Hash Maps / Sets – constant-time lookups

Priority Queues – top-K recommendation ranking



5.Dataset
The dataset consists of small, realistic CSV files that reflect typical music platform metadata.

5.1 Songs
Contains:
song_id
title
artist
BPM
energy
valence
danceability
popularity

5.2 Artists
Contains:
artist_id
name
genre

5.3 Users
Contains:
user_id
username

5.4 User-Song Interactions
Tracks:
play counts
likes
skips



6.Project Structure 

backend/
  core/
      entities.h
      data_loader.h
   data_structures/
      kdtree.h
      weighted_graph.h
      bipartite_graph.h
      union_find.h
      trie.h
   algorithms/
      recommendation_engine.h
   datasets/
      songs.csv
      artists.csv
      users.csv
      user_song_interactions.csv
      user_artist_interactions.csv
   main.cpp

visualizations/
    visualize_graphs.py
    requirements.txt

frontend/
    src/
    package.json

docs/
    ARCHITECTURE.md
    DSA_MAPPING.md
    README.md



7. Building and Running the Backend
7.1 Compile
   cd backend
g++ -std=c++17 -O2 main.cpp -o recommendation_engine

7.2 Run
./recommendation_engine

7.3 Debug Build
g++ -std=c++17 -g main.cpp -o recommendation_engine_debug

Address Sanitizer Build
g++ -std=c++17 -g -fsanitize=address main.cpp -o recommendation_engine_asan



8. Time and Space Complexity
| Module                   | Complexity | Notes                            |
| ------------------------ | ---------- | -------------------------------- |
| Content-Based Search     | O(k log n) | k nearest neighbors via K-D Tree |
| User Similarity          | O(E log k) | graph traversal + PQ             |
| Artist-Based             | O(A × S)   | weighted aggregation             |
| Community Detection      | O(α(n))    | almost O(1) operations           |
| Combined Recommendations | Sum of all | normalized score fusion          |


9. Visualizations
Python scripts in visualizations/ generate:
user similarity network
user–artist bipartite graphs
taste community clusters
2D projections of K-D Tree splits
These provide a graphical understanding of the relationships and clusters.


10. Cloning the Repository
Clone using HTTPS : git clone https://github.com/<your-username>/<your-repo-name>.git

Clone using SSH : git clone git@github.com:<your-username>/<your-repo-name>.git

Clone using GitHub CLI : gh repo clone <your-username>/<your-repo-name>

After Cloning : cd <your-repo-name>

Then continue with: cd backend
g++ -std=c++17 -O2 main.cpp -o recommendation_engine
./recommendation_engine

