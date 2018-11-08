/***************************************************************************
                          adcap.h  -  description
                             -------------------
    begin                : Sat Mar 16 2002
    copyright            : (C) 2002 by Michael Bridak
    email                : mcbib@jps.net
$Id: adcap.h,v 1.7 2003/04/14 05:51:03 mbridak Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License.     *
 *    .                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ADCAP_H
#define ADCAP_H

#include "coord.h"

/**
  *@author Michael Bridak
  */

class AdCap : public Coord  {
public:
	int ActiveSearch; /*Future flag to activate the seeker head*/
	int WireGuided; /*Is mk48 still got the wire attached?*/
	int DesiredHeading; /*Where I want to go*/
	int CurrentHeading; /*I'm headed this way...Get Out Of The Way!*/
	int SwimoutSpeed; /*Should we creep out or go balls out?*/
	int AttackSpeed; /*Speed up to me once active*/
	int Aquired; /*Target aquired or do a search pattern*/
	int RunningTime; /*How long can I swim for*/
	int RunToEnable; /*Distance to swim before activating*/
	AdCap();
	~AdCap();
};

#endif
