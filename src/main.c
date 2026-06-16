#include "raylib.h"
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define TILESIZE 32

typedef enum { IDLE = 0, WALK, RUN, ATTACK, NUM_ANIM } AnimState;
typedef enum { NW = 0, W, SW, S, SE, E, NE, N, NUM_DIR } Direction;
typedef enum { NONE = 0, GRASS, TREE, GTREE, LEAVES, NUM_ENV } EnvType;

typedef struct {
  Vector2 pos;
  Rectangle rec;
  Texture2D tex[NUM_ANIM];
  AnimState currState;
  Direction currDir;
  int currFrame;
  int frameCounter;
  char name[10];
} player;

typedef struct {
  Vector2 pos;
  Texture2D tex;
  Rectangle rec;
} Environment;

typedef struct {
  int layer1, layer2;
} Grid;

void player_anim(player *p, int frameSpeed);
Vector2 player_move(player *p, float speed, float sprint, float dash);

int main(void) {
  ChangeDirectory(TextFormat("%s/..", GetApplicationDirectory()));
  InitWindow(1920, 1080, "Blossom");
  GuiLoadStyle("styles/style_dark.rgs");

  // Player Init

  player p = {
      .pos = {(float)GetScreenWidth() / 2.0f, (float)GetScreenHeight() / 2.0f},
      .currState = IDLE,
      .currDir = S,
      .tex[IDLE] = LoadTexture("assets/player-anim/idle.png"),
      .tex[WALK] = LoadTexture("assets/player-anim/walk.png"),
      .tex[RUN] = LoadTexture("assets/player-anim/run.png"),
      .tex[ATTACK] = LoadTexture("assets/player-anim/attack.png"),
      .rec = {0, 0, 32, 32},
      .name = "Hunter",
  };

  player npc[10] = {
      {
          .pos = {500.0f, 500.0f},
          .currState = IDLE,
          .currDir = S,
          .tex[IDLE] = LoadTexture("assets/player-anim/idle.png"),
          .tex[WALK] = LoadTexture("assets/player-anim/walk.png"),
          .tex[RUN] = LoadTexture("assets/player-anim/run.png"),
          .tex[ATTACK] = LoadTexture("assets/player-anim/attack.png"),
          .rec = {0, 0, 32, 32},
          .name = "Gary",
      },
  };

  // TODO : Create switch for different levels and have an array for each level.
  // Combine the two grids somehow for the level array?
  Grid back_grid[512][512];
  Grid fore_grid[512][512];

  Texture2D envTex = LoadTexture("assets/trees.png");
  Environment env[10] = {
      {},
      {.tex = envTex, .rec = (Rectangle){352, 576, 32, 32}},
      {.tex = envTex, .rec = (Rectangle){224, 305, 176, 176}},
      {.tex = envTex, .rec = (Rectangle){272, 490, 36, 54}},
      {.tex = envTex, .rec = (Rectangle){736, 752, 32, 32}}};

  for (int i = 0; i < 256; i++) {
    for (int j = 0; j < 256; j++) {
      back_grid[i][j] = (i % 5 == 0 && j % 5 == 0) ? (Grid){NONE, LEAVES}
                                                   : (Grid){NONE, NONE};
      fore_grid[i][j] = (i % 10 == 0 && j % 10 == 0) ? (Grid){TREE, NONE}
                                                     : (Grid){NONE, NONE};
    }
  }

  // TODO : Get the offset correct.
  Camera2D cam = {(Vector2){p.pos.x - p.rec.width, p.pos.y - p.rec.height},
                  (Vector2){GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f},
                  0.0f, 1.0f};

  SetTargetFPS(180);

  while (!WindowShouldClose()) {

    // NOTE : Pre-rendering things
    player_anim(&p, 8);
    player_anim(&npc[0], 8);
    p.pos = Vector2Add(p.pos, player_move(&p, 100.0f, 1.5f, 1000.0f));

    cam.target = (Vector2){p.pos.x - (p.rec.width / 2.0f),
                           p.pos.y - (p.rec.height / 2.0f)};
    cam.zoom = expf(logf(cam.zoom) + ((float)GetMouseWheelMove() * 0.1f));
    if (cam.zoom > 3.0f)
      cam.zoom = 3.0f;
    else if (cam.zoom < 1.0f)
      cam.zoom = 1.0f;

    BeginDrawing();
    ClearBackground(BLACK);
    BeginMode2D(cam);

    // NOTE : Background textures
    for (int i = 0; i < 64; i++) {
      for (int j = 0; j < 40; j++) {
        DrawTextureRec(env[GRASS].tex, env[GRASS].rec,
                       (Vector2){(float)(i * TILESIZE), (float)(j * TILESIZE)},
                       WHITE);
        if (back_grid[i][j].layer2 == LEAVES)
          DrawTextureRec(
              env[LEAVES].tex, env[LEAVES].rec,
              (Vector2){(float)(i * TILESIZE), (float)(j * TILESIZE)}, WHITE);
      }
    }

    // NOTE : Animated textures
    DrawTextureRec(npc[0].tex[npc[0].currState], npc[0].rec, npc[0].pos,
                   WHITE);                                   // NPC
    DrawTextureRec(p.tex[p.currState], p.rec, p.pos, WHITE); // player

    // NOTE : Foreground textures
    for (int i = 0; i < 64; i++) {
      for (int j = 0; j < 40; j++) {
        if (fore_grid[i][j].layer1 == TREE)
          DrawTextureRec(
              env[TREE].tex, env[TREE].rec,
              (Vector2){(float)(i * TILESIZE) - (env[TREE].rec.width / 2.0f),
                        (float)(j * TILESIZE) - (env[TREE].rec.height)},
              WHITE);
      }
    }

    // NOTE : Dynamic UI
    DrawRectangle(npc[0].pos.x - 5, npc[0].pos.y - 20, 40, 20, WHITE);
    DrawText(npc[0].name, npc[0].pos.x, npc[0].pos.y - 15, 12, BLACK);

    EndMode2D();

    // NOTE : UI.
    DrawText("Alpha", (float)GetScreenWidth() - 80.0f, 0, 24, GRAY);

    GuiPanel((Rectangle){10, 10, 200, 100}, "Settings");
    GuiLabel((Rectangle){15, 50, 400, 1}, TextFormat("FPS: %i", GetFPS()));
    GuiLabel((Rectangle){15, 70, 150, 1},
             TextFormat("x: %.2f, y: %.2f", p.pos.x, p.pos.y));

    EndDrawing();
  }

  for (int i = 0; i < NUM_ANIM; i++)
    UnloadTexture(p.tex[i]);
  for (int i = 0; i < NUM_ENV; i++)
    UnloadTexture(env[i].tex);

  CloseWindow();
  return 0;
}

