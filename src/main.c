#include "raylib.h"
#include "raymath.h"

#define NUM_FRAMES 6
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

typedef enum {
  IDLE_RIGHT = 0,
  IDLE_LEFT,
  IDLE_UP,
  IDLE_DOWN,
  RUN_RIGHT,
  RUN_LEFT,
  RUN_UP,
  RUN_DOWN,
  NUM_ANIM
} AnimState;

typedef struct {
  Vector2 pos;
  Rectangle rec;
  Texture2D tex[NUM_ANIM];
  AnimState currState;
  bool isMoving;
} player;

void player_anim(player *p, int numFrames, int *currFrame, int *frameCounter,
                 int frameSpeed);
void player_move(player *p, float speed, float sprint);

int main(void) {
  ChangeDirectory(TextFormat(
      "%s/..", GetApplicationDirectory())); // Make sure the working directory
                                            // is set in blossom_c folder.
  InitWindow(1280, 720, "Blossom");
  GuiLoadStyle("styles/style_dark.rgs");

  Shader shader = LoadShader(0, "shaders/shader.frag");

  player p = {.pos = (Vector2){(float)GetScreenWidth() / 2.0f,
                               (float)GetScreenHeight() / 2.0f},
              .currState = IDLE_DOWN,
              .isMoving = false};

  p.tex[IDLE_DOWN] = LoadTexture("assets/Idle_Down.png");
  p.tex[IDLE_UP] = LoadTexture("assets/Idle_Up.png");
  p.tex[IDLE_LEFT] = LoadTexture("assets/Idle_Left.png");
  p.tex[IDLE_RIGHT] = LoadTexture("assets/Idle_Right.png");
  p.tex[RUN_LEFT] = LoadTexture("assets/Run_Left.png");
  p.tex[RUN_RIGHT] = LoadTexture("assets/Run_Right.png");
  p.tex[RUN_UP] = LoadTexture("assets/Run_Up.png");
  p.tex[RUN_DOWN] = LoadTexture("assets/Run_Down.png");
  p.rec = (Rectangle){0, 8, (float)p.tex[p.currState].width / NUM_FRAMES,
                      (float)p.tex[p.currState].height};

  int currFrame = 0;
  int frameCounter = 0;

  float resolution[2] = {GetScreenWidth(), GetScreenHeight()};

  SetTargetFPS(60);

  while (!WindowShouldClose()) {

    float time = GetTime();
    SetShaderValue(shader, GetShaderLocation(shader, "time"), &time,
                   SHADER_UNIFORM_FLOAT);

    // Everything that isn't a texture
    player_anim(&p, NUM_FRAMES, &currFrame, &frameCounter, 8);
    player_move(&p, 3.0f, 50.0f);

    BeginDrawing();

    ClearBackground(RAYWHITE);

    DrawText("Alpha", (float)GetScreenWidth() / 2.0f - 48.0f,
             (float)GetScreenHeight() / 2.0f - 60.0f, 48, GRAY);

    // BeginShaderMode(shader);
    //  Static textures
    DrawRectanglePro(
        (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight() / 2.0f},
        (Vector2){0, (-1) * (GetScreenHeight() / 2.0f)}, 0, DARKBROWN);
    DrawRectangleLinesEx(
        (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight() / 2.0f}, 5.0f,
        BLACK);

    // EndShaderMode();

    // Dynamic textures
    DrawTextureRec(p.tex[p.currState], p.rec, p.pos, WHITE); // player

    // UI.
    GuiPanel((Rectangle){10, 10, 65, 50}, "Settings");
    GuiLabel((Rectangle){15, 50, 40, 1}, TextFormat("FPS: %i", GetFPS()));

    EndDrawing();
  }

  for (int i = 0; i < NUM_ANIM; i++)
    UnloadTexture(p.tex[i]);

  UnloadShader(shader);
  CloseWindow();
  return 0;
}

void player_anim(player *p, int numFrames, int *currFrame, int *frameCounter,
                 int frameSpeed) {
  (*frameCounter)++;

  if (*frameCounter >= (60 / frameSpeed)) {
    *frameCounter = 0;
    (*currFrame)++;

    if (*currFrame >= numFrames)
      *currFrame = 0;

    p->rec.x =
        (float)*currFrame * (float)p->tex[p->currState].width / numFrames;
  }
}

void player_move(player *p, float speed, float sprint) {
  Vector2 move = Vector2Zero();

  if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
    move.x -= speed;
    if (!(p->currState == RUN_LEFT)) {
      p->currState = RUN_LEFT;
      p->isMoving = true;
    }
  }
  if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
    move.x += speed;
    if (!(p->currState == RUN_RIGHT)) {
      p->currState = RUN_RIGHT;
      p->isMoving = true;
    }
  }
  if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
    move.y -= speed;
    if (!(p->currState == RUN_UP)) {
      p->currState = RUN_UP;
      p->isMoving = true;
    }
  }
  if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
    move.y += speed;
    if (!(p->currState == RUN_DOWN)) {
      p->currState = RUN_DOWN;
      p->isMoving = true;
    }
  }
  if (IsKeyPressed(KEY_LEFT_SHIFT))
    move = Vector2Scale(move, sprint);

  p->pos = Vector2Add(p->pos, move);
}
