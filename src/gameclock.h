/***************************************************************************
                          gameclock.h  -  description
                             -------------------
    begin                : Sun May 18 19:17:34 2003
    copyright            : (C) 2003 Michael Bridak
    email                : michael.bridak@verizon.net
$Id: gameclock.h,v 1.2 2003/05/20 17:36:04 mbridak Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License.     *
 *    .                                   *
 *                                                                         *
 ***************************************************************************/
 
#ifndef _GAMECLOCK_H_
#define _GAMECLOCK_H_


class GameClock
{
	public:
		GameClock();
		 ~GameClock();
		void InitTime(int hours, int minutes, int seconds);
		void UpdateTime();
		void GetTime(int &hours, int &minutes, int &seconds);
		int GetTick();	
	private:
		int hours;
		int minutes;
		int seconds;
		int ticks;
};


#endif	//_GAMECLOCK_H_

