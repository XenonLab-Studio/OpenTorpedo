/***************************************************************************
                          towedarray.h  -  description
                             -------------------
    begin                : Sat Mar 16 2002
    copyright            : (C) 2002 by Michael Bridak
    email                :michael.bridak@verizon.net
$Id: towedarray.h,v 1.3 2003/04/14 05:52:40 mbridak Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License.     *
 *    .                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TOWEDARRAY_H
#define TOWEDARRAY_H

#include "stack.h"

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/**
  *@author Michael Bridak
  */

class TowedArray : public Stack  {
public:
	TowedArray();
	TowedArray(double, double);
	~TowedArray();
	void RecordPos(double lat, double lon);
	int Extend();
	void Stop();
	int ReelIn();
        int CutArray();    // cut the rope and let it go
	int GetLength();
	void OperateWinch();
	void ReturnLatLon(double &latitude, double &longitude);
	float ReturnHeading();
	float BearingAmbiguity(float);
	float BearingToTarget(double target_Latitude, double target_longitude);
	float CalcBearing(double observer_latitude, double observer_longitude ,double target_latitude,double target_longitude);
	double RangeToTarget(double target_latitude, double target_longitude);
	double latitude_array[100], longitude_array[100];
	float length;  // how far out is the array?
	int winch;   // is the array going out or coming in or stopped?
        int cut;     // did we cut the array?
};

#endif
