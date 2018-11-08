/***************************************************************************
                          control.cpp  -  description
                             -------------------
    begin                : Tue June 11 2002
    copyright            : (C) 2002 by Rick McDaniel
    email                : rickeym@swbell.net
$Id: control.cpp,v 1.3 2003/04/20 19:30:29 anoncvs_pygsl Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License.     *
 *    .                                   *
 *                                                                         *
 ***************************************************************************/
#include <string>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_thread.h"
#include "SDL/SDL_gfxPrimitives.h" //rdm 9/11
#include "dfont.h"
#include "submarine.h"
#include "files.h"

#include "control.h"
#include "dstack.h"
#include <cmath>
#include <cstdlib>

#include <iostream>
using namespace std;

// Use these for the display
DStack HeadingStack, SpeedStack, DepthStack;

// Used to remember last ordered heading, to clear display.
float old_heading;


Control::Control(Submarine *temp): Subs(temp)
{
  // Default Values
  BEMER = false;
  BSTD = false;
  B23 = false;
  B13 = false;
  ASTOP = false;
  A13 = true;
  A23 = false;
  ASTD = false;
  AFULL = false;
  AFLK = false;
  depthup = false;
  depthdown = false;
}

Control::~Control(){
}

void Control::InitGraphics(SDL_Surface *temp, SDL_Surface *tempcontrolscreen)
{
  screen=temp;
  controlscreen=tempcontrolscreen;
  
  temp = SDL_CreateRGBSurface(SDL_SWSURFACE, 420, 490, 16,
			      screen->format->Rmask,
			      screen->format->Gmask,
			      screen->format->Bmask,
			      screen->format->Amask);
  tempscreen=SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);
  
  LoadWidgets();
  DisplayWidgets();
  
//  orange = SDL_MapRGB(screen->format, 238, 118, 0);  //rdm 9/11  not needed
 // green = SDL_MapRGB(screen->format, 0, 128, 0);
 // black = SDL_MapRGB(screen->format, 0, 0, 0);

}

