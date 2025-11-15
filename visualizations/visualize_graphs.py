"""
NetworkX Graph Visualizations for Spotify DSA Project
Visualizes all graph structures used in the recommendation engine
"""

import networkx as nx
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from collections import defaultdict

# Configuration
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

def visualize_user_song_bipartite(users, songs, user_song_int, max_users=10, max_songs=20):
    """
    Visualizes User-Song Bipartite Graph
    Two distinct node sets: Users and Songs
    """
    print("\n[1] Creating User-Song Bipartite Graph...")

    # Create bipartite graph
    B = nx.Graph()

    # Filter to first N users for clarity
    user_ids = users['user_id'].head(max_users).tolist()
    filtered_interactions = user_song_int[user_song_int['user_id'].isin(user_ids)]

    # Get top songs from these users
    top_songs = filtered_interactions['song_id'].value_counts().head(max_songs).index.tolist()
    filtered_interactions = filtered_interactions[filtered_interactions['song_id'].isin(top_songs)]

    # Add nodes with bipartite attribute
    B.add_nodes_from(user_ids, bipartite=0)
    B.add_nodes_from(top_songs, bipartite=1)

    # Add edges with weights (play_count)
    for _, row in filtered_interactions.iterrows():
        if row['liked']:  # Only show liked songs
            B.add_edge(row['user_id'], row['song_id'], weight=row['play_count'])

    # Create layout
    pos = {}

    # Position users on the left
    user_nodes = [n for n in B.nodes() if n in user_ids]
    for i, node in enumerate(user_nodes):
        pos[node] = (0, i * 2)

    # Position songs on the right
    song_nodes = [n for n in B.nodes() if n in top_songs]
    for i, node in enumerate(song_nodes):
        pos[node] = (3, i * 1.5)

    # Draw graph
    plt.figure(figsize=FIGSIZE)

    # Draw nodes
    nx.draw_networkx_nodes(B, pos, nodelist=user_nodes,
                          node_color='lightblue', node_size=1500,
                          label='Users', alpha=0.9)
    nx.draw_networkx_nodes(B, pos, nodelist=song_nodes,
                          node_color='lightcoral', node_size=1200,
                          label='Songs', alpha=0.9)

    # Draw edges with varying thickness based on play count
    edges = B.edges()
    weights = [B[u][v]['weight'] for u, v in edges]
    max_weight = max(weights) if weights else 1
    widths = [3 * (w / max_weight) for w in weights]

    nx.draw_networkx_edges(B, pos, width=widths, alpha=0.3, edge_color='gray')

    # Labels
    labels = {}
    for node in user_nodes:
        labels[node] = node
    for node in song_nodes:
        song_title = songs[songs['song_id'] == node]['title'].values
        if len(song_title) > 0:
            title = song_title[0][:15] + '...' if len(song_title[0]) > 15 else song_title[0]
            labels[node] = title
        else:
            labels[node] = node

    nx.draw_networkx_labels(B, pos, labels, font_size=8, font_weight='bold')

    plt.title('User-Song Bipartite Graph\n(Users on left, Songs on right)',
              fontsize=16, fontweight='bold')
    plt.legend(fontsize=12)
    plt.axis('off')
    plt.tight_layout()
    plt.savefig('user_song_bipartite.png', dpi=300, bbox_inches='tight')
    print("   ✓ Saved: user_song_bipartite.png")
    plt.close()

def calculate_jaccard_similarity(set1, set2):
    """Calculate Jaccard similarity between two sets"""
    if len(set1) == 0 and len(set2) == 0:
        return 0.0
    intersection = len(set1.intersection(set2))
    union = len(set1.union(set2))
    return intersection / union if union > 0 else 0.0

def visualize_user_similarity_graph(users, user_song_int, min_similarity=0.2):
    """
    Visualizes User-User Weighted Similarity Graph
    Edge weights represent Jaccard similarity
    """
    print("\n[2] Creating User-User Similarity Weighted Graph...")

    G = nx.Graph()

    # Build user liked songs sets
    user_liked = defaultdict(set)
    for _, row in user_song_int[user_song_int['liked'] == True].iterrows():
        user_liked[row['user_id']].add(row['song_id'])

    user_ids = list(user_liked.keys())
    G.add_nodes_from(user_ids)

    # Calculate similarities and add edges
    similarities = []
    for i, user1 in enumerate(user_ids):
        for user2 in user_ids[i+1:]:
            similarity = calculate_jaccard_similarity(user_liked[user1], user_liked[user2])
            if similarity >= min_similarity:
                G.add_edge(user1, user2, weight=similarity)
                similarities.append(similarity)

    if len(similarities) == 0:
        print("   ⚠ No similarities found above threshold. Skipping visualization.")
        return

    # Layout
    pos = nx.spring_layout(G, k=2, iterations=50, seed=42)

    plt.figure(figsize=FIGSIZE)

    # Node colors based on degree (connectivity)
    degrees = dict(G.degree())
    node_colors = [degrees.get(node, 0) for node in G.nodes()]

    # Draw nodes
    nodes = nx.draw_networkx_nodes(G, pos, node_color=node_colors,
                                   node_size=800, cmap='YlOrRd',
                                   vmin=0, alpha=0.9)

    # Draw edges with thickness based on similarity
    edges = G.edges()
    weights = [G[u][v]['weight'] for u, v in edges]
    widths = [5 * w for w in weights]

    nx.draw_networkx_edges(G, pos, width=widths, alpha=0.4, edge_color='gray')

    # Labels
    nx.draw_networkx_labels(G, pos, font_size=9, font_weight='bold')

    plt.title('User-User Similarity Weighted Graph\n(Edge thickness = Jaccard similarity)',
              fontsize=16, fontweight='bold')

    # Colorbar
    sm = plt.cm.ScalarMappable(cmap='YlOrRd',
                               norm=plt.Normalize(vmin=0, vmax=max(node_colors)))
    sm.set_array([])
    cbar = plt.colorbar(sm, label='Node Degree (Connections)')

    plt.axis('off')
    plt.tight_layout()
    plt.savefig('user_similarity_graph.png', dpi=300, bbox_inches='tight')
    print("   ✓ Saved: user_similarity_graph.png")
    plt.close()

