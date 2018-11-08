/***************************************************************************
                          radar.cpp  -  description
                             -------------------
    begin                : Fri August 2 2001
    copyright            : (C) 2001 by Rick McDaniel
    email                : rickeym@swbell.net
$Id: radar.cpp,v 1.16 2003/05/17 22:25:34 mbridak Exp $
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
#include "math.h"
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_thread.h"
#include "dfont.h"
#include "files.h"
#include "submarine.h"

//VENZON:  need rotozoom now
#include "SDL_rotozoom.h"

// rdm windows
#include <iostream>
using namespace std;

#include "radar.h"

//VENZON:  I've replaced all occurrances of 10*tick with tick, since I upped
//the resolution by a factor of 10

Radar::Radar(Submarine *temp): Subs(temp)
{
  // Default values: Mast down, RangeRing off, RangeScale = 10

  MastHeight = 59;        // Max height of radar mast
  Mast = false;
  RangeRing = 0;
  RangeScale = 10;
  rangescale10 = true;
  rangescale20 = false;
  rangescale30 = false;
  rangescale40 = false;
  rangescale50 = false;
  rangescale60 = false;

  rangering0 = true;
  rangering10 = false;
  rangering15 = false;
  rangering20 = false;
  rangering25 = false;

  // radar sweep counter
  tick = 0;
  ALPHA = 255;

}
Radar::~Radar(){
	SDL_FreeSurface(blip);
	
	//gotta free it, or we get a segfault
	SDL_FreeSurface(sweep[0]);
}

void Radar::InitGraphics(SDL_Surface *temp, SDL_Surface *tempradarscreen)
{
	screen=temp;
	radarscreen=tempradarscreen;
	
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
	
	//load the picture of a blip (blip.png)
	temp = Load_Image("images/blip.png");
	if(temp != NULL) {
		blip = SDL_DisplayFormat(temp);
		SDL_SetAlpha(blip, SDL_SRCALPHA, 128);
	}
	if ( blip == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
  	SDL_FreeSurface(temp);
}

void Radar::LoadWidgets(){

	SDL_Surface *temp;

	// Load Range Scale Widgets
	temp = Load_Image("images/range10off.png");
	if(temp != NULL) range10off = SDL_DisplayFormat(temp);
	if ( range10off == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/range10on.png");
	if(temp != NULL) range10on = SDL_DisplayFormat(temp);
	if ( range10on == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/range20off.png");
	if(temp != NULL) range20off = SDL_DisplayFormat(temp);
	if ( range20off == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/range20on.png");
	if(temp != NULL) range20on = SDL_DisplayFormat(temp);
	if ( range20on == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/range30off.png");
	if(temp != NULL) range30off = SDL_DisplayFormat(temp);
	if ( range30off == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/range30on.png");
	if(temp != NULL) range30on = SDL_DisplayFormat(temp);
	if ( range30on == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/range40off.png");
	if(temp != NULL) range40off = SDL_DisplayFormat(temp);
	if ( range40off == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/range40on.png");
	if(temp != NULL) range40on = SDL_DisplayFormat(temp);
	if ( range40on == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);


	temp = Load_Image("images/range50off.png");
	if(temp != NULL) range50off = SDL_DisplayFormat(temp);
	if ( range50off == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/range50on.png");
	if(temp != NULL) range50on = SDL_DisplayFormat(temp);
	if ( range50on == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/range60off.png");
	if(temp != NULL) range60off = SDL_DisplayFormat(temp);
	if ( range60off == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/range60on.png");
	if(temp != NULL) range60on = SDL_DisplayFormat(temp);
	if ( range60on == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	// Load Range Ring Widgets 

	temp = Load_Image("images/ring0off.png");
	if(temp != NULL) ring0off = SDL_DisplayFormat(temp);
	if ( ring0off == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/ring0on.png");
	if(temp != NULL) ring0on = SDL_DisplayFormat(temp);
	if ( ring0on == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/ring5off.png");
	if(temp != NULL) ring5off = SDL_DisplayFormat(temp);
	if ( ring5off == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/ring5on.png");
	if(temp != NULL) ring5on = SDL_DisplayFormat(temp);
	if ( ring5on == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}

	SDL_FreeSurface(temp);

	temp = Load_Image("images/ring10off.png");
	if(temp != NULL) ring10off = SDL_DisplayFormat(temp);
	if ( ring10off == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/ring10on.png");
	if(temp != NULL) ring10on = SDL_DisplayFormat(temp);
	if ( ring10on == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/ring15off.png");
	if(temp != NULL) ring15off = SDL_DisplayFormat(temp);
	if ( ring15off == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/ring15on.png");
	if(temp != NULL) ring15on = SDL_DisplayFormat(temp);
	if ( ring15on == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);
	
	temp = Load_Image("images/ring20off.png");
	if(temp != NULL) ring20off = SDL_DisplayFormat(temp);
	if ( ring20off == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/ring20on.png");
	if(temp != NULL) ring20on = SDL_DisplayFormat(temp);
	if ( ring20on == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/ring25off.png");
	if(temp != NULL) ring25off = SDL_DisplayFormat(temp);
	if ( ring25off == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/ring25on.png");
	if(temp != NULL) ring25on = SDL_DisplayFormat(temp);
	if ( ring25on == NULL ) { 
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	// Load Mast Widgets
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

	// VENZON:  only need 1 sweep image now
	sweep[0] = Load_Image("images/sweep0.png");
    
}

void Radar::DisplayWidgets(){
  // Range Scale Widgets
  if(rangescale10)
    {
      dest.x = 748;
      dest.y = 320;
      dest.h = range10on->h; 
      dest.w = range10on->w;
      SDL_BlitSurface(range10on, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  else
    {
      dest.x = 748;
      dest.y = 320;
      dest.h = range10off->h; 
      dest.w = range10off->w;
      SDL_BlitSurface(range10off, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  
  if(rangescale20) 
    {
      dest.x = 795;
      dest.y = 320;
      dest.h = range20on->h; 
      dest.w = range20on->w;
      SDL_BlitSurface(range20on, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  else
    {
      dest.x = 795;
      dest.y = 320;
      dest.h = range20off->h; 
      dest.w = range20off->w;
      SDL_BlitSurface(range20off, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  
  if(rangescale30)
    {
      dest.x = 842;
      dest.y = 320;
      dest.h = range30on->h; 
      dest.w = range30on->w;
      SDL_BlitSurface(range30on, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  else
    {
      dest.x = 842;
      dest.y = 320;
      dest.h = range30off->h; 
      dest.w = range30off->w;
      SDL_BlitSurface(range30off, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  
  if(rangescale40)
    {
      dest.x = 748;
      dest.y = 367;
      dest.h = range40on->h; 
      dest.w = range40on->w;
      SDL_BlitSurface(range40on, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest);
    }
  else
    {
      dest.x = 748;
      dest.y = 367;
      dest.h = range40off->h; 
      dest.w = range40off->w;
      SDL_BlitSurface(range40off, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  
  if(rangescale50)
    {
      dest.x = 795;
      dest.y = 367;
      dest.h = range50on->h; 
      dest.w = range50on->w;
      SDL_BlitSurface(range50on, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  else
    {
      dest.x = 795;
      dest.y = 367;
      dest.h = range50off->h; 
      dest.w = range50off->w;
      SDL_BlitSurface(range50off, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  
  if(rangescale60)
    {
      dest.x = 842;
      dest.y = 367;
      
      dest.h = range60on->h; 
      dest.w = range60on->w;
      SDL_BlitSurface(range60on, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  else
    {
      dest.x = 842;
      dest.y = 367;
      dest.h = range60off->h; 
      dest.w = range60off->w;
      SDL_BlitSurface(range60off, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  
  // Mast Widgets
  if(Mast)
    {
      dest.x = 850;
      dest.y = 185;
      dest.h = mastupon->h; 
      dest.w = mastupon->w;
      SDL_BlitSurface(mastupon, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
      
      dest.x = 744;
      dest.y = 185;
      dest.h = mastdownoff->h; 
      dest.w = mastdownoff->w;
      SDL_BlitSurface(mastdownoff, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  else
    {
      dest.x = 850;
      dest.y = 185;
      dest.h = mastupoff->h; 
      dest.w = mastupoff->w;
      SDL_BlitSurface(mastupoff, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
      
      dest.x = 744;
      dest.y = 185;
      dest.h = mastdownon->h; 
      dest.w = mastdownon->w;
      SDL_BlitSurface(mastdownon, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  
  // Range Ring Widgets
  if(rangering0)
    {
      dest.x = 749;
      dest.y = 501;
      dest.h = ring0on->h; 
      dest.w = ring0on->w;
      SDL_BlitSurface(ring0on, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  else
    {
      dest.x = 749;
      dest.y = 501;
      dest.h = ring0off->h; 
      dest.w = ring0off->w;
      SDL_BlitSurface(ring0off, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    } 

  if(rangering5)
    {
      dest.x = 796;
      dest.y = 501;
      dest.h = ring5on->h; 
      dest.w = ring5on->w;
      SDL_BlitSurface(ring5on, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  else
    {
      dest.x = 796;
      dest.y = 501;
      dest.h = ring5off->h; 
      dest.w = ring5off->w;
      SDL_BlitSurface(ring5off, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    } 

  if(rangering10)
    {
      dest.x = 843;
      dest.y = 501;
      dest.h = ring10on->h; 
      dest.w = ring10on->w;
      SDL_BlitSurface(ring10on, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  else
    {
      dest.x = 843;
      dest.y = 501;
      dest.h = ring10off->h; 
      dest.w = ring10off->w;
      SDL_BlitSurface(ring10off, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }

  if(rangering15)
    {
      dest.x = 749;
      dest.y = 548;
      dest.h = ring15on->h; 
      dest.w = ring15on->w;
      SDL_BlitSurface(ring15on, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  else
    {
      dest.x = 749;
      dest.y = 548;
      dest.h = ring15off->h; 
      dest.w = ring15off->w;
      SDL_BlitSurface(ring15off, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    } 

  if(rangering20)
    {
      dest.x = 796;
      dest.y = 548;
      dest.h = ring20on->h; 
      dest.w = ring20on->w;
      SDL_BlitSurface(ring20on, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  else
    {
      dest.x = 796;
      dest.y = 548;
      dest.h = ring20off->h; 
      dest.w = ring20off->w;
      SDL_BlitSurface(ring20off, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }

  if(rangering25)
    {
      dest.x = 843;
      dest.y = 548;
      dest.h = ring25on->h; 
      dest.w = ring25on->w;
      SDL_BlitSurface(ring25on, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    }
  else
    {
      dest.x = 843;
      dest.y = 548;
      dest.h = ring25off->h; 
      dest.w = ring25off->w;
      SDL_BlitSurface(ring25off, NULL, screen, &dest); 
      SDL_UpdateRects(screen, 1, &dest); 
    } 


  // Need this for the time compression??
  SDL_UpdateRect(screen,0, 0, 0, 0);
  
}

void Radar::ClearScreen(){
  // Clear the screen
  SDL_Surface *temp;
  temp = Load_Image("images/ClearRadar.png");
  if(temp != NULL) ClearRadar = SDL_DisplayFormat(temp);
  if ( ClearRadar == NULL ) { 
    cerr<<"Function LoadWidgets()" << endl
	<< SDL_GetError() << endl;
    SDL_Quit();
    exit(0);
  }
  
  src.x = 0;
  src.y = 0;
  src.w = ClearRadar->w;
  src.h = ClearRadar->h;
  
  dest.x = 98; // Blit destination x&y to the upper left
  dest.y = 154;
  dest.w = ClearRadar->w; // Height and width equal to the
  dest.h = ClearRadar->h;  // source images....
  SDL_BlitSurface(ClearRadar, NULL, screen, &dest); // Do the actual Blit

  SDL_FreeSurface(temp);

  SDL_UpdateRects(screen, 1, &dest); //Show the screen.
  SDL_FreeSurface(ClearRadar); //Free up the surface memory.
  DisplayWidgets();
  
}

void Radar::ClearTextBox(){
  // Clear the screen
  SDL_Surface *temp;
  temp = Load_Image("images/ClearRadar2.png");
  if(temp != NULL) ClearRadar = SDL_DisplayFormat(temp);
  if ( ClearRadar == NULL ) { 
    cerr<<"Function LoadWidgets()" << endl
	<< SDL_GetError() << endl;
    SDL_Quit();
    exit(0);
  }
  
  src.x = 0;
  src.y = 0;
  src.w = ClearRadar->w;
  src.h = ClearRadar->h;

  dest.x = 139; // Blit destination x&y to the upper left
  dest.y = 572;
  dest.w = ClearRadar->w; // Height and width equal to the
  dest.h = ClearRadar->h;  // source images....
  SDL_BlitSurface(ClearRadar, NULL, screen, &dest); // Do the actual Blit

  SDL_FreeSurface(temp);

  SDL_UpdateRects(screen, 1, &dest); //Show the screen.
  SDL_FreeSurface(ClearRadar); //Free up the surface memory.
  DisplayWidgets();
  
}

void Radar::Sweep(float gametime)
{
	if(getMastStatus())  // If the mast is up start sweep
    {
      tick+=gametime/25.0f;
      
      if(tick > 360) tick -= 360;
	}
}

void Radar::DisplaySweep()
{
  if(getMastStatus())  // If the mast is up start sweep
    {
      
      src.x = 0;
      src.y = 0;
      src.w = sweep[0]->w;
      src.h = sweep[0]->h;
      
      dest.x = 193;
      dest.y = 245;
      dest.w = sweep[0]->w;
      dest.h = sweep[0]->h;
      
      //VENZON:  some rotozooming!
      SDL_SetAlpha(sweep[0], SDL_SRCALPHA, 128);
      SDL_FillRect(screen, &dest, black);
      SDL_Surface *sweeprot;
      
      //set the last argument of this function to 0 for a bit faster
      //(but much crappier looking) rotozooming
      sweeprot = rotozoomSurface(sweep[0], -tick, 1.0, 1);
      
      //the rotozoomer seems to create a surface with whatever width and
      //height necessary, so we have to recompute the destination coords
      src.w = sweeprot->w;
      src.h = sweeprot->h;
      dest.x = sweep[0]->w/2 + dest.x - sweeprot->w/2;
      dest.y = sweep[0]->h/2 + dest.y - sweeprot->h/2;
      dest.w = src.w;
      dest.h = src.h;
      
      //I'm not sure what this alpha stuff is for, but hopefully
      //the rotozoomer won't mess it up
      SDL_SetAlpha(sweeprot, SDL_SRCALPHA, 128);
      SDL_BlitSurface(sweeprot, NULL, screen, &dest); // Do the actual Blit
      SDL_FreeSurface(sweeprot);
    }
}

void Radar::DisplayRings()
{
  int number;
  
  if (RangeRing == 0)
    {
      number = 0;
    }
  else
    {
      number = int(float(RangeScale)/float(RangeRing));
    }

  for(int i = 0; i < number; i++)
    {
      DrawCircle(screen, 320, 371, int((144.0/getRangeScale())*RangeRing*(i+1)), orange);
    }
}

void Radar::DisplayContacts()
{
  float radians;
  int bearing, range, depth;
  Submarine *target;

  DisplaySweep();  // Start the radar sweep

  DisplayRings(); // Draw Range Rings

  // Note: Center of radar screen @ (x,y) = (316,374)
  // set dx = 144	dy = 144
 
  target = Subs->next; 
  while (target)
  {
    bearing = (int)Subs->BearingToTarget(target);
    range = (int)Subs->DistanceToTarget(target);
    depth = (int)Subs->Depth;
    
    radians = float(bearing) *(3.14/180.0);  // degrees to radians
    
    if(isTargetVisible(target, range, depth, 100, 3)) 
    {
      // Find where we plot the dot
      x = 316 + int((460-316)*(range/float(1000*RangeScale))*cos(1.57-radians)); 
      y = 374 - int((374-230)* (range/float(1000*RangeScale))*sin(1.57-radians)); 
      
      src.x = 0;
      src.y = 0;
      src.w = blip->w;
      src.h = blip->h;
      
      dest.x = x;
      dest.y = y;
      dest.w = blip->w;
      dest.h = blip->h;

      // Here's where we set the alpha level 

      if(bearing <= 180)
	{
	  if(tick >= bearing && tick <= ReciprocalBearing(bearing))
	    {
	      ALPHA = int(-(255.0/180.0)* float(DeltaBearing((int) tick, bearing))+255.0);
	    }
	  else
	    {
	      
	      ALPHA = 0;
	    }	  
	}
      
      if(bearing >= 180)
	{
	  if(tick <= bearing && tick >= ReciprocalBearing(bearing))
	    {
	      ALPHA = 0;
	    }
	  else
	    {
	      
	      ALPHA = int(-(255.0/180.0)* float(DeltaBearing((int) tick, bearing))+255.0);
	    }
	}
      
      SDL_SetAlpha(blip, SDL_SRCALPHA, ALPHA);
      SDL_FillRect(screen, &dest, black);

      SDL_BlitSurface(blip, NULL, screen, &dest); // Do the actual Blit

    }   // if visible
    target = target->next;
  }   // end of while
  SDL_UpdateRect(screen,0, 0, 0, 0);
}

void Radar::ClearRangeScale()
{
  rangescale10 = false;
  rangescale20 = false;
  rangescale30 = false;
  rangescale40 = false;
  rangescale50 = false;
  rangescale60 = false;
  ClearScreen();
}

void Radar::ClearRangeRing()
{
  rangering0 = false;
  rangering5 = false;
  rangering10 = false;
  rangering15 = false;
  rangering20 = false;
  rangering25 = false;
  ClearScreen();
}

// Toggle Range Scale
void Radar::ToggleRangeScale10(){
  ClearRangeScale();
  rangescale10 = !rangescale10;
  setRangeScale(10);
}
void Radar::ToggleRangeScale20(){
  ClearRangeScale();
  rangescale20 = !rangescale20;
  setRangeScale(20);
}
void Radar::ToggleRangeScale30(){
  ClearRangeScale();
  rangescale30 = !rangescale30;
  setRangeScale(30);
}
void Radar::ToggleRangeScale40(){
  ClearRangeScale();
  rangescale40 = !rangescale40;
  setRangeScale(40);
}
void Radar::ToggleRangeScale50(){
  ClearRangeScale();
  rangescale50 = !rangescale50;
  setRangeScale(50);
}
void Radar::ToggleRangeScale60(){
  ClearRangeScale();
  rangescale60 = !rangescale60;
  setRangeScale(60);
}

// Toggle Range Rings
void Radar::ToggleRangeRing0(){
  ClearRangeRing();
  rangering0 = !rangering0;
  setRangeRing(0);
}

void Radar::ToggleRangeRing5(){
  ClearRangeRing();
  rangering5 = !rangering5;
  setRangeRing(5);
}

void Radar::ToggleRangeRing10(){
  ClearRangeRing();
  rangering10 = !rangering10;
  setRangeRing(10);
}

void Radar::ToggleRangeRing15(){
  ClearRangeRing();
  rangering15 = !rangering15;
  setRangeRing(15);
}

void Radar::ToggleRangeRing20(){
  ClearRangeRing();
  rangering20 = !rangering20;
  setRangeRing(20);
}

void Radar::ToggleRangeRing25(){
  ClearRangeRing();
  rangering25 = !rangering25;
  setRangeRing(25);
}

void Radar::RaiseMast()
{
   Mast = true;
}

void Radar::LowerMast()
{
   Mast = false;
}


void Radar::ToggleMast()
{
  // ClearScreen();
  Mast = !Mast;
}
void Radar::setRangeScale(int RS)
{
  // Set Radar RangeScale
  RangeScale = (RS == 10 || RS == 20 || RS == 30 ||
		RS == 40 || RS == 50 || RS == 60) ? RS : 10;
}
void Radar::setRangeRing(int RR)
{
  // Set Radar RangeRing
  RangeRing = (RR == 0 || RR == 5 || RR == 10 ||
	       RR == 15 || RR == 20 || RR == 25) ? RR : 0;
}
int Radar::getRangeScale() const
{
  return RangeScale;
}
int Radar::getRangeRing() const
{
  return RangeRing;
}
bool Radar::getMastStatus()const
{
  return Mast;
}
int Radar::getAntennaHeight(int Depth,int SeaState) const
{
  // Effective Height of radar mast above water.
  // Note at SeaState > 5, must be on the surface to use radar
  // Also at SeaState > 5, effective length on mast = 1 ft.
  switch (SeaState)
    {
    case 0:
    case 1: return(MastHeight - Depth);
    case 2: return(MastHeight - Depth - 1);
    case 3: return(MastHeight - Depth - 3);
    case 4: return(MastHeight - Depth - 5);
    case 5: return(MastHeight - Depth - 8);
    default: return(MastHeight - Depth - 58);
    }
}

bool Radar::isTargetVisible(Submarine *target, int TargetRange, 
                            int ObserverDepth,
			    int TargetHeight, int SeaState)
{
  // Determines if observer's radar can detect target.
  // Returns a 1 if true or a 0 if false.
  
  // TargetRange is the true range to the target.
  
  // TargetHeight is the height of the tallest mast on a surface ship
  // (usually 50 ft - 175 ft), height of conning tower for a submarine
  // (usually 40-50 ft), or altitude for airplane.

  /* To see Target, Mast must be up, Range < RadarHorizon, Range < RangeScale.

     Radar horizon is the earliest posssible target detection due to
     the Earth-Curvature. Radar horizon = 2400(sqrt(ha)+ sqrt(ht))
     ha = antenna height(ft), ht = target height(ft), RadarHorizon(yds)

     When sea state (SS) <= 1, Depth should be < 58 ft or mast will be below
     water.  At SS = 2, depth <= 57 ft.  At SS = 3, depth <= 55 ft.
     At SS = 4, depth <= 53 ft.   At SS = 5, depth <= 50 ft.
  */

  bool boolean = false;

  // if target is under water we cannot see them
  if (target->Depth > 0.0)
     return false;

  CurrentAntennaHeight = getAntennaHeight(ObserverDepth,SeaState);
  if(CurrentAntennaHeight <= 0) return boolean;  // Mast is below water!
  
  RadarHorizon = 2400*(int)(sqrt(float(CurrentAntennaHeight)) + sqrt(float(TargetHeight)));
  
  if(target->Depth <= 0 && int(TargetRange) <= 1000*getRangeScale() && getMastStatus() == 1 &&
     TargetRange <= RadarHorizon) boolean = true;
  return boolean;
  
}

