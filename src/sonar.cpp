/***************************************************************************
                          sonar.cpp  -  description
                             -------------------
    begin                : Fri Apr 11 2002
    copyright            : (C) 2002 by Michael Bridak
    email                : michael.bridak@verizon.net
$Id: sonar.cpp,v 1.14 2003/09/21 21:52:40 mbridak Exp $
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
#include "message.h"
#include "submarine.h"
#include "towedarray.h"
#include "targetmotionanalysis.h"
#include "sonar.h"

using namespace std;

AnBqq5::AnBqq5(Submarine *temp, TowedArray &temp2, TargetMotionAnalysis &temp3,
msg &temp4): Subs(temp), TB16(temp2), Tma(temp3), Message(temp4)
{
	northcenter = true;
	arraychoice5by6 = true;
	bearingdisplay5by6 = true;
	sonarwidget = false;
	cursorBearing = 0;
}

AnBqq5::~AnBqq5(){}

void AnBqq5::InitGraphics()
{
	screen = SDL_GetVideoSurface();
	SDL_Surface *temp = SDL_CreateRGBSurface(SDL_SWSURFACE, 368, 280, 16,
					screen->format->Rmask,
					screen->format->Gmask,
					screen->format->Bmask,
					screen->format->Amask);
	sonarscreen=SDL_DisplayFormat(temp);
	towedarrayscreen=SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);
	uppersonarcrt=sonarscreen;
	lowersonarcrt=towedarrayscreen;
	if ( screen == NULL || sonarscreen == NULL || towedarrayscreen == NULL) {
		cerr << "OOPS" << SDL_GetError() << endl;
		exit(1);
	}
	string filename;
	string filename2;
	black = SDL_MapRGB(screen->format, 0, 0, 0);
	white = SDL_MapRGB(screen->format, 255, 255, 255);
	green = SDL_MapRGB(screen->format, 0,255,0);
        // medium_green = SDL_MapRGB(screen->format, 0, 160, 0);
        // dark_green = SDL_MapRGB(screen->format, 0, 100, 0);
}

void AnBqq5::ClearSonarData(){  //when the display is switched from north centered
	//to south centered, we must erase the old display data
			SDL_Rect rectangle;
			rectangle.x=0;
			rectangle.y=0;
			rectangle.h=200;
			rectangle.w=368;
			SDL_FillRect(sonarscreen, &rectangle, black);
			SDL_FillRect(towedarrayscreen, &rectangle, black);
}

inline void AnBqq5::DPixel(SDL_Surface *screen, int x, int y, Uint32 color)
{
	//this only works for 16bpp screens
	//place the pixel on the screen
	Uint16 *pixel_location;
	pixel_location = (Uint16 *)screen->pixels + y*screen->pitch/2 + x;
	*pixel_location = color;
}


inline void AnBqq5::DLine(SDL_Surface *screen, int X1, int Y1, int X2, int Y2, Uint32 Color)
{

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

void AnBqq5::Sonar(bool center)

/********************************************
	This routine checks the sonar event queue
	and if there is an event, plots the sound
	source on the waterfall display. It's called
	every 1/5 of a second. and in turn calls
	AdvanceSonarScreen() every second.
*********************************************/

{
	Uint32 tracecolor; //We have to put the color somewhere
	int direction;
	float signal;
        int ship_type;
        double combined_sound;

	if (Subs->GetCount()>0){
	//Are there any sound events at this time index to worry about?
		for (int event=1; event<=Subs->GetCount(); event++){
		//If so step through the events.
			Subs->GetEvent(event, &direction, &signal, &ship_type);
			//TargetId not used now but maybe later in TMA

			//visable mono color from 70 - 255
                        combined_sound = (signal * 4) + flowandambientnoise;
                        if (combined_sound > 255.0)
                            combined_sound = 255.0;
                        switch(ship_type)
                        {
                          case TYPE_SHIP:
			tracecolor=SDL_MapRGB(sonarscreen->format, 0,(int) combined_sound, 0); break;
                          case TYPE_TORPEDO:
                          tracecolor=SDL_MapRGB(sonarscreen->format,(int) combined_sound, 0, 0); break;
                          case TYPE_SUB:
                          default: 
                          tracecolor=SDL_MapRGB(sonarscreen->format, 0, 0, (int) combined_sound); break;
                         }
			//Change brightness based on strength of signal
			int tdirection = direction;
			for(int xx = 1;xx < 6; xx++){
				direction = (-3+RandInt(6))+tdirection;
				//throw some variances in the bearing nothing is perfekt!...
				if (direction < 0) direction += 360;    //Christ I hate roll overs in degrees
				if (direction > 359) direction -= 360;

				if (!center){ //Do we want the sonar display to be North centered ?
					DPixel(sonarscreen, direction, 0, tracecolor); //plots on 0-60 sec display
					DPixel(sonarscreen, direction, 70, tracecolor); //plots on 0-30 min display
					DPixel(sonarscreen, direction, 140, tracecolor); //plots on 0-2 hr display
				}
				else { //draw traces for south centered display
					DPixel(sonarscreen, ReciprocalBearing(direction),
					0, tracecolor); //plots on 0-60 sec display
					DPixel(sonarscreen, ReciprocalBearing(direction),
					70, tracecolor); //plots on 0-30 min display
 					DPixel(sonarscreen, ReciprocalBearing(direction),
					140, tracecolor); //plots on 0-2 hr display
				}
			}
		}
	}
	if (!center){
		DPixel(sonarscreen, (int)Subs->Heading, 0, white); //plot our heading
	}else{
		DPixel(sonarscreen, ReciprocalBearing((int)Subs->Heading),
		0, white); //plots our heading
	}
	AdvanceSonarDisplay(); //advance the screen.
	UpdateCursor();
}



