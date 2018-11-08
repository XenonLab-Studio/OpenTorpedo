/***************************************************************************
                             radar.h  -  description
                             -------------------
    begin                : Fri August 2 2001
    copyright            : (C) 2001 by Rick McDaniel
    email                : rickeym@swbell.net
$Id: radar.h,v 1.8 2003/04/14 05:51:04 mbridak Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License.     *
 *    .                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef RADAR_H
#define RADAR_H

// rdm windows
//#include <iostream>


/**Usage:

1. Make depth < 58 feet (for sea state <= 1).  Must be shallower at higher
   sea states.
2. Raise mast:  radar.RaiseMast()
  *@author Rick McDaniel
  */

class Radar {
   public:

	SDL_Surface *radarscreen;
	SDL_Surface *tempscreen;
	SDL_Surface *blip;
	SDL_Surface *sweep[1];	//only need 1 now
	SDL_Surface *ClearRadar;
	SDL_Rect src, dest;

	bool rangescale10, rangescale20, rangescale30;
	bool rangescale40, rangescale50, rangescale60;

	bool rangering0, rangering5, rangering10;
	bool rangering15, rangering20, rangering25;

	float tick;		//our radar sweep angle
	int ALPHA;

	Radar(Submarine *temp);
	~Radar();

	/** 	Initialize the graphic for this class, *screen is the main
		display screen passed in from main. */

	void InitGraphics(SDL_Surface *screen, SDL_Surface *radarscreen);
	
	void ClearScreen();
	
	void ClearTextBox();

	void LoadWidgets();

	void DisplayWidgets();

	void DisplayContacts();

	void DisplaySweep();
	
	void Sweep(float gametime);

	void DisplayRings();
	
	void ClearRangeScale();

	void ClearRangeRing();

	void ShowData(SDL_Surface *screen, int x, int y);

	int DeltaBearing(int bearing1, int bearing2);
	int ReciprocalBearing(int bearing1);

	void DrawPixel(SDL_Surface *screen, int x, int y, Uint32 color);
	void DrawArc(SDL_Surface *screen, int X1, int Y1, int Radius, int Theta1, int Theta2, Uint32 Color);
	void DrawCircle(SDL_Surface *screen, int X1, int Y1, int Radius, Uint32 Color);

	// Toggle Range Scale
	void ToggleRangeScale10();
	void ToggleRangeScale20();
	void ToggleRangeScale30();
	void ToggleRangeScale40();
	void ToggleRangeScale50();
	void ToggleRangeScale60();

	// Toggle Ring Ring
	void ToggleRangeRing0();
	void ToggleRangeRing5();
	void ToggleRangeRing10();
	void ToggleRangeRing15();
	void ToggleRangeRing20();
	void ToggleRangeRing25();

	void ToggleMast(); // Toggle Mast Status
        void RaiseMast();
        void LowerMast();

  // Set functions
	void setRangeScale(int);  // How far do we want to look!
	void setRangeRing(int);   // Used on the display panel

  // Get functions
	int getRangeScale() const;  // Get current RangeScale
	int getRangeRing() const;   // Get current RangeRing
	bool getMastStatus() const;  // Get the current mast position
	int getAntennaHeight(int, int) const; // Get current mast height
	int getBearing(int);  // Get target bearing
	int getRange(int); // Get target range
	float getRadarHorizon(int ObserverDepth, int TargetHeight, int SeaState);

	bool isTargetVisible(Submarine *, int, int, int, int); // Can we see the target

	SDL_Rect destination_rectangle;
	SDL_Rect source_rectangle;
	SDL_Surface *screen;
	SDL_Surface *range10off;
	SDL_Surface *range10on;
	SDL_Surface *range20off;
	SDL_Surface *range20on;
	SDL_Surface *range30off;
	SDL_Surface *range30on;
	SDL_Surface *range40off;
	SDL_Surface *range40on;
	SDL_Surface *range50off;
	SDL_Surface *range50on;
	SDL_Surface *range60off;
	SDL_Surface *range60on;

	SDL_Surface *ring0off;
	SDL_Surface *ring0on;
	SDL_Surface *ring5off;
	SDL_Surface *ring5on;
	SDL_Surface *ring10off;
	SDL_Surface *ring10on;
	SDL_Surface *ring15off;
	SDL_Surface *ring15on;
	SDL_Surface *ring20off;
	SDL_Surface *ring20on;
	SDL_Surface *ring25off;
	SDL_Surface *ring25on;

	SDL_Surface *mastdownoff;
	SDL_Surface *mastdownon;

	SDL_Surface *mastupoff;
	SDL_Surface *mastupon;

	Submarine *Subs;	

	bool Mast;
	int RangeScale;
	int RangeRing;
	int MastHeight;
	int CurrentAntennaHeight;
	int RadarHorizon;
	int bearing;
	int range;
	int x;
	int y;
	
	int tempint;
	int tempval;
	static char text[120];

	DFont largeFont(const char*, const char *);

	Uint32 orange;
	Uint32 black;
	
};

#endif
