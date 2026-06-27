#include <algorithm>
#include <bits/stdc++.h>
#include <filesystem>
#include <iostream>

const int WIDTH = 128, HEIGHT = 128;

class tilemap {
private:
public:
  tilemap() = default;
  ~tilemap() = default;

  struct MapData {
    int background[WIDTH][HEIGHT]; // 65,536 Bytes
    int foreground[WIDTH][HEIGHT]; // 65,536
  }; // 131,072 Bytes

  MapData init(std::string level) {
    MapData map;
    level = "../resources/levels/" + level + ".bin";
    std::fill(&map.background[0][0], &map.background[0][0] + (WIDTH * HEIGHT),
              1);

    if (std::filesystem::exists(level)) {

      std::ifstream file(level, std::ios::binary);
      if (file.is_open()) {
        file.read(reinterpret_cast<char *>(&map), sizeof(map));
        file.close();
      } else {
        std::cerr << "Error reading level file";
      }
    } else {
      std::ofstream file(level, std::ios::binary);
      if (file.is_open()) {
        file.write(reinterpret_cast<char *>(&map), sizeof(map));
        file.close();
      } else {
        std::cerr << "Error writing to level file";
      }
    }
    return map;
  }
};