void AnBqq5::DisplaySonar(){
	SDL_Rect destination_rectangle;
	SDL_Rect source_rectangle;
	DisplayBearingScale(northcenter);
	DisplayCursor();
	
	if(uppersonarcrt != NULL){
		source_rectangle.x = 0;
		source_rectangle.y = 0;
		source_rectangle.w = 360;
		source_rectangle.h = 200;
		destination_rectangle.x = 52;
		destination_rectangle.y = 150;
		destination_rectangle.w = 360;
		destination_rectangle.h = 200;
		SDL_BlitSurface(uppersonarcrt, &source_rectangle, screen, &destination_rectangle);
		SDL_UpdateRects(screen, 1, &destination_rectangle);
	}


	if(lowersonarcrt != NULL){
		source_rectangle.x = 0;
		source_rectangle.y = 0;
		source_rectangle.w = 360;
		source_rectangle.h = 200;
		destination_rectangle.x = 52;
		destination_rectangle.y = 450;
		destination_rectangle.w = 360;
		destination_rectangle.h = 200;
		SDL_BlitSurface(lowersonarcrt, &source_rectangle, screen, &destination_rectangle);
		SDL_UpdateRects(screen, 1, &destination_rectangle);
	}

}

void AnBqq5::UpdateDisplay(Submarine *current_target){
	SDL_Rect destination_rectangle;
	static char text[120];
	char filename[] = "images/largefont.png";
	char filename2[] = "data/largefont.dat";
	static DFont largeFont(filename, filename2);
	DisplaySonar(); //draw sonar screen
	destination_rectangle.x=830;
	destination_rectangle.y=410; //define a rectangle on the screen and make it black
	destination_rectangle.h=30;
	destination_rectangle.w=90;
	SDL_FillRect(screen, &destination_rectangle, black);
	SDL_UpdateRects(screen, 1, &destination_rectangle);
	destination_rectangle.y=498; //define a rectangle on the screen and make it black
	SDL_FillRect(screen, &destination_rectangle, black);
	SDL_UpdateRects(screen, 1, &destination_rectangle);
        if (current_target)
        {
	  tempint = (int)Subs->BearingToTarget( current_target );
	  sprintf(text, "%4i", tempint);
	  largeFont.PutString(screen, 840, 412, text);
	  Subs->DEAngle( current_target );
	  if (deAngle > 0){
		sprintf(text,"+%3.1f",deAngle);
  	  }else{
		sprintf(text,"%3.1f",deAngle);
	  }
	  largeFont.PutString(screen, 840, 498, text);
        }
}

void AnBqq5::TowedSonar(bool center)

/********************************************
	This routine checks the sonar event queue
	and if there is an event, plots the sound
	source on the waterfall display. It's called
	every 1/5 of a second. and in turn calls
	AdvanceSonarScreen() every second.
*********************************************/

