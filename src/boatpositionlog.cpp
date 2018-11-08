/***************************************************************************
                          boatpositionlog.cpp  -  description
                             -------------------
    begin                : Sat Mar 16 2002
    copyright            : (C) 2002 by Michael Bridak
    email                : michael.bridak@verizon.net
$Id: boatpositionlog.cpp,v 1.2 2003/04/14 05:51:03 mbridak Exp $
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
#include "boatpositionlog.h"

BoatPositionLog::BoatPositionLog(){
	size = 0;
	position temp;
	temp.lat=0;
	temp.lon=0;
	temp.bearing = 0.0;
	temp.tick = 0;
	for(int x=0; x<10; x++){
		storage[x]=temp;
	}
}
BoatPositionLog::~BoatPositionLog(){
}
/** Recpord the position of the boat. takes a type of
struct {
	float lat;
	float lon;
	double bearing
}; */
void BoatPositionLog::RecBoatPos(position temp){
	if (size > 9){
		PopRecord();
	}
	size ++;
	storage[size-1] = temp;
}
/** Remove the oldest record off stack. */
void BoatPositionLog::PopRecord(){
	for(int x = 0; x < size; x++){
		storage[x] = storage[x+1];
	}
	size --;
}
/** returns a log item */
void BoatPositionLog::GetLogItem(int item, position &loc){
	loc = storage[item];
}
