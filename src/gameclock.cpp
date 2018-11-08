/***************************************************************************
                          gameclock.cpp  -  description
                             -------------------
    begin                : Sun May 18 19:17:34 2003
    copyright            : (C) 2003 Michael Bridak
    email                : michael.bridak@verizon.net
$Id: gameclock.cpp,v 1.1 2003/05/19 02:26:36 mbridak Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License.     *
 *    .                                   *
 *                                                                         *
 ***************************************************************************/

#include <iostream>
#include "gameclock.h"

using namespace std;

GameClock::GameClock()
{
	hours=0;
	minutes=0;
	seconds=0;
	ticks=0;
}

GameClock::~GameClock()
{
}

void GameClock::InitTime(int hrs, int min, int sec)
{
	hours=hrs;
	minutes=min;
	seconds=sec;
}

void GameClock::UpdateTime()
{
	ticks++;
	seconds++;
	if(seconds > 59){
		minutes++;
		seconds=0;
	}
	if(minutes > 59){
		hours++;
		minutes=0;
	}
	if(hours > 23){
		hours=0;
	}
}

void GameClock::GetTime(int &hrs, int &min, int &sec)
{
	hrs=hours;
	min=minutes;
	sec=seconds;
}

int GameClock::GetTick()
{
	return ticks;
}