{
	Uint32 tracecolor; //We have to put the color somewhere
	int direction, ambiguous_direction, bearing, ambiguous_relative_bearing;
        int ship_type;
	float signal;
        double combined_sound;
	int TB16_Count = TB16.GetCount();
	if (TB16_Count > 0){
	//Are there any sound events at this time index to worry about?
		for (int event=1; event<=TB16_Count; event++){
		//If so step through the events.
			TB16.GetEvent(event, &bearing, &signal, &ship_type);
			ambiguous_relative_bearing = (int)TB16.BearingAmbiguity((float)bearing);
			//TargetId not used now but maybe later in TMA
			//visable mono color from 70 - 255
                        combined_sound = (signal * 4) + flowandambientnoise;
                        if (combined_sound > 255.0)
                           combined_sound = 255.0;
                        switch(ship_type)
                        {
                          case TYPE_SHIP:
                        tracecolor=SDL_MapRGB(sonarscreen->format, 0,(int) combined_sound, 0); break;
                          case TYPE_TORPEDO:
                          tracecolor=SDL_MapRGB(sonarscreen->format,(int) combined_sound, 0, 0); break;
                          case TYPE_SUB:
                          default:
                          tracecolor=SDL_MapRGB(sonarscreen->format, 0, 0, (int) combined_sound); break;
                         }

			//Change brightness based on strength of signal
			for(int xx = 1;xx < 6; xx++){
				direction = bearing;
				//if (direction >= 360) direction -= 360;
				direction = (-3+RandInt(6))+direction;
				//throw some variances in the bearing nothing is perfekt!...
				if (direction < 0) direction += 360;   
				if (direction > 359) direction -= 360; 
				ambiguous_direction = ambiguous_relative_bearing;
				//if (ambiguous_direction >= 360) ambiguous_direction -= 360;
				ambiguous_direction = (-3+RandInt(6))+ambiguous_direction;
				if (ambiguous_direction < 0) ambiguous_direction += 360;   
				if (ambiguous_direction > 359) ambiguous_direction -= 360; 
				if (!center){ //Do we want the sonar display to be North centered ?
					DPixel(towedarrayscreen, direction, 0, tracecolor); //plots on 0-60 sec display
					DPixel(towedarrayscreen, direction, 70, tracecolor); //plots on 0-30 min display
					DPixel(towedarrayscreen, direction, 140, tracecolor); //plots on 0-2 hr display
					DPixel(towedarrayscreen, ambiguous_direction, 0, tracecolor); //Do the same for the
					DPixel(towedarrayscreen, ambiguous_direction, 70, tracecolor); //ambiguous bearing
					DPixel(towedarrayscreen, ambiguous_direction, 140, tracecolor); //returned from towed array
				}else { //draw traces for south centered display
					DPixel(towedarrayscreen, ReciprocalBearing(direction),
					0, tracecolor); //plots on 0-60 sec display
					DPixel(towedarrayscreen, ReciprocalBearing(direction),
					70, tracecolor); //plots on 0-30 min display
 					DPixel(towedarrayscreen, ReciprocalBearing(direction),
					140, tracecolor); //plots on 0-2 hr display

					DPixel(towedarrayscreen, ReciprocalBearing(ambiguous_direction),
					0, tracecolor); //plots on 0-60 sec display
					DPixel(towedarrayscreen, ReciprocalBearing(ambiguous_direction),
					70, tracecolor); //plots on 0-30 min display
 					DPixel(towedarrayscreen, ReciprocalBearing(ambiguous_direction),
					140, tracecolor); //plots on 0-2 hr display
				}
			}
		}
	}
	if (!center){
		DPixel(towedarrayscreen, (int)Subs->Heading, 0, white); //plot our heading
	}else{
		DPixel(towedarrayscreen, ReciprocalBearing((int)Subs->Heading),
		0, white); //plots our heading
	}
	AdvanceTB16Screen(); //advance the screen.
}

void AnBqq5::AdvanceSonarDisplay(){
	SDL_Rect destination_rectangle;
	SDL_Rect source_rectangle;
	static int count = 0;
	static int count2 = 0;
        int my_noise;
	Uint32 noisecolor;
	// move the minute display down
	source_rectangle.x = 0; //sonar screen minus last line
	source_rectangle.y = 0;
	source_rectangle.w = 360;
	source_rectangle.h = 59;
	destination_rectangle.x = 0;
	destination_rectangle.y = 1;
	destination_rectangle.w = 360;
	destination_rectangle.h = 59;
	//cut sonarscreen and paste it back one line lower
	SDL_BlitSurface(sonarscreen, &source_rectangle, sonarscreen, &destination_rectangle);
	//Fill first line with background noise
	for(int nbp = 0; nbp<360; nbp++){
		bool sensordeaf = false;
		int array_heading;
		if(northcenter){
			array_heading = (int)Subs->Heading;
		}else{
			array_heading = ReciprocalBearing((int)Subs->Heading);
		}
		int recipbearing;
		if (array_heading >= 180){
		 	recipbearing = array_heading - 180;
		}else{
			recipbearing = array_heading + 180;
		}
		int bearing_to_target = nbp;
		if(recipbearing > bearing_to_target) bearing_to_target += 360;
		int relative_bearing = bearing_to_target - recipbearing;
		if(relative_bearing > 150 && relative_bearing < 210) sensordeaf = true;
		if (!sensordeaf){	
			// noisecolor=SDL_MapRGB(sonarscreen->format, 0,(int) fabs((flowandambientnoise - RandInt(40))) , 0);
                        my_noise = (int) fabs( (flowandambientnoise - RandInt(40) ) ) / 2;
                        noisecolor=SDL_MapRGB(sonarscreen->format, my_noise, my_noise, my_noise);
			DPixel(sonarscreen, nbp, 0, noisecolor);
		}
		else{
			DPixel(sonarscreen, nbp, 0, black);
		}
	}

// move the 0-30 min display down
	++count;
	if(count>29){
		count = 0;
		source_rectangle.x = 0; //sonar screen minus last line
		source_rectangle.y = 70;
		source_rectangle.w = 360;
		source_rectangle.h = 59;
		destination_rectangle.x = 0; 
		destination_rectangle.y = 71;
		destination_rectangle.w = 360;
		destination_rectangle.h = 59;
		//cut sonarscreen and paste it to buffer
		SDL_BlitSurface(sonarscreen, &source_rectangle, sonarscreen, &destination_rectangle);
		for(int nbp = 0; nbp<360; nbp++){
			bool sensordeaf = false;
			int array_heading;
			if(northcenter){
				array_heading = (int)Subs->Heading;
			}else{
				array_heading = ReciprocalBearing((int)Subs->Heading);
			}
			int recipbearing;
			if (array_heading >= 180){
			 	recipbearing = array_heading - 180;
			}else{
				recipbearing = array_heading + 180;
			}
			int bearing_to_target = nbp;
			if(recipbearing > bearing_to_target) bearing_to_target += 360;
			int relative_bearing = bearing_to_target - recipbearing;
			if(relative_bearing > 150 && relative_bearing < 210) sensordeaf = true;		
			if (!sensordeaf){	
				// noisecolor=SDL_MapRGB(sonarscreen->format, 0,(int) fabs((flowandambientnoise - RandInt(40))) , 0);
                        my_noise = (int) fabs( (flowandambientnoise - RandInt(40) ) ) / 2;
                        noisecolor = SDL_MapRGB(sonarscreen->format, my_noise, my_noise, my_noise);
				DPixel(sonarscreen, nbp, 70, noisecolor);
			}
			else{
				DPixel(sonarscreen, nbp, 70, black);
			}
		}
	}
// move the 0-2 hour display down
	++count2;
	if(count2 > 119){
		count2 = 0;
		source_rectangle.x = 0; //sonar screen minus last line
		source_rectangle.y = 140;
		source_rectangle.w = 360;
		source_rectangle.h = 59;
		destination_rectangle.x = 0;
		destination_rectangle.y = 141;
		destination_rectangle.w = 360;
		destination_rectangle.h = 59;
		//cut sonarscreen and paste it to buffer
		SDL_BlitSurface(sonarscreen, &source_rectangle, sonarscreen, &destination_rectangle);
		for(int nbp = 0; nbp<360; nbp++){
			bool sensordeaf = false;
			int array_heading;
			if(northcenter){
				array_heading = (int)Subs->Heading;
			}else{
				array_heading = ReciprocalBearing((int)Subs->Heading);
			}
			int recipbearing;
			if (array_heading >= 180){
			 	recipbearing = array_heading - 180;
			}else{
				recipbearing = array_heading + 180;
			}
			int bearing_to_target = nbp;
			if(recipbearing > bearing_to_target) bearing_to_target += 360;
			int relative_bearing = bearing_to_target - recipbearing;
			if(relative_bearing > 150 && relative_bearing < 210) sensordeaf = true;			
			if (!sensordeaf){	
				// noisecolor=SDL_MapRGB(sonarscreen->format, 0,(int) fabs((flowandambientnoise - RandInt(40))) , 0);

                        my_noise = (int) fabs( (flowandambientnoise - RandInt(40) ) ) / 2;
                        noisecolor = SDL_MapRGB(sonarscreen->format, my_noise, my_noise, my_noise);
				DPixel(sonarscreen, nbp, 140, noisecolor);
			}
			else{
				DPixel(sonarscreen, nbp, 140, black);
			}
		}
	}
}


