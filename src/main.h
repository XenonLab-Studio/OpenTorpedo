/***************************************************************************
                          main.h  -  description
                             -------------------
    begin                : Tue Sep 19 2000
    copyright            : (C) 2000 by Michael Bridak
    email                : michael.bridak@verizon.net
$Id: main.h,v 1.11 2003/07/18 03:50:00 mbridak Exp $
 ***************************************************************************/

/***************************************************************************

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License.
     .

 ***************************************************************************/

#ifndef MAIN_H
#define MAIN_H

#ifndef VERSION
#define VERSION 1.4
#endif

#include "helicopter.h"

// delay between game loops
#define GAME_DELAY 10
#define MAP_FACTOR 20
#define MAX_MAP_SCALE 500

#define PERISCOPE_DEPTH 50
#define MAX_SUBS 20

#define DEFAULT_SEA_STATE 3
#define SHIP_HEIGHT 100


/** Our main program and control loop
  *@author Michael Bridak
  */
// Some defs for easy living.
#define		SONAR	1
#define		NAVMAP	2
#define		TMA	3
#define		QUIT	4
#define		COMPRESSTIME	5
#define		UNCOMPRESSTIME	6
#define		INCREASESPEED	7
#define		DECREASESPEED	8
#define		DUMPSCREEN	9
#define		TURNPORT	10
#define		TURNSTARBOARD	11
#define		INCREASEDEPTH	12
#define		DECREASEDEPTH	13
#define		INCREASEMAPSCALE	14
#define		DECREASEMAPSCALE	15
#define		CENTERDISPLAY	16
#define		EXTENDARRAY	17
#define		RETRACTARRAY	18
#define		STOPWINCH	19
#define		PAUSEGAME	20
#define		SHIPCONTROL 21
#define		SCROLLMAPUP	22
#define		SCROLLMAPDOWN	23
#define		SCROLLMAPLEFT	24
#define		SCROLLMAPRIGHT	25
#define		TOGGLESPHERICALTOWED 26
#define		TOGGLETRUERELATIVE 27
#define		UPPERCRTBUTTON	28
#define		LOWERCRTBUTTON	29
#define		ASSIGNTRACKER 30
#define		TRACKER1 31
#define		TRACKER2 32
#define		TRACKER3 33
#define		TRACKER4 34
#define		ESM 35
#define		RADAR 36

// Radar Event Updates
#define		TOGGLER10	37
#define		TOGGLER20	38
#define		TOGGLER30	39
#define		TOGGLER40	40
#define		TOGGLER50	41
#define		TOGGLER60	42
#define		RADARDOWN	43
#define		RADARUP		44
#define		RING0	        45
#define		RING5	        46
#define		RING10   	47
#define		RING15  	48
#define		RING20  	49
#define		RING25  	50

// Esm Event Updates
#define		ESMDOWN		51
#define		ESMUP		52

// Control Station Updates
#define		ASTOP	        53
#define		A13	        54
#define		A23      	55
#define		ASTD     	56
#define		AFULL    	57
#define		AFLK            58
#define		B13     	59
#define		B23      	60
#define		BSTD     	61
#define		BEMER      	62
#define		DESIGNATECONTACT	63
#define 	SWITCHTARGET 64
#define 	WEAPONS 65
#define		LOAD_TORPEDO 66
#define		LOAD_NOISEMAKER 67
#define		UNLOAD_TUBE 68
#define		FIRE_TUBE 69
#define		WHICHTUBE 70
#define		USE_TUBE 71
#define		CUTARRAY 72
#define         SEND_PING 73
#define         UP_THERMAL 74
#define         HOLD_DEPTH 75
#define         DOWN_THERMAL 76
#define		GO_PERISCOPE_DEPTH 77
#define		GO_SURFACE 78


//globals

// int	ships = 19; // why does ships equal 19, isn't this set in CreateShips?
float	flowandambientnoise;
SDL_Surface	*screen=NULL; //The screen we are goin to be looking at
SDL_Surface	*towedarrayscreen=NULL; //scratch blit surface

// rdm
SDL_Surface	*radarscreen=NULL; //scratch blit surface
SDL_Surface	*esmscreen=NULL; //scratch blit surface
SDL_Surface *controlscreen=NULL;



