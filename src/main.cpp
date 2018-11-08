/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Tue Sep 19 17:16:48 PDT 2000
    copyright            : (C) 2000 by Michael Bridak
    email                : michael.bridak@verizon.net
$Id: main.cpp,v 1.28 2003/07/18 03:50:00 mbridak Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License.     *
 *    .                                   *
 *                                                                         *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/*Feeling left out? Get included!*/
#include <iostream>
#include <list>
#include <string.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <SDL.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_thread.h>
#include "dfont.h"
#include "draw.h"
#include "gameclock.h"
#include "submarine.h"
#include "towedarray.h"
#include "targetmotionanalysis.h"
#include "menu.h"
#include "message.h"
#include "mission.h"
#include "sonar.h"
#include "radar.h"
#include "files.h"
#include "esm.h"
#include "control.h"
#include "helicopter.h"
#include "main.h"
#include "sound.h"
#include "map.h"
#include "winfunctions.h"
#include <fstream>
#include <cstdlib>
#include <iomanip>
using namespace std;

MAP *my_map = NULL;

//#######GRAPHIC PRIMITIVES#############
void SetupScreen(bool full_screen){
	//Initialize the screen and some default colors

	if ( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_AUDIO) < 0 ) {
		cerr << "Couldn't initialize SDL: " << SDL_GetError() << endl;
		exit(1);
	}
	atexit(SDL_Quit);
	SDL_InitSubSystem(SDL_INIT_TIMER);
	if(full_screen){
		//Create main screen surface
		screen = SDL_SetVideoMode(1024, 768, 16, SDL_FULLSCREEN);
	}else{
		screen = SDL_SetVideoMode(1024, 768, 16, SDL_SWSURFACE);
	}

	if ( screen == NULL ) {
		cerr << "Couldn't set video mode..." << endl
		<< "OpenTorpedo requires 1024x768 @ 16 bpp" << endl
		<< SDL_GetError() << endl;
		exit(1);
	}
	SDL_WM_SetCaption("OpenTorpedo http://OpenTorpedo.sf.net", "OpenTorpedo");

	//define some standard colors
	textcolor = SDL_MapRGB(screen->format, 192, 255, 6);
	black = SDL_MapRGB(screen->format, 0, 0, 0);
	white = SDL_MapRGB(screen->format, 255, 255, 255);
	red = SDL_MapRGB(screen->format, 255, 99, 20);
        dark_red = SDL_MapRGB(screen->format, 140, 80, 10);
	green = SDL_MapRGB(screen->format, 0, 255, 0);
        dark_green = SDL_MapRGB(screen->format, 0, 120, 0);
	yellow = SDL_MapRGB(screen->format, 216, 255, 0);
        brown = SDL_MapRGB(screen->format, 120, 140, 0);
        grey = SDL_MapRGB(screen->format, 180, 180, 180);
        dark_grey = SDL_MapRGB(screen->format, 100, 100, 100);
	// mapcolor = SDL_MapRGB(screen->format, 130, 201, 225);
        mapcolor = SDL_MapRGB(screen->format, 10, 10, 100);
        // Init_Audio();
}

void MapIcon(int x, int y, int ShipType, int Friend, Uint32 color){
  // Display NTDS symbols according to the following Legend
  // Hostile:  Submarine = Red Bottom Half Diamond, Surface = Red Full Diamond,
  //           Aircraft = Red Top Half Diamond

  // Friendly: Submarine = green Bottom SemiCircle, Surface = green Circle,
  //           Aircraft = green Top SemiCircle

  // Unknown:  Submarine = grey Bottom Half Box, Surface = grey Box,
  //           Aircraft = grey Top Half Box

  // Neutral: Submarine = Yellow Bottom Half box, Surface = Yellow box,
  //           Aircraft = Yellow Top Half box

  // Is it a submarine?
  if(ShipType == TYPE_SUB) {
    switch(Friend){
    case 0: //Foe??
      DrawDiamond(screen, x-2, y+2, 7, 'B', black);  //Bottom Half Diamond
      DrawDiamond(screen, x, y, 7, 'B', color);  //Bottom Half Diamond
      break;
    case 1: //Friend??
      DrawArc(screen, x-2,y+2, 7, 180, 360, black); //Bottom SemiCircle
      DrawArc(screen, x-2, y+3, 7, 180, 360, black); // thickness
      DrawArc(screen, x ,y, 7, 180, 360, color); //Bottom SemiCircle
      DrawArc(screen, x, y + 1, 7, 180, 360, color); // thickness
      break;
    case 2://Unknown??
      DrawBox(screen, x-2 ,y+2, 7, 'B', black); //Bottom Half Box
      DrawBox(screen, x ,y, 7, 'B', color); //Bottom Half Box
      break;
    default: //Neutral
       DrawBox(screen, x-2 ,y+2, 7, 'B', black); //Bottom Half Box
       DrawBox(screen, x ,y, 7, 'B', color); //Bottom Half Box
    }
  }
 // Is it a surface ship?
    else if(ShipType == TYPE_SHIP) {
   switch(Friend){
   case 0: //Foe??
     DrawDiamond(screen, x-2 ,y+2, 7, 'F', black); //Diamond
     DrawDiamond(screen, x ,y, 7, 'F', color); //Diamond
     break;
   case 1: //Friend??
     DrawCircle(screen, x-2, y+2, 7, black); //Circle
     DrawCircle(screen, x, y, 7, color); //Circle
     break;
   case 2: //Unknown??
      DrawBox(screen, x-2 ,y+2, 7, 'F', black); //Box
      DrawBox(screen, x ,y, 7, 'F', color); //Box
      break;
   default: //Neutral
     DrawBox(screen, x-2 ,y+2, 7, 'F', black); //Cross
     DrawBox(screen, x ,y, 7, 'F', color); //Cross
    }
  }

  // Is it an airplane or Helo?
  else if(ShipType == TYPE_HELICOPTER) {
    switch(Friend){
    case 0: //Foe??
      DrawDiamond(screen, x, y, 7, 'T', color);  //Top Half Diamond
      break;
    case 1: //Friend??
      DrawArc(screen, x ,y, 7, 0, 180, color); //Top SemiCircle
      break;
    case 2: //Unknown??
      DrawBox(screen, x ,y, 7, 'T', color); //Top Half Box
      break;
    default: //Neutral
      DrawBox(screen, x ,y, 7, 'T', color); //Top Half Cross
    }
  }
  else if (ShipType == TYPE_TORPEDO)
  {
      DrawDiamond(screen, x, y, 7, 'B', color);
      DrawLine(screen, x, y-5, x, y, color);
  }
  else if (ShipType == TYPE_NOISEMAKER)
  {
      DrawBox(screen, x, y, 7, 'B', color);
  }
}


void DirectionalPointer(int x, int y, int heading, int speed,Uint32 color){
	//Draws a directional vane on a ships map icon.
	//the length will vary depending on the speed.

	double var1; //just a place to hold a value so we don't have to calc it twice.
	double destinationx; //the dest x&y point for
	double destinationy; //the drawn line.

	if((speed > 1) || (speed < -1)) speed=speed/2; //allow for 0 speed.
	var1 = heading * 0.017453; //convert degrees to radians.
	destinationx = (sin(var1) * speed) + x;
	destinationy = (-1) * (cos(var1) * speed) + y; 
	DrawLine(screen, x, y, (int)destinationx, (int)destinationy, color);
}

void LoadScreen(int item){
	//Loads a PN file into an SDL surface then
	//blits it to the display surface.
	//then deletes the temporary surface.


	SDL_Surface *IMGFile=NULL; // Get our selfs a surface to work with..
	SDL_Rect dest; //A rectangle structure to define a blit area
	switch (item)
	{
	case 0:
		IMGFile = Load_Image("images/tittle.png"); //fill the surface w/ the bmp
		break;
	case 1:
		IMGFile = Load_Image("images/Sonarscreen.png"); //fill the surface w/ the bmp
		break;
	case 2:
		IMGFile = Load_Image("images/Mapscreen.png"); //fill the surface w/ the bmp
		break;
	case 3:
		IMGFile = Load_Image("images/Mapscreen.png"); //fill the surface w/ the bmp
		break;
	case 4:
		IMGFile = Load_Image("images/ControlScreen.png"); //fill the surface w/ the bmp
		break;
	case 5:
		IMGFile = Load_Image("images/RadarScreen.png"); //fill the surface w/ the bmp
		break;
	case 6:
		IMGFile = Load_Image("images/ESMScreen.png"); //fill the surface w/ the bmp
		break;
        case 7:
                IMGFile = Load_Image("images/sub_surfacing.png"); break;
        case 8:
                IMGFile = Load_Image("images/sub_rising.png"); break;
        case 9:
                IMGFile = Load_Image("images/sub_menu.png"); break;
	default:
		cerr << "Unknown screen item" << endl;
		break;
	}
	if ( IMGFile == NULL ) { // Are we screwed??
		cerr << "Function LoadScreen()" << endl
				<< "Couldn't load screen item# " << item <<endl
				<< "SDL Error: " << SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
		return; // Whine Snivel and Bail..
	}
	dest.x = 0; // Blit destination x&y to the upper left
	dest.y = 0;
	dest.w = IMGFile->w; // Height and width equal to the
	dest.h = IMGFile->h;  // source images....
	SDL_BlitSurface(IMGFile, NULL, screen, &dest); // Do the actual Blit
	SDL_UpdateRects(screen, 1, &dest); //Show the screen...
	SDL_FreeSurface(IMGFile); //Free up the surface memory..
	return;
}

