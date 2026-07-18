class animation {
private:
  int counter = 0.0f;
  int frame = 0.0f;

public:
  animation() = default;
  ~animation() = default;

  Texture2D tex[5];
  Rectangle rec;
  int frameSpeed;

  void anim_update(int dir, int state, float deltaTime) {
    counter++;

    if (counter >= ((1 / deltaTime) / frameSpeed)) {
      counter = 0;
      frame++;

      if (frame >= (tex[state].width / rec.width))
        frame = 0;

      rec.x =
          (float)(frame * tex[state].width / (tex[state].width / rec.width));
      rec.y =
          (float)(dir * tex[state].height / (tex[state].height / rec.height));
    }
  }
};
