/***************************************************************************
                          message.h  -  description
                             -------------------
    begin                : Monday Jan 13 2003
    copyright            : (C) 2003 by Michael Bridak
    email                : michael.bridak@verizon.net
$Id: message.h,v 1.3 2003/04/20 16:44:30 mbridak Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License.     *
 *    .                                   *
 *                                                                         *
 ***************************************************************************/
 
#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

/**
  *@author Michael Bridak
  */

class msg {
public:	
	char buffer[20][256];
	
	void InitGraphics();

	/**	Places a one line message into the
		message buffer to be later displayed
		by the display_message() method.*/
	void post_message(char *temp_string);
	
	/**	Displays the tail end of the buffer
		onto the main screen.*/
	void display_message();
	
	/**	Our constructor....*/ 
	msg();
	
	/**	And the probably never to be used destructor..*/
	~msg();
private:
	/**	Our main storage for buffer text.*/

	SDL_Surface *screen;
	SDL_Surface *messagewindow;
	DFont largeFont(const char*, const char *);
	Uint32	textcolor;
	Uint32	black;
	Uint32	white;
	Uint32	red;
	Uint32	green;
	Uint32	yellow;
	
};

#endif