void LoadWidgets(){
	SDL_Surface *temp;

	temp = Load_Image("images/mapup.png");
	if(temp != NULL) navbuttonup = SDL_DisplayFormat(temp);
	if ( navbuttonup == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/mapdown.png");
	if(temp != NULL) navbuttondown = SDL_DisplayFormat(temp);
	if ( navbuttondown == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/tmaup.png");
	if(temp != NULL) tmabuttonup = SDL_DisplayFormat(temp);
	if ( tmabuttonup == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/tmadown.png");
	if(temp != NULL) tmabuttondown = SDL_DisplayFormat(temp);
	if ( tmabuttondown == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/shipcontrolup.png");
	if(temp != NULL) shipcontrolbuttonup = SDL_DisplayFormat(temp);
	if ( shipcontrolbuttonup == NULL ) { // Are we screwed??

		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/shipcontroldown.png");
	if(temp != NULL) shipcontrolbuttondown = SDL_DisplayFormat(temp);
	if ( shipcontrolbuttondown == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/esmup.png");
	if(temp != NULL) esmbuttonup = SDL_DisplayFormat(temp);
	if ( esmbuttonup == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/esmdown.png");
	if(temp != NULL) esmbuttondown = SDL_DisplayFormat(temp);
	if ( esmbuttondown == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/radarup.png");
	if(temp != NULL) radarbuttonup = SDL_DisplayFormat(temp);
	if ( radarbuttonup == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/radardown.png");
	if(temp != NULL) radarbuttondown = SDL_DisplayFormat(temp);
	if ( radarbuttondown == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/quitbuttonup.png");
	if(temp != NULL) quitbuttonup = SDL_DisplayFormat(temp);
	if ( quitbuttonup == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/quitbuttondown.png");
	if(temp != NULL) quitbuttondown = SDL_DisplayFormat(temp);
	if ( quitbuttondown == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/plusbuttonup.png");
	if(temp != NULL) plusbuttonup = SDL_DisplayFormat(temp);
	if ( quitbuttonup == NULL ) { // Are we screwed??

		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/plusbuttondown.png");
	if(temp != NULL) plusbuttondown = SDL_DisplayFormat(temp);
	if ( quitbuttondown == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/minusbuttonup.png");
	if(temp != NULL) minusbuttonup = SDL_DisplayFormat(temp);
	if ( minusbuttonup == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/minusbuttondown.png");
	if(temp != NULL) minusbuttondown = SDL_DisplayFormat(temp);
	if ( minusbuttondown == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/centerbuttondown.png");
	if(temp != NULL) centerbuttondown = SDL_DisplayFormat(temp);
	if ( centerbuttondown == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/centerbuttonup.png");
	if(temp != NULL) centerbuttonup = SDL_DisplayFormat(temp);
	if ( centerbuttonup == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/upbuttondown.png");
	if(temp != NULL) upbuttondown = SDL_DisplayFormat(temp);
	if ( upbuttondown == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/upbuttonup.png");
	if(temp != NULL) upbuttonup = SDL_DisplayFormat(temp);
	if ( upbuttonup == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/downbuttondown.png");
	if(temp != NULL) downbuttondown = SDL_DisplayFormat(temp);
	if ( centerbuttondown == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/downbuttonup.png");
	if(temp != NULL) downbuttonup = SDL_DisplayFormat(temp);
	if ( downbuttonup == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/leftbuttondown.png");
	if(temp != NULL) leftbuttondown = SDL_DisplayFormat(temp);
	if ( leftbuttondown == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/leftbuttonup.png");
	if(temp != NULL) leftbuttonup = SDL_DisplayFormat(temp);
	if ( leftbuttonup == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/rightbuttondown.png");
	if(temp != NULL) rightbuttondown = SDL_DisplayFormat(temp);
	if ( rightbuttondown == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/rightbuttonup.png");
	if(temp != NULL) rightbuttonup = SDL_DisplayFormat(temp);
	if ( rightbuttonup == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

        temp = Load_Image("images/noisemaker.png");
        if (temp)
        {
           noisemaker_image = SDL_DisplayFormat(temp);
           if (! noisemaker_image)
             cerr << "Error loading noisemaker image." << endl;
           SDL_FreeSurface(temp);
        }

        temp = Load_Image("images/torpedo.png");
        if (temp)
        {
           torpedo_image = SDL_DisplayFormat(temp);
           if (! noisemaker_image)
             cerr << "Error loading torpedo image." << endl;
           SDL_FreeSurface(temp);
        }

}



void UnLoadWidgets(){
	
	//free the surfaces

	SDL_FreeSurface(sonarbuttonup);
	SDL_FreeSurface(sonarbuttondown); 
	SDL_FreeSurface(navbuttonup);
	SDL_FreeSurface(navbuttondown);
	SDL_FreeSurface(tmabuttonup);
	SDL_FreeSurface(tmabuttondown);
	SDL_FreeSurface(quitbuttonup);
	SDL_FreeSurface(quitbuttondown); 
	SDL_FreeSurface(plusbuttonup);
	SDL_FreeSurface(plusbuttondown); 
	SDL_FreeSurface(minusbuttonup);
	SDL_FreeSurface(minusbuttondown); 
        SDL_FreeSurface(torpedo_image);
        SDL_FreeSurface(noisemaker_image);
}

void DisplayNavigationWidgets(){	
	if (navwidget){ // is the nav button down?
		destination_rectangle.x = 280; //upper left corner to
		destination_rectangle.y = 710; //place the button.
		destination_rectangle.h = navbuttondown->h; //height &
		destination_rectangle.w = navbuttondown->w;//width of button.
		SDL_BlitSurface(navbuttondown, NULL, screen, &destination_rectangle); // Do the Blit.
		SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
		if(mapcenter){
			destination_rectangle.x = 225; //upper left corner to
			destination_rectangle.y = 269; //place the button.
			destination_rectangle.h = centerbuttondown->h; //height &
			destination_rectangle.w = centerbuttondown->w;//width of button.
			SDL_BlitSurface(centerbuttondown, NULL, screen, &destination_rectangle); // Do the Blit.
			SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
		}else{
			destination_rectangle.x = 225; //upper left corner to
			destination_rectangle.y = 269; //place the button.
			destination_rectangle.h = centerbuttonup->h; //height &
			destination_rectangle.w = centerbuttonup->w;//width of button.
			SDL_BlitSurface(centerbuttonup, NULL, screen, &destination_rectangle); // Do the Blit.
			SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
		}
		 	destination_rectangle.x = 225; //upper left corner to
			destination_rectangle.y = 219; //place the button.
			destination_rectangle.h = upbuttonup->h; //height &
			destination_rectangle.w = upbuttonup->w;//width of button.
			SDL_BlitSurface(upbuttonup, NULL, screen, &destination_rectangle); // Do the Blit.
			SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
		 	destination_rectangle.x = 225; //upper left corner to
			destination_rectangle.y = 319; //place the button.
			destination_rectangle.h = downbuttonup->h; //height &
			destination_rectangle.w = downbuttonup->w;//width of button.
			SDL_BlitSurface(downbuttonup, NULL, screen, &destination_rectangle); // Do the Blit
			SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen..
		 	destination_rectangle.x = 175; //upper left corner to
			destination_rectangle.y = 269; //place the button.
			destination_rectangle.h = leftbuttonup->h; //height &
			destination_rectangle.w = leftbuttonup->w;//width of button.
			SDL_BlitSurface(leftbuttonup, NULL, screen, &destination_rectangle); // Do the Blit.
			SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen....
		 	destination_rectangle.x = 275; //upper left corner to
			destination_rectangle.y = 269; //place the button.
			destination_rectangle.h = rightbuttonup->h; //height &
			destination_rectangle.w = rightbuttonup->w;//width of button.
			SDL_BlitSurface(rightbuttonup, NULL, screen, &destination_rectangle); // Do the Blit.
			SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
		 	destination_rectangle.x = 175; //upper left corner to
			destination_rectangle.y = 359; //place the button.
			destination_rectangle.h = plusbuttonup->h; //height &
			destination_rectangle.w = plusbuttonup->w;//width of button.
			SDL_BlitSurface(plusbuttonup, NULL, screen, &destination_rectangle); // Do the Blit.
			SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen....
		 	destination_rectangle.x = 275; //upper left corner to
			destination_rectangle.y = 359; //place the button.
			destination_rectangle.h = minusbuttonup->h; //height &
			destination_rectangle.w = minusbuttonup->w;//width of button.
			SDL_BlitSurface(minusbuttonup, NULL, screen, &destination_rectangle); // Do the Blit.
			SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
	}else{ // our button must be up...
 		destination_rectangle.x = 280; //upper left corner to
		destination_rectangle.y = 710; //place the button.
		destination_rectangle.h = navbuttonup->h; //height &
		destination_rectangle.w = navbuttonup->w;//width of button.
		SDL_BlitSurface(navbuttonup, NULL, screen, &destination_rectangle); // Do the Blit.
		SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
	}
}

void DisplayTMAWidgets(){
	if (tmawidget){ // is the TMA button down?
	 	destination_rectangle.x = 380; //upper left corner to
		destination_rectangle.y = 710; //place the button.
		destination_rectangle.h = tmabuttondown->h; //height &
		destination_rectangle.w = tmabuttondown->w;//width of button.
		SDL_BlitSurface(tmabuttondown, NULL, screen, &destination_rectangle); // Do the Blit.
		SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
		if(Tma.centerGeoPlot){
			destination_rectangle.x = 225; //upper left corner to
			destination_rectangle.y = 269; //place the button.
			destination_rectangle.h = centerbuttondown->h; //height &
			destination_rectangle.w = centerbuttondown->w;//width of button.
			SDL_BlitSurface(centerbuttondown, NULL, screen, &destination_rectangle); // Do the Blit.
			SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
		}else{
			destination_rectangle.x = 225; //upper left corner to
			destination_rectangle.y = 269; //place the button.
			destination_rectangle.h = centerbuttonup->h; //height &
			destination_rectangle.w = centerbuttonup->w;//width of button.
			SDL_BlitSurface(centerbuttonup, NULL, screen, &destination_rectangle); // Do the Blit.
			SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
		}
		 	destination_rectangle.x = 225; //upper left corner to
			destination_rectangle.y = 219; //place the button.
			destination_rectangle.h = upbuttonup->h; //height &
			destination_rectangle.w = upbuttonup->w;//width of button.
			SDL_BlitSurface(upbuttonup, NULL, screen, &destination_rectangle); // Do the Blit.
			SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
		 	destination_rectangle.x = 225; //upper left corner to
			destination_rectangle.y = 319; //place the button.
			destination_rectangle.h = downbuttonup->h; //height &
			destination_rectangle.w = downbuttonup->w;//width of button.
			SDL_BlitSurface(downbuttonup, NULL, screen, &destination_rectangle); // Do the Blit
			SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen..
		 	destination_rectangle.x = 175; //upper left corner to
			destination_rectangle.y = 269; //place the button.
			destination_rectangle.h = leftbuttonup->h; //height &
			destination_rectangle.w = leftbuttonup->w;//width of button.
			SDL_BlitSurface(leftbuttonup, NULL, screen, &destination_rectangle); // Do the Blit.
			SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen....
		 	destination_rectangle.x = 275; //upper left corner to
			destination_rectangle.y = 269; //place the button.
			destination_rectangle.h = rightbuttonup->h; //height &
			destination_rectangle.w = rightbuttonup->w;//width of button.
			SDL_BlitSurface(rightbuttonup, NULL, screen, &destination_rectangle); // Do the Blit.
			SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
		 	destination_rectangle.x = 175; //upper left corner to
			destination_rectangle.y = 359; //place the button.
			destination_rectangle.h = plusbuttonup->h; //height &
			destination_rectangle.w = plusbuttonup->w;//width of button.
			SDL_BlitSurface(plusbuttonup, NULL, screen, &destination_rectangle); // Do the Blit.
			SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen....
		 	destination_rectangle.x = 275; //upper left corner to
			destination_rectangle.y = 359; //place the button.
			destination_rectangle.h = minusbuttonup->h; //height &
			destination_rectangle.w = minusbuttonup->w;//width of button.
			SDL_BlitSurface(minusbuttonup, NULL, screen, &destination_rectangle); // Do the Blit.
			SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
	}else{ // our button must be up...
		destination_rectangle.x = 380; //upper left corner to
		destination_rectangle.y = 710; //place the button.
		destination_rectangle.h = tmabuttonup->h; //height &
		destination_rectangle.w = tmabuttonup->w;//width of button.
		SDL_BlitSurface(tmabuttonup, NULL, screen, &destination_rectangle); // Do the Blit.
		SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
	}
}

void DisplayESMWidgets(){
 	destination_rectangle.x = 580; //upper left corner to
	destination_rectangle.y = 710; //place the button.

	if (esmwidget){ // is the nav button down?
		destination_rectangle.h =esmbuttondown->h; //height &
		destination_rectangle.w = esmbuttondown->w;//width of button.
		SDL_BlitSurface(esmbuttondown, NULL, screen, &destination_rectangle); // Do the Blit.
	}else{ // our button must be up...
		destination_rectangle.h = esmbuttonup->h; //height &
		destination_rectangle.w = esmbuttonup->w;//width of button.
		SDL_BlitSurface(esmbuttonup, NULL, screen, &destination_rectangle); // Do the Blit.
	}
	SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
}

void DisplayRADARWidgets(){
 	destination_rectangle.x = 680; //upper left corner to
	destination_rectangle.y = 710; //place the button.

	if (radarwidget){ // is the nav button down?
		destination_rectangle.h =radarbuttondown->h; //height &
		destination_rectangle.w = radarbuttondown->w;//width of button.
		SDL_BlitSurface(radarbuttondown, NULL, screen, &destination_rectangle); // Do the Blit.
	}else{ // our button must be up...
		destination_rectangle.h = radarbuttonup->h; //height &
		destination_rectangle.w = radarbuttonup->w;//width of button.
		SDL_BlitSurface(radarbuttonup, NULL, screen, &destination_rectangle); // Do the Blit.
	}
	SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
}

void DisplayShipControlWidgets(){
 	destination_rectangle.x = 480; //upper left corner to
	destination_rectangle.y = 710; //place the button.

	if (shipcontrolwidget){ // is the nav button down?
		destination_rectangle.h = shipcontrolbuttondown->h; //height &
		destination_rectangle.w = shipcontrolbuttondown->w;//width of button.
		SDL_BlitSurface(shipcontrolbuttondown, NULL, screen, &destination_rectangle); // Do the Blit.
	}else{ // our button must be up...
		destination_rectangle.h = shipcontrolbuttonup->h; //height &
		destination_rectangle.w = shipcontrolbuttonup->w;//width of button.
		SDL_BlitSurface(shipcontrolbuttonup, NULL, screen, &destination_rectangle); // Do the Blit.
	}
	SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
}
void DisplayWidgets(){
	SonarStation.DisplaySonarWidgets();
	DisplayNavigationWidgets();
	DisplayTMAWidgets();
	DisplayShipControlWidgets();
	DisplayESMWidgets();
	DisplayRADARWidgets();

	destination_rectangle.x = 780; //upper left corner to
	destination_rectangle.y = 710; //place the button.
	if (quitwidget){ // is the quit button down?
		destination_rectangle.h = quitbuttondown->h; //height &
		destination_rectangle.w = quitbuttondown->w;//width of button.
		SDL_BlitSurface(quitbuttondown, NULL, screen, &destination_rectangle); // Do the Blit.
	}else{ // our button must be up...
		destination_rectangle.h = quitbuttonup->h; //height &
		destination_rectangle.w = quitbuttonup->w;//width of button.
		SDL_BlitSurface(quitbuttonup, NULL, screen, &destination_rectangle); // Do the Blit.
	}
	SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...

	destination_rectangle.x = 880; //upper left corner to
	destination_rectangle.y = 710; //place the button.
	destination_rectangle.h = plusbuttonup->h; //height &
	destination_rectangle.w = plusbuttonup->w;//width of button.
	SDL_BlitSurface(plusbuttonup, NULL, screen, &destination_rectangle); // Do the Blit.
	SDL_UpdateRects(screen, 1, &destination_rectangle); //Show button on the screen...
	destination_rectangle.x = 970; //upper left corner to
	destination_rectangle.y = 710; //place the button.
	destination_rectangle.h = minusbuttonup->h; //height &
	destination_rectangle.w = minusbuttonup->w;//width of button.
	SDL_BlitSurface(minusbuttonup, NULL, screen, &destination_rectangle); // Do the Blit.
	SDL_UpdateRects(screen, 1, &destination_rectangle); //Show button on the screen...

}



// This function adds a torpedo to the linked-list of torpedoes
// The returned value is the new linked-list pointer, in case
// the new_torpedo is the first in the list.
Submarine *Add_Ship(Submarine *all_torp, Submarine *new_torp)
{
   Submarine *my_torp;

   // we do not have a torpedo to add
   if (! new_torp)
     return all_torp;

   // empty list
   if (! all_torp)
       return new_torp;

   // add one to an existing list
   my_torp = all_torp;
   while (my_torp->next)
      my_torp = my_torp->next;

   // add to end of list
   my_torp->next = new_torp;
   
   return all_torp;
}


// This function removes a torpedo from the list of all
// torpedoes. It returns the first item in the torpedo linked-list.
Submarine *Remove_Ship(Submarine *all_torpedoes, Submarine *old_torpedo)
{
   Submarine *my_torp, *previous;

   if (! all_torpedoes)
      return NULL;
   if (! old_torpedo)
      return all_torpedoes;

   my_torp = all_torpedoes;
   previous = NULL;
   while (my_torp)
   {
      // found match and it is the first in the list
      if ( (my_torp == old_torpedo) && (! previous) )
      {
          previous = my_torp->next;
          delete my_torp;
          return previous;
      }
      // found a match and it isn't the first in the list
      else if (my_torp == old_torpedo)
      {
         previous->next = my_torp->next;
         delete my_torp;
         return all_torpedoes;
      }
      // this one does not match, go to next node
      else
      {
          previous = my_torp;
          my_torp = my_torp->next;
      }
      
   }
   // item wasn't found, just return the complete list
   return all_torpedoes;
}



Helicopter *Add_Helicopter(Helicopter *all_helicopters, Helicopter *new_helicopter)
{
   Helicopter *current;

   if (! new_helicopter)
      return all_helicopters;
   if (! all_helicopters)
      return new_helicopter;

   current = all_helicopters;
   while (current->next)
      current = current->next;
   current->next = new_helicopter;

  return all_helicopters;
}



Helicopter *Remove_Helicopter(Helicopter *all_helicopters, Helicopter *old_helicopter)
{
   Helicopter *current, *previous = NULL;

   if (! all_helicopters)
      return NULL;
   if (! old_helicopter)
      return all_helicopters;

   current = all_helicopters;
   while (current)
   {
       // first in list
       if ( (current == old_helicopter) && (! previous) )
       {
          previous = current->next;
          delete current;
          return previous;
       }
       // not first in list
       else if (current == old_helicopter)
       {
          previous->next = current->next;
          delete current;
          return all_helicopters;
       }
       // this is not our node
       else
       {
          previous = current;
          current = current->next;
       }
   }
   return all_helicopters;
}





//######################################
void ShipHandeling(){
       Submarine *my_torp, *temp_torp;
       int status;
       Submarine *ship;
       Helicopter *helicopter;

       // see if we can use radar, esm, etc
       if (player->Depth > PERISCOPE_DEPTH)
       {
           EsmStation.LowerMast();
           RadarStation.LowerMast();
           player->using_radar = FALSE;
       }
       // if array is out, limit speed
       if (TB16.GetLength() > 0)
       {
            int max_speed = Subs->MaxSpeed / 2;
            if (Subs->Speed > max_speed)
               Subs->Speed = max_speed;
       }

        ship = Subs;
	while (ship)
        {
		ship->UpdateLatLon();	//Move all the ships
                // see what the AI wants to do
                if (ship != Subs)
                {
                  /*
                  if (ship->ShipType == TYPE_SUB)
                    torpedoes = ship->Sub_AI(Subs, torpedoes);
                  else if (ship->ShipType == TYPE_SHIP)
                    torpedoes = ship->Ship_AI(Subs, torpedoes);
                  */
                   torpedoes = ship->Sub_AI(Subs, torpedoes, helicopters);
                }
		ship->Handeling();	//Steer, Change Depth etc...

                // we were pinging, but it fades quickly
                if (ship->pinging > 0)
                   ship->pinging--;
                ship = ship->next;
	}
        helicopter = helicopters;
        while (helicopter)
        {
             helicopter->UpdateLatLon();
             torpedoes = helicopter->Helicopter_AI(Subs, torpedoes);
             #ifdef DEBUG_HELICOPTER
             printf("Course: %d\nSpeed: %d\nHeight: %d\n",
                     (int) helicopter->Heading, (int) helicopter->Speed,
                     (int) helicopter->Depth);
             printf("DX: %d DY: %d\nX: %d  Y: %d\n",
                    helicopter->destination_x, helicopter->destination_y,
                    (int) helicopter->Lat_TotalYards, 
                    (int) helicopter->Lon_TotalYards);
             #endif
             helicopter->Handle();
             helicopter = helicopter->next;
        }

	for (int x = 0; x < MAX_SUBS; x++){
		Contacts[x].UpdateContact();	
	}

        // move some torps
        my_torp = torpedoes;
        while (my_torp)
        {
           // check to see if this torpedo is chasing the player
           if ( (my_torp->target == player) && 
                (my_torp->fuel_remaining == (TORPEDO_FUEL - 30)) )
           {
               Message.post_message("Torpedo coming our way!");
               Message.display_message();
           }
           my_torp->UpdateLatLon();  
           my_torp->Torpedo_AI(Subs);   // see where we should be going
           my_torp->Handeling();    // change heading and depth
           status = my_torp->Check_Status();  // see if we ran into something
                                              // or we are out of fuel
           if (status == OUT_OF_FUEL)
           {
               if (my_torp->ShipType == TYPE_TORPEDO)
                  Message.post_message("A torpedo ran out of fuel.");
               else
                  Message.post_message("Noisemaker stopped running.");
               temp_torp = my_torp->next;
               if (current_target == my_torp)
                 current_target = NULL;
               Subs->Cancel_Target(my_torp);
               torpedoes = Remove_Ship(torpedoes, my_torp);
               my_torp = temp_torp;
               Message.display_message();
           }
           else if (status == HIT_TARGET)
           {
               int target_status = DAMAGE_OK;
               // damage target
               // we should always have a target, but just in case...
               if (my_torp->target)
               {
                  if (my_torp->target == player)
                     Message.post_message("We have been hit!");
                  else
                     Message.post_message("A torpedo hit its target!");
                  // radio warning to others
                  if ( (my_torp->target->ShipType == TYPE_SHIP) &&
                       (my_torp->target->mood == MOOD_CONVOY) )
                  {
                      my_torp->target->Radio_Signal(Subs, RADIO_UNDER_ATTACK);
                  }
                  target_status = my_torp->target->Take_Damage();
                  if (target_status == DAMAGE_SINK)
                  {
                     if (my_torp->target == player)
                       player = NULL;
                     if (my_torp->target->mission_status == MISSION_ALIVE)
                        my_mission_status = MISSION_FAILED;
                     Remove_Inactive_Ship(my_torp->target);
                  }
               }
               if (current_target == my_torp)
                  current_target = NULL;
               Subs->Cancel_Target(my_torp);
               temp_torp = my_torp->next;
               torpedoes = Remove_Ship(torpedoes, my_torp);
               my_torp = temp_torp;
               // Message.post_message("A torpedo hit its target!");
               if (target_status == DAMAGE_SINK)
                  Message.post_message("Target is sinking!");
               Message.display_message();
           }
           else
              my_torp = my_torp->next;
        }   // end of all torpedoes

        // see if we can still detect the current target
        if (current_target)
        {
            status = player->Can_Detect(current_target);
            if (status < CONTACT_WEAK) 
               current_target = NULL;
        }
}



// This function may get a little messy, we need to do a few things
// to remove a ship completely from the scenario.
// 1. Any torpedoes shooting at this target have to have their
//    target variable set to NULL
// 2. Any ships/subs who were tracking this ship need to stop
void Remove_Inactive_Ship(Submarine *victim)
{
    Submarine *torpedo;
    Submarine *ship;

    if (! victim)   // couldn't find victim, this should never happen, quit
        return;

    // 1. Cancel torpedoes targetting this ship
    torpedo = torpedoes;
    while (torpedo)
    {
        if (torpedo->target == victim)
          torpedo->target = NULL;
        torpedo = torpedo->next;
    }

    // 2. Ships who were tracking this ship need to stop
    ship = Subs;
    while (ship)
    {
       ship->Cancel_Target(victim);
       ship = ship->next;
    }

    Subs->last_target = NULL;
    Subs = Remove_Ship(Subs, victim);
    if (victim == current_target)
       current_target = NULL;
}






double RelativeBearing(Submarine *observer, Submarine *target){
	// Returns the relative bearing of target to observer. This bearing is
	//measured clockwise from on ships heading to target bearing.
	double relative_bearing;
	int observer_heading, bearing_to_target;
	bearing_to_target = (int) observer->BearingToTarget(target );
	observer_heading = (int)observer->Heading;
	if(observer_heading > bearing_to_target) bearing_to_target += 360;
	relative_bearing = bearing_to_target - observer_heading;
	return relative_bearing;
}

int ReciprocalBearing(int bearing){
	// returns the 180 degree opposite of the bearing given to it..
	int recipbearing;
	if (bearing >= 180){
		 recipbearing = bearing - 180;
	}else{
		recipbearing = bearing + 180;
	}
	return recipbearing;
}

double AngleOnBow(Submarine *observer, Submarine *target){
	// Returns the Angle on the Bow (AOB) of target relative to observer.
	// Note: -180 < Angle on Bow < 180
	// Positive AOB = Port AOB, Negative AOB = Starboard AOB
	// Examples:
	// target AOB port 30 -> +30
	// target AOB starboard 30 -> -30
	// target pointing at observer -> 0
	// target pointing away from observer -> -180
	double aob=0;
	aob = target->Heading - observer->Heading
	-	RelativeBearing(observer,target) - 180;
	if (aob > 180) aob = 360 - aob;
	if (aob < -180) aob = 360 + aob;
	return aob;
}

int minimize360 ( int compass_bearing ){
	// If course is bigger than 360 degrees, then subtract 360 degrees from it
	// and keep doing so until it is less than 360 degrees.
	while ( compass_bearing > 360 )
	{
		compass_bearing -= 360;
	}
	return compass_bearing;
}

int maximize360 ( int compass_bearing ){
	// If course is negative then, then add 360 degrees to it
	// and keep doing so until it is positive.
	while ( compass_bearing < 0 )
	{
		compass_bearing += 360;
	}
	return compass_bearing;
}

double CalculateRange(Submarine *observer, Submarine *target){
	//Calculates the distance in yards from the observer, to the target.

	double latdif = 0, londif = 0; //sqrt needs doubles
	LatLonDifference( observer,  target,  &latdif,  &londif );
	return sqrt((latdif * latdif) + (londif * londif));
}

void LatLonDifference(Submarine *x, Submarine *y, double *platdif, double *plondif){ // CHANGE TO REFERENCES
	//Returns in yards the difference between object x and object y

	if (x->Lat_TotalYards > y->Lat_TotalYards){
		*platdif = x->Lat_TotalYards - y->Lat_TotalYards;
	}
	else{
		*platdif = y->Lat_TotalYards - x->Lat_TotalYards;
	}

	if (x->Lon_TotalYards > y->Lon_TotalYards){
		*plondif = x->Lon_TotalYards - y->Lon_TotalYards;
	}
	else{
		*plondif = y->Lon_TotalYards - x->Lon_TotalYards;
	}
}

void CreateShips(int mission_number, MAP *map)
{
  char *ship_file, *mission_name;
  char filename[128];
  char line[256], *status;
  // int i;
  FILE *my_file, *mission_file;
  Submarine *new_ship;
  Helicopter *new_helicopter;
  int ship_type, ship_class;

  #ifndef WIN32
  snprintf(filename, 128, "data/ships%d.dat", mission_number);
  #else
  sprintf(filename, "data/ships%d.dat", mission_number);
  #endif

  ship_file = Find_Data_File(filename);
  my_file = fopen(ship_file, "r");
  if ( (ship_file) && (ship_file != filename) )
     free(ship_file);

  // idiot check
  if(!my_file){
    printf("Create ships: file missing\n");
    exit(1);
  }

  #ifndef WIN32
  snprintf(filename, 128, "data/mission%d.dat", mission_number);
  #else
  sprintf(filename, "data/mission%d.dat", mission_number);
  #endif
  mission_name = Find_Data_File(filename);
  mission_file = fopen(mission_name, "r");
  if ( (mission_name) && (mission_name != filename) )
    free(mission_name);

  
  // Read in the data from the ship file
  // Format of ship file: 1 row of data per ship
  // Active flag, Ship Type, Friend flag, Speed, DesiredSpeed, Depth, 
  // Desired depth, Heading, Desired
  // Heading, Rudder, Lat_TotalYards, Lon_TotalYards, PSCS

  // Notes: Ship Type: Sub=0, Surface=1, Aircraft=2
  //        Friend: Foe=0, Friend=1, Unknown=2, Neutral=3
  // i = 0;
  status = fgets(line, 256, my_file);
  while (status)
  {
   sscanf(line, "%d %d", &ship_type, &ship_class);
   if (ship_class == CLASS_HELICOPTER)
   {
         #ifdef DEBUG
         printf("Creating new helicopter.\n");
         #endif
         new_helicopter = new Helicopter();
         if (new_helicopter)
         {
             new_helicopter->map = map;
             sscanf(line, "%d %d %d %d %d %d %f %f %d",
                    &(new_helicopter->ShipType),
                    &(new_helicopter->ShipClass),
                    &(new_helicopter->Friend),
                    &(new_helicopter->DesiredSpeed),
                    &(new_helicopter->DesiredDepth),
                    &(new_helicopter->DesiredHeading),
                    &(new_helicopter->Lat_TotalYards),
                    &(new_helicopter->Lon_TotalYards),
                    &(new_helicopter->has_sonar) );
             // check mood 
        #ifndef WIN32
        if ( strcasestr(line, "convoy") )
            new_helicopter->mood = MOOD_CONVOY;
        else if ( strcasestr(line, "passive") )
            new_helicopter->mood = MOOD_PASSIVE;
        else if ( strcasestr(line, "attack") )
            new_helicopter->mood = MOOD_ATTACK;
        #else
        if ( my_strcasestr(line, "convoy") )
            new_helicopter->mood = MOOD_CONVOY;
        else if ( my_strcasestr(line, "passive") )
            new_helicopter->mood = MOOD_PASSIVE;
        else if ( my_strcasestr(line, "attack") )
            new_helicopter->mood = MOOD_ATTACK;
        #endif
        new_helicopter->Speed = new_helicopter->DesiredSpeed;
        new_helicopter->Heading = new_helicopter->DesiredHeading;
        new_helicopter->Depth = new_helicopter->DesiredDepth;
             // load mission
        new_helicopter->Load_Mission(mission_file);
             // load class
        sprintf(filename, "ships/class%d.shp", new_helicopter->ShipClass);
        ship_file = Find_Data_File(filename);
        new_helicopter->Load_Class(ship_file);
        // add helicopter to list
        helicopters = Add_Helicopter(helicopters, new_helicopter);
        if ( (ship_file) && (ship_file != filename) )
           free(ship_file);
        }    // end of created a helicopter

   } // end of helicopter

   else   // all other classes
   {
   #ifdef DEBUG
   printf("Creating new ship\n");
   #endif
     new_ship = new Submarine();
     if (new_ship)
     {
        #ifdef DEBUG
        printf("Added new ship to list of ships.\n");
        #endif
        new_ship->Init();
        new_ship->map = map;
        #ifdef DEBUG
        printf("Loaded: %s", line);
        #endif
        sscanf(line, "%d %d %d %d %d %d %f %f",
                  &(new_ship->ShipType),
                  &(new_ship->ShipClass), &(new_ship->Friend),
                  &(new_ship->DesiredSpeed), &(new_ship->DesiredDepth),
                  &(new_ship->DesiredHeading), &(new_ship->Lat_TotalYards),
                  &(new_ship->Lon_TotalYards)); // , &(new_ship->PSCS) );
        // check mood
        #ifndef WIN32
        if ( strcasestr(line, "convoy") )
            new_ship->mood = MOOD_CONVOY;
        else if ( strcasestr(line, "passive") )
            new_ship->mood = MOOD_PASSIVE;
        else if ( strcasestr(line, "attack") )
            new_ship->mood = MOOD_ATTACK;
        #else
        if ( my_strcasestr(line, "convoy") )
            new_ship->mood = MOOD_CONVOY;
        else if ( my_strcasestr(line, "passive") )
            new_ship->mood = MOOD_PASSIVE;
        else if ( my_strcasestr(line, "attack") )
            new_ship->mood = MOOD_ATTACK;
        #endif

     #ifdef DEBUG
     printf("%d %d %d %d %d %d %f %f %d\n",
                  (new_ship->ShipType),
                  (new_ship->ShipClass), (new_ship->Friend),
                  (new_ship->DesiredSpeed), (new_ship->DesiredDepth),
                  (new_ship->DesiredHeading), (new_ship->Lat_TotalYards),
                  (new_ship->Lon_TotalYards), new_ship->mood); 
                  // , (new_ship->PSCS) );

     #endif
     new_ship->Speed = new_ship->DesiredSpeed;
     new_ship->Depth = new_ship->DesiredDepth;
     new_ship->Heading = new_ship->DesiredHeading;
     #ifdef DEBUG
     printf("Set S: %f H: %f D: %f\n", new_ship->Speed,
            new_ship->Heading, new_ship->Depth);
     #endif
     new_ship->Load_Mission(mission_file);
     #ifdef DEBUG
     printf("Ship has mission flag: %d\n", new_ship->mission_status);
     #endif
     sprintf(filename, "ships/class%d.shp", new_ship->ShipClass);
     ship_file = Find_Data_File(filename);
     new_ship->Load_Class(ship_file);
     Subs = Add_Ship(Subs, new_ship);
     if ( (ship_file) && (ship_file != filename) )
        free(ship_file);
     // i+=1;
     }   // end of successfully created new ship

     }  // end of other classes

     status = fgets(line, 256, my_file);
  }
  // inClientFile.close();
  if (my_file)
     fclose(my_file);
  if (mission_file)
     fclose(mission_file);

  // ships = i - 1;
  // ships = i;
  // rdm 5/15/01 testing to be sure correct number of ships being read
  // cout << " Number of ships = " <<  i-1 << endl;
  if (Subs)
  {
      SonarStation.Subs = Subs;
      RadarStation.Subs = Subs;
      EsmStation.Subs = Subs;
      ControlStation.Subs = Subs;
      player = Subs;
  }
}

void UpdateSensors(){
//Added code to place our boats position into a FIFO so we
//can calculate where the sonar array is at any time.
//the cable on the TB16 is 2600ft. wich is 866yds. The FIFO is
//100 elements holding 2 double precision floats. So 866/100
//Is 8.66, Which is the resolution of our placement of the array.
        #ifdef DEBUG
        printf("Updating sensors and stuff\n");
        #endif
	static float distance_traveled;
	distance_traveled += Subs->Speed * 0.555;
	if(distance_traveled >= 8.66){ //if we've gone 8.66yds record our loc in the FIFO
		TB16.RecordPos(Subs->Lat_TotalYards,  Subs->Lon_TotalYards);
		distance_traveled = distance_traveled - 8.66;
	}
	TB16.OperateWinch();//extend/retract the array as needed.
	SoundEnvironment(); //lets give a listen...
	SonarStation.Sonar(SonarStation.GetNorthCenterState());
	SonarStation.TowedSonar(SonarStation.GetNorthCenterState());
}

void UpdateDisplay(){
	if (drawmap){
                Display_Target();
		DrawMap(); // fix me to do something useful!
                Draw_Depth_Meter(player, SCREEN_NAV);
	}
	if (drawsonar){
		SonarStation.UpdateDisplay(current_target);
	}
	if (drawtma){
		DisplayTMA();
	}
        if (drawweapons)
            DisplayWeapons();
	if (drawradar){
		RadarStation.DisplayContacts();	
	}
	if (drawesm){
		EsmStation.DisplayContacts();			
	}
	if (drawcontrol){
		ControlStation.Display();
                Draw_Depth_Meter(player, SCREEN_HELM);
	}

}



// Show information on the currently selected target
void Display_Target()
{
   char buffer[256];
   SDL_Rect rectangle;
   DFont fnt("images/font.png", "data/font.dat");
   float range;
   double bearing;

   // if (! current_target)
   //   return;

   // make empty box to the side of the screen
   rectangle.x = 120;
   rectangle.y = 400;
   rectangle.w = 200;
   rectangle.h = 160;
   SDL_FillRect(screen, &rectangle, black);

   if (current_target)
   { 
   // fill in data
   sprintf(buffer, "   Target");
   fnt.PutString(screen, 150, 400, buffer);
   sprintf(buffer, "Heading: %d", (int) current_target->Heading);
   fnt.PutString(screen, 140, 424, buffer);
   
   sprintf(buffer, "Spead: %d knots", (int) current_target->Speed);
   fnt.PutString(screen, 140, 436, buffer);

   range = Subs->DistanceToTarget(current_target);
   // range *= 0.000568;
   range *= YARDS_TO_MILES;
   sprintf(buffer, "Range: %2.1f miles", range);
   fnt.PutString(screen, 140, 448, buffer);

   bearing = Subs->BearingToTarget(current_target);
   sprintf(buffer, "Bearing: %2.0lf", bearing);
   fnt.PutString(screen, 140, 460, buffer); 

   sprintf(buffer, "Depth: %d feet", (int) current_target->Depth);
   fnt.PutString(screen, 140, 472, buffer);

   sprintf(buffer, "Type: %s ", current_target->ClassName );
   if (current_target->ClassType[0])
      strcat(buffer, current_target->ClassType); 
   fnt.PutString(screen, 140, 484, buffer);
   }   // end of valid target
   SDL_UpdateRects(screen, 1, &rectangle); 
}



void Draw_Depth_Meter(Submarine *my_sub, int screen_number)
{
   SDL_Rect rectangle;
   int y, index;

   if (screen_number == SCREEN_NAV)
   {
     rectangle.x = 890;
     rectangle.y = 145;
     rectangle.h = 500;
   }
   else   // helm screen
   {
     rectangle.x = 450;
     rectangle.y = 125;
     rectangle.h = 525;
   }
   rectangle.w = 10;

   if (! my_sub)
      return;
   SDL_FillRect(screen, &rectangle, mapcolor);
   if (my_sub->map)
   {
     for (index = 0; index < my_sub->map->Thermals_Between(0, MAX_DEPTH); index++)
     {
        y = my_sub->map->thermals[index];
        y = rectangle.y + (y / 10);
        FillRectangle(screen, rectangle.x, y, rectangle.x + 10, y+1, white);
     }
   }
   y = (my_sub->Depth / 10) + rectangle.y;
   FillRectangle(screen, rectangle.x, y, rectangle.x + 10, y+1, red);
   SDL_UpdateRects(screen, 1, &rectangle);
}


void DrawMap(){
	//Future home of an actual map display routine, right now just a blank field with dots

	SDL_Rect rectangle;
	rectangle.x=374;
	rectangle.y=145;   //define a rectangle on the screen and make it blue
	rectangle.h=500;
	rectangle.w=501;
	SDL_FillRect(screen, &rectangle, mapcolor);

	for (int x=374; x<874; x+= 10){ //Make meaningless dots grid on "Map"
		for (int y=145; y<637; y+= 10){
			DrawPixel(screen, x+5, y+5, white);
		}
	}
        #ifdef DEBUG
        printf("About to place ships.\n");
        #endif
	PlaceShips(mapscale, 0, 0, current_target);
	SDL_UpdateRects(screen, 1, &rectangle);

}

void PlaceShips(int scale, int change_scrollx, int change_scrolly, Submarine *current_target){
	//Places all Ships onto the map. Soon to change, so only registered contacts appear.
	//scale is in YDS per pixel
        int fresh;
	int x, y; //where to place the ships
	int xoffset = 374;  //offsets to move the ships to
	int yoffset = 145;  //the defined place on map screen
	static int scrolloffsetx=0; //offset to center map
	static int scrolloffsety=0;
	Uint32 color;
        Submarine *a_torp;
        Submarine *target_ship;
        Helicopter *a_helicopter;
        int layers;

	scale = scale * MAP_FACTOR;
	if(mapcenter){ //center map onto our own ntds symbol
		scrolloffsetx = 250 - ((int)Subs->Lat_TotalYards / scale);
		scrolloffsety = 250 - ((int)Subs->Lon_TotalYards / scale);
	}else{
		scrolloffsetx += change_scrollx;
		scrolloffsety += change_scrolly;
	}
        target_ship = Subs;
        #ifdef DEBUG
        printf("About to place ships on map.\n");
        #endif
        while (target_ship)
        {
             fresh = Subs->Can_Detect(target_ship);
             #ifdef DEBUG
             printf("Freshness factor %d\n", fresh);
             #endif
             if ( (fresh) || (target_ship == Subs) )
             {
		x = 500 - ((int)target_ship->Lat_TotalYards / scale);
		x = x - scrolloffsetx;
		y = 500 - ((int)target_ship->Lon_TotalYards / scale);
		y = y - scrolloffsety;
		if (x>10 && x<490 && y>10 && y<490){ //are we going to fall off the damn map???
			x = x + xoffset;
			y = y + yoffset;
                        // not only do we need friend/foe, but
                        // also fresh/old contact
                        // fresh = Subs->Can_Detect(target_ship);
			switch(target_ship->Friend){
				case 0: //Foe??
                                        if (fresh >= CONTACT_SOLID)
					  color = red;
                                        else
                                          color = dark_red;
					break;
				case 1: //Friend??
                                        if (fresh >= CONTACT_SOLID)
					  color = green;
                                        else
                                          color = dark_green;
					break;
				case 2: // Neither???
                                        if (fresh >= CONTACT_SOLID)
                       			   color = yellow;
                                        else
                                           color = brown;
                                        break;
				default: // Unknown
                                        if (fresh >= CONTACT_SOLID)
                                           color = grey;
                                        else
                                           color = dark_grey;
					break;
			}
			if (target_ship == Subs){  // Is it me???
				color = green;
			}
                        #ifdef DEBUG
                        printf("Deciding if to draw clearly.\n");
                        #endif
                        if ( (fresh >= CONTACT_WEAK) || (Subs == target_ship) )
                        {
			  MapIcon(x, y, (int)target_ship->ShipType, (int)target_ship->Friend, color); //Draw the NTDS symbol.
                          // check to see if we should highlight
                          if (current_target == target_ship)
                             MapIcon(x, y+1, (int)target_ship->ShipType, (int)target_ship->Friend, color);
			  DirectionalPointer(x-2, y+2, (int)target_ship->Heading, (int)target_ship->Speed, black);
			  DirectionalPointer(x, y, (int)target_ship->Heading, (int)target_ship->Speed, color);
			// Add pointer the show heading.
                        }   // end of we can hear you ok
		}   // end of we are on the map
             }    // end of able to detect
             target_ship = target_ship->next;
	}

        // now place torpedoes
        #ifdef DEBUG
        printf("About to draw torpedoes on map\n");
        #endif
        a_torp = torpedoes;
        while (a_torp)
        {
           x = 500 - ((int)a_torp->Lat_TotalYards / scale);
           x = x - scrolloffsetx;
           y = 500 - ((int)a_torp->Lon_TotalYards / scale);
           y = y - scrolloffsety;
           if (x>10 && x<490 && y>10 && y<490)  //are we going to fall off  map??
           {
              x = x + xoffset;
              y = y + yoffset;
              switch(a_torp->Friend){
                 case 0: //Foe??
                         color = red;
                         break;
                 case 1: //Friend??
                         color = green;
                         break;
                 case 2: // Neither???
                         color = yellow;
                         break;
                 default: // Unknown
                          color = grey;
                          break;
               }
               MapIcon(x, y, (int)a_torp->ShipType, (int)a_torp->Friend, color); 
           }   // within map limits
          
           a_torp = a_torp->next;
        }  // end of displaying torpedoes

       #ifdef DEBUG
       printf("About to draw helicopters on map.\n");
       #endif
       // we only draw helicopters if we are near the surface
       // and moving slow. This is crude, but it'll do for now
       layers = my_map->Thermals_Between(0, player->Depth);
       if ( (layers < 1) && (player->Speed < (player->MaxSpeed / 2) ) )
       {
       a_helicopter = helicopters;
       while (a_helicopter)
       {
           x = 500 - ((int)a_helicopter->Lat_TotalYards / scale);
           x = x - scrolloffsetx;
           y = 500 - ((int)a_helicopter->Lon_TotalYards / scale);
           y = y - scrolloffsety;
           if (x>10 && x<490 && y>10 && y<490)  // are we on the map
           {
               x = x + xoffset;
               y = y + yoffset;
               switch (a_helicopter->Friend)
               {
                  case FOE: color = red; break;
                  case FRIEND: color = green; break;
                  case NEUTRAL: color = grey; break;
                  case UNKNOWN:
                  default:  color = yellow; break;
               }   // end of friend/foe switch
               MapIcon(x, y, a_helicopter->ShipType, 
                       a_helicopter->Friend, color);
           }  // within map limits
           a_helicopter = a_helicopter->next;
       }   // end of drawing helicopters
       }   // end of if near surface
}



// calls the other detection functions to see if there is
// any way we can pick up the target
float Any_Detection(double Range, Submarine *observer, Submarine *target)
{
    float status;

    #ifdef DEBUG
    printf("Attempting all forms of detection.\n");
    #endif
    status = Radar_Detection(Range, observer, target);
    if (status)
    {
      // printf("I can see on radar target %d\n", target);
      return 2.0;
    }
    status = Esm_Detection(Range, observer, target);
    if (status)
    {
      // printf("I can pick up on esm target %d\n", target);
      return 1.0;
    }
    // this one takes the most math, so we do it last
    // status = Sonar_Detection(Range, observer, target);
    status = Sonar_Detection_New(Range, observer, target);
    // if (status)
    //   printf("I can hear target %d\n", target);
    return status;
}


// Can I see you on radar?
float Radar_Detection(double Range, Submarine *observer, Submarine *target)
{
   int depth, range;
   bool result;

   depth = (int) observer->Depth;
   range = (int) observer->DistanceToTarget(target);
   result = RadarStation.isTargetVisible(target, range, depth,
                         SHIP_HEIGHT, DEFAULT_SEA_STATE);
   if (result)
       return 1.0;
   else
      return 0.0;   
}


// Can I detect your radar?
float Esm_Detection(double Range, Submarine *observer, Submarine *target)
{
   int depth, range;
   bool result;

   depth = (int) observer->Depth;
   range = (int) observer->DistanceToTarget(target);
   result = EsmStation.isTargetVisible(target, range, depth,
                       SHIP_HEIGHT, TRUE, DEFAULT_SEA_STATE);
   if (result)
     return 1.0;
   else
     return 0.0;

}



/// Can the observer hear the target?
float Sonar_Detection_New(double Range, Submarine *observer, Submarine *target)
{

        float NauticalMiles = (float)Range / 2000.0;
        float HisPassiveSonarCrosssection = target->PSCS;
        float EffectiveTargetSpeed;
        float AmbientNoise;
        float OwnShipNoise;
        float TotalNoise;
        float TargetNoise;
        float Gb;
        float Lbp;
        float NoiseFromSpeed;
        float BasisNoiseLevel;
        float value;
        float SeaState = 3.0; // Anyone want to model the weather.
        int thermal_layers = 0;

        if (target->Speed <= 5.0){
             EffectiveTargetSpeed = 0.0;
        }else{
             EffectiveTargetSpeed = target->Speed - 5.0;
        }

        if (target->Speed < 20.0){
             NoiseFromSpeed = 1.30;
             BasisNoiseLevel = 0.0;
        }else{
             NoiseFromSpeed = 0.65;
             BasisNoiseLevel = 9.75;
        }
        if (my_map)
           thermal_layers = my_map->Thermals_Between(observer->Depth, target->Depth);
        #ifdef DEBUGMAP
        printf("There are %d thermal layers between us.\n", thermal_layers);
        #endif
        AmbientNoise = 89.0 + (5.0 * SeaState);
        OwnShipNoise = observer->RadiatedNoise();
        TotalNoise = 10.0 * log10(pow(10.0,OwnShipNoise/10.0) + pow(10.0,AmbientNoise/10.0));
        Gb = (TotalNoise - AmbientNoise) / 2.9;
        Lbp = AmbientNoise + Gb;
        TargetNoise = HisPassiveSonarCrosssection +
        ((NoiseFromSpeed * EffectiveTargetSpeed) + BasisNoiseLevel);
        #ifdef DEBUGMAP
        printf("Old target noise: %f\n", TargetNoise);
        #endif
        if (thermal_layers)
            TargetNoise -= TargetNoise * (thermal_layers * THERMAL_FILTER);
        #ifdef DEBUGMAP
        printf("New target noise: %f\n", TargetNoise);
        #endif
        value = TargetNoise - (20.0 * log10(NauticalMiles) + 1.1 * NauticalMiles) - Lbp;
        if (!observer) 
              SonarStation.flowandambientnoise = (Lbp - 34);
        if (value > -45.0){
                return (value - -45.0) + 1.0;
        }else{
                return 0.0;
        }
}





void SoundEnvironment(){
	///********************************************************************
	//This will gather sound information from all platforms and place
	//messages into each platforms sonar queue.
	//*********************************************************************

	int bearing;
	double Range;
        Submarine *target;
        int line = FALSE;

	//loop through each ship and let them listen
	//for the other ships...
        #ifdef DEBUG
        printf("Advancing sonar.\n");
        #endif
	Subs->AdvanceSonarHistory(); //advance sonar queue by 1/5th of a second
	TB16.AdvanceSonarHistory(); //advance sonar queue by 1/5th of a second
	// for (target=Subs->next; target; target = target->next){
        target = Subs->next;
        while (target)
        {
          #ifdef DEBUG
          printf("Checking if we can hear other ships.\n");
          #endif
		if (! player->InBaffles(target, 1, &TB16)){ //I'm not deaf?
			Range = CalculateRange(target, Subs);
			float signal;
			signal = Any_Detection(Range, Subs, target);
			if (signal){ // Are we audible?
                        #ifdef DEBUG
                        printf("Heard you, adding to list.\n");
                        #endif
				bearing = (int)Subs->BearingToTarget(target);  //Change me to float for better tma
                                // this weird check prevents us from
                                // erasing sonar data with radar/esm data
                                if ( (signal == 1.0) || (signal == 2.0) )
                                {
                                    float temp_signal;
                                    temp_signal = Sonar_Detection_New(Range, Subs, target);
				    Subs->RegisterEvent(bearing,temp_signal,target->ShipType);
                                }
                                else
                                   Subs->RegisterEvent(bearing, signal, target->ShipType);
                                // printf("Adding target %d to list.\n", target);
                                Subs->Add_Target(target, signal);
			}
                        else {
                          // printf("Lost target %d from list.\n", target);
                          Subs->Remove_Target(target);
                        }
		}
		else if (! player->InBaffles(target, 2, &TB16) && TB16.GetLength() >240){ // do the same for towed array
			Range = TB16.RangeToTarget(target->Lat_TotalYards, target->Lon_TotalYards);
			float signal;
			// signal = Sonar_Detection(Range, 0, target);
                        signal = Sonar_Detection_New(Range, Subs, target);
			if (signal){ // Are we audible?
				bearing = (int)TB16.BearingToTarget(target->Lat_TotalYards, target->Lon_TotalYards);  //Change me to float for better tma
				TB16.RegisterEvent(bearing,signal, target->ShipType);
                                // printf("Adding target %d to list.\n", target);
                                Subs->Add_Target(target, signal);
			}
                        else {
                          // printf("Lost target %d from list.\n", target);
                          Subs->Remove_Target(target);
                        }
		}
                else   // can't hear target with either sonar device
                  Subs->Remove_Target(target);

            target = target->next;
            if ( (!target) && (! line) )
            {
               target = torpedoes;
               line = TRUE;
            }
	}
}

/*********************************************
	This function will return if a target is in
	the observers Baffles and therefore not
	detectable. Values for 'int sensor' are 1 for
	spherical array, 2 for towed array, 3 for port
	hull array and 4 for starboard hull array.

	Might want to move the calculations of the
	baffle angles to the Coord class so they don't
	have to be calculated all the time.
*********************************************/
/*
Moving InBaffles to the Submarine class to keep things cleaner.
-- Jesse

int InBaffles(Submarine *observer, Submarine *target, int sensor){
	int array_heading;
	int relative_bearing;
	int sensordeaf=1;
	int bearing_to_target;

	switch(sensor){
		case 1:	//Spherical
			sensordeaf = 0;
			array_heading = (int)observer->Heading;
			bearing_to_target = (int)observer->BearingToTarget( target);
			if(array_heading > bearing_to_target) bearing_to_target += 360;
			relative_bearing = bearing_to_target - array_heading;
			if(relative_bearing > 150 && relative_bearing < 210) sensordeaf = 1;
			if(target == observer) sensordeaf = 1;
			break;
		case 2:	//Towed
			sensordeaf = 0;
			array_heading = (int)TB16.ReturnHeading();
			bearing_to_target = (int)TB16.BearingToTarget(target->Lat_TotalYards, target->Lon_TotalYards);
			if(array_heading > bearing_to_target) bearing_to_target += 360;
			relative_bearing = bearing_to_target - array_heading;
			if(relative_bearing < 30 || relative_bearing > 330) sensordeaf = 1;
			break;
		case 3: //port hull
		case 4: //sb hull
		default:
			break;
	}
	return sensordeaf;
}
*/


void DisplayTMA(int xoffset,int yoffset){
 	SDL_Rect source, destination;
	source.x = 0;
	source.y = 0;
	source.w = 500;
	source.h = 500;
	destination.x = 374;
	destination.y = 145;
	destination.w = 500;
	destination.h = 500;
	Tma.Lock();
	Tma.our_heading = (double)Subs->Heading;
	Tma.our_speed = (float)Subs->Speed;
	Tma.target_heading = (double)Subs->Heading;
	Tma.target_speed = (float)Subs->Speed;
	Tma.DisplayGeoPlot(xoffset,yoffset);
	//Tma.DisplayLOS();
	SDL_BlitSurface(Tma.GeoPlotScreen, &source, screen, &destination);
	SDL_UpdateRects(screen, 1, &destination);
	Tma.UnLock();
}

void DisplayWeapons()
{
    SDL_Rect weapons, tubes;
    DFont fnt("images/font.png", "data/font.dat");
    char text[256];
    int index, y1, y2;

    if (! update_weapons_screen)
        return;

    weapons.x = 150;
    weapons.y = 145;
    weapons.w = 200;
    weapons.h = 300;
    tubes.x = 374;
    tubes.y = 145;
    tubes.h = 500;
    tubes.w = 501;

    SDL_FillRect(screen, &weapons, black);
    sprintf(text, "WEAPONS");
    fnt.PutString(screen, 190, 150, text);
    sprintf(text, "     Torpedos: %d", Subs->TorpedosOnBoard);
    fnt.PutString(screen, 160, 174, text);
    sprintf(text, "Noise Makers: %d", Subs->NoiseMakers);
    fnt.PutString(screen, 160, 196, text);
    sprintf(text, "'T' to load torpedo");
    fnt.PutString(screen, 160, 300, text);
    sprintf(text, "'N' to load noise maker");
    fnt.PutString(screen, 160, 312, text);
    sprintf(text, "'U' to unload device");
    fnt.PutString(screen, 160, 324, text);
    sprintf(text, "'F' to fire");
    fnt.PutString(screen, 160, 336, text);
    SDL_UpdateRects(screen, 1, &weapons);

    SDL_FillRect(screen, &tubes, black);
    y1 = 150; y2 = 190;
    for (index = 0; index < MAX_TUBES; index++)
    {
        // draw tube
        DrawRectangle(screen, 390, y1, 500, y2, green);
        // draw buttons
        fnt.PutString(screen, 520, y1 + 5, "  Load");
        fnt.PutString(screen, 520, y1 + 18, "Torpedo");
        DrawRectangle(screen, 520, y1, 590, y2, green);
        fnt.PutString(screen, 600, y1 + 5, "   Load");
        fnt.PutString(screen, 600, y1 + 18, "Noise Maker");
        DrawRectangle(screen, 600, y1, 690, y2, green);
        fnt.PutString(screen, 700, y1 + 10, "Unload");
        DrawRectangle(screen, 700, y1, 760, y2, green);
        fnt.PutString(screen, 770, y1 + 10, "Fire!");
        DrawRectangle(screen, 770, y1, 815, y2, green);
        y1 += 50; y2 += 50;
    }
    SDL_UpdateRects(screen, 1, &tubes);

    // put stuff in the tubes
    y1 = 151;
    for (index = 0; index < MAX_TUBES; index++)
    {
       tubes.x = 391;
       tubes.y = y1;
       tubes.w = 105;
       tubes.h = 35;
       if (Subs->torpedo_tube[index] == TUBE_TORPEDO)
          SDL_BlitSurface(torpedo_image, NULL, screen, &tubes);
       else if (Subs->torpedo_tube[index] == TUBE_NOISEMAKER)
          SDL_BlitSurface(noisemaker_image, NULL, screen, &tubes);
       else
       {
         SDL_FillRect(screen, &tubes, black); 
         sprintf(text, "Tube %d", index + 1);
         fnt.PutString(screen, 420, y1 + 10, text);
       }
       SDL_UpdateRects(screen, 1, &tubes);
       y1 += 50;
    }
    update_weapons_screen = FALSE;
}



inline int RandInt(int TO){ //Returns a random interger...TO is upper limit
	return (rand()%TO);
}

inline int Clamp(int sample){ //holds a value to be between 0 and 255
	if (sample < 0){
		sample = 0;
	}
	if (sample > 255){
		sample = 255;
	}
	return sample;
}

inline double Clamp(double sample){ //Overloaded for floats
	if (sample < 0){
		sample = 0.0;
	}
	if (sample > 255){
		sample = 255.0;
	}
	return sample;
}

void ResetWidgetFlags(){
	//turns off all the widget redraw flags
	SonarStation.sonarwidget=false;
	navwidget=0;
	quitwidget=0;
	tmawidget=0;
	esmwidget=0;
	radarwidget=0;
	shipcontrolwidget=0;
}

void ShowStation(int station){

 // init everything to avoid overlap
 drawmap = drawweapons = drawsonar = drawradar = drawesm = drawcontrol = 0;
 navwidget = shipcontrolwidget = radarwidget = esmwidget = 0;
	switch (station){// which station are we at?
		case 1: //sonar screen
			LoadScreen(1); //load in the screen for the SONAR station
			ResetWidgetFlags(); //self explanitory
			SonarStation.sonarwidget = true; //depress the widget
			DisplayWidgets(); // display the widgets
			drawsonar = 1; //tell the sonar it's ok to draw itself
			break;
		case 2: //nav screen
			LoadScreen(2); //load in the screen for the NAV station
			ResetWidgetFlags(); //self explanitory
			navwidget = 1; //depress the widget
			DisplayWidgets(); // display the widgets
			drawmap = 1; //tell the Map it's ok to draw itself
			break;
		case 3: //tma screen
			LoadScreen(3); //load in the screen for the TMA station
			ResetWidgetFlags(); //self explanitory
			// tmawidget = 1; //depress the widget
			DisplayWidgets(); // display the widgets
			drawweapons = 1; //Turn on TMA screen updates
			break;
		case 4: //shipcontrol screen
			LoadScreen(4); //load in the screen for the shipcontrol station
			ResetWidgetFlags(); //self explanitory
			shipcontrolwidget = 1; //depress the widget
			DisplayWidgets(); // display the widgets
			drawcontrol = 1;
			ControlStation.InitGraphics(screen, controlscreen);
			break;
		case 5: //RADAR screen
			LoadScreen(5); //load in the screen for the RADAR station
			ResetWidgetFlags(); //self explanitory
			radarwidget = 1; //depress the widget
			DisplayWidgets(); // display the widgets
			drawradar = 1; //Turn on Radar screen updates
			RadarStation.InitGraphics(screen, radarscreen);
			break;
		case 6: //ESM screen
			LoadScreen(6); //load in the screen for the ESM station
			ResetWidgetFlags(); //self explanitory
			esmwidget = 1; //depress the widget
			DisplayWidgets(); // display the widgets
			drawesm = 1; //Turn on ESM screen updates
			EsmStation.InitGraphics(screen, esmscreen);
			break;
	}
}





/**************************************************
This is our callback function to handle
time critical Functions
**************************************************/
Uint32 timerfunc(Uint32 interval, void *param){

   should_update_everything = TRUE;
   return interval;
}

int Update_Everything()
{
        // param = NULL;
        #ifdef DEBUG
        printf("In timer function\n");
        #endif
        Clock.UpdateTime();
        ShipHandeling();
        if (player)
        {
           UpdateSensors();
           // see if the mission is over
           Check_Find(Subs);
           Check_Alive(Subs);
           my_mission_status = Mission_Status(Subs, my_mission_status);
        }  // player is dead, we should end the mission
        else
           my_mission_status = MISSION_FAILED;
        return TRUE;
}



/***************************************************
Another callback to handle the 60 second
resolution TMA stuff.
****************************************************/
Uint32 TmaTimer(Uint32 interval, void *param){
	static Uint32 tick = 0;
	Tma.Lock(); //Lock Tma access mutex
	param = NULL; //Quites error messages.
	tick ++; //record the time of the tma record.
	Tma.RecordBoatPosition(Subs->Lat_TotalYards, Subs->Lon_TotalYards, Subs->BearingToTarget(& (Subs[1])) , tick);
	Tma.UnLock(); //Unlock mutex
	return interval;
}

int HandleInput(SDL_Event &event, int &mousex, int &mousey){
	static string textline;
        int y1, y2, x_checks_out;
        int index;

	switch (event.type){
		case SDL_MOUSEBUTTONDOWN:
			mousex = event.button.x;
			mousey = event.button.y;
			if (mousey<740 && mousey>711){
				if (mousex <212 && mousex>180){   //SONAR widget
					return SONAR;
					break;
				}
				if (mousex <312 && mousex>280){  //Map Widget
					return NAVMAP;
					break;
				}
				if (mousex <412 && mousex>380){  //TMA Widget
				 	return WEAPONS;
					break;
				}
				if (mousex <512 && mousex>480){  //Ship Control Widget
					return SHIPCONTROL;
					break;
				}
				if (mousex <612 && mousex>580){  //ESM Widget
					return ESM;
					break;
				}
				if (mousex <712 && mousex>680){ //RADAR Widget
					return RADAR;
					break;
				}
				if (mousex <812 && mousex>780){ //Quit Widget
					return QUIT;
					break;
				}
				if (mousex < 908 && mousex > 884){ //Time Compression +
					return COMPRESSTIME;
					break;
				}
			
				if (mousex < 1000 && mousex > 972){  //Time Compression -
					return UNCOMPRESSTIME;
					break;
				}
			}
			if(drawmap || drawtma){
				if(mousex > 224 && mousex < 257){
					if(mousey >270 && mousey < 300){
						return CENTERDISPLAY;
						break;
					}
				}
				if(mousex > 224 && mousex < 257){
					if(mousey >220 && mousey < 245){
						return SCROLLMAPUP;
						break;
					}
				}
				if(mousex > 224 && mousex < 257){
					if(mousey >320 && mousey < 345){
						return SCROLLMAPDOWN;
						break;
					}
				}
				if(mousex > 175 && mousex < 210){
					if(mousey >270 && mousey < 300){
						return SCROLLMAPLEFT;
						break;
					}
				}
				if(mousex > 275 && mousex < 310){
					if(mousey >270 && mousey < 300){
						return SCROLLMAPRIGHT;
						break;
					}
				}
				if(mousex >175 && mousex < 210){
					if(mousey > 360 && mousey < 390){
						return INCREASEMAPSCALE;
						break;
					}
				}
				if(mousex >275 && mousex < 310){
					if(mousey > 360 && mousey < 390){
						return DECREASEMAPSCALE;
						break;
					}
				}
			}
			// Radar events
			if(drawradar){
				if(mousex > 105 && mousex < 512){
					if(mousey >156 && mousey < 566){
					RadarStation.ShowData(screen, mousex, mousey);
						break;
					}
				}

				if(mousex > 748 && mousex < 795){
					if(mousey >320 && mousey < 367){
						return TOGGLER10;
						break;
					}
				}
				
				if(mousex > 795 && mousex < 842){
					if(mousey >320 && mousey < 367){
						return TOGGLER20;
						break;
					}
				}
				
				if(mousex > 842 && mousex < 889){
					if(mousey >320 && mousey < 367){
						return TOGGLER30;
						break;
					}
				}

				if(mousex > 748 && mousex < 795){
					if(mousey >367 && mousey < 414){
						return TOGGLER40;
						break;
					}
				}
				
				if(mousex > 795 && mousex < 842){
					if(mousey >367 && mousey < 414){
						return TOGGLER50;
						break;
					}
				}

				if(mousex > 842 && mousex < 889){
					if(mousey >367 && mousey < 414){
						return TOGGLER60;
						break;
					}
				}

				if(mousex > 744 && mousex < 791){
					if(mousey >185 && mousey < 232){
						return RADARDOWN;
						break;
					}
				}

				if(mousex > 850 && mousex < 897){
					if(mousey >185 && mousey < 232){
						return RADARUP;
						break;
					}
				}

				if(mousex > 749 && mousex < 796){
					if(mousey >501 && mousey < 548){
						return RING0;
						break;
					}
				}
				
				if(mousex > 796 && mousex < 843){
					if(mousey >501 && mousey < 548){
					  return RING5;
					  break;
					}
				}

				if(mousex > 843 && mousex < 889){
				        if(mousey >501 && mousey < 548){
					  return RING10;
					  break;
					}
				}
				
				if(mousex > 548 && mousex < 796){
				        if(mousey >548 && mousey < 594){
					  return RING15;
					  break;
					}
				}

				if(mousex > 796 && mousex < 843){
				        if(mousey >548 && mousey < 594){
					  return RING20;
					  break;
					}
				}
				
				if(mousex > 843 && mousex < 889){
				        if(mousey >548 && mousey < 594){
					  return RING25;
					  break;
					}
				}
			}

                        // Weapon console events
                        if (drawweapons)
                        {
                            x_checks_out = TRUE;
                            y1 = 150; y2 = 190;
                            // check all x1-x2 possiblities
                            if ( (mousex > 520) && (mousex < 590) )
                               tube_action = LOAD_TORPEDO;
                            else if ( (mousex > 600) && (mousex < 680) )
                               tube_action = LOAD_NOISEMAKER;
                            else if ( (mousex > 700) && (mousex < 760) )
                               tube_action = UNLOAD_TUBE;
                            else if ( (mousex > 770) && (mousex < 815) )
                               tube_action = FIRE_TUBE;
                            else
                            {
                               tube_action = 0;
                               x_checks_out = FALSE;
                            }
                            // look at all y1-y2 button possibilities
                            if (x_checks_out)
                            {
                                index = 0;
                                tube_to_use = -1;
                                while ( (index < MAX_TUBES) && 
                                        (tube_to_use == -1) )
                                {
                                    if ((mousey > y1) && (mousey < y2) )
                                      tube_to_use = index;
                                    else
                                    {
                                        index++;
                                        y1 += 50; y2 += 50;
                                    }
                                }    // end of checking buttons vertically
                                if (tube_to_use == -1)
                                   tube_action = 0;
                            }
                            else
                              tube_action = 0;
                            if ( (tube_action) && (tube_to_use > -1) )
                              return USE_TUBE;
                                 

                        }   // end of weapons screen
			// ESM events
			if(drawesm){
				if(mousex > 744 && mousex < 791){
					if(mousey >185 && mousey < 232){
						return ESMDOWN;
						break;
					}
				}

				if(mousex > 850 && mousex < 897){
					if(mousey >185 && mousey < 232){
						return ESMUP;
						break;
					}
				}
			}

			// Control events
			if(drawcontrol){
			       if(mousex > 158 && mousex < 347){
					if(mousey >250 && mousey < 430){
					ControlStation.AdjustHeading(mousex, mousey);
						break;
					}
				}
				if(mousex > 112 && mousex < 126){
					if(mousey >254 && mousey < 293){
					  ControlStation.AdjustDepth(mousex);
						break;
					}
				}

				if(mousex > 385 && mousex < 401){
					if(mousey >257 && mousey < 296){
					  ControlStation.AdjustDepth(mousex);
						break;
					}
				}
				if(mousex > 364 && mousex < 404){
					if(mousey >403 && mousey < 418){
						return TURNSTARBOARD;
						break;
					}
				}

				if(mousex > 108 && mousex < 147){
					if(mousey >405 && mousey < 421){
						return TURNPORT;
						break;
					}
				}
				if(mousex > 232 && mousex < 252){
					if(mousey >581 && mousey < 607){
						return ASTOP;
						break;
					}
				}
				if(mousex > 263 && mousex < 287){
					if(mousey >581 && mousey < 607){
						return A13;
						break;
					}
				}
				if(mousex > 296 && mousex < 320){
				        if(mousey >581 && mousey < 607){
				                return A23;
				                break;
					}
				}
				if(mousex > 329 && mousex < 352){
					if(mousey >581 && mousey < 607){
						return ASTD;
						break;
					}
				}
				if(mousex > 364 && mousex < 384){
				        if(mousey >581 && mousey < 607){
				                return AFULL;
				                break;
					}
				}
				if(mousex > 397 && mousex < 416){
					if(mousey >581 && mousey < 607){
						return AFLK;
						break;
					}
				}
				if(mousex > 200 && mousex < 218){
					if(mousey >581 && mousey < 607){
						return B13;
						break;
					}
				}
				if(mousex > 166 && mousex < 185){
				        if(mousey >581 && mousey < 607){
				                return B23;
				                break;
					}
				}
				if(mousex > 134 && mousex < 152){
					if(mousey >581 && mousey < 607){
						return BSTD;
						break;
					}
				}
				if(mousex > 100 && mousex < 118){
					if(mousey >581 && mousey < 607){
						return BEMER;
						break;
					}
				}
				if(mousex > 717 && mousex < 757){
					if(mousey >585 && mousey < 601){
					  return EXTENDARRAY;
						break;
					}
				}
				if(mousex > 717 && mousex < 757){
					if(mousey >612 && mousey < 628){
					  return RETRACTARRAY;
						break;
					}
				}
			}

			if(drawsonar){
				if(mousex > 615 && mousex < 657){
					if(mousey > 162 && mousey < 210){
						return TOGGLETRUERELATIVE;
						break;
					}
				}
				if(mousex > 474 && mousex < 519){
					if(mousey > 358 && mousey < 401){
						return DESIGNATECONTACT;
						break;
					}
				}
				if(mousex > 615 && mousex < 657){
					if(mousey > 215 && mousey < 261){
						return TOGGLESPHERICALTOWED;
						break;
					}
				}
				if(mousex > 663 && mousex < 706){
					if(mousey > 162 && mousey < 210){
						return UPPERCRTBUTTON;
						break;
					}
				}
				if(mousex > 663 && mousex < 706){
					if(mousey > 215 && mousey < 261){
						return LOWERCRTBUTTON;
						break;
					}
				}
                                if (mousex > 472 && mousex < 520)
                                {
                                    if (mousey > 590 && mousey < 638)
                                       return SEND_PING;
                                }
                                if(mousex > 520 && mousex < 567)
                                {
                                    if (mousey > 590 && mousey < 638)
                                    {
                                       return CUTARRAY;
                                       break;
                                    }
                                }
				if(mousex >569  && mousex < 614){
					if(mousey > 593 && mousey < 638){
						return STOPWINCH;
						break;
					}
				}
				if(mousex > 617 && mousex < 660){
					if(mousey > 593 && mousey < 638){
						return EXTENDARRAY;
						break;
					}
				}
				if(mousex > 663 && mousex < 706){
					if(mousey > 593 && mousey < 638){
						return RETRACTARRAY;
						break;
					}
				}
				if(mousex > 476 && mousex < 517){
					if(mousey > 406 && mousey < 447){
						return ASSIGNTRACKER;
						break;
					}
				}
				if(mousex > 523 && mousex < 564){
					if(mousey > 406 && mousey < 447){
						return TRACKER1;
						break;
					}
				}
				if(mousex > 570 && mousex < 611){
					if(mousey > 406 && mousey < 447){
						return TRACKER2;
						break;
					}
				}
				if(mousex > 617 && mousex < 658){
					if(mousey > 406 && mousey < 447){
						return TRACKER3;
						break;
					}
				}
				if(mousex > 663 && mousex < 706){
					if(mousey > 406 && mousey < 447){
						return TRACKER4;
						break;
					}
				}
			}
			break;
		case SDL_QUIT:
			return QUIT;
			break;
		case SDL_KEYDOWN:  //Process key presses
			switch(event.key.keysym.sym){
				case SDLK_PAGEUP:  // PgUp   Make Einstein proud.
					return COMPRESSTIME;
					break;
				case SDLK_PAGEDOWN: // PgDown
					return UNCOMPRESSTIME;
					break;
				case SDLK_KP_PLUS:
				case SDLK_PLUS:  // Speed Up
					return INCREASESPEED;
					break;
				case SDLK_KP_MINUS:
				case SDLK_MINUS:  // Slow Down
					return DECREASESPEED;
					break;
				// case SDLK_n: //  Navigation
                                case SDLK_F2:
					return NAVMAP;
					break;
				// case SDLK_s: //  Sonar
                                case SDLK_F1:
					return SONAR;
					break;
				// case SDLK_t: //  TMA
                                case SDLK_F3:
					return WEAPONS;
				 	break;
				case SDLK_ESCAPE: // Quit...
					return QUIT;
					break;
                                case SDLK_TAB:  // switch target
                                        return SWITCHTARGET;
                                        break;
				case SDLK_F10: //  Dump the screen
					return DUMPSCREEN;
					break;
                                case SDLK_F4:
                                        return SHIPCONTROL;
                                        break;
                                case SDLK_F5:
                                        return ESM;
                                        break;
                                case SDLK_F6:
                                        return RADAR;
                                        break;
				case SDLK_KP4: // Turn Left
					return TURNPORT;
					break;
				case SDLK_KP6: // Turn Right
					return TURNSTARBOARD;
					break;
				case SDLK_KP2: // Dive
					return INCREASEDEPTH;
					break;
				case SDLK_KP8: //Surface
					return DECREASEDEPTH;
					break;
				case SDLK_INSERT:
					return INCREASEMAPSCALE;
					break;
				case SDLK_DELETE: 
					return DECREASEMAPSCALE;
					break;
				case SDLK_UP :
					return SCROLLMAPUP;
					break;
 				case SDLK_DOWN:
					return SCROLLMAPDOWN;
					break;
				case SDLK_LEFT:
					return SCROLLMAPLEFT;
					break;
				case SDLK_RIGHT:
					return SCROLLMAPRIGHT;
					break;
				case SDLK_c:
					return CENTERDISPLAY;
					break;
				case SDLK_e:
					return EXTENDARRAY;
					break;
				case SDLK_r:
					return RETRACTARRAY;
					break;
				case SDLK_w:
					return STOPWINCH;
					break;
				// case 19: // Pause The Game
				case SDLK_p: //  Pause The Game
					return PAUSEGAME;
					break;
                                case SDLK_t:
                                         tube_action = LOAD_TORPEDO;
                                         tube_to_use = -1;
                                        return WHICHTUBE;
                                case SDLK_n:
                                         tube_action = LOAD_NOISEMAKER;
                                         tube_to_use = -1;
                                         return WHICHTUBE;
                                case SDLK_u:
                                         tube_action = UNLOAD_TUBE;
                                         tube_to_use = -1;
                                         return WHICHTUBE;
                                case SDLK_f:
                                         tube_action = FIRE_TUBE;
                                         tube_to_use = -1;
                                         return WHICHTUBE;
                                case SDLK_1:
                                         tube_to_use = 0;
                                         return USE_TUBE;
                                case SDLK_2:
                                         tube_to_use = 1;
                                         return USE_TUBE;
                                case SDLK_3:
                                         tube_to_use = 2;
                                         return USE_TUBE;
                                case SDLK_4:
                                         tube_to_use = 3;
                                         return USE_TUBE;
                                case SDLK_5:
                                         tube_to_use = 4;
                                         return USE_TUBE;
                                case SDLK_6:
                                         tube_to_use = 5;
                                         return USE_TUBE;
                                case SDLK_g:
                                        return SEND_PING;
                                case SDLK_q:
                                        return UP_THERMAL;
                                case SDLK_a:
                                        return HOLD_DEPTH;
                                case SDLK_z:
                                        return DOWN_THERMAL;
                                case SDLK_s:
                                        return GO_SURFACE;
                                case SDLK_x:
                                        return GO_PERISCOPE_DEPTH;
				default:	
					return 0;
					break;
				}
		default:
			return 0;
			break;
	}
	return 0;
}

int main(int argc, char **argv){
	static char text[120];
        int status;
	int hours=0;
	int minutes=0;
	int seconds=0;
	int option_choice;//a place to put command line switches
	int screendumpcount = 0;
	int mousex = 0, mousey = 0; //where is the mouse?
	bool quit = false;  //Quit flag Duh!
	int station; //flag to decide which work station to display
	bool full_screen = false;
	char *textline = "";
	char file1[] = "images/font.png";
	char file2[] = "data/font.dat";
	char file3[] = "images/largefont.png";
	char file4[] = "data/largefont.dat";
        int mission_number = 0;
        int enable_sound = FALSE;
	SDL_Event event; //a typedef to hold events
	drawsonar = 0; // draw the sonar flag
	drawmap = 1; // draw the map flag
	drawradar = 0;
	drawesm = 0;
	drawcontrol = 0;
	northcenter = true; //make the sonar display N. centered
	Uint32 timer1; // timer2; // our event timers....
	SDL_TimerID timer_id, timer_id2;
        torpedoes = NULL;
        helicopters = NULL;
	tmamutex = SDL_CreateMutex();
	srand(time(NULL)); //Seed the random generator

	//Process commandline options.
        /*
	sprintf(text,"m:vwfkhs");
	while ((option_choice = getopt(argc, argv, text)) != -1){
        */
        status = 1;
        while (status < argc)
        {
             if (argv[status][0] == '-')
                option_choice = argv[status][1];
             else
                option_choice = '\0';
		switch (option_choice)
                {
                        case 'm':
                               // mission_number = atoi(optarg);
                               mission_number = atoi(argv[status + 1]);
                               status++;
                               break;
			case 'w': //they passed the '-w' flag.
				full_screen = false;
				break;
                        case 'f': // full screen
                                full_screen = true;
                                break;
/*
			case 'k': //This option is just for me cause when using kdevelop
							//I don't start in the right directory.
				chdir("./subsim");
				break;
*/
                        case 'v':
                                cout << "OpenTorpedo version " << VERSION << endl;
                                return 0;
			case 'h':
				cout << "Usage:" << endl
                                << "-m <mission> select specific mission." << endl
                                << "-f For full screen mode." << endl
                                << "-s Enable sound effects." << endl
				<< "-w For Windowed Mode." << endl
                                << "-v For version." << endl
				<< "-h For this message." << endl;
				return 0;
				break;
                        case 's':
                                enable_sound = TRUE;
                                break;
			default:
				cout << "Unknown command-line argument" << endl
				<< "Please use -h for a list of commands." << endl;
				return 1;
		}   // end of switch
               status++;
	}    // end of while
	SetupScreen(full_screen);
        Init_Audio(enable_sound);
	// CreateShips(mission_number);
	Tma.InitGraphics();
	SonarStation.InitGraphics();
//	msg Message;
	Message.InitGraphics();
	Clock.InitTime(12,15,0);
	SonarStation.LoadWidgets();
	SDL_EnableKeyRepeat(150,100);
	LoadWidgets();
	LoadScreen(0); //Display intro screen
	DFont fnt(file1, file2);
	static DFont fnt2(file3,file4);
        my_map = new MAP();
        #ifdef DEBUGMAP
        my_map->Test_Map();
        #endif
	SDL_UpdateRect(screen, 0, 0, 0, 0);
	timer1 = SDL_GetTicks();// initialize the timer
        SDL_Delay(1000);   // show splash screen for one second
	quit = false; //reset loop exit flag
	//LoadWidgets(); //load up the buttons
        LoadScreen(9);
        // main menu stuff goes here
        status = Main_Menu(&mission_number, screen);
        if (status == ACTION_QUIT)
          quit = true;
        
        CreateShips(mission_number, my_map);
	SDL_Rect rectangle;
	rectangle.x = 0;
	rectangle.y = 0;
	rectangle.w = 1024; // This block of code clears the screen
	rectangle.h = 768;
	SDL_FillRect(screen, &rectangle, black);
	timecompression = 1;
	station = 2; //default station
	ShowStation(station);
	// textline="OpenTorpedo VERSION 0.5";
        sprintf(text, "OpenTorpedo version %2.1f", VERSION);
	Message.post_message(text);
	textline="https://github.com/OpenTorpedo/OpenTorpedo.git";
	Message.post_message(textline);
	Message.display_message();
	sprintf(text, "[%i] ", timecompression);
	fnt.PutString(screen, 933, 718, text);
        my_mission_status = MISSION_STARTED;
	timer_id = SDL_AddTimer(1000, timerfunc, NULL);
	timer_id2 = SDL_AddTimer(60000, TmaTimer, NULL);
	timer1 = SDL_GetTicks();
        // timer2 = SDL_GetTicks();
	const int frameperiod = 100;	//how fast we want the displays to update (in milliseconds)... this allows for a fixed frame rate
	while (!quit)
        { 	//This is the main loop.....
		if (timer1 + (frameperiod) < SDL_GetTicks()){
			timer1 = SDL_GetTicks();
			RadarStation.Sweep(frameperiod*timecompression);
			UpdateDisplay();
			
			SDL_Rect rectangle;
			rectangle.x=16;
			rectangle.y=14;   //define a rectangle on the screen and make it black
			rectangle.h=72;
			rectangle.w=126;
			SDL_FillRect(screen, &rectangle, black);
                        sprintf(text, "Now - Wanted");
                        fnt.PutString(screen, 30, 20, text);
			sprintf(text, "S: [%3i]  -   [%3i]", (int)Subs->Speed, Subs->DesiredSpeed);
			fnt.PutString(screen, 30, 31, text);
			sprintf(text, "H: [%3i]  -   [%3i]", (int)Subs->Heading, Subs->DesiredHeading);
			fnt.PutString(screen, 30, 42, text);
			sprintf(text, "D: [%4i]  -  [%4i]", (int)Subs->Depth, Subs->DesiredDepth);
			fnt.PutString(screen, 30, 53, text);
			// sprintf(text, "ARRAY [%4i]", TB16.GetLength());
			// fnt.PutString(screen, 40, 53, text);
			Clock.GetTime(hours,minutes,seconds);
			sprintf(text, "%.2i:%.2i:%.2i", hours, minutes, seconds);
			fnt.PutString(screen, 40, 64, text);
			SDL_UpdateRects(screen, 1, &rectangle);			
			}

		while (SDL_PollEvent(&event)){
		//If there are events waiting take care of them
			switch (HandleInput(event, mousex, mousey)){
				case SONAR:
						ShowStation(1);
						UpdateDisplay();
                                                Message.post_message("Sonar station");
						Message.display_message();
						sprintf(text, "[%i] ", timecompression);
						fnt.PutString(screen, 933, 718, text);
					break;
				case NAVMAP:
					ShowStation(2);
					UpdateDisplay();
                                        Message.post_message("Naviation display");
					Message.display_message();
					sprintf(text, "[%i] ", timecompression);
					fnt.PutString(screen, 933, 718, text);
					break;
				case WEAPONS:
                                        update_weapons_screen = TRUE;
					ShowStation(3);
					UpdateDisplay();
                                        Message.post_message("Weapons console");
					Message.display_message();
					sprintf(text, "[%i] ", timecompression);
					fnt.PutString(screen, 933, 718, text);
					break;
                                case WHICHTUBE:
                                        update_weapons_screen = TRUE;
                                        UpdateDisplay();
                                        Message.post_message("Which tube (1-6)?");
                                        Message.display_message();
                                        break;
				case SHIPCONTROL:
					ShowStation(4);
					UpdateDisplay();
                                        Message.post_message("Helm control");
					Message.display_message();
					sprintf(text, "[%i] ", timecompression);
					fnt.PutString(screen, 933, 718, text);
					break;
				case ESM:
					ShowStation(6);
					UpdateDisplay();
                                        Message.post_message("ESM station");
					Message.display_message();
					sprintf(text, "[%i] ", timecompression);
					fnt.PutString(screen, 933, 718, text);
					break;
				case RADAR:
					ShowStation(5);
					UpdateDisplay();
                                        Message.post_message("Radar console");
					Message.display_message();
					sprintf(text, "[%i] ", timecompression);
					fnt.PutString(screen, 933, 718, text);
					break;
                                case SWITCHTARGET:
	                                current_target = Subs->Next_Target();
                                        if (drawmap)
                                        {
                                          DrawMap();
                                          // PlaceShips(mapscale, 0, -10, current_target);
                                        }
                                        SDL_Delay(100);
                                        break;
				case QUIT:
                                        printf("Got quit signal.\n");
					ResetWidgetFlags();
					quitwidget=1;
					DisplayWidgets();
					SDL_Delay(200);
					quitwidget=0;
					DisplayWidgets();
					SDL_Delay(500);
					quit = true;
					break;
				case COMPRESSTIME:  //  Make Einstein proud.
					destination_rectangle.x = 880; //upper left corner to
					destination_rectangle.y = 710; //place the button.
					destination_rectangle.h = plusbuttondown->h; //height &
					destination_rectangle.w = plusbuttondown->w;//width of button.
					SDL_BlitSurface(plusbuttondown, NULL, screen, &destination_rectangle); // Do the Blit.
					SDL_UpdateRects(screen, 1, &destination_rectangle); //Show button on the screen...
					timecompression++;
					if(timecompression > 8){
						timecompression = 8;
					}else{
						if (!pause_game){
							SDL_RemoveTimer(timer_id);
							SDL_RemoveTimer(timer_id2);
							timer_id = SDL_AddTimer(1000/timecompression, timerfunc, NULL);
							timer_id2 = SDL_AddTimer(60000/timecompression, TmaTimer, NULL);
						}
						sprintf(text, "[%i] ", timecompression);
						fnt.PutString(screen, 933, 718, text);
					}
					SDL_Delay(150);
					DisplayWidgets();
					break;
				case UNCOMPRESSTIME: // Take A Downer
					destination_rectangle.x = 970; //upper left corner to
					destination_rectangle.y = 710; //place the button.
					destination_rectangle.h = minusbuttondown->h; //height &
					destination_rectangle.w = minusbuttondown->w;//width of button.
					SDL_BlitSurface(minusbuttondown, NULL, screen, &destination_rectangle); // Do the Blit.
					SDL_UpdateRects(screen, 1, &destination_rectangle); //Show button on the screen...
					timecompression--;
					if(timecompression < 1){
						timecompression = 1;
					}else{
						if (!pause_game){
							SDL_RemoveTimer(timer_id);
							SDL_RemoveTimer(timer_id2);
							timer_id = SDL_AddTimer(1000/timecompression, timerfunc, NULL);
							timer_id2 = SDL_AddTimer(60000/timecompression, TmaTimer, NULL);
						}
						sprintf(text, "[%i] ", timecompression);
						fnt.PutString(screen, 933, 718, text);
						}
						SDL_Delay(150);
						DisplayWidgets();
						break;
				case INCREASESPEED:
					Subs->DesiredSpeed++;
					break;
				case DECREASESPEED:
					Subs->DesiredSpeed--;
					break;
				case DUMPSCREEN:
					screendumpcount++;
					sprintf(text, "screendump%i.bmp", screendumpcount );
					SDL_SaveBMP(screen,text); //screen dumps
					cerr << "Screen Dump" << endl;
					break;
				case TURNPORT:
					Subs->DesiredHeading--;
					if(Subs->DesiredHeading < 0){
						Subs->DesiredHeading += 360;
					}
					break;
				case TURNSTARBOARD:
					Subs->DesiredHeading++;
					if(Subs->DesiredHeading > 359){
						Subs->DesiredHeading -= 360;
					}
					break;
				case INCREASEDEPTH:
					Subs->DesiredDepth++;
                                        if (Subs->DesiredDepth > Subs->MaxDepth)
                                           Subs->DesiredDepth = Subs->MaxDepth;
					break;
				case DECREASEDEPTH:
					Subs->DesiredDepth--;
					if(Subs->DesiredDepth < 0){
						Subs->DesiredDepth = 0;
					}
					break;
				case INCREASEMAPSCALE:
					if (drawmap){
						mapscale++;
						if(mapscale>MAX_MAP_SCALE) mapscale=MAX_MAP_SCALE;
					 	destination_rectangle.x = 175; //upper left corner to
						destination_rectangle.y = 359; //place the button.
						destination_rectangle.h = plusbuttondown->h; //height &
						destination_rectangle.w = plusbuttondown->w;//width of button.
						SDL_BlitSurface(plusbuttondown, NULL, screen, &destination_rectangle); // Do the Blit.
						SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen....
						DrawMap();
						//UpdateDisplay();
						SDL_Delay(100);
						DisplayNavigationWidgets();
					}
					if(drawtma){
					 	destination_rectangle.x = 175; //upper left corner to
						destination_rectangle.y = 359; //place the button.
						destination_rectangle.h = plusbuttondown->h; //height &
						destination_rectangle.w = plusbuttondown->w;//width of button.
						SDL_BlitSurface(plusbuttondown, NULL, screen, &destination_rectangle); // Do the Blit.
						SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen....
						Tma.IncreasePlotScale();
						DisplayTMA();
						SDL_Delay(100);
						DisplayTMAWidgets();
					}
					break;
				case DECREASEMAPSCALE:
					if(drawmap){
						mapscale--;
						if(mapscale<1) mapscale = 1;
					 	destination_rectangle.x = 275; //upper left corner to
						destination_rectangle.y = 359; //place the button.
						destination_rectangle.h = minusbuttondown->h; //height &
						destination_rectangle.w = minusbuttondown->w;//width of button.
						SDL_BlitSurface(minusbuttondown, NULL, screen, &destination_rectangle); // Do the Blit.
						SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
						DrawMap();
						//UpdateDisplay();
						SDL_Delay(100);
						DisplayNavigationWidgets();
					}
					if(drawtma){
						Tma.DecreasePlotScale();
					 	destination_rectangle.x = 275; //upper left corner to
						destination_rectangle.y = 359; //place the button.
						destination_rectangle.h = minusbuttondown->h; //height &
						destination_rectangle.w = minusbuttondown->w;//width of button.
						SDL_BlitSurface(minusbuttondown, NULL, screen, &destination_rectangle); // Do the Blit.
						SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
						DisplayTMA();
						SDL_Delay(100);
						DisplayTMAWidgets();
					}
					break;
				case SCROLLMAPUP:
					if(drawmap){
					 	destination_rectangle.x = 225; //upper left corner to
						destination_rectangle.y = 219; //place the button.
						destination_rectangle.h = upbuttondown->h; //height &
						destination_rectangle.w = upbuttondown->w;//width of button.
						SDL_BlitSurface(upbuttondown, NULL, screen, &destination_rectangle); // Do the Blit.
						SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
						PlaceShips(mapscale, 0, -10, current_target);
						DrawMap();
						SDL_Delay(100);
						DisplayNavigationWidgets();
					}
					if(drawtma){
					 	destination_rectangle.x = 225; //upper left corner to
						destination_rectangle.y = 219; //place the button.
						destination_rectangle.h = upbuttondown->h; //height &
						destination_rectangle.w = upbuttondown->w;//width of button.
						SDL_BlitSurface(upbuttondown, NULL, screen, &destination_rectangle); // Do the Blit.
						SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
						PlaceShips(mapscale, 0, -10, current_target);
						DisplayTMA(0,-10);
						SDL_Delay(100);
						DisplayTMAWidgets();
					}
					break;
				case SCROLLMAPDOWN:
					if(drawmap){
					 	destination_rectangle.x = 225; //upper left corner to
						destination_rectangle.y = 319; //place the button.
						destination_rectangle.h = downbuttondown->h; //height &
						destination_rectangle.w = downbuttondown->w;//width of button.
						SDL_BlitSurface(downbuttondown, NULL, screen, &destination_rectangle); // Do the Blit
						SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen..
						PlaceShips(mapscale, 0, 10, current_target);
						DrawMap();
						SDL_Delay(100);
						DisplayNavigationWidgets();
					}
					if(drawtma){
					 	destination_rectangle.x = 225; //upper left corner to
						destination_rectangle.y = 319; //place the button.
						destination_rectangle.h = downbuttondown->h; //height &
						destination_rectangle.w = downbuttondown->w;//width of button.
						SDL_BlitSurface(downbuttondown, NULL, screen, &destination_rectangle); // Do the Blit
						SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen..
						PlaceShips(mapscale, 0, 10, current_target);
						DisplayTMA(0,10);
						SDL_Delay(100);
						DisplayTMAWidgets();
					}
					break;
				case SCROLLMAPLEFT:
					if(drawmap){
				 		destination_rectangle.x = 175; //upper left corner to
						destination_rectangle.y = 269; //place the button.
						destination_rectangle.h = leftbuttondown->h; //height &
						destination_rectangle.w = leftbuttondown->w;//width of button.
						SDL_BlitSurface(leftbuttondown, NULL, screen, &destination_rectangle); // Do the Blit.
						SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen....
						 PlaceShips(mapscale, -10, 0, current_target);
						DrawMap();
						SDL_Delay(100);
						DisplayNavigationWidgets();
					}
					if(drawtma){
				 		destination_rectangle.x = 175; //upper left corner to
						destination_rectangle.y = 269; //place the button.
						destination_rectangle.h = leftbuttondown->h; //height &
						destination_rectangle.w = leftbuttondown->w;//width of button.
						SDL_BlitSurface(leftbuttondown, NULL, screen, &destination_rectangle); // Do the Blit.
						SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen....
						 PlaceShips(mapscale, -10, 0, current_target);
						DisplayTMA(-10,0);
						SDL_Delay(100);
						DisplayTMAWidgets();
					}
					break;
				case SCROLLMAPRIGHT:
					if(drawmap){
				 		destination_rectangle.x = 275; //upper left corner to
						destination_rectangle.y = 269; //place the button.
						destination_rectangle.h = rightbuttondown->h; //height &
						destination_rectangle.w = rightbuttondown->w;//width of button.
						SDL_BlitSurface(rightbuttondown, NULL, screen, &destination_rectangle); // Do the Blit.
						SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
						 PlaceShips(mapscale, 10, 0, current_target);
						DrawMap();
						SDL_Delay(100);
						DisplayNavigationWidgets();
					}
					if(drawtma){
				 		destination_rectangle.x = 275; //upper left corner to
						destination_rectangle.y = 269; //place the button.
						destination_rectangle.h = rightbuttondown->h; //height &
						destination_rectangle.w = rightbuttondown->w;//width of button.
						SDL_BlitSurface(rightbuttondown, NULL, screen, &destination_rectangle); // Do the Blit.
						SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
						DisplayTMA(10,0);
						SDL_Delay(100);
						DisplayTMAWidgets();
					}
					break;
                                case UP_THERMAL:
                                        if ( (player) && (my_map) )
                                        {
                                           status = my_map->Next_Up(player->Depth);
                                           if (status != player->Depth)
                                           {
                                               player->DesiredDepth = status - 25;
                                               Message.post_message("Going up one thermal.");
                                               Message.display_message();
                                           }
                                        }
                                        break;
                                case DOWN_THERMAL:
                                        if ( (player) && (my_map) )
                                        {
                                            status = my_map->Next_Down(player->Depth);
                                            if (status != player->Depth)
                                            {
                                               player->DesiredDepth = status + 25;
                                               Message.post_message("Going down one thermal.");
                                               Message.display_message();
                                            }
                                        }
                                        break;
                                case HOLD_DEPTH:
                                        if (player)
                                          player->DesiredDepth = player->Depth;
                                        Message.post_message("Holding depth, Captain.");
                                        Message.display_message();
                                        break;
                                 case GO_SURFACE:
                                        if (player)
                                           player->DesiredDepth = 0;
                                        Message.post_message("Surfacing, Captain!");
                                        Message.display_message();
                                        break;
                                 case GO_PERISCOPE_DEPTH:
                                        if (player)
                                           player->DesiredDepth = PERISCOPE_DEPTH;
                                        Message.post_message("Heading to periscope depth.");
                                        Message.display_message();
                                        break;

				case TOGGLESPHERICALTOWED:
					SonarStation.ToggleArrayChoice();
					SonarStation.DisplaySonarWidgets();
					break;
				case TOGGLETRUERELATIVE:
					SonarStation.ToggleTrueRel();
					SonarStation.DisplaySonarWidgets();
					break;
				case UPPERCRTBUTTON:
					SonarStation.UpperCRT_Button();
					break;
				case LOWERCRTBUTTON:
					SonarStation.LowerCRT_Button();
					break;
				case CENTERDISPLAY:
					if(drawmap){
						if(mapcenter) {
							mapcenter = 0;
							DisplayNavigationWidgets();
						}else{
							mapcenter = 1;
							DisplayNavigationWidgets();
						}
						UpdateDisplay();
					}
					if(drawsonar){
						SonarStation.ToggleNorthCenter();
						SonarStation.ClearSonarData();
						UpdateDisplay();
						Message.display_message();
					}
					if(drawtma){
						Tma.ToggleGeoPlotCenter();
						DisplayTMAWidgets();
						UpdateDisplay();
						Message.display_message();
					}
					break;
				case EXTENDARRAY:
					status = TB16.Extend();
                                        if (status)
                                        {
					  SonarStation.DisplaySonarWidgets();
                                          Message.post_message("Extending sonar array.");
                                        }
                                        else
                                           Message.post_message("Unable to extend array.");
                                        Message.display_message();
					break;
				case RETRACTARRAY:
					status = TB16.ReelIn();
                                        if (status)
                                        {
					  SonarStation.DisplaySonarWidgets();
                                          Message.post_message("Retrieving sonar array.");
                                        }
                                        else
                                          Message.post_message("Unable to retrieve array.");
                                        Message.display_message();
					break;
                                case CUTARRAY:
                                        status = TB16.CutArray();
                                        SonarStation.DisplaySonarWidgets();
                                        if (status)
                                           Message.post_message("Cut towed array.");
                                        else
                                           Message.post_message("Cannot cut array.");
                                        Message.display_message();
                                        break;
				case STOPWINCH:
					SonarStation.StopWinch();
                                        Message.post_message("Stopping sonar array.");
                                        Message.display_message();
					break;
				case ASSIGNTRACKER:
					SonarStation.ToggleAssignTracker();
					SonarStation.DisplaySonarWidgets();
					break;
				case TRACKER1:
					if(SonarStation.assigntracker){
						SonarStation.ToggleAssignTracker();
						Tma.AssignTracker(0,0);
						SonarStation.DisplaySonarWidgets();
					}
					break;
				case TRACKER2:
					if(SonarStation.assigntracker){
						SonarStation.ToggleAssignTracker();
						Tma.AssignTracker(1,0);
						SonarStation.DisplaySonarWidgets();
					}
					break;
				case TRACKER3:
					if(SonarStation.assigntracker){
						SonarStation.ToggleAssignTracker();
						Tma.AssignTracker(2,0);
						SonarStation.DisplaySonarWidgets();
					}
					break;
				case TRACKER4:
					if(SonarStation.assigntracker){
						SonarStation.ToggleAssignTracker();
						Tma.AssignTracker(3,0);
						SonarStation.DisplaySonarWidgets();
					}
					break;
				case DESIGNATECONTACT:
					// textline="CON, SONAR, NEW CONTACT BEARING 123. DESIGNATEING CONTACT S1.";
					// Message.post_message(textline);
					break;
				case TOGGLER10:
					RadarStation.ToggleRangeScale10();
					RadarStation.DisplayWidgets();
					break;
				case TOGGLER20:
					RadarStation.ToggleRangeScale20();
					RadarStation.DisplayWidgets();
					break;
				case TOGGLER30:
					RadarStation.ToggleRangeScale30();
					RadarStation.DisplayWidgets();
					break;
				case TOGGLER40:
					RadarStation.ToggleRangeScale40();
					RadarStation.DisplayWidgets();
					break;
				case TOGGLER50:
					RadarStation.ToggleRangeScale50();
					RadarStation.DisplayWidgets();
					break;
				case TOGGLER60:
					RadarStation.ToggleRangeScale60();
					RadarStation.DisplayWidgets();
					break;
				case RING0:
					RadarStation.ToggleRangeRing0();
					RadarStation.DisplayWidgets();
					break;
				case RING5:
					RadarStation.ToggleRangeRing5();
					RadarStation.DisplayWidgets();
					break;
			        case RING10:
					RadarStation.ToggleRangeRing10();
					RadarStation.DisplayWidgets();
					break;
			        case RING15:
					RadarStation.ToggleRangeRing15();
					RadarStation.DisplayWidgets();
					break;
			        case RING20:
					RadarStation.ToggleRangeRing20();
					RadarStation.DisplayWidgets();
					break;
			     	case RING25:
					RadarStation.ToggleRangeRing25();
					RadarStation.DisplayWidgets();
					break;
				case RADARDOWN:
					RadarStation.LowerMast();
					RadarStation.DisplayWidgets();
					RadarStation.ClearScreen();
                                        if (player)
                                            player->using_radar = FALSE;
					break;
				case RADARUP:
                                        if (Subs->Depth <= PERISCOPE_DEPTH)
                                        {
					  RadarStation.RaiseMast();
					  RadarStation.DisplayWidgets();
                                          if (player)
                                             player->using_radar = TRUE;
                                        }
					break;
				case ESMDOWN:
					EsmStation.LowerMast();
					EsmStation.DisplayWidgets();
					EsmStation.ClearScreen();
					break;
				case ESMUP:
                                        if (Subs->Depth <= PERISCOPE_DEPTH)
                                        {
					  EsmStation.RaiseMast();
					  EsmStation.DisplayWidgets();
                                        }
					break;
			        case ASTOP:
					ControlStation.ToggleASTOP();
					ControlStation.DisplayWidgets();
					break;
			        case A13:
					ControlStation.ToggleA13();
					ControlStation.DisplayWidgets();
					break;
			        case A23:
					ControlStation.ToggleA23();
					ControlStation.DisplayWidgets();
					break;
			        case ASTD:
					ControlStation.ToggleASTD();
					ControlStation.DisplayWidgets();
					break;
				case AFULL:
					ControlStation.ToggleAFULL();
					ControlStation.DisplayWidgets();
					break;
			        case AFLK:
					ControlStation.ToggleAFLK();
					ControlStation.DisplayWidgets();
					break;
			        case B13:
					ControlStation.ToggleB13();
					ControlStation.DisplayWidgets();
					break;
				case B23:
					ControlStation.ToggleB23();
					ControlStation.DisplayWidgets();
					break;
			        case BSTD:
					ControlStation.ToggleBSTD();
					ControlStation.DisplayWidgets();
					break;
				case BEMER:
					ControlStation.ToggleBEMER();
					ControlStation.DisplayWidgets();
					break;
                                case SEND_PING:
                                        player->Send_Ping(Subs);
                                        Message.post_message("Sending active ping, sir.");
                                        Message.display_message();
                                        break;
                                case USE_TUBE:
                                        // check to see if we already have too
                                        // many items out there
                                        status = player->Count_Torpedoes(torpedoes);
                                        status += player->Count_Noisemakers(torpedoes);
                                        if (status >= MAX_PLAYER_WEAPONS)
                                        {
                                            Message.post_message("Tracking computer full, Captain.");
                                            Message.display_message();
                                            break;
                                        }
                                        // load, fire or unload a tube
                                        status = Subs->Use_Tube(tube_action, tube_to_use);
                                        if (status == TUBE_ERROR_FIRE_NOISEMAKER)
                                        {
                                            Submarine *new_torpedo, *my_torp;
                                            char *ship_file, filename[] = "ships/class6.shp";
                                            ship_file = Find_Data_File(filename);
                                            new_torpedo = Subs->Fire_Tube(NULL, ship_file);
                                            if ( (ship_file) && (ship_file != filename) )
                                                free(ship_file);
                                            if (new_torpedo)
                                            {
                                                new_torpedo->Friend = player->Friend;
                                                new_torpedo->owner = player;
                                                torpedoes = Add_Ship(torpedoes, new_torpedo);
                                                Message.post_message("Noise maker launched!");
                                                Message.display_message();
                                                my_torp = torpedoes;
                                                while (my_torp)
                                                {
                                                     if (my_torp->target == player)
                                                        my_torp->Is_Distracted_By_Noisemaker(new_torpedo);
                                                     my_torp = my_torp->next;
                                                }
                                            }
                                        }
                                        else if ( (status == TUBE_ERROR_FIRE_SUCCESS) && (current_target) )
                                        {
                                           char *ship_file, filename[] = "ships/class5.shp";
                                           ship_file = Find_Data_File(filename);
                                           if (current_target)
                                           {
                                              Submarine *new_torpedo;
                                              new_torpedo = Subs->Fire_Tube(current_target, ship_file );
                                              if ( (ship_file) && (ship_file != filename) )
                                                free(ship_file);
                                              if (new_torpedo)
                                              {
                                                  char *sound_file, *file_name = "sounds/torpedo-launch.ogg";
                                                  sound_file = Find_Data_File(file_name);
                                                  Play_Sound(sound_file);
                                                  if ( (sound_file) && (sound_file != file_name) )
                                                      free(sound_file);
                                                  new_torpedo->Friend = FRIEND;
                                                  new_torpedo->owner = Subs;
                                                  torpedoes = Add_Ship(torpedoes, new_torpedo);
                                                  Message.post_message("Torpedo launched!");
                                                  Message.display_message();
                                              }
                                           }
                                          }
                                          else if (status == TUBE_ERROR_FIRE_SUCCESS)
                                           {
                                               Message.post_message("Torpedo has no target.");
                                               Message.display_message();
                                               Subs->TorpedosOnBoard++;
                                           }
                                        

                                        tube_action = 0;
                                        tube_to_use = -1;
                                        update_weapons_screen = TRUE;
                                        UpdateDisplay();
                                        break;
				case PAUSEGAME:
					if (pause_game){ //UnPause
						pause_game = !pause_game;
						timer_id = SDL_AddTimer(1000/timecompression, timerfunc, NULL);
						timer_id2 = SDL_AddTimer(60000/timecompression, TmaTimer, NULL);
					}else{  //Pause
						pause_game = !pause_game;
						SDL_RemoveTimer(timer_id);
						SDL_RemoveTimer(timer_id2);
					}
					break;
				default:
					break;
				}
			}
                /*
                if (timer2 < (SDL_GetTicks() - (1000/timecompression) ) )
                {
                   #ifdef DEBUG
                   printf("Launching timer function.\n");
                   #endif
                   // timerfunc(0, NULL);
                   timer2 = SDL_GetTicks();
                }
                */
                if (should_update_everything)
                {
                     should_update_everything = FALSE;
                     Update_Everything();
                }
                if (my_mission_status == MISSION_SUCCESS)
                {
                   printf("Mission completed successfully!\n");
                   LoadScreen(7);
                   SDL_Delay(5000);
                   quit = true;
                }
                else if (my_mission_status == MISSION_FAILED)
                {
                   printf("Mission failed.\n");
                   LoadScreen(8);
                   SDL_Delay(5000);
                   quit = true;
                }

                SDL_Delay(GAME_DELAY);
		}   // end of main loop
        #ifdef DEBUG
        printf("Unloading widgets.\n");
        #endif
	UnLoadWidgets();
        // get rid of torpedoes
        #ifdef DEBUG
        printf("Destroying torpedoes\n");
        #endif
        while (torpedoes)
            torpedoes = Remove_Ship(torpedoes, torpedoes);
        #ifdef DEBUG
        printf("Destroying subs\n");
        #endif
        while (Subs)
            Subs = Remove_Ship(Subs, Subs);
        #ifdef DEBUG
        printf("Removing helicopters.\n");
        #endif
        while (helicopters)
          helicopters = Remove_Helicopter(helicopters, helicopters);
        #ifdef DEBUG
        printf("Killing SDL\n");
        #endif
        Clean_Up_Audio();
        if (my_map)
            delete my_map;
	SDL_Quit();
        return 0;   // just to make the compiler happy
}

