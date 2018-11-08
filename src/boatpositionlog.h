/***************************************************************************
                          boatpositionlog.h  -  description
                             -------------------
    begin                : Sat Mar 16 2002
    copyright            : (C) 2002 by Michael Bridak
    email                : michael.bridak@verizon.net
$Id: boatpositionlog.h,v 1.2 2003/04/14 05:51:03 mbridak Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License.     *
 *    .                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef BOATPOSITIONLOG_H
#define BOATPOSITIONLOG_H


/**A stack to hold our intermitent boat position
  *@author Michael Bridak
  */

class BoatPositionLog {
public:
	struct position {
		float lat;
		float lon;
		double bearing;
		Uint32 tick;
	};
	BoatPositionLog();
	~BoatPositionLog();
	  void RecBoatPos(position temp);
  /** Remove the oldest record off stack. */
  void PopRecord();
  /** returns a log item */
  void GetLogItem(int item, position &loc);

  /**  */
	position storage[10];

  /** number of items on the stack
 */
  int size;
};

#endif