void Control::LoadWidgets(){
  SDL_Surface *temp;
  
  temp = Load_Image("images/STOPoff.png");
  if(temp != NULL) STOPoff = SDL_DisplayFormat(temp);
  if ( STOPoff == NULL ) { 
    cerr<<"Function LoadWidgets()" << endl
	<< SDL_GetError() << endl;
    SDL_Quit();
    exit(0);
  }
  SDL_FreeSurface(temp);
  
  temp = Load_Image("images/STOPon.png");
  if(temp != NULL) STOPon = SDL_DisplayFormat(temp);
  if ( STOPon == NULL ) { 
    cerr<<"Function LoadWidgets()" << endl
	<< SDL_GetError() << endl;
    SDL_Quit();
    exit(0);
  }
  SDL_FreeSurface(temp);
  
  temp = Load_Image("images/A13off.png");
  if(temp != NULL) A13off = SDL_DisplayFormat(temp);
  if ( A13off == NULL ) { 
    cerr<<"Function LoadWidgets()" << endl
	<< SDL_GetError() << endl;
    SDL_Quit();
    exit(0);
  }
  SDL_FreeSurface(temp);
  
  temp = Load_Image("images/A13on.png");
  if(temp != NULL) A13on = SDL_DisplayFormat(temp);
  if ( A13on == NULL ) { 
    cerr<<"Function LoadWidgets()" << endl
	<< SDL_GetError() << endl;
    SDL_Quit();
    exit(0);
  }
  SDL_FreeSurface(temp);
  
  temp = Load_Image("images/A23off.png");
  if(temp != NULL) A23off = SDL_DisplayFormat(temp);
  if ( A23off == NULL ) { 
    cerr<<"Function LoadWidgets()" << endl
	<< SDL_GetError() << endl;
    SDL_Quit();
    exit(0);
  }
  SDL_FreeSurface(temp);
  
  temp = Load_Image("images/A23on.png");
  if(temp != NULL) A23on = SDL_DisplayFormat(temp);
  if ( A23on == NULL ) { 
    cerr<<"Function LoadWidgets()" << endl
	<< SDL_GetError() << endl;
    SDL_Quit();
    exit(0);
  }
  SDL_FreeSurface(temp);
  
  temp = Load_Image("images/ASTDoff.png");
  if(temp != NULL) ASTDoff = SDL_DisplayFormat(temp);
  if ( ASTDoff == NULL ) { 
    cerr<<"Function LoadWidgets()" << endl
	<< SDL_GetError() << endl;
    SDL_Quit();
    exit(0);
  }
  SDL_FreeSurface(temp);
  
  temp = Load_Image("images/ASTDon.png");
  if(temp != NULL) ASTDon = SDL_DisplayFormat(temp);
  if ( ASTDon == NULL ) { 
    cerr<<"Function LoadWidgets()" << endl
	<< SDL_GetError() << endl;
    SDL_Quit();
    exit(0);
  }
  SDL_FreeSurface(temp);
  
  temp = Load_Image("images/AFULLoff.png");
  if(temp != NULL) AFULLoff = SDL_DisplayFormat(temp);
  if ( AFULLoff == NULL ) { 
    cerr<<"Function LoadWidgets()" << endl
	<< SDL_GetError() << endl;
    SDL_Quit();
    exit(0);
  }
  SDL_FreeSurface(temp);
  
  temp = Load_Image("images/AFULLon.png");
  if(temp != NULL) AFULLon = SDL_DisplayFormat(temp);
  if ( AFULLon == NULL ) { 
    cerr<<"Function LoadWidgets()" << endl
	<< SDL_GetError() << endl;
    SDL_Quit();
    exit(0);
  }
  SDL_FreeSurface(temp);
  
  temp = Load_Image("images/AFLKoff.png");
  if(temp != NULL) AFLKoff = SDL_DisplayFormat(temp);
  if ( AFLKoff == NULL ) { 
    cerr<<"Function LoadWidgets()" << endl
	<< SDL_GetError() << endl;
    SDL_Quit();
    exit(0);
  }
  SDL_FreeSurface(temp);
  
  temp = Load_Image("images/AFLKon.png");
  if(temp != NULL) AFLKon = SDL_DisplayFormat(temp);
  if ( AFLKon == NULL ) { 
    cerr<<"Function LoadWidgets()" << endl
	<< SDL_GetError() << endl;
    SDL_Quit();
    exit(0);
  }
  SDL_FreeSurface(temp);

  temp = Load_Image("images/depthupoff.png");
  if(temp != NULL) depthupoff = SDL_DisplayFormat(temp);
  if ( depthupoff == NULL ) { 
    cerr<<"Function LoadWidgets()" << endl
	<< SDL_GetError() << endl;
    SDL_Quit();
    exit(0);
  }
  SDL_FreeSurface(temp);
  
  temp = Load_Image("images/depthupon.png");
  if(temp != NULL) depthupon = SDL_DisplayFormat(temp);
  if ( depthupon == NULL ) { 
    cerr<<"Function LoadWidgets()" << endl
	<< SDL_GetError() << endl;
    SDL_Quit();
    exit(0);
  }
  SDL_FreeSurface(temp);

 temp = Load_Image("images/depthdownoff.png");
  if(temp != NULL) depthdownoff = SDL_DisplayFormat(temp);
  if ( depthdownoff == NULL ) { 
    cerr<<"Function LoadWidgets()" << endl
	<< SDL_GetError() << endl;
    SDL_Quit();
    exit(0);
  }
  SDL_FreeSurface(temp);
  
  temp = Load_Image("images/depthdownon.png");
  if(temp != NULL) depthdownon = SDL_DisplayFormat(temp);
  if ( depthdownon == NULL ) { 
    cerr<<"Function LoadWidgets()" << endl
	<< SDL_GetError() << endl;
    SDL_Quit();
    exit(0);
  }
  SDL_FreeSurface(temp);

}