float Radar::getRadarHorizon(int ObserverDepth, int TargetHeight, int SeaState)
{
  /*     Radar horizon is the earliest posssible target detection due to
	 the Earth-Curvature. Radar horizon = 2400(sqrt(ha)+ sqrt(ht))
	 ha = antenna height(ft), ht = target height(ft), RadarHorizon(yds)
	 
	 When sea state (SS) <= 1, Depth should be < 58 ft or mast will be below
	 water.  At SS = 2, depth <= 57 ft.  At SS = 3, depth <= 55 ft.
	 At SS = 4, depth <= 53 ft.   At SS = 5, depth <= 50 ft.
  */
  
  CurrentAntennaHeight = getAntennaHeight(ObserverDepth,SeaState);
  if(CurrentAntennaHeight <= 0) return 0;  // Mast is below water!
  
  return(2400.0*(sqrt(float(CurrentAntennaHeight)) + sqrt(float(TargetHeight))));
  
}

int Radar::getBearing(int TargetBearing)
{
  // Returns the bearing to the target.
  bearing = TargetBearing;
  return bearing;
}

int Radar::getRange(int TargetRange)
{
  // Returns the range to the target.
  range = TargetRange;
  return range;
}

void Radar::ShowData(SDL_Surface *screen, int x, int y){
  static char text[120];
  //	string file1 = "font.png";
  //string file2 = "font.dat";
  char filename[] = "images/largefont.png";
  char filename2[] = "data/largefont.dat";
  static DFont largeFont(filename, filename2);
  
  double c1, c2, c3, c4, c5, c6;
  int bearing;
  int range;
  
  c1 = double(316);
  c2 = double((460 - 316))/double(RangeScale);
  c3 = double(374);
  c4 = double((374 - 230))/double(RangeScale);
  
  c5 = c2*(double(y)-c3);
  c6 = c4*(double(x)-c1);
  
  ClearTextBox();
  
  
  if(x >= 316)
    {
      bearing = int(90.0 + (180/3.14)*atan(c5/c6));
    }
  else
    {
      bearing = int(270.0 + (180/3.14)*atan(c5/c6));
    }
  
  range = int(1000.0*sqrt(pow(((double(x)-c1)/c2),2)+pow(((double(y)-c3)/c4),2)));
  
  
  src.x=166;
  src.y=608; //define a rectangle on the screen and make it black
  src.h=30;
  src.w=90;
  
  sprintf(text, "  %i  ", bearing);
  largeFont.PutString(screen, 170, 615, text);
  
  src.x=383;
  src.y=608; //define a rectangle on the screen and make it black
  src.h=30;
  src.w=90;
  
  sprintf(text, "  %i  ", range);
  largeFont.PutString(screen, 370, 615, text);
  
  return;
  
}