SDL_Surface	*tempscreen=NULL; //scratch screen
SDL_Surface	*sonarbuttonup=NULL; //button element
SDL_Surface	*sonarbuttondown=NULL; //button element
SDL_Surface	*navbuttonup=NULL; //button element
SDL_Surface	*navbuttondown=NULL; //button element
SDL_Surface	*tmabuttonup=NULL; //button element
SDL_Surface	*tmabuttondown=NULL; //button element
SDL_Surface	*shipcontrolbuttonup=NULL;//button element
SDL_Surface	*shipcontrolbuttondown=NULL;//button element
SDL_Surface	*esmbuttonup=NULL;//button element
SDL_Surface	*esmbuttondown=NULL;//button element
SDL_Surface	*radarbuttonup=NULL;//button element
SDL_Surface	*radarbuttondown=NULL;//button element
SDL_Surface	*quitbuttonup=NULL; //button element
SDL_Surface	*quitbuttondown=NULL; //button element
SDL_Surface	*plusbuttonup=NULL; //button element
SDL_Surface	*plusbuttondown=NULL; //button element
SDL_Surface	*centerbuttonup=NULL; //button element
SDL_Surface	*centerbuttondown=NULL; //button element
SDL_Surface	*leftbuttonup=NULL; //button element
SDL_Surface	*leftbuttondown=NULL; //button element
SDL_Surface	*rightbuttonup=NULL; //button element
SDL_Surface	*rightbuttondown=NULL; //button element
SDL_Surface	*upbuttonup=NULL; //button element
SDL_Surface	*upbuttondown=NULL; //button element
SDL_Surface	*downbuttonup=NULL; //button element
SDL_Surface	*downbuttondown=NULL; //button element
SDL_Surface	*minusbuttonup=NULL; //button element
SDL_Surface	*minusbuttondown=NULL; //button element
SDL_Surface	*ncscale=NULL;
SDL_Surface	*scscale=NULL;
SDL_Surface	*truerel[2]={NULL, NULL};
SDL_Surface	*sphertowed[2]={NULL, NULL};
SDL_Surface	*tb16winchon=NULL;
SDL_Surface	*tb16winchoff=NULL;
SDL_Surface	*extendtb16[2]={NULL, NULL};
SDL_Surface	*retracttb16[2]={NULL, NULL};
SDL_Surface	*uppercrtoff=NULL;
SDL_Surface	*uppercrton=NULL;
SDL_Surface	*lowercrtoff=NULL;
SDL_Surface	*lowercrton=NULL;
SDL_Surface	*assigntrackerwidget[2]={NULL, NULL};
SDL_Surface	*tracker1[2]={NULL, NULL};
SDL_Surface	*tracker2[2]={NULL, NULL};
SDL_Surface	*tracker3[2]={NULL, NULL};
SDL_Surface	*tracker4[2]={NULL, NULL};
SDL_Surface	*noisemaker_image = NULL;
SDL_Surface	*torpedo_image = NULL;


//a mutex so we don't crash and burn
//when accessing the same globals
//in a callback function.
SDL_mutex *tmamutex;

SDL_Rect destination_rectangle; //rectangle structure for blitting

DFont fnt(const char*, const char *);
DFont fnt2(const char*, const char *);

int tube_action = 0;    // what are we doing with a torpedo tube
int tube_to_use = -1;   // the tube to perform this action on

int drawsonar; //flags for UpdateDisplay()
int drawmap; //flags for UpdateDisplay()
int drawtma; //flags for UpdateDisplay()
//rdm
int drawweapons;  // display weapons screen
int drawradar; //flags for UpdateDisplay()
int drawesm; //flags for UpdateDisplay()
int drawcontrol; //flags for UpdateDisplay

int mapscale = 25;
int mapcenter = 1;   // by default, center the map on our ship
int timecompression;
int sonarwidget, navwidget, quitwidget, tmawidget; //button pressed flags
int shipcontrolwidget, esmwidget, radarwidget; //shouldn't these be bools?
Uint32 textcolor, black, white, red, green, yellow, grey, mapcolor;//Place to hold color info
Uint32 dark_green, dark_red, brown, dark_grey;
bool show_spherical_traces = true, northcenter = true, pause_game = false;
bool assigntracker = false;
Submarine *current_target = NULL;
int update_weapons_screen = TRUE;
int my_mission_status;
int should_update_everything = TRUE;

GameClock Clock;
// Submarine Subs[MAX_SUBS]; //Somewhere to put our little floating buddies.
Submarine *Subs = NULL;   // all subs, ships 
Submarine *player = NULL;
Contact Contacts[MAX_SUBS]; //Workable contacts.
Submarine *torpedoes;   // a linked-list of torpedoes
Helicopter *helicopters;  // a linked-list of helicopters
TowedArray TB16;
TargetMotionAnalysis Tma;
msg Message;
AnBqq5 SonarStation(Subs, TB16, Tma, Message);