def visualize_user_artist_bipartite(users, artists, user_artist_int, max_users=12, max_artists=15):
    """
    Visualizes User-Artist Bipartite Graph
    Edge weights represent normalized play counts
    """
    print("\n[3] Creating User-Artist Bipartite Graph...")

    B = nx.Graph()

    # Filter data
    user_ids = users['user_id'].head(max_users).tolist()
    filtered_int = user_artist_int[user_artist_int['user_id'].isin(user_ids)]

    # Get top artists
    top_artists = filtered_int['artist_id'].value_counts().head(max_artists).index.tolist()
    filtered_int = filtered_int[filtered_int['artist_id'].isin(top_artists)]

    # Normalize play counts per user
    max_plays = filtered_int.groupby('user_id')['play_count'].max().to_dict()

    # Add nodes
    B.add_nodes_from(user_ids, bipartite=0)
    B.add_nodes_from(top_artists, bipartite=1)

    # Add edges
    for _, row in filtered_int.iterrows():
        normalized_weight = row['play_count'] / max_plays.get(row['user_id'], 1)
        B.add_edge(row['user_id'], row['artist_id'], weight=normalized_weight)

    # Layout
    pos = {}
    user_nodes = [n for n in B.nodes() if n in user_ids]
    artist_nodes = [n for n in B.nodes() if n in top_artists]

    for i, node in enumerate(user_nodes):
        pos[node] = (0, i * 2)

    for i, node in enumerate(artist_nodes):
        pos[node] = (3, i * 2)

    plt.figure(figsize=FIGSIZE)

    # Draw nodes
    nx.draw_networkx_nodes(B, pos, nodelist=user_nodes,
                          node_color='skyblue', node_size=1500,
                          label='Users', alpha=0.9)
    nx.draw_networkx_nodes(B, pos, nodelist=artist_nodes,
                          node_color='lightgreen', node_size=1500,
                          label='Artists', alpha=0.9)

    # Draw edges
    edges = B.edges()
    weights = [B[u][v]['weight'] for u, v in edges]
    widths = [4 * w for w in weights]

    nx.draw_networkx_edges(B, pos, width=widths, alpha=0.3, edge_color='purple')

    # Labels
    labels = {}
    for node in user_nodes:
        labels[node] = node
    for node in artist_nodes:
        artist_name = artists[artists['artist_id'] == node]['artist_name'].values
        if len(artist_name) > 0:
            labels[node] = artist_name[0][:12]
        else:
            labels[node] = node

    nx.draw_networkx_labels(B, pos, labels, font_size=9, font_weight='bold')

    plt.title('User-Artist Bipartite Graph\n(Edge thickness = Normalized play count)',
              fontsize=16, fontweight='bold')
    plt.legend(fontsize=12)
    plt.axis('off')
    plt.tight_layout()
    plt.savefig('user_artist_bipartite.png', dpi=300, bbox_inches='tight')
    print("   ✓ Saved: user_artist_bipartite.png")
    plt.close()

def visualize_taste_communities(users, user_song_int, min_similarity=0.3):
    """
    Visualizes Taste Communities formed by Union-Find
    Different colors represent different communities
    """
    print("\n[4] Creating Taste Communities Graph (Union-Find)...")

    # Simulate Union-Find clustering
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

    # Build user liked songs sets
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

    # Assign community IDs
    communities = defaultdict(list)
    for user in user_ids:
        root = uf.find(user)
        communities[root].append(user)

    print(f"   Found {len(communities)} communities")

    # Layout
    pos = nx.spring_layout(G, k=2.5, iterations=50, seed=42)

    plt.figure(figsize=FIGSIZE)

    # Color nodes by community
    colors = plt.cm.tab20(np.linspace(0, 1, len(communities)))
    community_colors = {}

    for idx, (root, members) in enumerate(communities.items()):
        color = colors[idx % len(colors)]
        for member in members:
            community_colors[member] = color

    node_colors = [community_colors[node] for node in G.nodes()]

    # Draw nodes
    nx.draw_networkx_nodes(G, pos, node_color=node_colors,
                          node_size=1000, alpha=0.9, edgecolors='black', linewidths=2)

    # Draw edges
    edges = G.edges()
    weights = [G[u][v]['weight'] for u, v in edges]
    widths = [3 * w for w in weights]

    nx.draw_networkx_edges(G, pos, width=widths, alpha=0.3, edge_color='gray')

    # Labels
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
