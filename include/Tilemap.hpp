#include <bits/stdc++.h>
#include <filesystem>
#include <iostream>

class tilemap {
private:
public:
  tilemap() = default;
  ~tilemap() = default;

  const static int TILE = 128;

  struct MapData {
    int background[TILE][TILE];
    int foreground[TILE][TILE];
  };
  MapData mapData;

  MapData init(std::string level) {
    level = "../resources/levels/" + level + ".bin";
    if (std::filesystem::exists(level)) {
      std::ifstream file(level, std::ios::binary);
      if (file.is_open()) {
        file.read(reinterpret_cast<char *>(&mapData), sizeof(mapData));
        file.close();
      } else
        std::cerr << "Error reading level file";
    } else {
      std::ofstream file(level, std::ios::binary);
      if (file.is_open()) {
        file.write(reinterpret_cast<char *>(&mapData), sizeof(mapData));
        file.close();
      } else
        std::cerr << "Error writing to level file";
    }
    return mapData;
  }

  void update(std::string level, MapData *map) {
    level = "../resources/levels/" + level + ".bin";
    if (std::filesystem::exists(level)) {
      std::ofstream file(level, std::ios::binary);
      if (file.is_open()) {
        file.write(reinterpret_cast<char *>(map), sizeof(tilemap::MapData));
        file.close();
      } else {
        std::cerr << "Error writing to level file " << level;
      }
    }
  }
};
