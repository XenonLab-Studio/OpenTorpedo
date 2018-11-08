/***************************************************************************
                          contact.cpp  -  description
                             -------------------
    begin                : Sat Mar 16 2002
    copyright            : (C) 2002 by Michael Bridak
    email                : michael.bridak@verizon.net
$Id: contact.cpp,v 1.4 2003/05/20 17:36:04 mbridak Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License.     *
 *    .                                   *
 *                                                                         *
 ***************************************************************************/

#include "contact.h"

Contact::Contact(){
	//Fill in some inital values
	Active=false;	//Contact not active
	Friend=2;		//Unknown alliance
	ShipType=1;		//Surface contact
}
Contact::~Contact(){
}
void Contact::UpdateContact(){
if(Active) UpdateLatLon();
}
