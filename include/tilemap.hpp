#include <bits/stdc++.h>
#include <filesystem>
#include <iostream>

const int TILE = 128;

class tilemap {
private:
public:
  tilemap() = default;
  ~tilemap() = default;

  struct MapData {
    int background[TILE][TILE];       // 65,536 Bytes
    int foreground[TILE][TILE] = {1}; // 65,536
  }; // 131,072 Bytes

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

  MapData update(std::string level) {
    if (std::filesystem::exists(level)) {
      std::ofstream file(level, std::ios::binary);
      if (file.is_open()) {
        // TODO : How do I intuitively write to the MapData arrays from ImGui
        // window using mouse data?
        //
        // NOTE : Create an ImGui window that draws a tilemap of the size of the
        // array currently loaded. Set what ID you want to have saved to copy to
        // the tile you click. Check the position of the mouse click to know
        // what tile it is in and then apply the currently saved ID to that
        // position in the array. The i and j value should give the tile
        // location from the loops.
      } else
        std::cerr << "Error writing to level file";
    }
    return map;
  }
};
