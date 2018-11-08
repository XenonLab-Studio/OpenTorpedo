 /***************************************************************************
                          esm.cpp  -  description
                             -------------------
    begin                : Wed August 15 2001
    copyright            : (C) 2001 by Rick McDaniel
    email                : rickeym@swbell.net
$Id: esm.cpp,v 1.5 2003/04/14 05:51:03 mbridak Exp $ 
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
#include "dfont.h"
#include "files.h"
#include "submarine.h"

#include "esm.h"
#include "dstack.h"
#include <cmath>
#include <cstdlib>

// rdm windows
#include <iostream>
using namespace std;

DStack EsmStack;


Esm::Esm(Submarine *temp):Subs(temp)
{
  // Default values: mast down, power off
  MastHeight = 68;  // Max height of esm mast
  Mast = false;
}
Esm::~Esm(){
}

void Esm::InitGraphics(SDL_Surface *temp, SDL_Surface *tempesmscreen)
{
	screen=temp;
	esmscreen=tempesmscreen;
	
	temp = SDL_CreateRGBSurface(SDL_SWSURFACE, 420, 490, 16,
			screen->format->Rmask,
			screen->format->Gmask,
			screen->format->Bmask,
			screen->format->Amask);
	tempscreen=SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);

	LoadWidgets();
	DisplayWidgets();
	orange = SDL_MapRGB(screen->format, 238, 118, 0);
	black = SDL_MapRGB(screen->format, 0, 0, 0);
}

void Esm::LoadWidgets(){

	SDL_Surface *temp;

	temp = Load_Image("images/mastdownoff.png");
	if(temp != NULL) mastdownoff = SDL_DisplayFormat(temp);
	if ( mastdownoff == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);


	temp = Load_Image("images/mastdownon.png");
	if(temp != NULL) mastdownon = SDL_DisplayFormat(temp);
	if ( mastdownon == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/mastupoff.png");
	if(temp != NULL) mastupoff = SDL_DisplayFormat(temp);
	if ( mastupoff== NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);


	temp = Load_Image("images/mastupon.png");
	if(temp != NULL) mastupon = SDL_DisplayFormat(temp);
	if ( mastupon == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/styllus.png");
	if(temp != NULL) styllus = SDL_DisplayFormat(temp);
	if ( styllus == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

}

void Esm::DisplayWidgets(){

// Center of ESM screen at (316,325)
//	dest.x = 316;
//	dest.y = 325;
//	dest.h = styllus->h; 
//	dest.w = styllus->w;
//	SDL_BlitSurface(styllus, NULL, screen, &dest); 
//	SDL_UpdateRects(screen, 1, &dest); 

	if(Mast)
	{
		dest.x = 845;
		dest.y = 187;
		dest.h = mastupon->h; 
		dest.w = mastupon->w;
		SDL_BlitSurface(mastupon, NULL, screen, &dest); 
		SDL_UpdateRects(screen, 1, &dest); 

		dest.x = 740;
		dest.y = 187;
		dest.h = mastdownoff->h; 
		dest.w = mastdownoff->w;
		SDL_BlitSurface(mastdownoff, NULL, screen, &dest); 
		SDL_UpdateRects(screen, 1, &dest); 
	}
	else
	{
		dest.x = 845;
		dest.y = 187;
		dest.h = mastupoff->h; 
		dest.w = mastupoff->w;
		SDL_BlitSurface(mastupoff, NULL, screen, &dest); 
		SDL_UpdateRects(screen, 1, &dest); 

		dest.x = 740;
		dest.y = 187;
		dest.h = mastdownon->h; 
		dest.w = mastdownon->w;
		SDL_BlitSurface(mastdownon, NULL, screen, &dest); 
		SDL_UpdateRects(screen, 1, &dest); 
	}
	
	// Need this for the time compression??
	//	SDL_UpdateRect(screen,0, 0, 0, 0);

}

void Esm::ClearScreen(){
		// Clear the screen
	SDL_Surface *temp;
	temp = Load_Image("images/ClearEsm.png");
	if(temp != NULL) ClearEsm = SDL_DisplayFormat(temp);
	if ( ClearEsm == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	
	SDL_FreeSurface(temp);

	temp = Load_Image("images/ClearEsm2.png");
	if(temp != NULL) ClearEsm2 = SDL_DisplayFormat(temp);
	if ( ClearEsm2 == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	
	SDL_FreeSurface(temp);

	src.x = 0;
	src.y = 0;
	src.w = ClearEsm->w;
	src.h = ClearEsm->h;

	dest.x = 105; // Blit destination x&y to the upper left
	dest.y = 169;
	dest.w = ClearEsm->w; // Height and width equal to the
	dest.h = ClearEsm->h;  // source images....
	SDL_BlitSurface(ClearEsm, NULL, screen, &dest); // Do the actual Blit
	SDL_UpdateRects(screen, 1, &dest); //Show the screen...
	SDL_FreeSurface(ClearEsm); //Free up the surface memory.

	src.x = 0;
	src.y = 0;
	src.w = ClearEsm2->w;
	src.h = ClearEsm2->h;

	dest.x = 693; // Blit destination x&y to the upper left
	dest.y = 308;
	dest.w = ClearEsm2->w; // Height and width equal to the
	dest.h = ClearEsm2->h;  // source images....
	SDL_BlitSurface(ClearEsm2, NULL, screen, &dest); // Do the actual Blit
	SDL_UpdateRects(screen, 1, &dest); //Show the screen...
	SDL_FreeSurface(ClearEsm2); //Free up the surface memory.

}

void Esm::LowerMast()
{
   Mast = false;
}

void Esm::RaiseMast()
{
  Mast = true;
}

void Esm::ToggleMast()
{
  Mast = !Mast;
}

bool Esm::getMastStatus()const
{
  return Mast;
}

int Esm::getAntennaHeight(int Depth,int SeaState) const
{
  // Effective Height of esm mast above water.
  switch (SeaState)
    {
    case 0: // case 0 and case 1 are same
    case 1: return(MastHeight - Depth);
    case 2: return(MastHeight - Depth - 1);
    case 3: return(MastHeight - Depth - 3);
    case 4: return(MastHeight - Depth - 5);
    case 5: return(MastHeight - Depth - 8);
    default: return(MastHeight - Depth - 67);
    }
}
bool Esm::isTargetVisible(Submarine *target, int TargetRange, int ObserverDepth,
			  int TargetHeight, bool TargetRadar, int SeaState)
{
  // Determines if observer's esm can detect target radar.
  // Returns a 1 if true or a 0 if false.

  // TargetRange is the true range to the target.

  // TargetHeight is the height of the tallest mast on a surface ship
  // (usually 50 ft - 175 ft), height of conning tower for a submarine
  // (usually 40-50 ft), or altitude for airplane.

  /* To see Target, Range < EsmHorizon, Targets radar must be on.

     Esm horizon is the earliest posssible target detection due to
     the Earth-Curvature. Esm horizon = 3600(sqrt(ha)+ sqrt(ht))
     ha = antenna height(ft), ht = target height(ft), EsmHorizon(yds)

     Note: ESM horizon > Radar Horizon since can detect targets radar
     long before he can detect esm mast.
  */

  bool boolean = false;
  // we cannot detect subs on esm
  if (target->Depth > 0.0)
       return false;

  CurrentAntennaHeight = getAntennaHeight(ObserverDepth,SeaState);
  if(CurrentAntennaHeight <= 0) return boolean;  // Mast is below water!

  if(TargetRadar == 1 && getMastStatus() == 1 
	  && TargetRange <= getEsmHorizon(ObserverDepth, TargetHeight, SeaState)) 
	  boolean = true;
  return boolean;
}
int Esm::getBearing(int TargetBearing, int ObserverDepth,
		    int TargetHeight, int SeaState)
{
  // Returns the bearing to the target based on accuracy of
  // esm mast.

  // The greater the EsmHorizon, the greater the error.

  EsmHorizon = getEsmHorizon(ObserverDepth, TargetHeight, SeaState);

  if (EsmHorizon > 30000)
    bearing = TargetBearing - 5 + rand()%10;
  else if (EsmHorizon > 20000 && EsmHorizon < 30000)
    bearing = TargetBearing - 3 + rand()%6;
  else if (EsmHorizon > 15000 && EsmHorizon < 20000)
    bearing = TargetBearing - 2 + rand()%4;
  else
    bearing = TargetBearing - 1 + rand()%2;

  return bearing;
}
int Esm::getSignalStrength(Submarine *Target, int TargetRange, int ObserverDepth,
			   int TargetHeight, bool TargetRadar,int SeaState)
{
  // Returns the signal strength of the target radar.
  // The signal strength is determined by the Detection Probability.

  // The detection probability is a function of Target Range, Antenna
  // Height, and TargetHeight


  if(isTargetVisible(Target, TargetRange, ObserverDepth, TargetHeight,
 	     TargetRadar, SeaState) == 0)
    {
      SignalStrength = 0;
      return SignalStrength;
    }
  else
    {
      DetectProb = TargetRange - 600*getAntennaHeight(ObserverDepth,SeaState)
	                       - 20*TargetHeight;
      if (DetectProb > 35000)
	SignalStrength =  1;
      else if (DetectProb > 25000 && DetectProb < 35000)
	SignalStrength = 2;
      else if (DetectProb > 18000 && DetectProb < 25000)
	SignalStrength = 3;
      else if (DetectProb > 14000 && DetectProb < 18000)
	SignalStrength = 4;
      else
	SignalStrength = 5;
    }
  return SignalStrength;
}

