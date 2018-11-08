#include <SDL.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "SDL/SDL_image.h"
#include <SDL/SDL_thread.h>
#include <math.h>
#include "draw.h"


void DrawPixel(SDL_Surface *screen, int x, int y, Uint32 color){
        //this only works for 16bpp screens
        //are we outside the screen?????
        //If we are bail out now before it's too late!

        if (x > 1023 || x < 0 || y > 759 || y < 0) {
                return;
        }

        //place the pixel on the screen
        Uint16 *pixel_location;
        pixel_location = (Uint16 *)screen->pixels + y*screen->pitch/2 + x;
        *pixel_location = color;
}

void DrawLine(SDL_Surface *screen, int X1, int Y1, int X2, int Y2, Uint32 Color){

//don't even ask me about this stuff All I know is it works
//and thats ALL I care about...

        int dx,dy,sdx,sdy,py,px,x,y;
        dx = X2 - X1;
        dy = Y2 - Y1;
        if (dx < 0) sdx = -1;
        else sdx = 1;
        if (dy < 0) sdy = -1;
        else sdy = 1;
        dx = sdx * dx + 1;
        dy = sdy * dy + 1;
        x = 0;
        y = 0;
        px = X1;
        py = Y1;
        if (dx >= dy){
                for (int x = 0; x < dx; x++){
                        DrawPixel(screen, px, py, Color);
                        y = y + dy;
                        if (y >= dx){
                                y = y - dx;
                                py = py + sdy;
                        }
                        px = px + sdx;
                }
        }else{
                for (int y = 0; y < dy; y++){
                        DrawPixel(screen, px, py, Color);
                        x = x + dx;
                        if (x >= dy){
                                x = x - dy;
                                px = px + sdx;
                        }
                        py = py + sdy;
                }
        }
}


void DrawArc(SDL_Surface *screen, int X1, int Y1, int Radius, int Theta1, int Theta2, Uint32 Color){

//Draw an arc  at (X1,Y1) of a given radius from theta1 to theta2 using specified Color.
        int x, y, xc, yc, radius;
        int theta, theta1, theta2;
        xc = X1;
        yc = Y1;
        radius = Radius;
        theta1 = Theta1;
        theta2 = Theta2;

        for(theta=theta1;theta<=theta2;theta+=5) {
          x = xc + int(radius*cos(theta*3.14/180.0));
          y = yc - int(radius*sin(theta*3.14/180.0));
          DrawPixel(screen, x, y, Color);
        }
}

void DrawCircle(SDL_Surface *screen, int X1, int Y1, int Radius, Uint32 Color){

//Draw a circle  at (X1,Y1) of a given radius using specified Color.
/*
        int xc, yc, radius;
        xc = X1;
        yc = Y1;
        radius = Radius;
*/
        DrawArc(screen, X1, Y1, Radius, 0, 360, Color);
}


void FillCircle(SDL_Surface *screen, int X1, int Y1, int Radius, Uint32 Color)
{
   int my_radius;
   my_radius = Radius;
   while (my_radius > 1)
   {
       DrawCircle(screen, X1, Y1, my_radius, Color);
       my_radius--;
   }
}

void DrawDiamond(SDL_Surface *screen, int X1, int Y1, int Size, char Direction, Uint32 Color){

  //Draw a diamond  at (X1,Y1) of Size using specified Color.
  // Direction: T = Top, B = Bottom, F = Full
        int xc, yc;
        int size;
        xc = X1;
        yc = Y1;
        size = Size;

        switch(Direction){

        case 'B':
          DrawLine(screen, xc-size, yc, xc, yc+size, Color); //bottomleft
          DrawLine(screen, xc+size, yc, xc, yc+size, Color); //bottomright
          break;

        case 'T':
          DrawLine(screen, xc, yc-size, xc-size, yc, Color); //topleft
          DrawLine(screen, xc, yc-size, xc+size, yc, Color); //topright
          break;

        default:
        DrawLine(screen, xc-size, yc, xc, yc+size, Color); //bottomleft
        DrawLine(screen, xc+size, yc, xc, yc+size, Color); //bottomright
        DrawLine(screen, xc, yc-size, xc-size, yc, Color); //topleft
        DrawLine(screen, xc, yc-size, xc+size, yc, Color); //topright
        }
}

