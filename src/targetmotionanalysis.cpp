/***************************************************************************
                          targetmotionanalysis.cpp  -  description
                             -------------------
    begin                : Sat Mar 16 2002
    copyright            : (C) 2002 by Michael Bridak
    email                : michael.bridak@verizon.net
$Id: targetmotionanalysis.cpp,v 1.7 2003/05/17 22:27:28 mbridak Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License.     *
 *    .                                   *
 *                                                                         *
 ***************************************************************************/

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_thread.h"
#include "targetmotionanalysis.h"
#include "math.h"

TargetMotionAnalysis::TargetMotionAnalysis(){
	MutexLock = SDL_CreateMutex();
	plotscale = 1;
	centerGeoPlot = true;
}

TargetMotionAnalysis::~TargetMotionAnalysis(){
	SDL_DestroyMutex(MutexLock);
}

/** Returns TRUE if tracker is in use */
bool TargetMotionAnalysis::GetTrackerState(int tracker) const{
	return trackerstate[tracker];
}

/** Frees a tracker to be used later. */
void TargetMotionAnalysis::ClearTracker(int tracker){
	trackerstate[tracker]=false;
}

/** Assign a tracker to a target */
void TargetMotionAnalysis::AssignTracker(int tracker, int target){
   trackerstate[tracker]=true;
}

/** Assign Target designation and Tracker to trace currently under
cursor on SONAR screen. */
void TargetMotionAnalysis::DesignateContact(){
}

/** Display a GeoPlot for the designated contact based on the
position of the boat and the bearing history of the contact. */
void TargetMotionAnalysis::DisplayGeoPlot(int xoffset, int yoffset){
	ClearGeoPlot();
	if(size !=0){
		PlotHistory(plotscale,xoffset,yoffset);
	}
	SDL_Rect rectangle;
	rectangle.x=0;
	rectangle.y=0;   
	rectangle.h=500;  
	rectangle.w=500;
	SDL_UpdateRects(GeoPlotScreen, 1, &rectangle);
}

/** Creates a speed ruler which we will use to drag around the
GeoPlot to test solutions.
Extend ruler backwards in time by plotting course using reciprocal
bearings. */
void TargetMotionAnalysis::CreateSpeedRuler(){
}

/** Display the LOS Diagram*/
void TargetMotionAnalysis::DisplayLOS(){

	double bearing_to_target;
	double relative_bearing;
	double recipbearing;
	double var1;//a place to hold a value so we don't have to calc it twice
	double destinationx; //the dest x&y point for
	double destinationy; //the drawn line.
	double direction;
	int x,y;

	bearing_to_target = storage[size-1].bearing;
	ClearGeoPlot();

	//Draw LOS
	DrawLine(250, 292, 250, 100, white);
	

	//Draw our ships angle relative to the LOS
	if(our_heading > bearing_to_target) bearing_to_target += 360;
	relative_bearing = bearing_to_target - our_heading;

	x=250;
	y=292;
	direction = 360 - relative_bearing;
	our_speed=(our_speed*3);
	var1 = direction * 0.017453; //convert degrees to radians.
	destinationx = (sin(var1) * our_speed) + x;
	destinationy = (-1) * (cos(var1) * our_speed) + y; 
	DrawLine(x, y, (int)destinationx, (int)destinationy, yellow);

	//Draw targets angle relative to the LOS
	x=250;
	y=100;
	if (bearing_to_target >= 180){
		 recipbearing = bearing_to_target - 180;
	}else{
		recipbearing = bearing_to_target + 180;
	}
	direction = target_heading + (180 - recipbearing);
	target_speed=(target_speed*3);
	var1 = direction * 0.017453; //convert degrees to radians.
	destinationx = (sin(var1) * target_speed) + x;
	destinationy = (-1) * (cos(var1) * target_speed) + y; 
	DrawLine(x, y, (int)destinationx, (int)destinationy, red);

	SDL_Rect rectangle;
	rectangle.x=0;
	rectangle.y=0;   
	rectangle.h=500;  
	rectangle.w=500;
	SDL_UpdateRects(GeoPlotScreen, 1, &rectangle);

}

/** Plots the amount of error from the expected bearing to target
and the actual bearing recieved. */
void TargetMotionAnalysis::BearingDevitaionPlot(){
}

/** Record the boats current position*/
void TargetMotionAnalysis::RecordBoatPosition(float latitude, float longitude, double bearing, Uint32 tick){
	Lock();
	position temp;
	temp.lat = latitude;
	temp.lon = longitude;
	temp.bearing = bearing;
	temp.tick = tick;
	RecBoatPos(temp);
	UnLock();
}

/** Lock Mutex */
void TargetMotionAnalysis::Lock(){
	SDL_mutexP(MutexLock);
}

/** Unlock Mutex */
void TargetMotionAnalysis::UnLock(){
	SDL_mutexV(MutexLock);
}

