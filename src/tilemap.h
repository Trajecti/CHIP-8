#ifndef TILEMAP.H
#define TILEMAP.H
#include "SDL2/SDL.h"
#include <vector>
#include <map>

class tileMap {
 public:
  const int MAP_HEIGHT, MAP_WIDTH;
  const int SCREEN_HEIGHT, SCREEN_WIDTH;
  void draw(unsigned short* tile_buffer, int buffer_size);
  void setKeys(std::map<SDL_Keycode,char>);
  char waitForKeyPress();
  tileMap(const int, const int, const int, const int);
  ~tileMap();
private:
    //The window we'll be rendering to
    const int TILE_NUM;
    int tile_height, tile_width;
    SDL_Window* window;
    SDL_Surface* surface;
    std::vector<SDL_Rect> tiles;
    std::map<SDL_Keycode,char> keys;
    SDL_Rect genRect(int x, int y);
};
#endif
