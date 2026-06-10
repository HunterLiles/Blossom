#include "raylib.h"
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define SWIDTH 1920
#define SHEIGHT 1080
#define NUM_ROWS 8
#define PIXELS 32
#define MAX_ENV 25
#define MAX_NPC 25

typedef enum { IDLE = 0, WALK, RUN, ATTACK, NUM_ANIM } AnimState;
typedef enum { NW = 0, W, SW, S, SE, E, NE, N, NUM_DIR } Direction;
typedef enum { GRASS = 0, TREE, GTREE, LEAVES, LTREE, GLTREE, NUM_ENV } EnvType;

typedef struct {
  Vector2 pos;
  Rectangle rec;
  Texture2D tex[NUM_ANIM];
  AnimState currState;
  Direction currDir;
} player;

typedef struct {
  Vector2 pos;
  Rectangle rec;
  Texture2D tex;
} NPC;

typedef struct {
  Vector2 pos;
  Texture2D tex;
  Rectangle rec;
} Environment;

typedef struct {
  int layer1, layer2;
} Grid;

player p;

void player_anim(player *p, int *currFrame, int *frameCounter, int frameSpeed);
Vector2 player_move(player *p, float speed, float sprint, float dash);

int main(void) {
  ChangeDirectory(TextFormat("%s/..", GetApplicationDirectory()));
  InitWindow(SWIDTH, SHEIGHT, "Blossom");
  GuiLoadStyle("styles/style_dark.rgs");

  // Player Init
  p.pos = (Vector2){(float)GetScreenWidth() / 2.0f,
                    (float)GetScreenHeight() / 2.0f};
  p.currState = IDLE;
  p.currDir = S;
  p.tex[IDLE] = LoadTexture("assets/player-anim/idle.png");
  p.tex[WALK] = LoadTexture("assets/player-anim/walk.png");
  p.tex[RUN] = LoadTexture("assets/player-anim/run.png");
  p.tex[ATTACK] = LoadTexture("assets/player-anim/attack.png");
  p.rec = (Rectangle){0, 0, PIXELS, PIXELS};

  // Tilemap setup
  Grid back_grid[512][512];
  Grid fore_grid[512][512];

  Texture2D envTex = LoadTexture("assets/trees.png");
  Environment env[MAX_ENV] = {
      {.tex = envTex, .rec = (Rectangle){352, 576, PIXELS, PIXELS}},
      {.tex = envTex, .rec = (Rectangle){224, 490, 36, 54}},
      {.tex = envTex, .rec = (Rectangle){272, 490, 36, 54}},
      {.tex = envTex, .rec = (Rectangle){736, 752, PIXELS, PIXELS}}};

  NPC npc[MAX_NPC] = {
      {(Vector2){250, 250}, (Rectangle){0, 96, 32, 32}, p.tex[IDLE]}};

  for (int i = 0; i < 512; i++) {
    for (int j = 0; j < 512; j++) {
      back_grid[i][j] = (i % 5 == 0 && j % 5 == 0) ? (Grid){GRASS, LEAVES}
                                                   : (Grid){GRASS, -1};
      fore_grid[i][j] =
          (i % 5 == 0 && j % 5 == 0) ? (Grid){TREE, -1} : (Grid){-1, -1};
    }
  }

  int currFrame = 0;
  int frameCounter = 0;

  SetTargetFPS(60);

  while (!WindowShouldClose()) {

    // Everything that isn't a texture
    player_anim(&p, &currFrame, &frameCounter, 8);
    p.pos = Vector2Add(p.pos, player_move(&p, 3.0f, 1.5f, 50.0f));

    BeginDrawing();

    ClearBackground(RAYWHITE);

    //  Background textures
    for (int i = 0; i < 64; i++) {
      for (int j = 0; j < 64; j++) {
        DrawTextureRec(
            env[back_grid[i][j].layer1].tex, env[back_grid[i][j].layer1].rec,
            (Vector2){(float)(i * PIXELS), (float)(j * PIXELS)}, WHITE);
        DrawTextureRec(
            env[back_grid[i][j].layer2].tex, env[back_grid[i][j].layer2].rec,
            (Vector2){(float)(i * PIXELS), (float)(j * PIXELS)}, WHITE);
      }
    }

    DrawText("Alpha", (float)GetScreenWidth() - 80.0f, 0, 24, GRAY);

    // Dynamic textures
    DrawTextureRec(npc[0].tex, npc[0].rec, npc[0].pos, WHITE); // NPC
    DrawTextureRec(p.tex[p.currState], p.rec, p.pos, WHITE);   // player

    // Foreground textures
    for (int i = 0; i < 64; i++) {
      for (int j = 0; j < 64; j++) {
        DrawTextureRec(
            env[fore_grid[i][j].layer1].tex, env[fore_grid[i][j].layer1].rec,
            (Vector2){(float)(i * PIXELS), (float)(j * PIXELS) - PIXELS},
            WHITE);
        DrawRectangleLines((float)(i * PIXELS), (float)(j * PIXELS), PIXELS,
                           PIXELS, GRAY);
      }
    }

    // UI.
    GuiPanel((Rectangle){10, 10, 65, 50}, "Settings");
    GuiLabel((Rectangle){15, 50, 40, 1}, TextFormat("FPS: %i", GetFPS()));

    EndDrawing();
  }

  for (int i = 0; i < NUM_ANIM; i++)
    UnloadTexture(p.tex[i]);
  for (int i = 0; i < NUM_ENV; i++)
    UnloadTexture(env[i].tex);

  CloseWindow();
  return 0;
}

void player_anim(player *p, int *currFrame, int *frameCounter, int frameSpeed) {

  (*frameCounter)++;

  if (*frameCounter >= (60 / frameSpeed)) {
    *frameCounter = 0;
    (*currFrame)++;

    if (*currFrame >= (p->tex[p->currState].width / PIXELS))
      *currFrame = 0;

    p->rec.x = (float)(*currFrame * p->tex[p->currState].width /
                       (p->tex[p->currState].width / (float)PIXELS));
    p->rec.y =
        (float)(p->currDir * p->tex[p->currState].height / (float)NUM_ROWS);
  }
}

Vector2 player_move(player *p, float speed, float sprint, float dash) {
  Vector2 move = Vector2Zero();
  p->pos.y = Clamp(p->pos.y, 0.0f, GetScreenHeight() - PIXELS);
  p->pos.x = Clamp(p->pos.x, 0.0f, GetScreenWidth() - PIXELS);

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
