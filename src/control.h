/***************************************************************************
                          control.h  -  description
                             -------------------
    begin                : Tue June 11 2002
    copyright            : (C) 2002 by Rick McDaniel
    email                : rickeym@swbell.net
$Id: control.h,v 1.3 2003/04/14 05:51:03 mbridak Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License.     *
 *    .                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTROL_H
#define CONTROL_H


/**
  *@author Rick McDaniel
  */

class Control {
 public: 
  Submarine *Subs;	
  
  SDL_Rect src, dest;
  
  SDL_Surface *controlscreen;
  SDL_Surface *tempscreen;

  SDL_Surface *ClearControl;
  
  Control(Submarine *temp);
  ~Control();
  
  bool A13, A23, ASTD, AFULL, AFLK;
  bool ASTOP, B13, B23, BSTD, BEMER;
  
  bool depthup, depthdown;

  void  InitGraphics(SDL_Surface *temp, SDL_Surface *tempcontrolscreen);
  
  void ClearHeading();
  void ClearOrdHeading();


  void ClearSpeed();
  void ClearOrdSpeed();
  
  void ClearDepth();
  void ClearOrdDepth();

  void LoadWidgets();
  
  void DisplayWidgets();
  
  void ToggleBEMER();
  void ToggleBSTD();
  void ToggleB23();
  void ToggleB13();
  void ToggleASTOP();
  void ToggleA13();
  void ToggleA23();
  void ToggleASTD();
  void ToggleAFULL();
  void ToggleAFLK();
  
  void Display();

  void AdjustHeading(int x, int y);

  void AdjustDepth(int x);
  
 // void DPixel(SDL_Surface *screen, int x, int y, Uint32 color);  //rdm not needed
  
 // void DLine(SDL_Surface *screen, int X1, int Y1, int X2, int Y2, Uint32 Color);  //rdm not needed
  
 private:
  
  SDL_Surface *screen;
  SDL_Surface *STOPoff;
  SDL_Surface *STOPon;
  SDL_Surface *A13off;
  SDL_Surface *A13on;
  SDL_Surface *A23off;
  SDL_Surface *A23on;
  SDL_Surface *ASTDoff;
  SDL_Surface *ASTDon;
  SDL_Surface *AFULLoff;
  SDL_Surface *AFULLon;
  SDL_Surface *AFLKoff;
  SDL_Surface *AFLKon;
  SDL_Surface *depthupoff;
  SDL_Surface *depthupon;
  SDL_Surface *depthdownoff;
  SDL_Surface *depthdownon;

  
  int x;
  int y;
  // Uint32 orange, green, black; //rdm 9/11 not needed
  
  static char text[120];
  DFont largeFont(const char*, const char *);
  DFont fnt(const char*, const char *);
  DFont fnt2(const char*, const char *);
  
};

#endif
