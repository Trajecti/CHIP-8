#include "tilemap.h"
#include <iostream>
#include <utility>

tileMap::tileMap(const int M_WIDTH, const int M_HEIGHT,
                 const int S_WIDTH, const int S_HEIGHT):
    MAP_WIDTH(M_WIDTH), MAP_HEIGHT(M_HEIGHT), SCREEN_WIDTH(S_WIDTH),
    SCREEN_HEIGHT(S_HEIGHT), TILE_NUM(M_WIDTH*M_HEIGHT) {
    this->window = nullptr;
    this->surface = nullptr;

    this->tile_width = SCREEN_WIDTH/MAP_WIDTH;
    this->tile_height = SCREEN_HEIGHT/MAP_HEIGHT;
    //Initialization flag
    bool success = true;

    if ((tile_width < 1) || (tile_height < 1)) {
        success = false;
        return;
    }

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        std::cerr << "SDL could not be initialized! SDL_Error: " << SDL_GetError() << std::endl;
        success = false;
    } else {
        //Create window
        window = SDL_CreateWindow( "CHIP-8 Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( window == NULL ) {
            std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            success = false;
        } else {
            surface = SDL_GetWindowSurface( window );
            if ( surface == NULL ) {
              std::cerr << "Surface could not be created! SDL_Error: " << SDL_GetError() << std::endl;
              success = false;
            }
            for (int i = 0; i < MAP_HEIGHT; ++i) {
                for (int j = 0; j < MAP_WIDTH; ++j) {
                    tiles.push_back(genRect(j * tile_width, i * tile_height));
                }
            }
        }
    }
    return;
}

char tileMap::waitForKeyPress() {
    SDL_Event press_event;
    while (SDL_PollEvent(&press_event)) {
        if (press_event.type == SDL_KEYDOWN) {
            if (keys.count(press_event.key.keysym.sym)) {
                return keys[press_event.key.keysym.sym];
            }
        }
    }
}

SDL_Rect tileMap::genRect(int x, int y) {
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = tile_width;
    rect.h = tile_height;
    return rect;
}

void tileMap::draw(unsigned short* tile_buffer, int buffer_size) {
    if (TILE_NUM != buffer_size) {
        std::cerr << "Error: tileMap::updateMap - Buffer size is not the same as the number of tiles." << std::endl;
        return;
    }
    SDL_Surface* black = SDL_CreateRGBSurface(0, tile_width, tile_height, 32, 0, 0, 0, 0);
    SDL_Surface* white = SDL_CreateRGBSurface(0, tile_width, tile_height, 32, 255, 255, 255, 0);
    for (int i = 0; i < TILE_NUM; ++i) {
        if (tile_buffer[i]) {
            SDL_FillRect(surface, &tiles[i], SDL_MapRGB(white->format, 255, 255, 255));
        } else {
            SDL_FillRect(surface, &tiles[i], SDL_MapRGB(black->format, 0, 0, 0));
        }
    }
}

tileMap::~tileMap(){
    //Close window
    SDL_DestroyWindow( window );
    window = NULL;
    //Quit SDL
    SDL_Quit();
}
