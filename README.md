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
To fix encoding also run (if needed) : chcp 65001
$OutputEncoding = [Console]::OutputEncoding = [Text.UTF8Encoding]::UTF8
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
1. Money Trees Score - 10.51 
2. Praise God - 7.91 
3. God's Plan - 7.61 
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


# 📊 Visualizations (Graph Analysis)

This project includes several visualizations generated using Python and NetworkX/Matplotlib.
These diagrams help explain how the recommendation system understands users, songs, and artists internally.

Commands

1. Navigate to the visualizations folder : cd visualizations

2. Install required Python packages
   
If you are using pip : pip install -r requirements.txt
Or with pip3 : pip3 install -r requirements.txt

3. Run the visualization script : python visualize_graphs.py

If your system uses Python 3 explicitly : python3 visualize_graphs.py







---

## 1. 🎧 Taste Communities (Union-Find Clustering)

This graph shows **four listening communities** formed using a **Union-Find (Disjoint Set)** data structure.

* Each circle represents a user.
* Users with similar listening patterns are merged into the same cluster.
* Colors indicate the community they belong to.
* Connections represent shared preferences or similarity edges.

**Purpose:**
The recommendation engine uses these communities to give *community-influenced suggestions* — songs liked by similar users in the same cluster.

---

## 2. 🎤 User–Artist Bipartite Graph

*(Edge thickness = normalized play count)*

This bipartite graph connects **users (left)** to **artists (right)**.

* Users are shown in blue nodes.
* Artists are shown in green nodes.
* Each edge represents the user’s listening intensity for that artist.
* Thicker edges mean higher play counts.

**Purpose:**
This graph powers **artist-based collaborative filtering**, allowing the system to recommend songs from artists a user has historically engaged with strongly.

---

## 3. 🎶 K-D Tree Song Space (2D Projection)

*Energy vs Danceability, colored by genre*

This scatter plot shows the **distribution of songs** in the feature space used by the K-D Tree.

* X-axis = Energy
* Y-axis = Danceability
* Colors = different genres
* Labels show popular song names used during KNN searches

**Purpose:**
This visualization demonstrates how the system performs **content-based filtering** using nearest-neighbor search in a multi-dimensional feature space.

---

## 4. 🎵 User–Song Bipartite Graph

*(Users on left, Songs on right)*

A second bipartite graph showing the direct relationship between **users** and the **songs they’ve liked or listened to**.

* Blue nodes = users
* Pink/red nodes = songs
* Edge thickness loosely represents total interactions

**Purpose:**
This graph is central to **user–user collaborative filtering**, where similarity is determined by overlap in liked songs.

---

## 5. 👥 User–User Similarity Weighted Graph

*(Edge thickness = Jaccard similarity)*

This graph connects users based on how similar their listening habits are.

* Users with more songs in common have thicker edges.
* Node color corresponds to degree (how many strong connections they have).
* Highly connected users form the core of recommendation neighborhoods.

**Purpose:**
This visualization shows the backbone of **similarity-based recommendations**, where the system chooses the closest neighbors to influence the final personalized ranking.

---



![WhatsApp Image 2025-11-14 at 3 38 57 PM (1)](https://github.com/user-attachments/assets/259c1c55-2e27-4e05-93e2-615ed41df0b4)
![WhatsApp Image 2025-11-14 at 3 38 57 PM (2)](https://github.com/user-attachments/assets/a0340aab-a51e-4efb-a1f8-c4382e2c5749)
![WhatsApp Image 2025-11-14 at 3 38 57 PM](https://github.com/user-attachments/assets/f3c488e1-2586-4f32-9dcf-64091cbed89a)
![WhatsApp Image 2025-11-14 at 3 38 58 PM (1)](https://github.com/user-attachments/assets/2aca4bc8-46c1-48cb-86d3-6a2026a6bd73)
![WhatsApp Image 2025-11-14 at 3 38 58 PM](https://github.com/user-attachments/assets/ea27e5d1-41ec-4b36-986e-1fc36b9ecd2e)





