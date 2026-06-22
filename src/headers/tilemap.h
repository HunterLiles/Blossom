#include <raylib.h>

#define TILE 32

typedef struct {
  int foreground[128][128];
  int background[128][128];
  Rectangle rec;
} tilemap;

tilemap tilemap_init(char *level);
void tilemap_load();
