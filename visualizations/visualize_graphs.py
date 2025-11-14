import networkx as nx
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from collections import defaultdict

plt.style.use('seaborn-v0_8-darkgrid')
FIGSIZE = (16, 12)

def load_datasets(base_path='../backend/datasets/'):
    """Load all CSV datasets"""
    songs = pd.read_csv(base_path + 'songs.csv')
    artists = pd.read_csv(base_path + 'artists.csv')
    users = pd.read_csv(base_path + 'users.csv')
    user_song_int = pd.read_csv(base_path + 'user_song_interactions.csv')
    user_artist_int = pd.read_csv(base_path + 'user_artist_interactions.csv')

    return songs, artists, users, user_song_int, user_artist_int

def visualize_taste_communities(users, user_song_int, min_similarity=0.3):
    """
    Visualizes Taste Communities formed by Union-Find
    Different colors represent different communities
    """
    print("\n[4] Creating Taste Communities Graph (Union-Find)...")

    # Union-Find clustering
    class UnionFind:
        def __init__(self):
            self.parent = {}

        def make_set(self, x):
            if x not in self.parent:
                self.parent[x] = x

        def find(self, x):
            if self.parent[x] != x:
                self.parent[x] = self.find(self.parent[x])
            return self.parent[x]

        def union(self, x, y):
            root_x = self.find(x)
            root_y = self.find(y)
            if root_x != root_y:
                self.parent[root_x] = root_y

    G = nx.Graph()
    uf = UnionFind()

    # Build sets from user liked songs
    user_liked = defaultdict(set)
    for _, row in user_song_int[user_song_int['liked'] == True].iterrows():
        user_liked[row['user_id']].add(row['song_id'])

    user_ids = list(user_liked.keys())

    for user in user_ids:
        uf.make_set(user)
        G.add_node(user)

    # Calculate similarities and union similar users
    for i, user1 in enumerate(user_ids):
        for user2 in user_ids[i+1:]:
            similarity = calculate_jaccard_similarity(user_liked[user1], user_liked[user2])
            if similarity >= min_similarity:
                G.add_edge(user1, user2, weight=similarity)
                uf.union(user1, user2)

    # Community ids
    communities = defaultdict(list)
    for user in user_ids:
        root = uf.find(user)
        communities[root].append(user)

    print(f"   Found {len(communities)} communities")

    # Layout
    pos = nx.spring_layout(G, k=2.5, iterations=50, seed=42)

    plt.figure(figsize=FIGSIZE)

    # Colour nodes based on community
    colors = plt.cm.tab20(np.linspace(0, 1, len(communities)))
    community_colors = {}

    for idx, (root, members) in enumerate(communities.items()):
        color = colors[idx % len(colors)]
        for member in members:
            community_colors[member] = color

    node_colors = [community_colors[node] for node in G.nodes()]

    nx.draw_networkx_nodes(G, pos, node_color=node_colors,
                          node_size=1000, alpha=0.9, edgecolors='black', linewidths=2)

    edges = G.edges()
    weights = [G[u][v]['weight'] for u, v in edges]
    widths = [3 * w for w in weights]

    nx.draw_networkx_edges(G, pos, width=widths, alpha=0.3, edge_color='gray')

    nx.draw_networkx_labels(G, pos, font_size=9, font_weight='bold', font_color='white')

    plt.title(f'Taste Communities (Union-Find)\n{len(communities)} communities formed',
              fontsize=16, fontweight='bold')
    plt.axis('off')
    plt.tight_layout()
    plt.savefig('taste_communities.png', dpi=300, bbox_inches='tight')
    print("   ✓ Saved: taste_communities.png")
    plt.close()
