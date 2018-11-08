/***************************************************************************
                          coord.h  -  description
                             -------------------
    begin                : Sat Mar 16 2002
    copyright            : (C) 2002 by Michael Bridak
    email                : michael.bridak@verizon.net
$Id: coord.h,v 1.5 2003/04/14 05:51:03 mbridak Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License.     *
 *    .                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef COORD_H
#define COORD_H

// These map values are also present in the MAP class.
#ifndef MAP_MIN
#define MAP_MIN 0
#endif
#ifndef MAP_MAX
#define MAP_MAX 300000
#endif

#ifndef RADIAN_RATIO
#define RADIAN_RATIO 6.28318530717958647692
#endif

/**
  *@author Michael Bridak
  */

class Coord {
public:
	float Heading;
	float Speed;
	int Lat_Degrees;
	int Lat_Minutes;
	int Lat_Seconds;
	float Lat_SubSeconds;
	float Lat_TotalYards;
	int Lon_Degrees;
	int Lon_Minutes;
	int Lon_Seconds;
	float Lon_SubSeconds;
	float Lon_TotalYards;
	void UpdateLatLon();
	void LatLonPercent(float heading, float &lop, float &lap);
	Coord();
	~Coord();
	};
#endif
