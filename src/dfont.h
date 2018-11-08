// $Id: dfont.h,v 1.3 2003/04/14 05:51:03 mbridak Exp $
#ifndef DFONT_H
#define DFONT_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include <iostream>
using std::ios;

#include <fstream>
using std::istream;
using std::ostream;
using std::ofstream;
using std::ifstream;
using std::cerr;
using std::cin;
using std::cout;

#include <map>



class DFont
{
 protected:
  std::map <char, SDL_Rect> fonts;
  SDL_Surface *fontSurface;
  int gap;

 private:
  void initMap (char *);	
  void loadMap (char *);
  void loadImage (char *);
  Uint32 GetPixel (SDL_Surface *, Sint32, Sint32);

 public:
  DFont (char *, char * );		
  ~DFont ();
  void PutString(SDL_Surface *, Sint16 , Sint16 , char * );
  void PutChar(SDL_Surface *, Sint16, Sint16, char);
};

ostream& operator << (ostream & , SDL_Rect const &);//output a record
istream& operator >> (istream & , SDL_Rect &);//input a record
#endif
