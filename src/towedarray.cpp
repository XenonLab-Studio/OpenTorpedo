/***************************************************************************
                          towedarray.cpp  -  description
                             -------------------
    begin                : Sat Mar 16 2002
    copyright            : (C) 2002 by Michael Bridak
    email                : michael.bridak@verizon.net
$Id: towedarray.cpp,v 1.3 2003/04/14 05:51:04 mbridak Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License.     *
 *    .                                   *
 *                                                                         *
 ***************************************************************************/

#include "towedarray.h"
#include "math.h"

TowedArray::TowedArray(){
	length = 0;
        cut = FALSE;
}
TowedArray::TowedArray(double latitude, double longitude){
	for (int counter=0; counter<100; counter--){
		latitude_array[counter] =  latitude;
		longitude_array[counter] =  longitude;
	}
	length = 0;
        cut = FALSE;
}
TowedArray::~TowedArray(){
}
void TowedArray::RecordPos(double latitude, double longitude){
	for (int counter=99; counter>0; counter--){
		latitude_array[counter] = latitude_array[counter-1];
		longitude_array[counter] = longitude_array[counter-1];
	}
	latitude_array[0] = latitude;
	longitude_array[0] = longitude;
}
int TowedArray::Extend(){
        if (! cut)
        {
	   winch = 1;
           return TRUE;
        }
        return FALSE;
}
void TowedArray::Stop(){
	winch = 0;
}
int TowedArray::ReelIn(){
    if (! cut)
    {
	winch = 2;
        return TRUE;
    }
    return FALSE;
}
int TowedArray::CutArray()
{
    // if the array is already cut or reeled in, we can't cut it
    if (cut)
       return FALSE;
    if (length == 0)
       return FALSE;
    cut = TRUE;
    length = 0;
    winch = 0;
    return TRUE;
}
int TowedArray::GetLength(){
return (int)length;
}
void TowedArray::OperateWinch(){
	switch(winch){
		case 0: //Winch is stopped, Do nothing.
			break;
		case 1: //Extend the winch.
			if(length<2600){
				length += 5; //5ft per second @ 1/5th sec interval
			}
			break;
		case 2: //Retract the winch.
			if(length>0){
				length -= 5;
			}
			break;
		default: //just in case.....
			//cerr << "Uh, Capt'n somethings wrong with the winch." << endl;
			break;
	}
}

void TowedArray::ReturnLatLon(double &rlat, double &rlon){
	int index;
	index = (int)((length/3)/10);
	rlat =  latitude_array[index];
	rlon = longitude_array[index];
}

float TowedArray::ReturnHeading(){
	int head_index;
	int tail_index;
	float bearing; //atan() needs doubles
	bearing =-1; //if the array is not extended enough
							//bearing will return as a neg #
	if(length > 240){
		tail_index = (int)((length/3)/10);
		head_index = tail_index - 8;
		bearing = CalcBearing(latitude_array[tail_index], longitude_array[tail_index], latitude_array[head_index], longitude_array[head_index]);
	}
	return bearing;
}

float TowedArray::BearingAmbiguity(float bearing){

	float ambiguous_bearing;
	float our_bearing;
	float bearing_difference;
	our_bearing=ReturnHeading();
	bearing_difference = our_bearing - bearing;
	if(bearing_difference < 0) bearing_difference += 360;
	
	ambiguous_bearing = our_bearing + bearing_difference;
	return ambiguous_bearing;
}

float TowedArray::BearingToTarget(double target_latitude, double target_longitude){

	double array_latitude, array_longitude, array_heading, actual_bearing;
	float bearing = -1;

	if(GetLength() > 240){
		array_heading = ReturnHeading();
		ReturnLatLon(array_latitude, array_longitude);
		actual_bearing = CalcBearing(array_latitude, array_longitude, target_latitude, target_longitude);
		if(array_heading > actual_bearing){
			actual_bearing += 360.0;
		}
	bearing = actual_bearing;
	}
	return bearing;
}

float TowedArray::CalcBearing(double observer_latitude, double observer_longitude ,double target_latitude,double target_longitude){

	double latdif=0, londif=0, bearing=0; //atan() needs doubles

	if (observer_latitude > target_latitude){
		latdif = observer_latitude - target_latitude;
	}else{
		latdif = target_latitude - observer_latitude;
	}
	if (observer_longitude > target_longitude){
		londif = observer_longitude - target_longitude;
	}else{
		londif = target_longitude - observer_longitude;
	}

	if ((observer_longitude < target_longitude) &&
		(observer_latitude < target_latitude)){
		bearing = (360 - ((atan(latdif / londif) * 360) / 6.28318530717958647692));
	}
	if ((observer_longitude < target_longitude) &&
		(observer_latitude > target_latitude)){
		bearing = (0 + ((atan(latdif / londif) * 360) / 6.28318530717958647692));
	}
	if ((observer_longitude > target_longitude) &&
		(observer_latitude < target_latitude)){
		bearing = (180 + ((atan(latdif / londif) * 360) / 6.28318530717958647692));
	}
	if ((observer_longitude > target_longitude) &&
		(observer_latitude > target_latitude)){
		bearing = (180 - ((atan(latdif / londif) * 360) / 6.28318530717958647692));
	}
	if (londif == 0){
		if (observer_latitude > target_latitude){
			bearing = 90;
		}else{
			bearing = 270;
		}
	}
	if (latdif == 0){
		if (observer_longitude > target_longitude){
			bearing = 180;
		}else{
			bearing = 0;
		}
	}
	return (float)bearing;
}

double TowedArray::RangeToTarget(double target_latitude, double target_longitude)
{
	//Calculates the distance in yards from the towed array, to the target.

	double observer_latitude, observer_longitude;
	double latdif = 0, londif = 0;

	ReturnLatLon(observer_latitude, observer_longitude);

	if (observer_latitude > target_latitude){
		latdif = observer_latitude - target_latitude;
	}else{
		latdif = target_latitude - observer_latitude;
	}
	if (observer_longitude > target_longitude){
		londif = observer_longitude - target_longitude;
	}else{
		londif = target_longitude - observer_longitude;
	}
	return sqrt((latdif * latdif) + (londif * londif));
}

