#include <algorithm>
#include <bits/stdc++.h>
#include <filesystem>
#include <iostream>

const int WIDTH = 128, HEIGHT = 128;

class tilemap {
private:
  struct MapData {
    int background[WIDTH][HEIGHT]; // 65,536 Bytes
    int foreground[WIDTH][HEIGHT]; // 65,536
  }; // 131,072 Bytes

public:
  tilemap() = default;
  ~tilemap() = default;

  MapData tilemap_init(std::string level) {
    MapData map;
    std::fill(&map.background[0][0], &map.background[0][0] + (WIDTH * HEIGHT),
              1);
    for (int i = 0; i < HEIGHT; i++) {
      for (int j = 0; j < WIDTH; j++) {
        if (i % 10 == 0 && j % 10 == 0)
          map.foreground[i][j] = 2;
      }
    }
    level += ".bin";
    if (std::filesystem::exists(level)) {
      std::ifstream file(level, std::ios::binary);
      if (file.is_open()) {
        file.read(reinterpret_cast<char *>(&map), sizeof(map));
        file.close();
      } else {
        std::cerr << "Error reading file";
      }
    } else {
      std::ofstream file(level, std::ios::binary);
      if (file.is_open()) {
        file.write(reinterpret_cast<char *>(&map), sizeof(map));
        file.close();
      } else {
        std::cerr << "Error writing to file";
      }
    }

    return map;
  }
};
