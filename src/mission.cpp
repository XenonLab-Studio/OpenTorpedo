#include <stdio.h>
#include "mission.h"


// This function checks to see what our missions status should be. It
// returns MISSION_STARTED, MISSION_SUCCESS or MISSION_FAILED
// The old_status should also be one of these, indicating what our
// status was before this function was called.
// For example "my_mission = Mission_Status(Subs, my_mission);"
int Mission_Status(Submarine *all_subs, int old_status)
{
   Submarine *current_sub;
   int found;

   // if the mission already failed it is too late to save it
   if (old_status == MISSION_FAILED)
      return MISSION_FAILED;

   if (! all_subs)
     return MISSION_FAILED;

   // go through each sub and check its status
   current_sub = all_subs;
   found = FALSE;
   while ( (current_sub) && (! found) )
   {
       if (current_sub->mission_status == MISSION_NONE)
       {
          #ifdef DEBUG
          printf("Moving to next ship that might have a mission.\n");
          #endif
          current_sub = current_sub->next;
       }
       else
       {
          #ifdef DEBUG
          printf("Found ship that still has a mission.\n");
          #endif
          found = TRUE;
       }
   }

   // mission not complete
   if (found)
       return MISSION_STARTED;
   return MISSION_SUCCESS;
}



int Check_Find(Submarine *all_subs)
{
  Submarine *player, *current_sub;
  int signal_strength;

  if (! all_subs)
     return FALSE;

  player = all_subs;
  current_sub = all_subs->next;
  while (current_sub)
  {
      // we are supposed to be found?
      if (current_sub->mission_status == MISSION_FIND)
      {
         #ifdef DEBUG
         printf("This ship wants to be found.\n");
         #endif
         signal_strength = player->Can_Detect(current_sub);
         // we can hear you 
         if (signal_strength == CONTACT_SOLID)
         {
           #ifdef DEBUG
           printf("This ship has a contact level of %d\n", signal_strength);
           #endif
           current_sub->mission_status = MISSION_NONE;
         }
      }
      current_sub = current_sub->next;
  }

  return TRUE;
}



// This function checks to see if the mission depends on
// any ships remaining alive for a given amount of time.
// Any ships which have the MISSION_ALIVE flag set will
// have their timer reduced. If the timer hits zero (0)
// then we change the mission flag to be MISSION_NONE.
// This function returns TRUE if everything goes normally
// or FALSE if we encounter an error.
int Check_Alive(Submarine *all_subs)
{
   Submarine *current;

   if (! all_subs)
      return FALSE;

   current = all_subs;
   while (current)
   {
      if (current->mission_status == MISSION_ALIVE)
      {
          current->mission_timer--;
          if (current->mission_timer < 1)
             current->mission_status = MISSION_NONE;
      }
      current = current->next;
   }

   return TRUE;
}
