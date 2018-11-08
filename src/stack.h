/***************************************************************************
                          stack.h  -  description
                             -------------------
    begin                : Sat Mar 16 2002
    copyright            : (C) 2002 by Michael Bridak
    email                : michael.bridak@verizon.net
$Id: stack.h,v 1.4 2003/04/14 05:51:04 mbridak Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License.     *
 *    .                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef STACK_H
#define STACK_H

/**Holds sound information for the SONAR
  *@author Michael Bridak
  * Re-written to use linked list instead of array
  * by Jesse
  */


#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

typedef struct
{
  int direction, ship_type;
  float signal_strength;
  void *next;
} STACK_DATA;

class Stack {
public:
	/** how many events for this second*/
	int counter;
	/** storage space */
	STACK_DATA *stack, *top_of_stack;
	/** Advance sound history, drop old events */
	void AdvanceSonarHistory();
	/** Place a sound event on in the Queue */
	int RegisterEvent(int direction, float signalstrength, int ship_type);
	/** Get how many events are in the Queue for a particular second */
	int  GetCount();
	/** Get sound information */
	int GetEvent(int event, int *Direction, float *SignalStrength, int *ship_type);
	Stack();
	~Stack();
};

#endif
