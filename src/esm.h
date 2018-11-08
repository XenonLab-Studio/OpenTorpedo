/***************************************************************************
                          esm.h  -  description
                             -------------------
    begin                : Sat Mar 16 2002
    copyright            : (C) 2002 by Michael Bridak
    email                : michael.bridak@verizon.net
$Id: esm.h,v 1.5 2003/04/14 05:51:03 mbridak Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License.     *
 *    .                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ESM_H
#define ESM_H


/**
  *@author Michael Bridak
  */

class Esm {
public: 

	Submarine *Subs;	
	SDL_Rect src, dest;

	SDL_Surface *esmscreen;
	SDL_Surface *tempscreen;
	SDL_Surface *blip;
	SDL_Surface *sweep1;
	SDL_Surface *ClearEsm;
	SDL_Surface *ClearEsm2;

	Esm(Submarine *temp);
	~Esm();
	
	void RaiseMast(); // Raise esm mast
	void LowerMast(); // Lower esm mast
	void ToggleMast(); //Toggle Mast Status

	// Set functions
	void setRangeScale(int);  // How far do we want to look!
	void setRangeRing(int);   // Used on the display panel


	// Get functions
	bool getMastStatus() const;  // Get the current mast position
	int getAntennaHeight(int, int) const; // Get current mast height
	int getBearing(int, int, int, int);  // Get the Esm bearing
	int getSignalStrength(Submarine *, int, int, int, bool, int); // Get the Esm signal strength
	int getEsmHorizon(int, int, int);  // Get the Esm Horizon

	bool isTargetVisible(Submarine *, int TargetRange, int ObserverDepth,
			  int TargetHeight, bool TargetRadar, int SeaState);

	void ClearScreen();

	void LoadWidgets();

	void DisplayWidgets();

	void DisplayContacts();
	void InitGraphics(SDL_Surface *temp, SDL_Surface *tempradarscreen);
	void DLine(SDL_Surface *screen, int X1, int Y1, int X2, int Y2, Uint32 Color);
	void DPixel(SDL_Surface *screen, int x, int y, Uint32 color);


	private:
	SDL_Rect destination_rectangle;
	SDL_Rect source_rectangle;
	SDL_Surface *screen;

	SDL_Surface *mastdownoff;
	SDL_Surface *mastdownon;

	SDL_Surface *mastupoff;
	SDL_Surface *mastupon;

	SDL_Surface *styllus;


	bool Mast;
	int MastHeight;
	int CurrentAntennaHeight;
	int EsmHorizon;
	int bearing;
	int SignalStrength;
	int DetectProb;
	int x;
	int y;
	Uint32 orange, black;

	static char text[120];
	DFont largeFont(const char*, const char *);
	DFont fnt(const char*, const char *);
	DFont fnt2(const char*, const char *);
};

#endif