void AnBqq5::AdvanceTB16Screen()

/******************************************
	This routine advances the waterfall display
	on the TB16 towed array screen.
*******************************************/

{

	static int count = 0;
	static int count2 = 0;
	Uint32 noisecolor;
        int my_noise;

// move the minute display down

	SDL_Rect source, destination;
	source.x = 0; //sonar screen minus last line
	source.y = 0;
	source.w = 360;
	source.h = 59;

	destination.x = 0;
	destination.y = 1;
	destination.w = 360;
	destination.h = 59;

	//cut towedarrayscreen and paste it to buffer
	SDL_BlitSurface(towedarrayscreen, &source, towedarrayscreen, &destination);



	//erase portion of screen
	DLine(towedarrayscreen, 0, 0, 360, 0, black);
	if (TB16.GetLength() > 240){
		for(int nbp = 0; nbp<360; nbp++){
		
			bool sensordeaf = false;
			int array_heading;
			if(northcenter){
				array_heading = (int)TB16.ReturnHeading();
			}else{
				array_heading = ReciprocalBearing((int)TB16.ReturnHeading());
			}
			int bearing_to_target = nbp;
			if(array_heading > bearing_to_target) bearing_to_target += 360;
			int relative_bearing = bearing_to_target - array_heading;
			if(relative_bearing > 150 && relative_bearing < 210) sensordeaf = true;
			if(!sensordeaf){
				// noisecolor=SDL_MapRGB(towedarrayscreen->format, 0,(int) fabs((flowandambientnoise - RandInt(40))), 0);
                        my_noise = (int) fabs( (flowandambientnoise - RandInt(40) ) ) / 2;
                        noisecolor = SDL_MapRGB(towedarrayscreen->format, my_noise, my_noise, my_noise);
				DPixel(towedarrayscreen, nbp, 0, noisecolor);
			}
			else{
				DPixel(towedarrayscreen, nbp, 0, black);
			}
		}
	}


// move the 0-30 min display down
	++count;
	if(count>29){
		count = 0;

		source.x = 0; //sonar screen minus last line
		source.y = 70;
		source.w = 360;
		source.h = 59;

		destination.x = 0;
		destination.y = 71;
		destination.w = 360;
		destination.h = 59;

		//cut towedarrayscreen and paste it to buffer
		SDL_BlitSurface(towedarrayscreen, &source, towedarrayscreen, &destination);
		if (TB16.GetLength() > 240){
			for(int nbp = 0; nbp<360; nbp++){
			
				bool sensordeaf = false;
				int array_heading;
				if(northcenter){
					array_heading = (int)TB16.ReturnHeading();
				}else{
					array_heading = ReciprocalBearing((int)TB16.ReturnHeading());
				}
				int bearing_to_target = nbp;
				if(array_heading > bearing_to_target) bearing_to_target += 360;
				int relative_bearing = bearing_to_target - array_heading;
				if(relative_bearing > 150 && relative_bearing < 210) sensordeaf = true;
				if(!sensordeaf){
					// noisecolor=SDL_MapRGB(towedarrayscreen->format, 0,(int) fabs((flowandambientnoise - RandInt(40))), 0);
                        my_noise = (int) fabs( (flowandambientnoise - RandInt(40) ) ) / 2;
                        noisecolor = SDL_MapRGB(towedarrayscreen->format, my_noise, my_noise, my_noise);
					DPixel(towedarrayscreen, nbp, 70, noisecolor);
				}
				else{
					DPixel(towedarrayscreen, nbp, 70, black);
				}
			}
		}
	}
// move the 0-2 hour display down
	++count2;
	if(count2 > 119){
		count2 = 0;

		source.x = 0; //sonar screen minus last line
		source.y = 140;
		source.w = 360;
		source.h = 59;

		destination.x = 0; // temp buffer
		destination.y = 141;
		destination.w = 360;
		destination.h = 59;

		//cut towedarrayscreen and paste it to buffer
		SDL_BlitSurface(towedarrayscreen, &source, towedarrayscreen, &destination);
		if (TB16.GetLength() > 240){
			for(int nbp = 0; nbp<360; nbp++){
				bool sensordeaf = false;
				int array_heading;
				if(northcenter){
					array_heading = (int)TB16.ReturnHeading();
				}else{
					array_heading = ReciprocalBearing((int)TB16.ReturnHeading());
				}
				int bearing_to_target = nbp;
				if(array_heading > bearing_to_target) bearing_to_target += 360;
				int relative_bearing = bearing_to_target - array_heading;
				if(relative_bearing > 150 && relative_bearing < 210) sensordeaf = true;
				if(!sensordeaf){
					// noisecolor=SDL_MapRGB(towedarrayscreen->format, 0,(int) fabs((flowandambientnoise - RandInt(40))), 0);
                        my_noise = (int) fabs( (flowandambientnoise - RandInt(40) ) ) / 2;
                        noisecolor = SDL_MapRGB(towedarrayscreen->format, my_noise, my_noise, my_noise);
					DPixel(towedarrayscreen, nbp, 140, noisecolor);
				}
				else{
					DPixel(towedarrayscreen, nbp, 140, black);
				}
			}
		}
	}
}


