#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  static char vertDebugStr[120];
  static char windowSizeStr[30];
  // static bool NRFull = false;

  // #ifndef PLATFORM_WEB
  //   if (NRFull) {
  //     bool wasFull = IsWindowFullscreen();
  //     ToggleFullscreen();
  //     if (wasFull)
  //       SetWindowSize(OGscreenSize.x, screenSize.y);
  //     NRFull = false;
  //   }
  //   if (IsKeyPressed(KEY_F11)) {
  //     NRFull = true;
  //     SetWindowSize(1920, 1080);
  //   }
  // #endif /* ifndef PLATFORM_WEB */
  // checking debug binds
  //
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

  // other user input
  if (IsKeyDown(KEY_W))
    player.pos.y -= player.speed;
  else if (IsKeyDown(KEY_S))
    player.pos.y += player.speed;
  if (IsKeyDown(KEY_A))
    player.pos.x -= player.speed;
  else if (IsKeyDown(KEY_D))
    player.pos.x += player.speed;

  // debug and stuff
  strcpy(vertDebugStr, "");
  int prevPos = 0;
  fprintf(stderr, "line: ");
  for (int i = 1; i < map.size.x; ++i) {
    char tmp[20];

    int pos = (int)(tileSize.x * i * screenScale.x);
    if (((pos - prevPos) - (int)(tileSize.x * screenScale.x)) > 0) {
      fprintf(stderr, "dec ");
      --pos;
    }

    sprintf(tmp, "%i, ", pos - prevPos - (int)(tileSize.x * screenScale.x));
    if (i % 40 == 0) {
      strcat(tmp, "\n");
    }
    strcat(vertDebugStr, tmp);

    prevPos = pos;
  }
  fprintf(stderr, "\n");
  sprintf(windowSizeStr, "%i, %i", (int)screenSize.x, (int)screenSize.y);

  // drawing
  BeginDrawing();
  ClearBackground(RAYWHITE);
  for (int i = 0; i < map.size.y; ++i) {
    for (int k = 0; k < map.size.x; ++k) {
      int tile = map.data[(int)(k + i * map.size.x)];
      Vector2 pos = {tileSize.x * i * screenScale.x,
                     tileSize.y * k * screenScale.y};
      DrawTexturePro(textures.tiles, tileLookUp[tile],
                     (Rectangle){pos.x, pos.y, tileSize.x * screenScale.x,
                                 tileSize.y * screenScale.y},
                     player.pos, 0, WHITE);
    }
  }
  DrawFPS(10, 10);
  DrawText(windowSizeStr, 10, 35, 20, BLACK);
  DrawText(vertDebugStr, 10, 65, 10, BLACK);
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
  player.speed = 10;

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
