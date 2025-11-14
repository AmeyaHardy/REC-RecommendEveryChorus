 🎼 RecommendationEveryChorus (REC)

### A Music Recommendation Engine using Classical DSA (C++17)

RecommendationEveryChorus (REC) is an academic project that recreates a miniature Spotify-like recommendation engine using only **Data Structures and Algorithms**, implemented from scratch in **modern C++17**.
No machine learning libraries, no frameworks — just pure algorithms.

---

## 🔍 1. Project Summary

This project demonstrates how real-world recommendation systems can be built using classical concepts:

* 🎧 **Content-based filtering** with a K-D Tree
* 👥 **Collaborative filtering** through user similarity graphs
* 🎤 **Artist-based modeling** via bipartite graphs
* 🪢 **Community detection** using Union-Find (Disjoint Sets)
* 🔤 **Autocomplete** using a Trie
* 🏆 **Top-K ranking** using priority queues
* ⚡ **O(1) lookups** using hash maps and hash sets

All algorithms, data structures, and utility modules are handwritten.

---

## 🧱 2. System Architecture

```
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
docs/
```

---

## 🎵 3. Recommendation Pipeline

### 3.1 Content-Based Filtering

A **4-dimensional K-D Tree** stores songs using audio features (energy, valence, danceability, tempo).
Nearest-neighbor queries produce content-based recommendations.

### 3.2 User–User Collaborative Filtering

A **weighted user similarity graph** is built using:

* liked-song overlap
* play-count similarity
* cosine/Jaccard measures

Top similar users are retrieved using a priority queue.

### 3.3 Artist-Based Collaborative Filtering

A **user ↔ artist bipartite graph** aggregates artist affinities using play counts.

### 3.4 Taste Communities

A **Union-Find** clusters users with similar listening patterns.
Community preferences are added to the final scoring.

### 3.5 Combined Recommendation

All scores are normalized and fused to generate the final ranked list.

---

## 📁 4. Dataset Summary

Datasets are stored in `backend/datasets/`.

### Included Files:

* `songs.csv` — song metadata + audio features
* `artists.csv` — artist info
* `users.csv` — user list
* `user_song_interactions.csv` — liked + play count data
* `user_artist_interactions.csv` — aggregated artist play counts

Your updated datasets (the ones you provided) should be pasted directly into the two CSV files above.

---

## 🛠️ 5. Build & Run

### 5.1 Compile

```bash
cd backend
g++ -std=c++17 -O2 main.cpp -o rec_engine
```

### 5.2 Run

```bash
./rec_engine
```

### 5.3 Debug Build

```bash
g++ -std=c++17 -g main.cpp -o rec_engine_debug
```

### 5.4 Sanitizer Build (memory / UB checks)

```bash
g++ -std=c++17 -g -fsanitize=address -fsanitize=undefined main.cpp -o rec_engine_asan
```

---

## 🌐 6. Clone the Repository

```bash
git clone https://github.com/AmeyaHardy/REC-RecommendEveryChorus.git
cd REC-RecommendEveryChorus
```

GitHub CLI:

```bash
gh repo clone AmeyaHardy/REC-RecommendEveryChorus
```

SSH:

```bash
git clone git@github.com:AmeyaHardy/REC-RecommendEveryChorus.git
```

---

## 📊 7. Example Output (Shortened)

```
Loading datasets...
Loaded 100 songs, 22 artists, 20 users.

Building K-D Tree...
Building user similarity graph...
Building bipartite graph...
Clustering communities...

Generating recommendations for User U001...
Top Recommendations:
1. Praise God
2. Paranoid
3. Off The Grid
...
```

---

## 📈 8. Time Complexity Overview

| Component             | Data Structure      | Complexity |
| --------------------- | ------------------- | ---------- |
| K-D Tree Build        | Median partitioning | O(n log n) |
| K-NN Search           | K-D Tree            | O(k log n) |
| User Similarity Graph | Weighted Graph      | O(E log k) |
| Artist-Based Scoring  | Bipartite Graph     | O(A × S)   |
| Community Merging     | Union-Find          | O(α(n))    |
| Autocomplete          | Trie                | O(L)       |

Where:

* `n` = number of songs
* `U` = number of users
* `A` = artists
* `S` = songs per user

---

## 📚 9. Documentation

The `docs/` folder contains:

* **ARCHITECTURE.md** — detailed architecture & diagrams
* **DSA_MAPPING.md** — full mapping of algorithms to features
* **README.md** — this documentation

---

## 🚀 10. Future Enhancements

* Matrix factorization (SVD)
* Locality-Sensitive Hashing for approximate NN
* Graph embeddings
* REST API layer
* PostgreSQL integration
* Multi-threaded recommendation generation
* Docker support

---

## 🤝 11. Contributing

Pull requests, code improvements, dataset enhancements, or documentation updates are welcome.

---

## 📜 12. License

This project is available under the **MIT License**.

---

## 📬 13. Contact

For issues or contributions, open an Issue or Pull Request on GitHub.

---

If you want, I can also create:

* A shorter README version
* A college-submission PDF report
* A polished PowerPoint
* A complete GitHub repo description & tags

Just tell me.

10. Cloning the Repository
Clone using HTTPS : git clone https://github.com/AmeyaHardy/REC-RecommendEveryChorus.git

Clone using SSH : git clone git@github.com:AmeyaHardy/REC-RecommendEveryChorus.git

Clone using GitHub CLI : gh repo clone AmeyaHardy/REC-RecommendEveryChorus

After Cloning : cd REC-RecommendEveryChorus

Then continue with: cd backend
g++ -std=c++17 -O2 main.cpp -o recommendation_engine
./recommendation_engine