int Radar::DeltaBearing(int bearing1, int bearing2)
{
	// Difference between two bearings
	int deltabearing;
        // for POSIX systems
        #ifndef WIN32
	if (fabs(bearing1 - bearing2) < 180){
		 deltabearing = (int) fabs(bearing1 - bearing2);
	}else{
		deltabearing = (int) (360 - fabs(bearing1 - bearing2));
	}
        // WIN32 prefers abs when dealignwith int
        #else
        if (abs(bearing1 - bearing2) < 180){
                 deltabearing = (int) abs(bearing1 - bearing2);
        }else{
                deltabearing = (int) (360 - abs(bearing1 - bearing2));
        }
        #endif
	return deltabearing;
}

int  Radar::ReciprocalBearing(int bearing)
{
	// returns the 180 degree opposite of the bearing given to it..
	int recipbearing;
	if (bearing >= 180){
		 recipbearing = bearing - 180;
	}else{
		recipbearing = bearing + 180;
	}
	return recipbearing;
}

void Radar::DrawPixel(SDL_Surface *screen, int x, int y, Uint32 color)
{
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

void Radar::DrawArc(SDL_Surface *screen, int X1, int Y1, int Radius, int Theta1, int Theta2, Uint32 Color)
{

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

void Radar::DrawCircle(SDL_Surface *screen, int X1, int Y1, int Radius, Uint32 Color)
{

//Draw a circle  at (X1,Y1) of a given radius using specified Color.
	int xc, yc, radius;
	xc = X1;
	yc = Y1;
	radius = Radius;
	DrawArc(screen, xc, yc, radius, 0, 360, Color);
}




