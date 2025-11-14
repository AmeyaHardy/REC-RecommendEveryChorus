# REC-RecommendEveryChorus

Spotify-Like Music Recommendation System


Academic Project — Data Structures & Algorithms (C++17)
This project implements a miniature music recommendation engine inspired by Spotify, built entirely using core Data Structures and Algorithms in C++17.
All algorithms and data structures are implemented from scratch for academic learning.



Project Overview
This system demonstrates how classical DSA concepts can be applied to real-world recommendation engines.
Core Objectives
Apply advanced DSA in a real application
Implement both content-based and collaborative filtering
Build everything without external ML libraries
Provide clean, documented, easily compilable C++17 code



Features
Sr.No  | Pillar                               | Data Structure Used | Algorithm                   | Complexity |
------ | ------------------------------------ | ------------------- | --------------------------- | ---------- |
  1    | Content-Based Filtering              | K-D Tree            | K-NN                        | O(k log n) |
  2    | User-User Collaborative Filtering    | Weighted Graph      | Similarity + Priority Queue | O(E log k) |
  3    | Artist-Based Collaborative Filtering | Bipartite Graph     | Weighted aggregation        | O(A × S)   |
  4    | Community-Based Filtering            | Union-Find          | Community detection         | O(α(n))    |



Data Structures Implemented
K-D Tree – multi-dimensional nearest neighbor search
Weighted Graph – models user similarity
Bipartite Graph – user–artist relationships
Union-Find – clustering into taste communities
Trie – prefix-based search and autocomplete
Hash Maps / Sets – constant-time lookups
Priority Queues – top-K recommendation ranking



Project Structure 

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



###  Compile and Run C++ Backend

```bash
cd backend

# Compile with C++17 standard
g++ -std=c++17 -O2 main.cpp -o recommendation_engine

# Run the program
./recommendation_engine
```


###  Generate Graph Visualizations

```bash
cd visualizations

# Install Python dependencies
pip install -r requirements.txt

# Generate all visualizations
python visualize_graphs.py
```

**Output Files:**
- `user_song_bipartite.png`
- `user_similarity_graph.png`
- `user_artist_bipartite.png`
- `taste_communities.png`
- `kdtree_2d_projection.png`


###  Run Frontend (Optional)

```bash
cd frontend

# Install dependencies
npm install

# Start development server
npm run dev
```

Access at: `http://localhost:3000`

---



 Dataset

### Realistic Synthetic Data

- **100 Songs:** Real song names (Sicko Mode, Blinding Lights, etc.)
- **22 Artists:** Real artists (Travis Scott, The Weeknd, Drake, etc.)
- **20 Users:** Fictional usernames
- **400+ Interactions:** User-song play counts and likes
- **Audio Features:** BPM, energy, danceability, valence, mood

All datasets are in CSV format under `backend/datasets/`.



Documentation
The following files are in the docs/ folder:
ARCHITECTURE.md — complete system architecture
DSA_MAPPING.md — mapping of DSAs to system components
README.md — detailed project explanation
Each data structure header contains internal documentation.

