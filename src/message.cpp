/***************************************************************************
                          msg.cpp  -  description
                             -------------------
    begin                : Monday Jan 13 2003
    copyright            : (C) 2003 by Michael Bridak
    email                : michael.bridak@verizon.net
$Id: message.cpp,v 1.7 2003/09/21 21:52:40 mbridak Exp $
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
#include "files.h"
#include "dfont.h"
#include "message.h"

//using namespace std;

msg::msg(){
	//clear out garbage from buffer. 
	for (int x = 0; x < 20; x++){
		buffer[x][0]= '\0';
	}

}

msg::~msg(){
	//Our default Destructor...
}
void msg::InitGraphics(){

	screen = SDL_GetVideoSurface();
	SDL_Surface *temp = SDL_CreateRGBSurface(SDL_SWSURFACE, 800, 72, 16,
			screen->format->Rmask,
			screen->format->Gmask,
			screen->format->Bmask,
			screen->format->Amask);
	messagewindow=SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);
	textcolor = SDL_MapRGB(screen->format, 192, 255, 6);
	black = SDL_MapRGB(screen->format, 0, 0, 0);
	white = SDL_MapRGB(screen->format, 255, 255, 255);
	red = SDL_MapRGB(screen->format, 255, 99, 20);
	green = SDL_MapRGB(screen->format, 0, 255, 0);
	yellow = SDL_MapRGB(screen->format, 216, 255, 0);

}

void msg::post_message(char *temp_string){
	for (int x = 1; x < 20; x++){
		// buffer[x-1]=buffer[x];
             strcpy(buffer[x-1], buffer[x]);
	}
	strcpy(buffer[19], temp_string);
	//display_message();
}
void msg::display_message(){
	char filename1[] = "images/font.png";
	char filename2[] = "data/font.dat";
	static DFont font(filename1, filename2);
	//Clearout window contents
	SDL_Rect rectangle;
	SDL_Rect destination_rectangle;
	SDL_Rect source_rectangle;
	rectangle.x=0;
	rectangle.y=0;  
	rectangle.h=72;   
	rectangle.w=800;
	SDL_FillRect(messagewindow, &rectangle, black);

	//print bottom 4 lines of buffer[]
	for (int x = 16; x < 20; x++){
		font.PutString(messagewindow, 5,(15*(x-15))-6, buffer[x]);
	}
	
	//paste surface into screen
	source_rectangle.x = 0;
	source_rectangle.y = 0;
	source_rectangle.h = 72;
	source_rectangle.w = 800;

	destination_rectangle.x = 175;
	destination_rectangle.y = 15;
	destination_rectangle.h = 72;
	destination_rectangle.w = 800;
	SDL_BlitSurface(messagewindow, &source_rectangle, screen, &destination_rectangle);
	SDL_UpdateRects(screen, 1, &destination_rectangle);
}
