#include "raylib.h"
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

const int NUM_ROWS = 8;
const int PIXELS = 32;

typedef enum { IDLE = 0, WALK, RUN, ATTACK, NUM_ANIM } AnimState;

typedef enum {
  NW = 0,
  W,
  SW,
  S,
  SE,
  E,
  NE,
  N,
  NUM_DIR
} Direction; // Get attack to have the correct directions.

typedef struct {
  Vector2 pos;
  Rectangle rec;
  Texture2D tex[NUM_ANIM];
  AnimState currState;
  Direction currDir;
} player;

player p;

void player_anim(player *p, int *currFrame, int *frameCounter, int frameSpeed);
Vector2 player_move(player *p, float speed, float sprint);
void player_init(player *p);

int main(void) {
  ChangeDirectory(TextFormat("%s/..", GetApplicationDirectory()));
  InitWindow(1280, 720, "Blossom");
  GuiLoadStyle("styles/style_dark.rgs");
  Texture2D background = LoadTexture("assets/nature/nature_4/origbig.png");

  Texture2D enviroment[3] = {
      LoadTexture(""), LoadTexture(""),
      LoadTexture("")}; // Is this the best way to do this?

  player_init(&p);

  int currFrame = 0;
  int frameCounter = 0;

  SetTargetFPS(60);

  while (!WindowShouldClose()) {

    // Everything that isn't a texture
    player_anim(&p, &currFrame, &frameCounter, 8);
    p.pos = Vector2Add(p.pos, player_move(&p, 3.0f, 50.0f));

    BeginDrawing();

    ClearBackground(RAYWHITE);

    DrawText("Alpha", (float)GetScreenWidth() / 2.0f - 48.0f,
             (float)GetScreenHeight() / 2.0f - 60.0f, 48, GRAY);
    DrawTexture(background, 0, 0, WHITE);

    //  Static textures
    DrawRectanglePro(
        (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight() / 2.0f},
        (Vector2){0, (-1) * (GetScreenHeight() / 2.0f)}, 0, DARKBROWN);
    DrawRectangleLinesEx(
        (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight() / 2.0f}, 5.0f,
        BLACK);

    // Dynamic textures
    DrawTextureRec(p.tex[p.currState], p.rec, p.pos, WHITE); // player

    // UI.
    GuiPanel((Rectangle){10, 10, 65, 50}, "Settings");
    GuiLabel((Rectangle){15, 50, 40, 1}, TextFormat("FPS: %i", GetFPS()));

    EndDrawing();
  }

  for (int i = 0; i < NUM_ANIM; i++)
    UnloadTexture(p.tex[i]);

  CloseWindow();
  return 0;
}

void player_init(player *p) {
  p->pos = (Vector2){(float)GetScreenWidth() / 2.0f,
                     (float)GetScreenHeight() / 2.0f};
  p->currState = IDLE;
  p->currDir = S;
  p->tex[IDLE] = LoadTexture("assets/player-anim/idle.png");
  p->tex[WALK] = LoadTexture("assets/player-anim/walk.png");
  p->tex[RUN] = LoadTexture("assets/player-anim/run.png");
  p->tex[ATTACK] = LoadTexture("assets/player-anim/attack.png");
  p->rec = (Rectangle){0, 0, PIXELS, PIXELS};
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

Vector2 player_move(player *p, float speed, float sprint) {
  Vector2 move = Vector2Zero();
  p->pos.y =
      Clamp(p->pos.y, GetScreenHeight() / 2.0f, GetScreenHeight() - PIXELS);
  p->pos.x = Clamp(p->pos.x, 0.0f, GetScreenWidth() - PIXELS);

  move.x = (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) ? move.x - speed : move.x;
  move.x = (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) ? move.x + speed : move.x;
  move.y = (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) ? move.y - speed : move.y;
  move.y = (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) ? move.y + speed : move.y;
  move = IsKeyPressed(KEY_LEFT_SHIFT) ? Vector2Scale(move, sprint)
                                      : move; // turn this back into a sprint.

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