void Control::DisplayWidgets(){
  if(ASTOP)
    {
      dest.x = 231;
      dest.y = 582;
      dest.h = STOPon->h; 
      dest.w = STOPon->w;
      SDL_BlitSurface(STOPon, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  else
    {
      dest.x = 231;
      dest.y = 582;
      dest.h = STOPoff->h; 
      dest.w = STOPoff->w;
      SDL_BlitSurface(STOPoff, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }

  if(A13)
    {
      dest.x = 264;
      dest.y = 582;
      dest.h = A13on->h; 
      dest.w = A13on->w;
      SDL_BlitSurface(A13on, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  else
    {
      dest.x = 264;
      dest.y = 582;
      dest.h = A13off->h; 
      dest.w = A13off->w;
      SDL_BlitSurface(A13off, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }

  if(A23)
    {
      dest.x = 297;
      dest.y = 581;
      dest.h = A23on->h; 
      dest.w = A23on->w;
      SDL_BlitSurface(A23on, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  else
    {
      dest.x = 297;
      dest.y = 581;
      dest.h = A23off->h; 
      dest.w = A23off->w;
      SDL_BlitSurface(A23off, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
	
  if(ASTD)
    {
      dest.x = 330;
      dest.y = 581;
      dest.h = ASTDon->h; 
      dest.w = ASTDon->w;
      SDL_BlitSurface(ASTDon, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  else
    {
      dest.x = 330;
      dest.y = 581;
      dest.h = ASTDoff->h; 
      dest.w = ASTDoff->w;
      SDL_BlitSurface(ASTDoff, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }

  if(AFULL)
    {
      dest.x = 363;
      dest.y = 581;
      dest.h = AFULLon->h; 
      dest.w = AFULLon->w;
      SDL_BlitSurface(AFULLon, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  else
    {
      dest.x = 363;
      dest.y = 581;
      dest.h = AFULLoff->h; 
      dest.w = AFULLoff->w;
      SDL_BlitSurface(AFULLoff, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
		
  if(AFLK)
    {
      dest.x = 396;
      dest.y = 581;
      dest.h = AFLKon->h; 
      dest.w = AFLKon->w;
      SDL_BlitSurface(AFLKon, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  else
    {
      dest.x = 396;
      dest.y = 581;
      dest.h = AFLKoff->h; 
      dest.w = AFLKoff->w;
      SDL_BlitSurface(AFLKoff, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }

  if(B13)
    {
      dest.x = 198;
      dest.y = 582;
      dest.h = A13on->h; 
      dest.w = A13on->w;
      SDL_BlitSurface(A13on, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  else
    {
      dest.x = 198;
      dest.y = 582;
      dest.h = A13off->h; 
      dest.w = A13off->w;
      SDL_BlitSurface(A13off, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  if(B23)
    {
      dest.x = 165;
      dest.y = 581;
      dest.h = A23on->h; 
      dest.w = A23on->w;
      SDL_BlitSurface(A23on, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  else
    {
      dest.x = 165;
      dest.y = 581;
      dest.h = A23off->h; 
      dest.w = A23off->w;
      SDL_BlitSurface(A23off, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  if(BSTD)
    {
      dest.x = 132;
      dest.y = 581;
      dest.h = ASTDon->h; 
      dest.w = ASTDon->w;
      SDL_BlitSurface(ASTDon, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  else
    {
      dest.x = 132;
      dest.y = 581;
      dest.h = ASTDoff->h; 
      dest.w = ASTDoff->w;
      SDL_BlitSurface(ASTDoff, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  
  if(BEMER)
    {
      dest.x = 99;
      dest.y = 581;
      dest.h = AFLKon->h; 
      dest.w = AFLKon->w;
      SDL_BlitSurface(AFLKon, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  else
    {
      dest.x = 99;
      dest.y = 581;
      dest.h = AFLKoff->h; 
      dest.w = AFLKoff->w;
      SDL_BlitSurface(AFLKoff, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }

 if(depthdown)
    {
      dest.x = 111;
      dest.y = 253;
      dest.h = depthdownon->h; 
      dest.w = depthdownon->w;
      SDL_BlitSurface(depthdownon, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  else
    {
      dest.x = 111;
      dest.y = 253;
      dest.h = depthdownoff->h; 
      dest.w = depthdownoff->w;
      SDL_BlitSurface(depthdownoff, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
   if(depthup)
    {
      dest.x = 384;
      dest.y = 256;
      dest.h = depthupon->h; 
      dest.w = depthupon->w;
      SDL_BlitSurface(depthupon, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  else
    {
      dest.x = 384;
      dest.y = 256;
      dest.h = depthupoff->h; 
      dest.w = depthupoff->w;
      SDL_BlitSurface(depthupoff, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
}

void Control::ClearHeading(){

 // Clear the screen
  SDL_Surface *temp;
  temp = Load_Image("images/ClearControl.png");
  if(temp != NULL) ClearControl = SDL_DisplayFormat(temp);
  if ( ClearControl == NULL ) { 
    cerr<<"Function LoadWidgets()" << endl
	<< SDL_GetError() << endl;
    SDL_Quit();
    exit(0);
  }

  SDL_FreeSurface(temp);
  
  src.x = 0;
  src.y = 0;
  src.w = ClearControl->w;
  src.h = ClearControl->h;

  dest.x = 107; // Blit destination x&y to the upper left
  dest.y = 140;
  dest.w = ClearControl->w; // Height and width equal to the
  dest.h = ClearControl->h;  // source images....
  SDL_BlitSurface(ClearControl, NULL, screen, &dest); // Do the actual Blit

  SDL_UpdateRects(screen, 1, &dest); //Show the screen.
  SDL_FreeSurface(ClearControl); //Free up the surface memory.
}

void Control::ClearOrdHeading(){
	int x, y;
	double x1, y1, x2, y2;//rdm 9/11
	
 // Clear Last Ordered Heading Compass //rdm 9/11
	x = int(251.0 + 58.0*cos(1.57-old_heading*3.14/180.0));
	y = int(344.0 - 58.0*sin(1.57-old_heading*3.14/180.0));
//	filledTrigonRGBA(screen, 251, 341, 251, 347, x, y, 0, 0, 0, 255);  //rdm 9/11
	
	x1 = int(251.0 + 4.0*sin(1.57-old_heading*3.14/180.0));
	y1 = 	int(344.0 + 4.0*cos(1.57-old_heading*3.14/180.0));
	x2 = int(251.0 - 4.0*sin(1.57-old_heading*3.14/180.0));
	y2 = int(344.0 - 4.0*cos(1.57-old_heading*3.14/180.0));
	
	filledTrigonRGBA(screen, x1, y1, x2, y2, x, y, 0, 0, 0, 255); 

 // Clear the screen
  SDL_Surface *temp;
  temp = Load_Image("images/ClearControl.png");
  if(temp != NULL) ClearControl = SDL_DisplayFormat(temp);
  if ( ClearControl == NULL ) { 
    cerr<<"Function LoadWidgets()" << endl
	<< SDL_GetError() << endl;
    SDL_Quit();
    exit(0);
  }

  SDL_FreeSurface(temp);
  
  src.x = 0;
  src.y = 0;
  src.w = ClearControl->w;
  src.h = ClearControl->h;

  dest.x = 107; // Blit destination x&y to the upper left
  dest.y = 195;
  dest.w = ClearControl->w; // Height and width equal to the
  dest.h = ClearControl->h;  // source images....
  SDL_BlitSurface(ClearControl, NULL, screen, &dest); // Do the actual Blit

  SDL_UpdateRects(screen, 1, &dest); //Show the screen.
  SDL_FreeSurface(ClearControl); //Free up the surface memory.
  DisplayWidgets();

}

void Control::ClearDepth(){

 // Clear the screen
  SDL_Surface *temp;
  temp = Load_Image("images/ClearControl.png");
  if(temp != NULL) ClearControl = SDL_DisplayFormat(temp);
  if ( ClearControl == NULL ) { 
    cerr<<"Function LoadWidgets()" << endl
	<< SDL_GetError() << endl;
    SDL_Quit();
    exit(0);
  }

  SDL_FreeSurface(temp);
  
  src.x = 0;
  src.y = 0;
  src.w = ClearControl->w;
  src.h = ClearControl->h;

  dest.x = 211; // Blit destination x&y to the upper left
  dest.y = 140;
  dest.w = ClearControl->w; // Height and width equal to the
  dest.h = ClearControl->h;  // source images....
  SDL_BlitSurface(ClearControl, NULL, screen, &dest); // Do the actual Blit

  SDL_UpdateRects(screen, 1, &dest); //Show the screen.
  SDL_FreeSurface(ClearControl); //Free up the surface memory.
}

void Control::ClearOrdDepth(){

 // Clear the screen
  SDL_Surface *temp;
  temp = Load_Image("images/ClearControl.png");
  if(temp != NULL) ClearControl = SDL_DisplayFormat(temp);
  if ( ClearControl == NULL ) { 
    cerr<<"Function LoadWidgets()" << endl
	<< SDL_GetError() << endl;
    SDL_Quit();
    exit(0);
  }

  SDL_FreeSurface(temp);
  
  src.x = 0;
  src.y = 0;
  src.w = ClearControl->w;
  src.h = ClearControl->h;

  dest.x = 211; // Blit destination x&y to the upper left
  dest.y = 195;
  dest.w = ClearControl->w; // Height and width equal to the
  dest.h = ClearControl->h;  // source images....
  SDL_BlitSurface(ClearControl, NULL, screen, &dest); // Do the actual Blit


  SDL_UpdateRects(screen, 1, &dest); //Show the screen.
  SDL_FreeSurface(ClearControl); //Free up the surface memory.
  DisplayWidgets();
}

void Control::ClearOrdSpeed(){
  BEMER = false;
  BSTD = false;
  B23 = false;
  B13 = false;
  ASTOP = false;
  A13 = false;
  A23 = false;
  ASTD = false;
  AFULL = false;
  AFLK = false;

 // Clear the screen
  SDL_Surface *temp;
  temp = Load_Image("images/ClearControl.png");
  if(temp != NULL) ClearControl = SDL_DisplayFormat(temp);
  if ( ClearControl == NULL ) { 
    cerr<<"Function LoadWidgets()" << endl
	<< SDL_GetError() << endl;
    SDL_Quit();
    exit(0);
  }

  SDL_FreeSurface(temp);
  
  src.x = 0;
  src.y = 0;
  src.w = ClearControl->w;
  src.h = ClearControl->h;

  dest.x = 317; // Blit destination x&y to the upper left
  dest.y = 194;
  dest.w = ClearControl->w; // Height and width equal to the
  dest.h = ClearControl->h;  // source images....
  SDL_BlitSurface(ClearControl, NULL, screen, &dest); // Do the actual Blit


  SDL_UpdateRects(screen, 1, &dest); //Show the screen.
  SDL_FreeSurface(ClearControl); //Free up the surface memory.
  DisplayWidgets();
}

void Control::ClearSpeed(){
 // Clear the screen
  SDL_Surface *temp;
  temp = Load_Image("images/ClearControl.png");
  if(temp != NULL) ClearControl = SDL_DisplayFormat(temp);
  if ( ClearControl == NULL ) { 
    cerr<<"Function LoadWidgets()" << endl
	<< SDL_GetError() << endl;
    SDL_Quit();
    exit(0);
  }

  SDL_FreeSurface(temp);
  
  src.x = 0;
  src.y = 0;
  src.w = ClearControl->w;
  src.h = ClearControl->h;

  dest.x = 317; // Blit destination x&y to the upper left
  dest.y = 139;
  dest.w = ClearControl->w; // Height and width equal to the
  dest.h = ClearControl->h;  // source images....
  SDL_BlitSurface(ClearControl, NULL, screen, &dest); // Do the actual Blit

  SDL_UpdateRects(screen, 1, &dest); //Show the screen.
  SDL_FreeSurface(ClearControl); //Free up the surface memory.
}

void Control::ToggleBEMER(){
  ClearOrdSpeed();
  BEMER = !BEMER;
  Subs->DesiredSpeed = -16; 
}
void Control::ToggleBSTD(){
  ClearOrdSpeed();
  BSTD = !BSTD;
  Subs->DesiredSpeed = -12; 
}
void Control::ToggleB23(){
  ClearOrdSpeed();
  B23 = !B23;
  Subs->DesiredSpeed = -8; 
}
void Control::ToggleB13(){
  ClearOrdSpeed();
  B13 = !B13;
 Subs->DesiredSpeed = -4; 
}
void Control::ToggleASTOP(){
  ClearOrdSpeed();
  ASTOP = !ASTOP;
  Subs->DesiredSpeed = 0;
}
void Control::ToggleA13(){
  ClearOrdSpeed();
  A13 = !A13;
  Subs->DesiredSpeed = 5;
}
void Control::ToggleA23(){
  ClearOrdSpeed();
  A23 = !A23;
  Subs->DesiredSpeed = 10;
}
void Control::ToggleASTD(){
  ClearOrdSpeed();
  ASTD = !ASTD;
  Subs->DesiredSpeed = 15;
}
void Control::ToggleAFULL(){
  ClearOrdSpeed();
  AFULL = !AFULL;
  Subs->DesiredSpeed = 20; 
}
void Control::ToggleAFLK(){
  ClearOrdSpeed();
  AFLK = !AFLK;
  Subs->DesiredSpeed = 32; 
}

void Control::Display(){
  static char text[120];
  char file1[] = "images/font.png";
  char file2[] = "data/font.dat";
  DFont fnt(file1, file2);
  float radians, previous_radians;
  float previous_speed, previous_depth;
  double x1, y1, x2, y2;

  
  if (depthup) Subs->DesiredDepth--;   // Take her up!!
  if (depthdown) Subs->DesiredDepth++; // Take her down!!
  
  // Heading Screen
  src.x=110;
  src.y=142; //define a rectangle on the screen and make it black
  src.h=15;
  src.w=75;
  
  sprintf(text, "%i",(int)Subs[0].Heading);
  fnt.PutString(screen, 145, 144, text);

  // Ordered Heading Screen
  src.x=110;
  src.y=196; //define a rectangle on the screen and make it black
  src.h=15;
  src.w=75;

  sprintf(text, "%i",(int)Subs[0].DesiredHeading);
  fnt.PutString(screen, 145, 198, text);

//Replot desired heading compass if we left control screen before turn completed  //rdm 9/11
  if (Subs->DesiredHeading != Subs->Heading ) 
  {
 	// Draw a green filled triangle with vertices (x1, y1), (x2, y2), (x3, y3) and RGBA color (r, g, b, a)
	x = int(251.0 + 58.0*cos(1.57-Subs[0].DesiredHeading*3.14/180.0));
  	y = int(344.0 - 58.0*sin(1.57-Subs[0].DesiredHeading*3.14/180.0));
 
  	x1 = int(251.0 + 4.0*sin(1.57-Subs[0].DesiredHeading*3.14/180.0));
	y1 = 	int(344.0 + 4.0*cos(1.57-Subs[0].DesiredHeading*3.14/180.0));
	x2 = int(251.0 - 4.0*sin(1.57-Subs[0].DesiredHeading*3.14/180.0));
	y2 = int(344.0 - 4.0*cos(1.57-Subs[0].DesiredHeading*3.14/180.0));
	
	filledTrigonRGBA(screen, x1, y1, x2, y2, x, y, 0, 128, 0, 255);   
  }
  
  // Depth Screen
  src.x=212;
  src.y=142; 
  src.h=15;
  src.w=75;
  
  sprintf(text, "%i", (int)Subs[0].Depth);
  fnt.PutString(screen, 247, 144, text);

  // Desired Depth Screen
  src.x=212;
  src.y=197; 
  src.h=15;
  src.w=75;
  
  sprintf(text, "%i", (int)Subs[0].DesiredDepth);
  fnt.PutString(screen, 247, 199, text);

  // Clear the Ordered Depth Boxes if needed.
  if(Subs->DesiredDepth == 100.0) ClearOrdDepth();
  if(Subs->DesiredDepth == 1000.0) ClearOrdDepth();
  
  
  // Speed Screen
  src.x=320;
  src.y=142; 
  src.h=15;
  src.w=75;
  
  sprintf(text, "%d", int(Subs->Speed));
  fnt.PutString(screen, 355, 144, text);
  
  // Desired Speed Screen
  src.x=320;
  src.y=197; 
  src.h=15;
  src.w=75;
  
  sprintf(text, "%i", (int)Subs->DesiredSpeed);
  fnt.PutString(screen, 355, 199, text);
  
  radians = Subs->Heading *(3.14/180.0);  // degrees to radians

  if (!HeadingStack.empty()) // Is there is data on the stack?
    { 
      previous_radians = HeadingStack.pop();
      previous_speed = SpeedStack.pop();
      previous_depth = DepthStack.pop();

      // Next lines clear the display as nec to prevent garbage on screen      
      if(Subs->Heading <= 10.0 && previous_radians*(180.0/3.14) > 10.0) ClearHeading();
      if(Subs->Heading <= 100.0 && previous_radians*(180.0/3.14) > 100.0) ClearHeading();
      if(Subs->Speed <= 10.0 && previous_speed > 10.0) ClearSpeed();
      if(Subs->Speed >= -10.0 && previous_speed < -10.0) ClearSpeed();
      if(Subs->Speed >= 0.0 && previous_speed < 0.0) ClearSpeed();
 
      if(Subs->Depth <= 100.0 && previous_depth > 100.0) ClearDepth();
      if(Subs->Depth <= 1000.0 && previous_depth > 1000.0) ClearDepth();
      

      // Clear Old Data from Heading Compass //rdm 9/11
  		// Draw a orange filled triangle with vertices (x1, y1), (x2, y2), (x3, y3) and RGBA color (r, g, b, a)
    	x = int(251.0 + 58.0*cos(1.57-previous_radians));
		y = int(344.0 - 58.0*sin(1.57-previous_radians));
		
		x1 = int(251.0 + 4.0*sin(1.57-previous_radians));
		y1 = 	int(344.0 + 4.0*cos(1.57-previous_radians));
		x2 = int(251.0 - 4.0*sin(1.57-previous_radians));
		y2 = int(344.0 - 4.0*cos(1.57-previous_radians));
	
		filledTrigonRGBA(screen, x1, y1, x2, y2, x, y, 0, 0, 0, 255); 
		    
    }

  // Draw Desired Heading Compass //rdm 9/11
	// Draw a orange filled triangle with vertices (x1, y1), (x2, y2), (x3, y3) and RGBA color (r, g, b, a)
		x = int(251.0 + 58.0*cos(1.57-radians));
  		y = int(344.0 - 58.0*sin(1.57-radians));
  		
		x1 = int(251.0 + 4.0*sin(1.57-radians));
		y1 = 	int(344.0 + 4.0*cos(1.57-radians));
		x2 = int(251.0 - 4.0*sin(1.57-radians));
		y2 = int(344.0 - 4.0*cos(1.57-radians));
	
		filledTrigonRGBA(screen, x1, y1, x2, y2, x, y, 238, 118, 0, 255); 
		
  // Push the data on the stack
  HeadingStack.push(radians);
  SpeedStack.push(Subs->Speed);
  DepthStack.push(Subs->Depth);

 SDL_UpdateRect(screen,0, 0, 0, 0);

}

void Control::AdjustHeading(int x, int y){
  
  double c1, c3, c5, c6;
  double x1, y1, x2, y2;
  int heading;
  
  c1 = double(251);
  c3 = double(344);
  
 // c5 = 60.0*(double(y)-c3);
 // c6 = 60.0*(double(x)-c1);

	c5 = 58.0*(double(y)-c3);
 	c6 = 58.0*(double(x)-c1);  
  
  
  ClearOrdHeading();
  
  if(x >= 251)
    {
      heading = int(90.0 + (180/3.14)*atan(c5/c6));
    }
  else
    {
      heading = int(270.0 + (180/3.14)*atan(c5/c6));
    }
  
  Subs->DesiredHeading = heading;
  old_heading = heading;
  
   // Draw Desired Heading Compass//rdm 9/11
	// Draw a green filled triangle with vertices (x1, y1), (x2, y2), (x3, y3) and RGBA color (r, g, b, a)
	x = int(251.0 + 58.0*cos(1.57-heading*3.14/180.0));
  	y = int(344.0 - 58.0*sin(1.57-heading*3.14/180.0));
//  	filledTrigonRGBA(screen, 251, 341, 251, 347, x, y, 0, 128, 0, 255);  //rdm 9/11
 
  	x1 = int(251.0 + 4.0*sin(1.57-heading*3.14/180.0));
	y1 = 	int(344.0 + 4.0*cos(1.57-heading*3.14/180.0));
	x2 = int(251.0 - 4.0*sin(1.57-heading*3.14/180.0));
	y2 = int(344.0 - 4.0*cos(1.57-heading*3.14/180.0));
	
	filledTrigonRGBA(screen, x1, y1, x2, y2, x, y, 0, 128, 0, 255); 
  return;

}

void Control::AdjustDepth(int x){ 

  ClearOrdDepth();
  if(x > 112 && x < 126)
    {
      if(depthup)
	{
	  depthup = false;
	}
      else
	{
	  depthdown = true;
	  depthup = false;
	}
    }

  if(x > 385 && x < 401)
    {
      if(depthdown)
	{
	  depthdown = false; 
	}
      else
	{
	  depthdown = false;
	  depthup = true;
	}
    }  
  DisplayWidgets();
  return;
}

//Not needed if use SDL_gfx rdm 9/11

/*
void Control::DLine(SDL_Surface *screen, int X1, int Y1, int X2, int Y2, Uint32 Color)
{

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
      DPixel(screen, px, py, Color);
      y = y + dy;
      if (y >= dx){
	y = y - dx;
	py = py + sdy;
      }
      px = px + sdx;
    }
  }else{
    for (int y = 0; y < dy; y++){
      DPixel(screen, px, py, Color);
      x = x + dx;
      if (x >= dy){
	x = x - dy;
	px = px + sdx;
      }
      py = py + sdy;
    }
  }
}

void Control::DPixel(SDL_Surface *screen, int x, int y, Uint32 color)
{
  //this only works for 16bpp screens
  //are we outside the screen?????
  //If we are bail out now before it's too late!
  
  //cerr << ".";
  if (x > 1023 || x < 0 || y > 759 || y < 0) {
    return;
  }
  
  //place the pixel on the screen
  Uint16 *pixel_location;
  pixel_location = (Uint16 *)screen->pixels + y*screen->pitch/2 + x;
  *pixel_location = color;
}
*/