void DrawBox(SDL_Surface *screen, int X1, int Y1, int Size, char Direction, Uint32 Color){

  //Draw a Box  at (X1,Y1) of Size using specified Color.
  // Direction: T = Top, B = Bottom, F = Full
        int xc, yc;
        int size;
        char direction;
        xc = X1;
        yc = Y1;
        size = Size;
        direction = Direction;

        switch(direction){

        case 'T':
          DrawLine(screen, xc-size, yc-size, xc-size, yc, Color);
          DrawLine(screen, xc-size, yc-size, xc+size, yc-size, Color);
          DrawLine(screen, xc+size, yc-size, xc+size, yc, Color);
          break;

        case 'B':
          DrawLine(screen, xc-size, yc, xc-size, yc+size, Color);
          DrawLine(screen, xc-size, yc+size, xc+size, yc+size, Color);
          DrawLine(screen, xc+size, yc, xc+size, yc+size, Color);
          break;

        default:
        DrawLine(screen, xc-size, yc-size, xc+size, yc-size, Color);
        DrawLine(screen, xc+size, yc-size, xc+size, yc+size, Color);
        DrawLine(screen, xc-size, yc-size, xc-size, yc+size, Color);
        DrawLine(screen, xc-size, yc+size, xc+size, yc+size, Color);
        }
}


void DrawRectangle(SDL_Surface *screen, int x1, int y1, int x2, int y2, Uint32 color)
{
   DrawLine(screen, x1, y1, x1, y2, color);
   DrawLine(screen, x1, y1, x2, y1, color);
   DrawLine(screen, x1, y2, x2, y2, color);
   DrawLine(screen, x2, y1, x2, y2, color);
}


void FillRectangle(SDL_Surface *screen, int x1, int y1, int x2, int y2, Uint32 color)
{
   int x, y;

   for (y = y1; y <= y2; y++)
   {
      for (x = x1; x <= x2; x++)
      {
         DrawPixel(screen, x, y, color);
      }
   }
}


 
void DrawCross(SDL_Surface *screen, int X1, int Y1, int Size, char Direction, Uint32 Color){

  //Draw a Cross  at (X1,Y1) of Size using specified Color.
  // Direction: T = Top, B = Bottom, F = Full
        int xc, yc;
        int size;
        char direction;
        xc = X1;
        yc = Y1;
        size = Size;
        direction = Direction;

        switch(direction){

        case 'T':
          DrawLine(screen, xc-size, yc-size/2, xc-size, yc, Color);
          DrawLine(screen, xc+size, yc-size/2, xc+size, yc, Color);
          DrawLine(screen, xc-size/2, yc-size, xc+size/2, yc-size, Color);
          DrawLine(screen, xc-size/2, yc-size, xc-size/2, yc-size/2, Color);
          DrawLine(screen, xc+size/2, yc-size, xc+size/2, yc-size/2, Color);
          DrawLine(screen, xc+size/2, yc-size/2, xc+size, yc-size/2, Color);
          DrawLine(screen, xc-size, yc-size/2, xc-size/2, yc-size/2, Color);
          break;

        case 'B':
          DrawLine(screen, xc-size, yc, xc-size, yc+size/2, Color);
          DrawLine(screen, xc+size, yc, xc+size, yc+size/2, Color);
          DrawLine(screen, xc-size/2, yc+size, xc+size/2, yc+size, Color);
          DrawLine(screen, xc-size/2, yc+size/2, xc-size/2, yc+size, Color);
          DrawLine(screen, xc+size/2, yc+size/2, xc+size/2, yc+size, Color);
          DrawLine(screen, xc-size, yc+size/2, xc-size/2, yc+size/2, Color);
          DrawLine(screen, xc+size/2, yc+size/2, xc+size, yc+size/2, Color);
          break;

        default:
          DrawLine(screen, xc-size, yc-size/2, xc-size, yc+size/2, Color);
          DrawLine(screen, xc+size, yc-size/2, xc+size, yc+size/2, Color);
          DrawLine(screen, xc-size/2, yc-size, xc+size/2, yc-size, Color);
          DrawLine(screen, xc-size/2, yc+size, xc+size/2, yc+size, Color);
          DrawLine(screen, xc-size/2, yc-size, xc-size/2, yc-size/2, Color);
          DrawLine(screen, xc-size/2, yc+size/2, xc-size/2, yc+size, Color);
          DrawLine(screen, xc+size/2, yc-size, xc+size/2, yc-size/2, Color);
          DrawLine(screen, xc+size/2, yc+size/2, xc+size/2, yc+size, Color);
          DrawLine(screen, xc-size, yc+size/2, xc-size/2, yc+size/2, Color);
          DrawLine(screen, xc+size/2, yc-size/2, xc+size, yc-size/2, Color);
          DrawLine(screen, xc-size, yc-size/2, xc-size/2, yc-size/2, Color);
          DrawLine(screen, xc+size/2, yc+size/2, xc+size, yc+size/2, Color);
        }
}