Radar RadarStation(Subs);
Esm EsmStation(Subs);
Control ControlStation(Subs);


//Danger  I don't know what I'm doing here
struct position {
	float lat;
	float lon;
};
std::list<position> SubPosLog;  //STL incompetence DANGER!
//End Danger


//declarations
void	SetupScreen(bool);
void	CreateShips(int mission_number, MAP *map);
void	UpdateSensors(void);
void	UpdateDisplay(void);
void	TakeCommands(void);
double	RelativeBearing(Submarine *observer, Submarine *target);
int	ReciprocalBearing(int bearing);
double	AngleOnBow(Submarine *observer, Submarine *target);
int	maximize360 ( int course );
int	minimize360 ( int course );
double	CalculateRange(Submarine *observer, Submarine *target);
void	LatLonDifference(Submarine *observer, Submarine *target, double *platdif, double *plondif);
void	PositionCursor(int, int);
void	ClearScreen(void);
void    Display_Target(void);
void	DrawMap(void);
void    Draw_Depth_Meter(Submarine *my_sub, int which_screen);
/*
void	DrawPixel(SDL_Surface *screen, int x, int y, Uint32 Color);
void	DrawLine(SDL_Surface *screen, int X1, int Y1, int X2, int Y2, Uint32 Color);
void	DrawArc(SDL_Surface *screen, int X1, int Y1, int Radius, int Theta1, int Theta2, Uint32 Color);
void	DrawCircle(SDL_Surface *screen, int X1, int Y1, int Radius, Uint32 Color);
void	DrawDiamond(SDL_Surface *screen, int X1, int Y1, int Size, char Direction, Uint32 Color);
void	DrawBox(SDL_Surface *screen, int X1, int Y1, int Size, char Direction, Uint32 Color);
void    DrawRectangle(SDL_Surface *screen, int x1, int y1, int x2, int y2, Uint32 Color);
void	DrawCross(SDL_Surface *screen, int X1, int Y1, int Size, char Direction, Uint32 Color);
*/
void	PlaceShips(int, int = 0, int = 0, Submarine *target=NULL); // the int = 0's are default values
// inline void	DrawPixel(SDL_Surface *screen, int x, int y, Uint32 Color);
// inline void	DrawLine(SDL_Surface *screen, int X1, int Y1, int X2, int Y2, Uint32 Color);
void	LoadScreen(int screen_to_load);
void	ShowStation(int station);
void	DisplaySonar();
void	DisplayTMA(int xoffset=0, int yoffset=0);
void    DisplayWeapons(void);
void	LoadWidgets(void);
void	UnLoadWidgets(void);
void	DisplayWidgets(void);
void	DisplayNavigationWidgets();
void	DisplayTMAWidgets();
void	DisplayESMWidgets();
void	DisplayRADARWidgets();
void	ResetWidgetFlags(void);
inline int	RandInt(int to);
void	MapIcon(int x, int y, int ShipType, int Friend, Uint32 color);
void	DirectionalPointer(int X, int Y, int Heading, int speed, Uint32 Color);
int	InBaffles(Submarine *observer, Submarine *target, int sensor);
void	SoundEnvironment(void);
float	Any_Detection(double Range, Submarine *observer, Submarine *target);
float	Radar_Detection(double Range, Submarine *observer, Submarine *target);
float	Esm_Detection(double Range, Submarine *observer, Submarine *target);
// float	Sonar_Detection(double Range, int ObserverShipId, int TargetShipId);
float   Sonar_Detection_New(double Range, Submarine *observer, Submarine *target);
inline int		Clamp(int);
inline double	Clamp(double);
Uint32	TmaTimer(Uint32 interval, void *param);
Uint32	timerfunc(Uint32 interval, void *param);

Submarine *Add_Ship(Submarine *all_torpedoes, Submarine *new_torpedo);
Submarine *Remove_Ship(Submarine *all_torpedoes, Submarine *old_torpedo);
void Remove_Inactive_Ship(Submarine *victim);  // find and remove inactive ship
Helicopter *Add_Helicopter(Helicopter *all_helicopters, Helicopter *new_helicopter);
Helicopter *Remove_Helicopter(Helicopter *all_helicopters, Helicopter*old_helicopter);

#endif

