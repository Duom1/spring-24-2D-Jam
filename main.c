#include <raylib.h>
#include <raymath.h>
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

typedef struct {
  Vector2 pos;
  int speed;
} Player;
Player player;

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
  // checking debug binds
  updateSizeAndScale();
  if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) {
    if (IsKeyPressed(KEY_ZERO)) {
      SetWindowSize(OGscreenSize.x * screenScale.y, screenSize.y);
    }
    if (IsKeyPressed(KEY_LEFT_BRACKET)) {
      tileSize.x -= 10;
      tileSize.y -= 10;
    } else if (IsKeyPressed(KEY_RIGHT_BRACKET)) {
      tileSize.x += 10;
      tileSize.y += 10;
    }
  }
  // movement
  if (IsKeyDown(KEY_W))
    player.pos.y -= player.speed;
  else if (IsKeyDown(KEY_S))
    player.pos.y += player.speed;
  if (IsKeyDown(KEY_A))
    player.pos.x -= player.speed;
  else if (IsKeyDown(KEY_D))
    player.pos.x += player.speed;

  // drawing
  BeginDrawing();
  ClearBackground(RAYWHITE);
  for (int i = 0; i < map.size.y; ++i) {
    for (int k = 0; k < map.size.x; ++k) {
      int tile = map.data[(int)(k + i * map.size.x)];
      DrawTexturePro(textures.tiles, tileLookUp[tile],
                     (Rectangle){tileSize.x * i * screenScale.x,
                                 tileSize.y * k * screenScale.y,
                                 tileSize.x * screenScale.x,
                                 tileSize.y * screenScale.y},
                     player.pos, 0.0, WHITE);
    }
  }
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
    for (int k = 0; k < mapImage.width; ++k) {
      Color pc = GetImageColor(mapImage, i, k);
      if (ColorEqual(pc, (Color){0, 0, 255, 255}))
        map.data[k + i * mapImage.width] = TILE_WATER;
      if (ColorEqual(pc, (Color){0, 255, 0, 255}))
        map.data[k + i * mapImage.width] = TILE_DIRT;
    }
  }
  UnloadImage(mapImage);

  player.pos = (Vector2){0, 0};
  player.speed = 5;

#ifdef PLATFORM_WEB
  emscripten_set_main_loop(updateDrawFrame, TARGET_FPS, 1);
#else
  SetTargetFPS(TARGET_FPS);
  while (!WindowShouldClose()) {
    updateDrawFrame();
  }
#endif

  free(map.data);
  UnloadTexture(textures.tiles);
  CloseWindow();

  return 0;
}