void player_anim(player *p, int frameSpeed) {

  p->frameCounter++;

  if (p->frameCounter >= ((1 / GetFrameTime()) / frameSpeed)) {
    p->frameCounter = 0;
    p->currFrame++;

    if (p->currFrame >= (p->tex[p->currState].width / 32))
      p->currFrame = 0;

    p->rec.x = (float)(p->currFrame * p->tex[p->currState].width /
                       (p->tex[p->currState].width / (float)32));
    p->rec.y = (float)(p->currDir * p->tex[p->currState].height / 8.0f);
  }
}

Vector2 player_move(player *p, float speed, float sprint, float dash) {
  Vector2 move = Vector2Zero();
  // TODO : Make a struct for maps and store the number of tiles in each map so
  // the max width and height of each map is easily gotten.
  p->pos.y = Clamp(p->pos.y, 0.0f, GetScreenHeight() - p->rec.width);
  p->pos.x = Clamp(p->pos.x, 0.0f, GetScreenWidth() - p->rec.height);

  speed *= GetFrameTime();
  speed = (IsKeyDown(KEY_LEFT_SHIFT)) ? speed * sprint : speed;
  move.x = (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) ? move.x - speed : move.x;
  move.x = (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) ? move.x + speed : move.x;
  move.y = (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) ? move.y - speed : move.y;
  move.y = (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) ? move.y + speed : move.y;
  move = IsKeyPressed(KEY_SPACE) ? Vector2Scale(move, dash) : move;

  p->currState = (move.x == 0 && move.y == 0) ? IDLE : p->currState;
  if (move.x > 0) {
    p->currState = WALK;
    p->currDir = E;
  }
  if (move.x < 0) {
    p->currState = WALK;
    p->currDir = W;
  }
  if (move.y > 0) {
    p->currState = WALK;
    p->currDir = S;
  }
  if (move.y < 0) {
    p->currState = WALK;
    p->currDir = N;
  }
  if (move.x > 0 && move.y > 0) {
    p->currState = WALK;
    p->currDir = SE;
  }
  if (move.x < 0 && move.y > 0) {
    p->currState = WALK;
    p->currDir = SW;
  }
  if (move.x > 0 && move.y < 0) {
    p->currState = WALK;
    p->currDir = NE;
  }
  if (move.x < 0 && move.y < 0) {
    p->currState = WALK;
    p->currDir = NW;
  }

  return move;
}
