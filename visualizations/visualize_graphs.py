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



def visualize_kdtree_2d_projection(songs):
    """
    Visualizes K-D Tree structure in 2D (energy vs danceability)
    Shows how songs are clustered in feature space
    """
    print("\n[5] Creating K-D Tree 2D Projection...")

    plt.figure(figsize=(14, 10))

    # Create scatter plot colored by genre
    genres = songs['genre'].unique()
    colors = plt.cm.tab10(np.linspace(0, 1, len(genres)))
    genre_colors = {genre: colors[i] for i, genre in enumerate(genres)}

    for genre in genres:
        genre_songs = songs[songs['genre'] == genre]
        plt.scatter(genre_songs['energy'], genre_songs['danceability'],
                   label=genre, alpha=0.7, s=150,
                   c=[genre_colors[genre]], edgecolors='black', linewidths=1)

    # Add some song labels for clarity
    sample_songs = songs.sample(min(15, len(songs)), random_state=42)
    for _, song in sample_songs.iterrows():
        plt.annotate(song['title'][:15],
                    (song['energy'], song['danceability']),
                    fontsize=7, alpha=0.7,
                    xytext=(5, 5), textcoords='offset points')

    plt.xlabel('Energy', fontsize=14, fontweight='bold')
    plt.ylabel('Danceability', fontsize=14, fontweight='bold')
    plt.title('K-D Tree Song Space (2D Projection)\nEnergy vs Danceability colored by Genre',
              fontsize=16, fontweight='bold')
    plt.legend(fontsize=10, loc='best')
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.savefig('kdtree_2d_projection.png', dpi=300, bbox_inches='tight')
    print("   ✓ Saved: kdtree_2d_projection.png")
    plt.close()

def main():
    print("=" * 70)
    print(" SPOTIFY DSA PROJECT - GRAPH VISUALIZATIONS")
    print(" NetworkX Implementation")
    print("=" * 70)

    try:
        # Load datasets
        print("\nLoading datasets...")
        songs, artists, users, user_song_int, user_artist_int = load_datasets()
        print(f"✓ Loaded: {len(songs)} songs, {len(artists)} artists, {len(users)} users")

        # Create visualizations
        print("\nGenerating visualizations...")
        print("-" * 70)

        visualize_user_song_bipartite(users, songs, user_song_int)
        visualize_user_similarity_graph(users, user_song_int)
        visualize_user_artist_bipartite(users, artists, user_artist_int)
        visualize_taste_communities(users, user_song_int)
        visualize_kdtree_2d_projection(songs)

        print("\n" + "=" * 70)
        print(" ALL VISUALIZATIONS COMPLETED SUCCESSFULLY")
        print("=" * 70)
        print("\nGenerated files:")
        print("  1. user_song_bipartite.png")
        print("  2. user_similarity_graph.png")
        print("  3. user_artist_bipartite.png")
        print("  4. taste_communities.png")
        print("  5. kdtree_2d_projection.png")
        print()

    except FileNotFoundError as e:
        print(f"\n❌ Error: Could not find dataset files.")
        print(f"   {e}")
        print("\n   Please ensure datasets are in '../backend/datasets/' directory")
    except Exception as e:
        print(f"\n❌ Error: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()

