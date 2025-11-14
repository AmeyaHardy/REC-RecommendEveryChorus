#ifndef ENTITIES_H
#define ENTITIES_H

#include <string>
#include <vector>

// Song entity with metadata
struct Song {
    std::string song_id;
    std::string title;
    std::string artist_id;
    std::string genre;
    int bpm;
    double energy;
    double danceability;
    double valence;
    std::string mood;

    //K-D Tree [bpm, energy, danceability, valence]
    std::vector<double> getFeatureVector() const {
        return {static_cast<double>(bpm), energy, danceability, valence};
    }

    Song() : bpm(0), energy(0.0), danceability(0.0), valence(0.0) {}

    Song(std::string id, std::string t, std::string aid, std::string g,
         int b, double e, double d, double v, std::string m)
        : song_id(id), title(t), artist_id(aid), genre(g),
          bpm(b), energy(e), danceability(d), valence(v), mood(m) {}
};

//Artist entity
struct Artist {
    std::string artist_id;
    std::string artist_name;
    std::string genre;

    Artist() {}

    Artist(std::string id, std::string name, std::string g)
        : artist_id(id), artist_name(name), genre(g) {}
};

// User entity with metadata
struct User {
    std::string user_id;
    std::string username;
    std::vector<std::string> liked_songs;
    std::vector<std::string> play_history;
    int community_id;

    User() : community_id(-1) {}

    User(std::string id, std::string name)
        : user_id(id), username(name), community_id(-1) {}
};

struct UserSongInteraction {
    std::string user_id;
    std::string song_id;
    bool liked;
    int play_count;

    UserSongInteraction() : liked(false), play_count(0) {}

    UserSongInteraction(std::string uid, std::string sid, bool l, int pc)
        : user_id(uid), song_id(sid), liked(l), play_count(pc) {}
};

struct UserArtistInteraction {
    std::string user_id;
    std::string artist_id;
    int play_count;

    UserArtistInteraction() : play_count(0) {}

    UserArtistInteraction(std::string uid, std::string aid, int pc)
        : user_id(uid), artist_id(aid), play_count(pc) {}
};

struct Recommendation {
    std::string song_id;
    std::string title;
    double score;
    std::string source; // Pillar name

    Recommendation() : score(0.0) {}

    Recommendation(std::string sid, std::string t, double s, std::string src)
        : song_id(sid), title(t), score(s), source(src) {}

    // Sorting by max score
    bool operator<(const Recommendation& other) const {
        return score > other.score;
    }
};

#endif