/** Initialize the graphics */
void TargetMotionAnalysis::InitGraphics(){
	SDL_Surface *screen = SDL_GetVideoSurface();
	SDL_Surface *temp = SDL_CreateRGBSurface(SDL_SWSURFACE, 500, 500, 16,
		screen->format->Rmask,
		screen->format->Gmask,
		screen->format->Bmask,
		screen->format->Amask);
	GeoPlotScreen=SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);
 	textcolor = SDL_MapRGB(screen->format, 192, 255, 6);
	black = SDL_MapRGB(screen->format, 0, 0, 0);
	white = SDL_MapRGB(screen->format, 255, 255, 255);
	red = SDL_MapRGB(screen->format, 255, 99, 20);
	green = SDL_MapRGB(screen->format, 0, 255, 0);
	yellow = SDL_MapRGB(screen->format, 216, 255, 0);
	mapcolor = SDL_MapRGB(screen->format, 130, 201, 225);
	ClearGeoPlot();
}
/** No descriptions */
void TargetMotionAnalysis::PlotPixel(int x, int y, Uint32 color){
	if (x > 499 || x < 0 || y > 499 || y < 0) {
		return;
	}

	//place the pixel on the screen
	Uint16 *pixel_location;
	pixel_location = (Uint16 *)GeoPlotScreen->pixels + y*GeoPlotScreen->pitch/2 + x;
	*pixel_location = color;
}
/** erases the work screen */
void TargetMotionAnalysis::ClearGeoPlot(){
	SDL_Rect rectangle;
	rectangle.x=0;
	rectangle.y=0;   //define a rectangle on the screen and make it black
	rectangle.h=500;   
	rectangle.w=500;
	SDL_FillRect(GeoPlotScreen, &rectangle, black);
	SDL_UpdateRects(GeoPlotScreen, 1, &rectangle);
}
/** Plot the boats history */
void TargetMotionAnalysis::PlotHistory(int scale, int change_scrollx, int change_scrolly){
	//scale is in YDS per pixel
	int x, y; //where to place the ships
	int xoffset = 0;  //offsets to move the ships to
	int yoffset = 0;  //the defined place on map screen
	static int scrolloffsetx=0; //offset to center map
	static int scrolloffsety=0;
	double var1; //just a place to hold a value so we don't have to calc it twice.
	double destinationx; //the dest X&Y point for
	double destinationy; //the drawn line.
	int length = 2000;

	scale = scale * 5;
	if(centerGeoPlot){ //center map onto our own ships location
		scrolloffsetx = 250 - (int)(storage[size-1].lat / scale);
		scrolloffsety = 250 - (int)(storage[size-1].lon / scale);
	}else{
		scrolloffsetx += change_scrollx;
		scrolloffsety += change_scrolly;
	}
	for (int index = 0; index < size; index ++){
		x = 500 - (int)(storage[index].lat / scale);
		x = x - scrolloffsetx;
		y = 500 - (int)(storage[index].lon / scale);
		y = y - scrolloffsety;
		if (x>10 && x<490 && y>10 && y<490){
			x = x + xoffset;
			y = y + yoffset;
			DrawDot(x, y, yellow);
		}
		var1 = storage[index].bearing * 0.017453; //convert degrees to radians.
		destinationx = (sin(var1) * length) + x;
		destinationy = (-1) * (cos(var1) * (length/.75)) + y; //adjust to account for 4:3 screen ratio.
		DrawLine(x, y, (int)destinationx, (int)destinationy, white);
	}

}
/** Draws a line....What were you expecting? */
void TargetMotionAnalysis::DrawLine(int X1,int Y1,int X2,int Y2,Uint32 Color){
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
			PlotPixel(px, py, Color);
			y = y + dy;
			if (y >= dx){
				y = y - dx;
				py = py + sdy;
			}
			px = px + sdx;
		}
	}else{
		for (int y = 0; y < dy; y++){
			PlotPixel(px, py, Color);
			x = x + dx;
			if (x >= dy){
				x = x - dy;
				px = px + sdx;
			}
			py = py + sdy;
		}
	}
}
/** No descriptions */
void TargetMotionAnalysis::DrawDot(int x, int y, Uint32 color){
	int xc, yc, radius;
	int theta, theta1, theta2;
	xc = x;
	yc = y;
	radius = 2;
	theta1 = 0;
	theta2 = 360;

	for(theta=theta1;theta<=theta2;theta+=5) {
	  x = xc + int(radius*cos(theta*3.14/180.0));
	  y = yc - int(radius*sin(theta*3.14/180.0));
	  PlotPixel(x, y, color);
	}
}

/** Increments plotscale to zoom in on the GeoPlot */
void TargetMotionAnalysis::IncreasePlotScale(){
	plotscale++;
	if(plotscale > 15){
		plotscale = 15;
	}
}
/** Decriment the plotscale to zoom out of GeoPlot */
void TargetMotionAnalysis::DecreasePlotScale(){
	plotscale--;
	if(plotscale < 1){
		plotscale = 1;
	}
}
/** No descriptions */
void TargetMotionAnalysis::ToggleGeoPlotCenter(){
	centerGeoPlot = !centerGeoPlot;
}
/** Fill in the initial values for the solution. */
void TargetMotionAnalysis::InitializeSolution(float LatYards, float LonYards, int Speed, int Heading){
Solution[0].Lat_TotalYards = LatYards;
Solution[0].Lon_TotalYards = LonYards;
Solution[0].Speed = Speed;
Solution[0].Heading = Heading;
}
