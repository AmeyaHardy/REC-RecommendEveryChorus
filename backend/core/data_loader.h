#ifndef DATA_LOADER_H
#define DATA_LOADER_H

#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <iostream>
#include "entities.h"

class DataLoader {
private:

    static std::vector<std::string> splitCSV(const std::string& line) {
        std::vector<std::string> result;
        std::stringstream ss(line);
        std::string cell;

        while (std::getline(ss, cell, ',')) {
            // Trim whitespace
            size_t start = cell.find_first_not_of(" \t\r\n");
            size_t end = cell.find_last_not_of(" \t\r\n");
            if (start != std::string::npos) {
                result.push_back(cell.substr(start, end - start + 1));
            } else {
                result.push_back("");
            }
        }

        return result;
    }

    static bool stringToBool(const std::string& str) {
        return str == "true" || str == "True" || str == "1";
    }

public:

    static std::vector<Song> loadSongs(const std::string& filepath) {
        std::vector<Song> songs;
        std::ifstream file(filepath);

        if (!file.is_open()) {
            std::cerr << "Error: Could not open " << filepath << std::endl;
            return songs;
        }

        std::string line;
        std::getline(file, line); // Skip header

        while (std::getline(file, line)) {
            if (line.empty()) continue;

            auto fields = splitCSV(line);
            if (fields.size() >= 9) {
                Song song(
                    fields[0],                    // song_id
                    fields[1],                    // title
                    fields[2],                    // artist_id
                    fields[3],                    // genre
                    std::stoi(fields[4]),         // bpm
                    std::stod(fields[5]),         // energy
                    std::stod(fields[6]),         // danceability
                    std::stod(fields[7]),         // valence
                    fields[8]                     // mood
                );
                songs.push_back(song);
            }
        }

        file.close();
        std::cout << "Loaded " << songs.size() << " songs." << std::endl;
        return songs;
    }


    static std::vector<Artist> loadArtists(const std::string& filepath) {
        std::vector<Artist> artists;
        std::ifstream file(filepath);

        if (!file.is_open()) {
            std::cerr << "Error: Could not open " << filepath << std::endl;
            return artists;
        }

        std::string line;
        std::getline(file, line); // Skip header

        while (std::getline(file, line)) {
            if (line.empty()) continue;

            auto fields = splitCSV(line);
            if (fields.size() >= 3) {
                Artist artist(
                    fields[0],  // artist_id
                    fields[1],  // artist_name
                    fields[2]   // genre
                );
                artists.push_back(artist);
            }
        }

        file.close();
        std::cout << "Loaded " << artists.size() << " artists." << std::endl;
        return artists;
    }


    static std::vector<User> loadUsers(const std::string& filepath) {
        std::vector<User> users;
        std::ifstream file(filepath);

        if (!file.is_open()) {
            std::cerr << "Error: Could not open " << filepath << std::endl;
            return users;
        }

        std::string line;
        std::getline(file, line); // Skip header

        while (std::getline(file, line)) {
            if (line.empty()) continue;

            auto fields = splitCSV(line);
            if (fields.size() >= 2) {
                User user(
                    fields[0],  // user_id
                    fields[1]   // username
                );
                users.push_back(user);
            }
        }

        file.close();
        std::cout << "Loaded " << users.size() << " users." << std::endl;
        return users;
    }

    static std::vector<UserSongInteraction> loadUserSongInteractions(const std::string& filepath) {
        std::vector<UserSongInteraction> interactions;
        std::ifstream file(filepath);

        if (!file.is_open()) {
            std::cerr << "Error: Could not open " << filepath << std::endl;
            return interactions;
        }

        std::string line;
        std::getline(file, line); // Skip header

        while (std::getline(file, line)) {
            if (line.empty()) continue;

            auto fields = splitCSV(line);
            if (fields.size() >= 4) {
                UserSongInteraction interaction(
                    fields[0],                  // user_id
                    fields[1],                  // song_id
                    stringToBool(fields[2]),    // liked
                    std::stoi(fields[3])        // play_count
                );
                interactions.push_back(interaction);
            }
        }

        file.close();
        std::cout << "Loaded " << interactions.size() << " user-song interactions." << std::endl;
        return interactions;
    }


    static std::vector<UserArtistInteraction> loadUserArtistInteractions(const std::string& filepath) {
        std::vector<UserArtistInteraction> interactions;
        std::ifstream file(filepath);

        if (!file.is_open()) {
            std::cerr << "Error: Could not open " << filepath << std::endl;
            return interactions;
        }

        std::string line;
        std::getline(file, line); // Skip header

        while (std::getline(file, line)) {
            if (line.empty()) continue;

            auto fields = splitCSV(line);
            if (fields.size() >= 3) {
                UserArtistInteraction interaction(
                    fields[0],           // user_id
                    fields[1],           // artist_id
                    std::stoi(fields[2]) // play_count
                );
                interactions.push_back(interaction);
            }
        }

        file.close();
        std::cout << "Loaded " << interactions.size() << " user-artist interactions." << std::endl;
        return interactions;
    }

    static std::unordered_map<std::string, Song> createSongMap(const std::vector<Song>& songs) {
        std::unordered_map<std::string, Song> song_map;
        for (const auto& song : songs) {
            song_map[song.song_id] = song;
        }
        return song_map;
    }

    static std::unordered_map<std::string, Artist> createArtistMap(const std::vector<Artist>& artists) {
        std::unordered_map<std::string, Artist> artist_map;
        for (const auto& artist : artists) {
            artist_map[artist.artist_id] = artist;
        }
        return artist_map;
    }

    static std::unordered_map<std::string, User> createUserMap(const std::vector<User>& users) {
        std::unordered_map<std::string, User> user_map;
        for (const auto& user : users) {
            user_map[user.user_id] = user;
        }
        return user_map;
    }

    static std::unordered_map<std::string, std::vector<std::string>> createArtistSongsMap(
        const std::vector<Song>& songs
    ) {
        std::unordered_map<std::string, std::vector<std::string>> artist_songs;
        for (const auto& song : songs) {
            artist_songs[song.artist_id].push_back(song.song_id);
        }
        return artist_songs;
    }
};

#endif // DATA_LOADER_H
