#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif

#define TARGET_FPS 240
#define ZERO_VECTOR2                                                           \
  (Vector2) { 0, 0 }

#define ORIGINAL_SCREEN_SIZE                                                   \
  { (float)1920 / 2, (float)1080 / 2 }

Vector2 screenSize = ORIGINAL_SCREEN_SIZE;
Vector2 OGscreenSize = ORIGINAL_SCREEN_SIZE;
Vector2 screenScale = {1.0, 1.0};

typedef struct {
  Texture tiles;
} Textures;
Textures textures;

enum {
  TILE_WATER,
  TILE_DIRT,
};

Rectangle tileLookUp[] = {{0, 0, 16, 16}, {16, 0, 16, 16}};

typedef struct {
  Vector2 size;
  int *data;
} Map;

Map map;

Vector2 tileSize = {100, 100};

void updateSizeAndScale(void) {
  screenSize.x = GetScreenWidth();
  screenSize.y = GetScreenHeight();
  screenScale.x = screenSize.x / OGscreenSize.x;
  screenScale.y = screenSize.y / OGscreenSize.y;
}

bool ColorEqual(Color color1, Color color2) {
  return (color1.r == color2.r) && (color1.g == color2.g) &&
         (color1.b == color2.b) && (color1.a == color2.a);
}

void updateDrawFrame(void) {
  updateSizeAndScale();
  if (IsKeyPressed(KEY_ZERO) && IsKeyDown(KEY_LEFT_CONTROL)) {
    SetWindowSize(OGscreenSize.x * screenScale.y, screenSize.y);
  }
  if (IsKeyPressed(KEY_MINUS) && IsKeyDown(KEY_LEFT_CONTROL)) {
    Vector2AddValue(tileSize, 10);
  } else if (IsKeyPressed(KEY_KP_ENTER) && IsKeyDown(KEY_LEFT_CONTROL)) {
    Vector2AddValue(tileSize, -10);
  }
  BeginDrawing();
  ClearBackground(RAYWHITE);
  // for (int i = 0; i < MAP_HEIGHT; ++i) {
  //   for (int k = 0; k < MAP_WIDTH; ++k) {
  //     int tile = map[i][k];
  //     DrawTexturePro(textures.tiles, tileLookUp[tile],
  //                    (Rectangle){tileSize.x * i * screenScale.x,
  //                                tileSize.y * k * screenScale.y,
  //                                tileSize.x * screenScale.x,
  //                                tileSize.y * screenScale.y},
  //                    ZERO_VECTOR2, 0.0, WHITE);
  //   }
  // }
  DrawFPS(10, 10);
  EndDrawing();
}

int main(void) {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(screenSize.x, screenSize.y, "Rescue the vampire");

  // loading screen
  BeginDrawing();
  ClearBackground(BLACK);
  DrawText("Loading...", 10, 10, 40, GREEN);
  EndDrawing();

  textures.tiles = LoadTexture("./res/tiles.png");
  Image mapImage = LoadImage("./res/map.png");
  map.size.x = mapImage.width;
  map.size.y = mapImage.height;
  map.data = malloc(sizeof(int) * mapImage.height * mapImage.height);
  for (int i = 0; i < mapImage.height; ++i) {
    for (int k = 0; k < mapImage.width; ++i) {
      Color pc = GetImageColor(mapImage, i, k);
      if (ColorEqual(pc, (Color){0, 0, 255, 255}))
        map.data[k + i * mapImage.width] = TILE_WATER;
    }
  }
  UnloadImage(mapImage);

#ifdef PLATFORM_WEB
  emscripten_set_main_loop(updateDrawFrame, TARGET_FPS, 1);
#else
  SetTargetFPS(TARGET_FPS);
  while (!WindowShouldClose()) {
    updateDrawFrame();
  }
#endif

  UnloadTexture(textures.tiles);
  CloseWindow();

  return 0;
}
