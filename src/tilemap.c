#include "tilemap.h"
#include <stdio.h>
#include <stdlib.h>

// TODO : Load in the text file first to see if it already exists. If it does
// then load in the current data from that, if it doesn't then make a new one
// that is set to the map struct all set to zero.
//
// Take the new .bin file and hotreload it to allow for a ineditor tilemap
// system to make creating levels easy. (Might need ImGui for this)
tilemap tilemap_init(char *level) {
  tilemap map = {
      .rec = {0, 0, 32, 32},
      .background = {},
      .foreground = {},
  };
  FILE *f = fopen(TextFormat("%c.data", level), "wb");
  if ((f = fopen("temp.bin", "wb")) == NULL) {
    printf("Error! opening file");
    exit(1);
  }
  fwrite(map.background, sizeof(int), sizeof(tilemap), f);
  fclose(f);
  return map;
}

void tilemap_load() {}