void AnBqq5::DisplayBearingScale(bool center){
	SDL_Rect destination_rectangle;
	if(center){
		destination_rectangle.x = 50; //upper left corner to
		destination_rectangle.y = 130; //place the scale
		destination_rectangle.h = ncscale->h; //height &
		destination_rectangle.w = ncscale->w;//width of button.
		SDL_BlitSurface(ncscale, NULL, screen, &destination_rectangle); // Do the Blit.
		SDL_UpdateRects(screen, 1, &destination_rectangle);
		destination_rectangle.x = 50; //upper left corner to
		destination_rectangle.y = 430; //place the scale
		SDL_BlitSurface(ncscale, NULL, screen, &destination_rectangle); // Do the Blit.
		SDL_UpdateRects(screen, 1, &destination_rectangle);
	}else{
		destination_rectangle.x = 50; //upper left corner to
		destination_rectangle.y = 130; //place the scale
		destination_rectangle.h = scscale->h; //height &
		destination_rectangle.w = scscale->w;//width of button.
		SDL_BlitSurface(scscale, NULL, screen, &destination_rectangle); // Do the Blit.
		SDL_UpdateRects(screen, 1, &destination_rectangle);
		destination_rectangle.x = 50; //upper left corner to
		destination_rectangle.y = 430; //place the scale
		SDL_BlitSurface(scscale, NULL, screen, &destination_rectangle); // Do the Blit.
		SDL_UpdateRects(screen, 1, &destination_rectangle);
	}
}

