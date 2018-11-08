/***************************************************************************
                          targetmotionanalysis.h  -  description
                             -------------------
    begin                : Sat Mar 16 2002
    copyright            : (C) 2002 by Michael Bridak
    email                : michael.bridak@verizon.net
$Id: targetmotionanalysis.h,v 1.6 2003/04/20 16:55:49 mbridak Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License.     *
 *    .                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TARGETMOTIONANALYSIS_H
#define TARGETMOTIONANALYSIS_H
#define MAXTRACKERS 13

#include "boatpositionlog.h"
#include "contact.h"

/**
  *@author Michael Bridak
  */

class TargetMotionAnalysis : public BoatPositionLog  {
public:
	Contact Solution[12];
	Uint32 textcolor, black, white, red, green, yellow, mapcolor;//Place to hold color info

	double our_heading;
	double target_heading;

	float our_speed;
	float target_speed;	

	/** an SDL surface to plot our GeoPlot on */
	SDL_Surface *GeoPlotScreen;

  /**  */
  int plotscale;

  /** Geoplot centering flag */
  bool centerGeoPlot;

	TargetMotionAnalysis();
	~TargetMotionAnalysis();

  /** Frees a tracker to be used later. */
  void ClearTracker(int tracker);

  /** Returns the TRUE if tracker is in use */
  bool GetTrackerState(int tracker) const;

  /** Assign a tracker to a target */
  void AssignTracker(int tracker, int target);

  /** Assign Target designation and Tracker to trace currently under
cursor on SONAR screen.
 */
  void DesignateContact();

  /** Display a GeoPlot for the designated contact based on the
position of the boat and the bearing history of the contact.
 */
  void DisplayGeoPlot(int xoffset, int yoffset);

  /** Display the LOS Diagram
 */
  void DisplayLOS();

  /** Creates a speed ruler which we will use to drag around the
GeoPlot to test solutions.
 */
  void CreateSpeedRuler();

  /** Plots the amount of error from the expected bearing to target
and the actual bearing recieved.
 */
  void BearingDevitaionPlot();

  /** Record the boats current position
 */
  void RecordBoatPosition(float latitude, float longitude, double bearing, Uint32 tick);

  /** Lock Mutex */
  void Lock();

  /** Unlock Mutex */
  void UnLock();

  /** Initialize the graphics */
  void InitGraphics();

  /** No descriptions */
  void PlotPixel(int x, int y, Uint32 color);

  /** erases the work screen */
  void ClearGeoPlot();

  /** Plot the boats history */
  void PlotHistory(int scale, int change_scrollx, int change_scrolly);

  /** Draws a line....What were you expecting? */
  void DrawLine(int X1,int Y1,int X2,int Y2,Uint32 Color);

  /** No descriptions */
  void DrawDot(int x, int y, Uint32 color);

  /** Increments plotscale to zoom in on the GeoPlot */
  void IncreasePlotScale();

  /** Decriment the plotscale to zoom out of GeoPlot */
  void DecreasePlotScale();

  /** No descriptions */
  void ToggleGeoPlotCenter();

  /** Fill in the initial values for the solution. */
  void InitializeSolution(float LatYards, float LonYards, int Speed, int Heading);

private: // Private attributes
  /**  */
  bool trackerstate[MAXTRACKERS];
	struct packet {
		float LatYards;
		float LonYards;
		float BearingToTarget;
	};



	SDL_mutex *MutexLock;
};

#endif
