/***************************************************************************
                          coord.cpp  -  description
                             -------------------
    begin                : Sat Mar 16 2002
    copyright            : (C) 2002 by Michael Bridak
    email                : michael.bridak@verizon.net
$Id: coord.cpp,v 1.7 2003/05/20 17:36:04 mbridak Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License.     *
 *    .                                   *
 *                                                                         *
 ***************************************************************************/

#include "coord.h"

Coord::Coord(){
}
Coord::~Coord(){
}

void Coord::UpdateLatLon()
{
	//procedure to update the latitude and longitude of a moving object.
	//This is what actually moves an object.

	float amountofchange = 0;
	float LongitudePercent = 0;
	float LatitudePercent = 0;
	LatLonPercent(Heading, LongitudePercent, LatitudePercent);
	amountofchange = (Speed * 0.555) * LongitudePercent;
	if ((Heading > 270) || (Heading < 90)){
		Lon_TotalYards = Lon_TotalYards + amountofchange;
	}
	if ((Heading < 270) && (Heading > 90)){
		Lon_TotalYards = Lon_TotalYards - amountofchange;
		// if (Lon_TotalYards < 0){
		//	Lon_TotalYards = 0;
		// }
	}
	amountofchange = (Speed * 0.555) * LatitudePercent;
	if ((Heading <= 359) && (Heading > 180)){
		Lat_TotalYards = Lat_TotalYards + amountofchange;
	}
	if ((Heading > 0) && (Heading < 180)){
		Lat_TotalYards = Lat_TotalYards - amountofchange;
		// if (Lat_TotalYards < 0){
		//	Lat_TotalYards = 0;
		// }
	}

        // Important, we need to keep the ships on the map.
        // The map is in the upp-erleft corner of the quad so
        // higher numbers are up and to the left.
        if (Lon_TotalYards < MAP_MIN)
        {
            Lon_TotalYards = MAP_MIN;
            Heading = 0;
        }
        else if (Lon_TotalYards > MAP_MAX)
        {
             Lon_TotalYards = MAP_MAX;
             Heading = 180;
        }
        if (Lat_TotalYards < MAP_MIN)
        {
            Lat_TotalYards = MAP_MIN;
            Heading = 270;
        }
        else if (Lat_TotalYards > MAP_MAX)
        {
            Lat_TotalYards = MAP_MAX;
            Heading = 90;
        }
 
        
/*
	Lon_Degrees = (int)Lon_TotalYards / 120000;
	Lon_Minutes = ((int)Lon_TotalYards - (Lon_Degrees *120000)) / 2000;
	Lon_Seconds = Lon_TotalYards - (Lon_Degrees * 120000) - (Lon_Minutes * 2000) / 33.33;
	Lon_SubSeconds = Lon_TotalYards - (Lon_Degrees * 120000) - (Lon_Minutes * 2000) - (Lon_Seconds * 33.33) / 0.555;

	Lat_Degrees = (int)Lat_TotalYards / 120000;
	Lat_Minutes = ((int)Lat_TotalYards - (Lat_Degrees *120000)) / 2000;
	Lat_Seconds = Lat_TotalYards - (Lat_Degrees * 120000) - (Lat_Minutes * 2000) / 33.33;
	Lat_SubSeconds = Lat_TotalYards - (Lat_Degrees * 120000) - (Lat_Minutes * 2000) - (Lat_Seconds * 33.33) / 0.555;
*/

}

void Coord::LatLonPercent(float heading, float &lonpercent, float &latpercent)
{
	//Calculate the % of change of Latitude VS Longitude based on heading"
	float llb=0;
	if ((heading < 90) &&  (heading > 0)){
		latpercent = (1.1111111111 * heading);
		lonpercent = (100.0 - latpercent);
	}
	if ((heading > 90) && (heading < 180)){
		llb = (heading - 90.0);
		lonpercent = (1.1111111111 * llb);
		latpercent = (100.0 - lonpercent);
	}
	if ((heading > 180) && (heading < 270)){
		llb = (heading - 180.0);
		latpercent = (1.1111111111 * llb);
		lonpercent = (100.0 - latpercent);
	}
	if ((heading > 270) && (heading < 360)){
		llb = (heading - 270.0);
		lonpercent = (1.1111111111 * llb);
		latpercent = (100.0 - lonpercent);
	}
	if ((heading == 0) || (heading == 360) || (heading == 180)){
		lonpercent = 100.0;
		latpercent = 0.0;
	}
	if ((heading == 270) || (heading == 90)){
		latpercent = 100.0;
		lonpercent = 0.0;
	}

	lonpercent = lonpercent / 100.0;
	latpercent = latpercent / 100.0;
}