int Esm::getEsmHorizon(int ObserverDepth, int TargetHeight, int SeaState)
{
  CurrentAntennaHeight = getAntennaHeight(ObserverDepth,SeaState);
  if(CurrentAntennaHeight <= 0) return 0;  // Mast is below water!

  EsmHorizon = 3600*(int)(sqrt(float(CurrentAntennaHeight)) +
			  sqrt(float(TargetHeight)));

  return EsmHorizon;
}

void Esm::DisplayContacts(){	
	static char text[120];
	char file1[] = "images/font.png";
	char file2[] = "images/font.dat";
	DFont fnt(file1, file2);
	//	static DFont largeFont(filename.c_str(), filename2.c_str());
	float radians;
	float radians_old;
	int bearing, range, depth;
        Submarine *target;
        int count;

	// Note: Center of radar screen at (x,y) = (323,330)
	// set dx = 144	dy = 144

	src.x=695;
	src.y=417; //define a rectangle on the screen and make it black
	src.h=50;
	src.w=174;

        /*
	if (!EsmStack.empty()) // Is there is data on the stack?
	  { 
            // TODO: FIX THIS
	    for(count=1; count<8;count++)
	      {
		radians_old[count] = EsmStack.pop();
	    
		// Clear Old Data from Heading Compass
		x = int(326.0 + (460.0 - 326.0)*cos(1.57-radians_old[count]));
		y = int(383.0 - (460.0 - 326.0)*sin(1.57-radians_old[count]));
		DLine(screen, 326, 383, x, y, black);
	      }
	  }
        */
        while (! EsmStack.empty() )
        {
           radians_old = EsmStack.pop();
           x = int(326.0 + (460.0 - 326.0)*cos(1.57-radians_old));
           y = int(383.0 - (460.0 - 326.0)*sin(1.57-radians_old));
           DLine(screen, 326, 383, x, y, black); 
        }

        target = Subs->next;
        count = 0;
	while (target)
        {
		bearing = (int)Subs->BearingToTarget(target);
		range = (int)Subs->DistanceToTarget(target);
		depth = (int)Subs->Depth;

//		bearing = getBearing((int)Subs[0].BearingToTarget(Subs[target]), depth, 100, 3);
		
		radians = float(bearing) *(3.14/180.0);  // degrees to radians

		if(isTargetVisible(target, range, depth,100, true, 3)) 
		  {
                    count++;
		    // Plot a line at the correct bearing 	
		    x = int(326.0 + (460.0 - 326.0)*cos(1.57-radians));
		    y = int(383.0 - (460.0 - 326.0)*sin(1.57-radians));
		    DLine(screen, 326, 383, x, y, orange);
		    
		    sprintf(text, "BEARING  %i   SS %i", bearing,
			    getSignalStrength(target, range,60,100,true,3));
		    fnt.PutString(screen, 698, 300 + 11*count, text);
		    // Push the data on the stack
		    EsmStack.push(radians);		    

		  }

            target = target->next;
	}
	SDL_UpdateRect(screen,0, 0, 0, 0);
}

void Esm::DLine(SDL_Surface *screen, int X1, int Y1, int X2, int Y2, Uint32 Color)
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

void Esm::DPixel(SDL_Surface *screen, int x, int y, Uint32 color)
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
