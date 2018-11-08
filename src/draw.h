#ifndef DRAW_HEADER_FILE__
#define DRAW_HEADER_FILE__

#include <SDL.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
// #include "SDL/SDL_image.h"
#include <SDL/SDL_thread.h>

#define SCREEN_NAV 2
#define SCREEN_HELM 4


void DrawPixel(SDL_Surface *screen, int x, int y, Uint32 color);

void DrawLine(SDL_Surface *screen, int X1, int Y1, int X2, int Y2, Uint32 Color);

void DrawArc(SDL_Surface *screen, int X1, int Y1, int Radius, int Theta1, int Theta2, Uint32 Color);

void DrawCircle(SDL_Surface *screen, int X1, int Y1, int Radius, Uint32 Color);

void FillCircle(SDL_Surface *screen, int X1, int Y1, int Radius, Uint32 Color);

void DrawDiamond(SDL_Surface *screen, int X1, int Y1, int Size, char Direction, Uint32 Color);

void DrawBox(SDL_Surface *screen, int X1, int Y1, int Size, char Direction, Uint32 Color);

void DrawRectangle(SDL_Surface *screen, int x1, int y1, int x2, int y2, Uint32 color);

void FillRectangle(SDL_Surface *screen, int x1, int y1, int x2, int y2, Uint32 color);

void DrawCross(SDL_Surface *screen, int X1, int Y1, int Size, char Direction, Uint32 Color);

#endif
