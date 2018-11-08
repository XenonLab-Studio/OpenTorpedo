#ifndef MISSION_HEADER_FILE__
#define MISSION_HEADER_FILE__

#include "submarine.h"

#define MISSION_STARTED 1
#define MISSION_SUCCESS 2
#define MISSION_FAILED 3

// Go through all ships and see if they still have a mission
// flag. 
// If all mission flags have been set and the mission has not
// already failed, then mark the mission a success.
// This function returns the new mission status.
int Mission_Status(Submarine *all_subs, int old_status);


// This function checks to see if a ship has its mission status
// set to MISSION_FIND. If it does, then we check to see if the
// player can detect the ship. If the player can, we clear the
// ship's mission flag. This function returns TRUE normally or
// FALSE if an error occurs.
int Check_Find(Submarine *all_subs);

// This function checks to see if any vessels need to
// remain alive to complete the mission. The function
// checks the alive timer and reduces it. If the mission
// timer reaches zero (0), then we change the status 
// of the vessel to MISSION_NONE from MISSION_ALIVE.
// Returns TRUE if everything goes normally or FALSE
// if an error occurs.
int Check_Alive(Submarine *all_subs);

#endif

