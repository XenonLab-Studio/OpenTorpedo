/***************************************************************************
                          stack.cpp  -  description
                             -------------------
    begin                : Sat Mar 16 2002
    copyright            : (C) 2002 by Michael Bridak
    email                : michael.bridak@verizon.net
$Id: stack.cpp,v 1.5 2003/04/14 05:51:04 mbridak Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License.     *
 *    .                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdlib.h>
#include "stack.h"

Stack::Stack()
{
	counter = 0;
        stack = NULL;
        top_of_stack = NULL;
}


Stack::~Stack()
{
  STACK_DATA *my_stack, *previous;

  my_stack = stack;
  while (my_stack)
  {
     previous = my_stack;
     my_stack = (STACK_DATA *) my_stack->next;
     free(previous);
  }
}

int Stack::RegisterEvent(int direction, float signal, int ship_type)
{
    STACK_DATA *current;

    current = (STACK_DATA *) calloc(1, sizeof(STACK_DATA));
    if (! current)
       return FALSE;

    current->direction = direction;
    current->signal_strength = signal;
    current->ship_type = ship_type;
    if (top_of_stack)
    {
        top_of_stack->next = current;
        top_of_stack = current;
    }
    else   // nothing in stack
    {
        top_of_stack = stack = current;
    }
    counter++;
    return TRUE;
}

int Stack::GetEvent(int event, int *rdirection, float *rsignal, int *ship_type)
{
    int index = 1;
    STACK_DATA *current;

    current = stack;
    while ( (current) && (index < event) )
    {
        current = (STACK_DATA *) current->next;
        index++;
    }

    if ( (index == event) && (current) )
    {
        *rdirection = current->direction;
        *ship_type = current->ship_type;
        *rsignal = current->signal_strength;
        return TRUE;
    }
    else
      return FALSE;
}

int Stack::GetCount()
{
	return counter;
}

void Stack::AdvanceSonarHistory()
{
   STACK_DATA *current, *previous;
   current = stack;
   while (current)
   {
      previous = current;
      current = (STACK_DATA *) current->next;
      free(previous);
   }
   stack = top_of_stack = NULL;
   counter = 0;
}