void AnBqq5::LoadWidgets(){
	//cerr << "LoadWidgets()" << endl;
	SDL_Surface *temp;

	temp = Load_Image("images/nc_scale.png");
	if(temp != NULL) ncscale = SDL_DisplayFormat(temp);
	if ( ncscale == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/sc_scale.png");
	if(temp != NULL) scscale = SDL_DisplayFormat(temp);
	if ( scscale == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);
	temp = Load_Image("images/sonarup.png");
	if(temp != NULL) sonarbuttonup = SDL_DisplayFormat(temp);
	if ( sonarbuttonup == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);
	temp = Load_Image("images/sonardown.png");
	if(temp != NULL) sonarbuttondown = SDL_DisplayFormat(temp);
	if ( sonarbuttondown == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);
	temp = Load_Image("images/rel_true.png");
	if(temp != NULL) truerel[0] = SDL_DisplayFormat(temp);
	if ( truerel[0] == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/true_rel.png");
	if(temp != NULL) truerel[1] = SDL_DisplayFormat(temp);
	if ( truerel[1] == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/sphericaltowed.png");
	if(temp != NULL) sphertowed[1] = SDL_DisplayFormat(temp);
	if ( sphertowed[1] == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/towedspherical.png");
	if(temp != NULL) sphertowed[0] = SDL_DisplayFormat(temp);
	if ( sphertowed[0] == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/uppercrtoff.png");
	if(temp != NULL) uppercrtoff = SDL_DisplayFormat(temp);
	if ( uppercrtoff == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/uppercrton.png");
	if(temp != NULL) uppercrton = SDL_DisplayFormat(temp);
	if ( uppercrton == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/lowercrtoff.png");
	if(temp != NULL) lowercrtoff = SDL_DisplayFormat(temp);
	if ( lowercrtoff == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/lowercrton.png");
	if(temp != NULL) lowercrton = SDL_DisplayFormat(temp);
	if ( lowercrton == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/tb16winchon.png");
	if(temp != NULL) tb16winchon = SDL_DisplayFormat(temp);
	if ( tb16winchon == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/tb16winchoff.png");
	if(temp != NULL) tb16winchoff = SDL_DisplayFormat(temp);
	if ( tb16winchoff == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/extendtb16on.png");
	if(temp != NULL) extendtb16[1] = SDL_DisplayFormat(temp);
	if ( extendtb16[1] == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/extendtb16off.png");
	if(temp != NULL) extendtb16[0] = SDL_DisplayFormat(temp);
	if ( extendtb16[0] == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/retracttb16on.png");
	if(temp != NULL) retracttb16[1] = SDL_DisplayFormat(temp);
	if ( retracttb16[1] == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/retracttb16off.png");
	if(temp != NULL) retracttb16[0] = SDL_DisplayFormat(temp);
	if ( retracttb16[0] == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

        temp = Load_Image("images/cutarray.png");
        if (temp != NULL)
            cutarray = SDL_DisplayFormat(temp);
        if (! cutarray)
        {
           cerr << "Error loading cutarray.png in LoadWidgets()" << endl
                << SDL_GetError() << endl;
                SDL_Quit();
                exit(0);
        }

        temp = Load_Image("images/ping_button.png");
        if (temp != NULL)
            sendping = SDL_DisplayFormat(temp);
        if (! sendping)
        {
           cerr << "Error loading ping_button.png in LoadWidgets()" << endl
                << SDL_GetError() << endl;
                SDL_Quit();
                exit(0);
        }

	temp = Load_Image("images/assigntrackeroff.png");
	if(temp != NULL) assigntrackerwidget[0] = SDL_DisplayFormat(temp);
	if ( assigntrackerwidget[0] == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/assigntrackeron.png");
	if(temp != NULL) assigntrackerwidget[1] = SDL_DisplayFormat(temp);
	if ( assigntrackerwidget[1] == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/track1.png");
	if(temp != NULL) tracker1[0] = SDL_DisplayFormat(temp);
	if ( tracker1[0] == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/track1assigned.png");
	if(temp != NULL) tracker1[1] = SDL_DisplayFormat(temp);
	if ( tracker1[1] == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/track2.png");
	if(temp != NULL) tracker2[0] = SDL_DisplayFormat(temp);
	if ( tracker2[0] == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/track2assigned.png");
	if(temp != NULL) tracker2[1] = SDL_DisplayFormat(temp);
	if ( tracker2[1] == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/track3.png");
	if(temp != NULL) tracker3[0] = SDL_DisplayFormat(temp);
	if ( tracker3[0] == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/track3assigned.png");
	if(temp != NULL) tracker3[1] = SDL_DisplayFormat(temp);
	if ( tracker3[1] == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/track4.png");
	if(temp != NULL) tracker4[0] = SDL_DisplayFormat(temp);
	if ( tracker4[0] == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);

	temp = Load_Image("images/track4assigned.png");
	if(temp != NULL) tracker4[1] = SDL_DisplayFormat(temp);
	if ( tracker4[1] == NULL ) { // Are we screwed??
		cerr<<"Function LoadWidgets()" << endl
		<< SDL_GetError() << endl;
		SDL_Quit();
		exit(0);
	}
	SDL_FreeSurface(temp);
}

void AnBqq5::DisplaySonarWidgets(){
	SDL_Rect destination_rectangle;
 	destination_rectangle.x = 180; //upper left corner to
	destination_rectangle.y = 710; //place the button.
	if (sonarwidget){ // Is our button down?
		destination_rectangle.h = sonarbuttondown->h; //height &
		destination_rectangle.w = sonarbuttondown->w;//width of button.
		SDL_BlitSurface(sonarbuttondown, NULL, screen, &destination_rectangle); // Do the Blit.
		SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
		destination_rectangle.x = 614;
		destination_rectangle.y = 161;
		destination_rectangle.h = truerel[bearingdisplay5by6]->h; //height &
		destination_rectangle.w = truerel[bearingdisplay5by6]->w;//width of button
		SDL_BlitSurface(truerel[bearingdisplay5by6], NULL, screen, &destination_rectangle); // Do the Blit..
		SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
		destination_rectangle.x = 614;
		destination_rectangle.y = 215;
		destination_rectangle.h = sphertowed[arraychoice5by6]->h; //height &
		destination_rectangle.w = sphertowed[arraychoice5by6]->w;//width of button
		SDL_BlitSurface(sphertowed[arraychoice5by6], NULL, screen, &destination_rectangle); // Do the Blit..
		SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
		destination_rectangle.x = 661;
		destination_rectangle.y = 161;
		destination_rectangle.h = uppercrtoff->h; //height &
		destination_rectangle.w = uppercrtoff->w;//width of button
		SDL_BlitSurface(uppercrtoff, NULL, screen, &destination_rectangle); // Do the Blit..
		SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
		destination_rectangle.x = 661;
		destination_rectangle.y = 215;
		destination_rectangle.h = lowercrtoff->h; //height &
		destination_rectangle.w = lowercrtoff->w;//width of button
		SDL_BlitSurface(lowercrtoff, NULL, screen, &destination_rectangle); // Do the Blit..
		SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
                destination_rectangle.x = 520;
                destination_rectangle.y = 590;
                destination_rectangle.h = cutarray->h;
                destination_rectangle.w = cutarray->w;
                SDL_BlitSurface(cutarray, NULL, screen, &destination_rectangle);
                SDL_UpdateRects(screen, 1, &destination_rectangle);

                destination_rectangle.x = 472;
                destination_rectangle.y = 590;
                destination_rectangle.h = sendping->h;
                destination_rectangle.w = sendping->w;
                SDL_BlitSurface(sendping, NULL, screen, &destination_rectangle);
                SDL_UpdateRects(screen, 1, &destination_rectangle);

		destination_rectangle.x = 567;
		destination_rectangle.y = 590;
		destination_rectangle.h = tb16winchoff->h; //height &
		destination_rectangle.w = tb16winchoff->w;//width of button
		SDL_BlitSurface(tb16winchoff, NULL, screen, &destination_rectangle); // Do the Blit..
		SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
		destination_rectangle.x = 614;
		destination_rectangle.y = 590;
		destination_rectangle.h = extendtb16[TB16.winch==1]->h; //height &
		destination_rectangle.w = extendtb16[TB16.winch==1]->w;//width of button
		SDL_BlitSurface(extendtb16[TB16.winch==1], NULL, screen, &destination_rectangle); // Do the Blit..
		SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
		destination_rectangle.x = 661;
		destination_rectangle.y = 590;
		destination_rectangle.h = retracttb16[TB16.winch==2]->h; //height &
		destination_rectangle.w = retracttb16[TB16.winch==2]->w;//width of button
		SDL_BlitSurface(retracttb16[TB16.winch==2], NULL, screen, &destination_rectangle); // Do the Blit..
		SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
		destination_rectangle.x = 473;
		destination_rectangle.y = 403;
		destination_rectangle.h = assigntrackerwidget[0]->h; //height &
		destination_rectangle.w = assigntrackerwidget[0]->w;//width of button
		SDL_BlitSurface(assigntrackerwidget[assigntracker], NULL, screen, &destination_rectangle); // Do the Blit..
		SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
		destination_rectangle.x = 520;
		destination_rectangle.y = 403;
		destination_rectangle.h = tracker1[0]->h; //height &
		destination_rectangle.w = tracker1[0]->w;//width of button
		SDL_BlitSurface(tracker1[Tma.GetTrackerState(0)], NULL, screen, &destination_rectangle); // Do the Blit..
		SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
		destination_rectangle.x = 567;
		destination_rectangle.y = 403;
		destination_rectangle.h = tracker2[0]->h; //height &
		destination_rectangle.w = tracker2[0]->w;//width of button
		SDL_BlitSurface(tracker2[Tma.GetTrackerState(1)], NULL, screen, &destination_rectangle); // Do the Blit..
		SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
		destination_rectangle.x = 614;
		destination_rectangle.y = 403;
		destination_rectangle.h = tracker3[0]->h; //height &
		destination_rectangle.w = tracker3[0]->w;//width of button
		SDL_BlitSurface(tracker3[Tma.GetTrackerState(2)], NULL, screen, &destination_rectangle); // Do the Blit..
		SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
		destination_rectangle.x = 661;
		destination_rectangle.y = 403;
		destination_rectangle.h = tracker4[0]->h; //height &
		destination_rectangle.w = tracker4[0]->w;//width of button
		SDL_BlitSurface(tracker4[Tma.GetTrackerState(3)], NULL, screen, &destination_rectangle); // Do the Blit..
		SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
	}else{ // our button must be up...
		destination_rectangle.h = sonarbuttonup->h; //height &
		destination_rectangle.w = sonarbuttonup->w;//width of button.
		SDL_BlitSurface(sonarbuttonup, NULL, screen, &destination_rectangle); // Do the Blit.
		SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
	}
}


void AnBqq5::UpperCRT_Spherical(){
	uppersonarcrt = sonarscreen;
}

void AnBqq5::LowerCRT_Spherical(){
	lowersonarcrt = sonarscreen;
}

void AnBqq5::UpperCRT_TowedArray(){
	uppersonarcrt = towedarrayscreen;
}

void AnBqq5::LowerCRT_TowedArray(){
	lowersonarcrt = towedarrayscreen;
}

void AnBqq5::ToggleArrayChoice(){
	arraychoice5by6 = !arraychoice5by6;
}

void AnBqq5::ToggleTrueRel(){
	bearingdisplay5by6 = !bearingdisplay5by6;
}

void AnBqq5::ToggleAssignTracker(){
	assigntracker = !assigntracker;
}

bool AnBqq5::GetNorthCenterState(){
	return northcenter;
}

void AnBqq5::ToggleNorthCenter(){
	northcenter = !northcenter;
}

void AnBqq5::UpperCRT_Button(){
	SDL_Rect destination_rectangle;
	destination_rectangle.x = 661;
	destination_rectangle.y = 161;
	destination_rectangle.h = uppercrton->h; //height &
	destination_rectangle.w = uppercrton->w;//width of button
	SDL_BlitSurface(uppercrton, NULL, screen, &destination_rectangle); // Do the Blit..
	SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
	SDL_Delay(200);
	if(arraychoice5by6){
		UpperCRT_Spherical();
	}else{
		UpperCRT_TowedArray();
	}
	DisplaySonarWidgets();
}

void AnBqq5::LowerCRT_Button(){
	SDL_Rect destination_rectangle;
	destination_rectangle.x = 661;
	destination_rectangle.y = 215;
	destination_rectangle.h = lowercrton->h; //height &
	destination_rectangle.w = lowercrton->w;//width of button
	SDL_BlitSurface(lowercrton, NULL, screen, &destination_rectangle); // Do the Blit..
	SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
	SDL_Delay(200);
	if(arraychoice5by6){
		LowerCRT_Spherical();
	}else{
		LowerCRT_TowedArray();
	}
	DisplaySonarWidgets();
}

void AnBqq5::StopWinch(){
	SDL_Rect destination_rectangle;
	destination_rectangle.x = 567;
	destination_rectangle.y = 590;
	destination_rectangle.h = tb16winchon->h; //height &
	destination_rectangle.w = tb16winchon->w;//width of button
	SDL_BlitSurface(tb16winchon, NULL, screen, &destination_rectangle); // Do the Blit..
	SDL_UpdateRects(screen, 1, &destination_rectangle); //Show the screen...
	TB16.Stop();
	SDL_Delay(200);
	DisplaySonarWidgets();
}

int AnBqq5::RandInt(int TO) //Returns a random interger...TO is upper limit
{
	return (rand()%TO);
}

int AnBqq5::ReciprocalBearing(int bearing)
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



void AnBqq5::DisplayCursor()
{
	SDL_Rect rectangle;
	//Draw Cursor
	if(!northcenter){
		DLine(screen, 51+cursorBearing, 140, 51+cursorBearing, 147, green);
		rectangle.x=51+cursorBearing;
	}else{
		int recipCursorBearing = ReciprocalBearing(cursorBearing);
		DLine(screen, 51+recipCursorBearing, 140, 51+recipCursorBearing, 147, green);
		rectangle.x=51+recipCursorBearing;		
	}
	rectangle.y=140;
	rectangle.h=7;
	rectangle.w=1;
	SDL_UpdateRects(screen, 1, &rectangle);
}
void AnBqq5::UpdateCursor()
{
	int direction, temp;
        int ship_type;
	float signal;
	if (Subs->GetCount()>0){
	//Are there any sound events at this time index to worry about?
		for (int event=1; event<=Subs->GetCount(); event++){
		//If so step through the events. 
			Subs->GetEvent(event, &direction, &signal, &ship_type);
			//Okay this used to go through each bearing one by one and
			//check to see if there was a sonar contact. But this is slow..
			//So we are now incrementing by 5 degrees to speed thing along
			//In doing so we now have to chech relative_bearings because
			//the contact might get passed by if it's on a 0/359 deg boundary.
			temp = direction;
			if(cursorBearing > temp) temp += 360;
			// relative_bearing = temp - cursorBearing;
		
			//if(relative_bearing < 3 || relative_bearing >357){
			// cout << "TargetId: " << TargetId << " - Bearing: " << direction <<endl;
				/*
				static char text[120];
				sprintf(text, "TARGET-ID %3i - BEARING %3i", TargetId, direction);
				string textline = text;
				Message.post_message(textline);
				Message.display_message();
				*/
			//}
		}
	}
	//If cursor located on undesignated contact, Halt on contact.
	//else 
	cursorBearing += 5;
	if(cursorBearing>360) cursorBearing -= 360;
	//Some debug crap
	/*if (cursorBearing > 360) cout << "cursor over " << cursorBearing <<endl;
	if (cursorBearing < 0) cout << "cursor under " << cursorBearing << endl;*/
}
