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
    int foreground[TILE][TILE] = {1};
  };

  MapData map;

  MapData init(std::string level) {
    level = "../resources/levels/" + level + ".bin";

    for (size_t i{}; i < TILE; i++) {
      for (size_t j{}; j < TILE; j++) {
        map.background[i][j] = 1;
        if (i % 10 == 0 && j % 10 == 0)
          map.foreground[i][j] = 1;
      }
    }

    if (std::filesystem::exists(level)) {
      std::ifstream file(level, std::ios::binary);
      if (file.is_open()) {
        file.read(reinterpret_cast<char *>(&map), sizeof(map));
        file.close();
      } else
        std::cerr << "Error reading level file";
    } else {
      std::ofstream file(level, std::ios::binary);
      if (file.is_open()) {
        file.write(reinterpret_cast<char *>(&map), sizeof(map));
        file.close();
      } else
        std::cerr << "Error writing to level file";
    }
    return map;
  }
};
