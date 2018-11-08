/***************************************************************************
                          Sonar.h  -  description
                             -------------------
    begin                : Fri Apr 11 2002
    copyright            : (C) 2002 by Michael Bridak
    email                : michael.bridak@verizon.net
$Id: sonar.h,v 1.10 2003/09/20 22:53:15 mbridak Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License.     *
 *    .                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SONAR_H
#define SONAR_H
/**
  *@author Michael Bridak
  */

class AnBqq5{
public:
	Submarine *Subs;
	int cursorBearing;
	bool bearingdisplay5by6;
	bool arraychoice5by6;
	bool sonarwidget;
	bool assigntracker;
	float flowandambientnoise;
	AnBqq5(Submarine *temp, TowedArray &temp2, TargetMotionAnalysis &temp3, msg &temp4);
	~AnBqq5();

	/** 	Initialize the graphic for this class, *screen is the main
		display screen passed in from main. */

	void InitGraphics();

	inline void DPixel(SDL_Surface *screen, int x, int y, Uint32 color);

	inline void DLine(SDL_Surface *screen, int X1, int Y1, int X2, int Y2, Uint32 Color);

	void Sonar(bool center);

	void DisplaySonar();

	/**	The Main entery point to do a gross update of all the Sonar
		stations sub screen. */

	void UpdateDisplay(Submarine *target);
	void ClearSonarData();

	void TowedSonar(bool center);

	void AdvanceSonarDisplay();

	void AdvanceTB16Screen();

	/**	Displays the degree bearing scale in the waterfall displays
		the bool center flag is to chech if we want north (0 degree)
		or south (180 degree) as the scales center. */

	void DisplayBearingScale(bool center);

	/**	Loads in the widgets */

	void LoadWidgets();

	/**	Displays the widgets needed for the sonar display*/

	void DisplaySonarWidgets();

	/**	Switches the upper sonar waterfall display screen to show the
		history of the Spherical array. */

	void UpperCRT_Spherical();

	/**	Switches the lower sonar waterfall display screen to show the
		history of the Spherical array. */

	void LowerCRT_Spherical();

	/**	Switches the upper sonar waterfall display screen to show the
		history of the TB16 array. */

	void UpperCRT_TowedArray();

	/**	Switches the lower sonar waterfall display screen to show the
		history of the TB16 array. */

	void LowerCRT_TowedArray();

	/**	Toggles the bool variable arraychoice which affects the sonar
		widget display and decided which array, Spherical or the towed
		TB16, will appear when either the upper crt or lower crt widget
		is clicked. */

	void ToggleArrayChoice();

	/**	Toggles the truerel bool variable. At this time the only affect
		it to the sonar widget display. But will soon determin if the
		waterfall display shows us true or reletive traces. */

	void ToggleTrueRel();

	/**	Called when the user clickes the "Upper CRT" widget on the sonar
		screen. When called, it will switch the upper waterfall display
		to show the desired sonar sensors history. */

	void UpperCRT_Button();

	/**	Called when the user clickes the "Lower CRT" widget on the sonar
		screen. When called, it will switch the lower waterfall display
		to show the desired sonar sensors history. */

	void LowerCRT_Button();

	/**	Called when the user clicks on the assign tracker widget on the
		sonar screen. Toggles the assigntracker bool variable. When the
		flag is true, and the user clicks on one of the tracker widgets,
		that tracker will be assigned to the sound source currently
		under the sonar cursor. */

	void ToggleAssignTracker();
	
	bool GetNorthCenterState();

	void ToggleNorthCenter();


	/**	Called when the user clicks on the "Stop TB16 Winch" widget on
		the sonar screen. This unhighlights the widget and calls the
		TB16.Stop() routine. */

	void StopWinch();

	int RandInt(int TO);

	int ReciprocalBearing(int bearing);

	void DisplayCursor();
	void UpdateCursor();

private:
	SDL_Rect destination_rectangle;
	SDL_Rect source_rectangle;
	SDL_Surface *screen;
	SDL_Surface *sonarscreen;
	SDL_Surface *towedarrayscreen;
	SDL_Surface *uppersonarcrt;
	SDL_Surface *lowersonarcrt;
	SDL_Surface *ncscale;
	SDL_Surface *scscale;
	SDL_Surface *sonarbuttondown;
	SDL_Surface *sonarbuttonup;
	SDL_Surface *truerel[2];
	SDL_Surface *sphertowed[2];
	SDL_Surface *uppercrtoff;
	SDL_Surface *uppercrton;
	SDL_Surface *lowercrtoff;
	SDL_Surface *lowercrton;
	SDL_Surface *tb16winchoff;
	SDL_Surface *tb16winchon;
	SDL_Surface *extendtb16[2];
	SDL_Surface *retracttb16[2];
        SDL_Surface *cutarray;
        SDL_Surface *sendping;
	SDL_Surface *assigntrackerwidget[2];
	SDL_Surface *tracker1[2];
	SDL_Surface *tracker2[2];
	SDL_Surface *tracker3[2];
	SDL_Surface *tracker4[2];
	DFont largeFont(const char*, const char *);
	TowedArray &TB16;
	TargetMotionAnalysis &Tma;
	msg &Message;
	int tempint;
	int tempval;
	float deAngle;
	static char text[120];
	Uint32 black;
	Uint32 white;
	Uint32 green;
	bool northcenter;
};

#endif
