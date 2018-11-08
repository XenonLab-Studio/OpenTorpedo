// $Id: dstack.cpp,v 1.4 2003/04/14 05:51:03 mbridak Exp $
// Simple Stack Class                 

#include <stdio.h>
#include <stdlib.h>
#include "dstack.h"

DStack::DStack()               
{                                  
   bottom_ = NULL;
   top_ = NULL;
   number_items = 0;
}

DStack::~DStack()                   // how to reclaim memory from local
{                                   //   stacks when exiting functions
   clear_stack(); 
}


// erase any/all items on the stack
void DStack::clear_stack()
{
   STACKVALUE *current, *next_value;
   current = bottom_;
   while (current)
   {
       next_value = (STACKVALUE *) current->next;
       free(current);
       current = next_value;
   }

   bottom_ = top_ = NULL;
   number_items = 0;
}


int DStack::num_items()        // number of items currently in stack
{
    return number_items;
}

int DStack::push(float val)        // push a new value 
{
    STACKVALUE *new_value;

    new_value = (STACKVALUE *) calloc(1, sizeof(STACKVALUE));
    if (! new_value)
      return FALSE;
    new_value->value = val;
    if (top_)
    {
        top_->next = new_value;
        new_value->previous = top_;
        top_ = new_value;
    }
    else   // no values exist
    {
      bottom_ = top_ = new_value;
    }
    number_items++;
    return TRUE;
}

float DStack::pop()                 // pop value from top
{
   float my_return = 0.0;
   STACKVALUE *new_top = NULL;

   if (top_)
   {
      my_return = top_->value;
      new_top = (STACKVALUE *) top_->previous; 
      free(top_);
      top_ = new_top;
      number_items--;
      if (! top_)
        bottom_ = NULL;
   }
   return my_return;
}



int DStack::empty()           // 1 if empty, 0 otherwise
{
   if (number_items)
      return FALSE;
   else
      return TRUE; 
}



void DStack::print()
{
   STACKVALUE *current;
   printf("Stack holds %d items.\n", number_items);
   current = bottom_;
   while (current)
   {
       printf("Value: %f\n", current->value);
       current = (STACKVALUE *) current->next;
   }
}

