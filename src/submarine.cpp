/***************************************************************************
                          submarine.cpp  -  description
                             -------------------
    begin                : Sat Mar 16 2002
    copyright            : (C) 2002 by Michael Bridak
    email                : michael.bridak@verizon.net
$Id: submarine.cpp,v 1.6 2003/04/14 05:51:04 mbridak Exp $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License.     *
 *    .                                   *
 *                                                                         *
 ***************************************************************************/

#include <iostream>
#include <fstream>
#include <math.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "submarine.h"
#include "files.h"
#include "sound.h"
#include "map.h"
#ifdef WIN32
#include "winfunctions.h"
#endif

using namespace std;

Submarine::Submarine()
{
    Init();
}

Submarine::~Submarine()
{
}



void Submarine::Init()
{
        int index;

        //Passive Sonar Cross Section
        //in future make different pscs's
        //based on angle of view, front
        //side and rear.

        PSCS=83.0;
        Active = FALSE;
        CavitationFlag=0;
        Depth=0;
        MaxDepth=999;
        Speed=5.0;
        MaxSpeed=32;
        MinSpeed=-12;
        DesiredSpeed=0;
        Rudder=10;
        TorpedosOnBoard=24;
        NoiseMakers = 12;
        ShipClass = CLASS_MERCHANT;
        // clear targets
        targets = NULL;
        last_target = NULL;
        current_target = -1;
        for (index = 0; index < MAX_TUBES; index++)
           torpedo_tube[index] = TUBE_EMPTY;
        next = NULL;
        target = NULL;
        owner = NULL;
        fuel_remaining = INT_MAX; 
        hull_strength = 1;
        has_sonar = TRUE;
        mission_status = MISSION_NONE;
        mission_timer = 0;
        mood = MOOD_PASSIVE;
        convoy_course_change = CONVOY_CHANGE_COURSE;
        radio_message = RADIO_NONE;
        pinging = FALSE;
        using_radar = FALSE;
        map = NULL;
        ClassName[0] = '\0';
        ClassType[0] = '\0';
}



float Submarine::RadiatedNoise(){
	//This will return how noisey you are

	float	unit=40;
	float	multiplier=4.2;
	float	value=0;

	CheckForCavitation();
	value = PSCS + (unit * CavitationFlag) + (((CheckNegSpeed(Speed)>4.9)*multiplier) * CheckNegSpeed(Speed));
        if (pinging)
          value += PING_NOISE;
	return value;
}

float Submarine::CheckNegSpeed(float value){
	if(value < 0){
		return 100.0 - (100.0 + value);
	}else{
		return value;
	}
}
void Submarine::CheckForCavitation(){

	//Check to see if we are cavitating...
	// 4-5kts @ 0-100ft.
	// 5-14kts @ 100-150ft
	// 14-35kts @ 150-600ft

	CavitationFlag = 0;

	if ((Depth <100) && (CheckNegSpeed(Speed) > 4.0)){
		if ((CheckNegSpeed(Speed) - 4.0) > (float) Depth / 100.0){
			CavitationFlag = 1;
		}
		return;
	}

	if ((Depth < 150) && (CheckNegSpeed(Speed) > 5.0)){
		if (((CheckNegSpeed(Speed) - 5.0) * 5.555) > (Depth -100)){
			CavitationFlag = 1;
		}
		return;
	}

	if ((Depth < 600) && (CheckNegSpeed(Speed) > 14.0)){
		if (((CheckNegSpeed(Speed) - 14.0) * 21.428) > (Depth - 150)){
			CavitationFlag = 1;
		}
		return;
	}
}
double Submarine::BearingToTarget(Submarine *Target){
	double latdif=0, londif=0, bearing = 0; //atan() needs doubles
	//LatLonDifference(observer, target, &latdif, &londif);

        if (! Target)
           return 0;
	if (Lat_TotalYards > Target->Lat_TotalYards){
		latdif = Lat_TotalYards - Target->Lat_TotalYards;
	}
	else{
		latdif = Target->Lat_TotalYards - Lat_TotalYards;
	}

	if (Lon_TotalYards > Target->Lon_TotalYards){
		londif = Lon_TotalYards - Target->Lon_TotalYards;
	}
	else{
		londif = Target->Lon_TotalYards - Lon_TotalYards;
	}

	if ((Lon_TotalYards < Target->Lon_TotalYards) &&
	(Lat_TotalYards < Target->Lat_TotalYards)){
		bearing = (360 - ((atan(latdif / londif) * 360) / 6.28318530717958647692));
	}
	else if ((Lon_TotalYards < Target->Lon_TotalYards) &&
	(Lat_TotalYards > Target->Lat_TotalYards)){
		bearing = (0 + ((atan(latdif / londif) * 360) / 6.28318530717958647692));
	}
	else if ((Lon_TotalYards > Target->Lon_TotalYards) &&
	(Lat_TotalYards < Target->Lat_TotalYards)){
		bearing = (180 + ((atan(latdif / londif) * 360) / 6.28318530717958647692));
	}
	else if ((Lon_TotalYards > Target->Lon_TotalYards) &&
	(Lat_TotalYards > Target->Lat_TotalYards)){
		bearing = (180 - ((atan(latdif / londif) * 360) / 6.28318530717958647692));
	}
	if (londif == 0){
		if (Lat_TotalYards > Target->Lat_TotalYards){
			bearing = 90;
		}else{
			bearing = 270;
		}
	}
	if (latdif == 0){
		if (Lon_TotalYards > Target->Lon_TotalYards){
			bearing = 180;
		}else{
			bearing = 0;
		}
	}
	return bearing;
}


double Submarine::BearingToOrigin(Submarine *Target)
{
        double latdif=0, londif=0, bearing = 0; //atan() needs doubles
        //LatLonDifference(observer, target, &latdif, &londif);

        if (! Target)
           return 0;
        if (Lat_TotalYards > Target->origin_x){
                latdif = Lat_TotalYards - Target->origin_x;
        }
        else{
                latdif = Target->origin_x - Lat_TotalYards;
        }

        if (Lon_TotalYards > Target->origin_y){
                londif = Lon_TotalYards - Target->origin_y;
        }
        else{
                londif = Target->origin_y - Lon_TotalYards;
        }

        if ((Lon_TotalYards < Target->origin_y) &&
        (Lat_TotalYards < Target->origin_x)){
                bearing = (360 - ((atan(latdif / londif) * 360) / 6.28318530717958647692));
        }
        if ((Lon_TotalYards < Target->origin_y) &&
        (Lat_TotalYards > Target->origin_x)){
                bearing = (0 + ((atan(latdif / londif) * 360) / 6.28318530717958647692));
        }
        if ((Lon_TotalYards > Target->origin_y) &&
        (Lat_TotalYards < Target->origin_x)){
                bearing = (180 + ((atan(latdif / londif) * 360) / 6.28318530717958647692));
        }
        if ((Lon_TotalYards > Target->origin_y) &&
        (Lat_TotalYards > Target->origin_x)){
                bearing = (180 - ((atan(latdif / londif) * 360) / 6.28318530717958647692));
        }

        if (londif == 0){
                if (Lat_TotalYards > Target->origin_x){
                        bearing = 90;
                }else{
                        bearing = 270;
                }
        }
        if (latdif == 0){
                if (Lon_TotalYards > Target->origin_y){
                        bearing = 180;
                }else{
                        bearing = 0;
                }
        }
        return bearing;

}



double Submarine::DistanceToTarget(Submarine *Target){
	double latdif = 0, londif = 0; //sqrt needs doubles
	//LatLonDifference( observer,  target,  &latdif,  &londif );

        if (! Target)
            return 0;

	if (Lat_TotalYards > Target->Lat_TotalYards){
		latdif = Lat_TotalYards - Target->Lat_TotalYards;
	}
	else{
		latdif = Target->Lat_TotalYards - Lat_TotalYards;
	}

	if (Lon_TotalYards > Target->Lon_TotalYards){
		londif = Lon_TotalYards - Target->Lon_TotalYards;
	}
	else{
		londif = Target->Lon_TotalYards - Lon_TotalYards;
	}
	return sqrt((latdif * latdif) + (londif * londif));
}

float Submarine::DEAngle(Submarine *Target){
	double distance = 0.0;
	int depthDifference = 0;
	double deang = 0.0;

	distance = DistanceToTarget(Target);

	if (Depth < Target->Depth){
		depthDifference = (int)(Target->Depth - Depth);
	}else{
		depthDifference = (int)(Depth - Target->Depth);
	}

     if (depthDifference != 0) depthDifference = depthDifference * 3;

	if (distance > depthDifference){
		deang =  (atan(distance / depthDifference) * 360) / 6.283185307179;
	}else{
		deang =  (atan(depthDifference / distance) * 360) / 6.283185307179;
	}

	if (Depth > Target->Depth){
		deang = 90.0 - deang;
	}else{
		deang = deang - 90.0;
	}

	return (float)deang;

}



void Submarine::Handeling(){

	// This is where we change the ships characteristics 
        // based on the desired variables
	// First we'll do depth

	// float UpBubble; //The greater the amount the faster the change.
	// float DownBubble;
	float AmountOfChange; //How much to turn the boat.
        float temp_speed = Speed;
        float delta_depth = 0.0;

        if (temp_speed < 0)
            temp_speed = -temp_speed;
        // keep desired depth sane
        if (DesiredDepth < 0)
            DesiredDepth = 0;
        else if (DesiredDepth > MaxDepth)
            DesiredDepth = MaxDepth;

        delta_depth = temp_speed * PLANES_CHANGE;
        if (delta_depth < 1.0)
           delta_depth = 1.0;
        else if (delta_depth > 5.0)
           delta_depth = 5.0;
        if (ShipType == TYPE_TORPEDO)
           delta_depth *= 2.0;

	if (DesiredDepth > Depth){  //Do we need to go up?
		// UpBubble = 0.05; //5% up bubble if we're close
		// if ((DesiredDepth - Depth) > 20){
		//	UpBubble = 0.1; //10% Up Bubble
		// }
                // UpBubble *= 4;
		// Depth += ((temp_speed * PLANES_CHANGE) * UpBubble); //feet per second @ 1kt = 0.185
                Depth += delta_depth;
		if (Depth > DesiredDepth){         //if we have risen past the desired depth
			Depth = DesiredDepth;     //Flatten us out
		}
	}

	if (DesiredDepth < Depth){  //Do we need to go down?
		// DownBubble= 0.05; //5% Down Bubble if we're close
		// if ((Depth - DesiredDepth) >20){
		//	DownBubble = 0.1; //10% down bubble
		// }
                // UpBubble *= 4;
		// Depth -= ((temp_speed * PLANES_CHANGE) * DownBubble); 
                Depth -= delta_depth;
		if (Depth < DesiredDepth){
			Depth = DesiredDepth;
		}
	}

	//Change Heading

	// AmountOfChange = (Rudder * Speed) * 0.012;
        AmountOfChange = (Rudder * temp_speed) * RUDDER_CHANGE;
        #ifdef DEBUG
        printf("Rudder change %f\n", AmountOfChange);
        #endif
	if (Heading > DesiredHeading){
		if ((Heading - DesiredHeading) < 180){
			Heading = Heading - AmountOfChange;
			if ((Heading < DesiredHeading) &&
			((DesiredHeading - Heading) < AmountOfChange)){
				Heading = (float)DesiredHeading;
			}
		}
		else{
			Heading = Heading + AmountOfChange;
			if ((Heading > DesiredHeading) &&
			((Heading - DesiredHeading) < AmountOfChange)){
				Heading = (float)DesiredHeading;
			}
		}
	}
	else{
		if (Heading < DesiredHeading){
			if ((DesiredHeading - Heading) < 180){
				Heading += AmountOfChange;
				if ((Heading > DesiredHeading) &&
				((Heading - DesiredHeading) < AmountOfChange)){
					Heading = (float)DesiredHeading;
				}
			}
			else{
				Heading = Heading - AmountOfChange;
				if ((Heading < DesiredHeading) &&
				((DesiredHeading - Heading) < AmountOfChange)){
					Heading = (float)DesiredHeading;
				}
			}
		}
	}

	if (Heading > 360){
		Heading = Heading - 360.0;
	}
	else{
		if (Heading < 0){
			Heading = Heading + 360.0;
		}
	}

	// Change Speed

	if(DesiredSpeed > MaxSpeed) DesiredSpeed = MaxSpeed;
	if(DesiredSpeed < MinSpeed) DesiredSpeed = MinSpeed;
	if (DesiredSpeed > Speed){ //Speed Up
		Speed += 0.45; //a little less than 1/2 a knot per second..
		if (Speed > DesiredSpeed) Speed = DesiredSpeed; // Did we go past target speed?
	}

	if (Speed > DesiredSpeed){ //Slow Down
		Speed -= 0.45;
		if (Speed < DesiredSpeed) Speed=DesiredSpeed; // Did we slow too much?
	}

}



// We can detect a new target
// Make sure it is on the list.
// Function returns TRUE on success or FALSE on error
int Submarine::Add_Target(Submarine *new_sub, float signal_strength)
{
   int add_contact, found = FALSE;
   Target *my_target, *new_target, *last_target = NULL;

   if (signal_strength == 2.0)
      add_contact = CONTACT_WEAK;
   else if (signal_strength == 1.0)
      add_contact = CONTACT_WEAK;
   else if (signal_strength == CONTACT_PING)
      add_contact = CONTACT_WEAK;
   else
      add_contact = 1;

   // check to see if target is already in the list
   my_target = targets;
   while ( (my_target) && (! found) )
   {
       if (my_target->sub == new_sub)
          found = TRUE;
       else
       {
          last_target = my_target;
          my_target = (Target *) my_target->next;
       }
   }   // end of looking for existing target

   if (found)
   { 
      if ( (signal_strength == CONTACT_PING) && 
           (my_target->contact_strength < CONTACT_WEAK) )
         my_target->contact_strength = CONTACT_WEAK + 2;
      else if (signal_strength == CONTACT_PING)
         my_target->contact_strength = my_target->contact_strength;
      else if ( (add_contact == CONTACT_WEAK) &&
                (my_target->contact_strength >= CONTACT_WEAK) )
         my_target->contact_strength = my_target->contact_strength;
      else
         my_target->contact_strength += add_contact;
 
      if (my_target->contact_strength > CONTACT_SOLID)
         my_target->contact_strength = CONTACT_SOLID;
      return TRUE;
   }
   else // need new contact
   {
      #ifdef DEBUG
      printf("Adding new target\n");
      #endif
      new_target = (Target *) calloc( 1, sizeof(Target) );
      if (! new_target)
          return FALSE;
      new_target->sub = new_sub;
      new_target->contact_strength = add_contact;
      new_target->next = NULL;   // not needed, but let's be careful
      if (last_target)
         last_target->next = new_target;
      else
         targets = new_target;
   }
   return TRUE;
}



// Find and reduce a target from the list
void Submarine::Remove_Target(Submarine *old_sub)
{
   int found = FALSE;
   Target *my_target;

   my_target = targets;
   while ( (! found) && (my_target) )
   {
       if (my_target->sub == old_sub)
       {
          if (my_target->contact_strength < 1)
             Cancel_Target(old_sub);
          else
             my_target->contact_strength -= 2;
          found = TRUE;
       }
       else
          my_target = (Target *)my_target->next;
   }
}

// Find and completely remove a target from the list
void Submarine::Cancel_Target(Submarine *old_sub)
{
   int found = FALSE;
   Target *my_target, *previous = NULL;

   #ifdef DEBUG
   printf("Removing target we can't hear.\n");
   #endif
   my_target = targets;
   while ( (!found) && (my_target) )
   {
       if (my_target->sub == old_sub)
       {
         if (previous)
            previous->next = my_target->next;
         else   // first item in the list
           targets = (Target *) my_target->next;

         my_target->next = NULL;
         if (my_target == last_target)
            last_target = NULL;
         free(my_target);
         found = TRUE;
       }
       else
       {
          previous = my_target;
          my_target = (Target *)my_target->next;
       }
   }
}




// This function will let us know if a ship is on our target list
// Returns the signal strength if the ship exists or FALSE if it does not.
int Submarine::Can_Detect(Submarine *a_sub)
{
    int found = FALSE;
    Target *my_target;

    my_target = (Target *)targets;
    while ( (my_target) && (! found) )
    {
       if (my_target->sub == a_sub)
          found = TRUE;
       else
          my_target = (Target *)my_target->next;
    }
    if (found)
      return my_target->contact_strength;
    else 
      return FALSE;
}




// Find the next available target. Return the target
// pointer or NULL if no target is found
/*
Submarine *Submarine::Next_Target()
{
     Target *original = last_target;
     Target *my_target;

     if (! targets)
     {
         last_target = NULL;
         return NULL;    // no possible targets
     }
     if (! original)     // nothing selected before, return first item
     {
        last_target = targets;
        return (Submarine *)last_target->sub;
     }
     
     my_target = (Target *) original->next;
     if (my_target)
     {
        last_target = my_target;
        return (Submarine *)last_target->sub;
     }
     else
     {
        last_target = targets;
        return (Submarine *)last_target->sub;
     }
}
*/
Submarine *Submarine::Next_Target()
{
    Target *original = NULL;
    Target *my_target;
    int found = FALSE;

    // no possible targets to return
    if (! targets)
    {
        last_target = NULL;
        return NULL;
    }

    if (! last_target)
    {
       last_target = targets;
       my_target = last_target;
    }
    else   // we used to have a target
    {
       my_target = (Target *) last_target->next;
       if (! my_target)
          my_target = targets;
    }

    // starting from the last target, try to find something
    // we can use
    while ( (! found) && (my_target != original) )
    {
        #ifdef DEBUG
        printf("Testing signal strength: %d\n", my_target->contact_strength);
        #endif
        if (my_target->contact_strength >= CONTACT_WEAK)
           found = TRUE;
        else
        {
            if (! original)
               original = my_target;
            my_target = (Target *) my_target->next;
            if (! my_target)
               my_target = targets;
        }

     }     

     if (found)
     {
        #ifdef DEBUG
        printf("Returning new target.\n");
        #endif
        last_target = my_target;
        return (Submarine *) my_target->sub;
     }
     else
     {
        #ifdef DEBUG
        printf("No target found.\n");
        #endif
        last_target = targets;
        return NULL;
     }
}



// This function should be called right after the ship is created. It
// will load the ship's class specific information from the appropriate
// data file.
// The data file's format will be
// maxspeed maxdepth name rudder torpedoes-carried hull_strength has_sonar, PSCS class-name class-type
int Submarine::Load_Class(char *my_file)
{
    // confirm file exists
    if (! my_file)
      return FALSE;
    ifstream infile(my_file, ios::in);
    if (! infile)
       return FALSE;

    // load data
    infile >> MaxSpeed >> MaxDepth >> Rudder >> TorpedosOnBoard >> hull_strength >> has_sonar >> PSCS >> ClassName >> ClassType;
    infile.close();
    // if we carry torpedoes, we also carry noisemakers
    NoiseMakers = TorpedosOnBoard / 2;
    return TRUE;
}


// This function tries to load a single line from the passed file
// and translates the contents to the ship's mission.
// The function returns TRUE on success and FALSE if an
// error occurs.
int Submarine::Load_Mission(FILE *from_file)
{
    char line[256];
    char *status;

    if (! from_file)
       return FALSE;

    memset(line, '\0', 256);
    status = fgets(line, 256, from_file);
    if (status)
    {
    #ifndef WIN32
    if (! strncasecmp(line, "sink", 4) )
        mission_status = MISSION_SINK;
    else if (! strncasecmp(line, "find", 4) )
        mission_status = MISSION_FIND;
    else if (! strncasecmp(line, "alive", 5) )
        mission_status = MISSION_ALIVE;
    else
        mission_status = MISSION_NONE;
    #else
    if (! my_strncasecmp(line, "sink", 4) )
        mission_status = MISSION_SINK;
    else if (! my_strncasecmp(line, "find", 4) )
        mission_status = MISSION_FIND;
    else if (! my_strncasecmp(line, "alive", 5) )
        mission_status = MISSION_ALIVE;
    else
        mission_status = MISSION_NONE;
    #endif

    if (mission_status == MISSION_ALIVE)
        sscanf( &(line[6]), "%d", &mission_timer);
    }
    else    // could not read line, default to none
       mission_status = MISSION_NONE;
      
    return TRUE;
}




// This function will attempt to manipulate a torpedo tube. We can
// load a torpedo into a tube, load a noisemaker, empty a tube or
// fire the contents of the tube.
// The function returns a status number based on what happened or
// did not happen.
int Submarine::Use_Tube(int action, int tube_number)
{
   if ( (tube_number < 0) || (tube_number >= MAX_TUBES) )
      return TUBE_ERROR_NUMBER;

   switch (action)
   {
     case LOAD_TORPEDO:
          if (TorpedosOnBoard <= 0)
              return TUBE_ERROR_NO_TORPEDO;

          if (torpedo_tube[tube_number] == TUBE_EMPTY)
	  {
              torpedo_tube[tube_number] = TUBE_TORPEDO;
              TorpedosOnBoard--;
              return TUBE_ERROR_TORPEDO_SUCCESS;
          }
          else
            return TUBE_ERROR_FULL;
          break;
     case LOAD_NOISEMAKER:
          if (NoiseMakers <= 0)
               return TUBE_ERROR_NO_NOISE;
          if (torpedo_tube[tube_number] == TUBE_EMPTY)
          {
              torpedo_tube[tube_number] = TUBE_NOISEMAKER;
              NoiseMakers--;
              return TUBE_ERROR_NOISEMAKER_SUCCESS;
          }
          else
             return TUBE_ERROR_FULL;
          break;
      case UNLOAD_TUBE:
            if (torpedo_tube[tube_number] == TUBE_TORPEDO)
                TorpedosOnBoard++;
            else if (torpedo_tube[tube_number] == TUBE_NOISEMAKER)
                NoiseMakers++;
            torpedo_tube[tube_number] = TUBE_EMPTY;
            return TUBE_ERROR_UNLOAD_SUCCESS;
      case FIRE_TUBE:
            if (torpedo_tube[tube_number] == TUBE_EMPTY)
               return TUBE_ERROR_FIRE_FAIL;
            else if (torpedo_tube[tube_number] == TUBE_TORPEDO)
            {
               torpedo_tube[tube_number] = TUBE_EMPTY;
               return TUBE_ERROR_FIRE_SUCCESS;
            }
            else if (torpedo_tube[tube_number] == TUBE_NOISEMAKER)
            {
               torpedo_tube[tube_number] = TUBE_EMPTY;
               return TUBE_ERROR_FIRE_NOISEMAKER;
            } 
            break;

     default: 
          return TUBE_ERROR_ACTION;
   }

   return TRUE;     // this shouldn't happen, but just in case
}




// This function launches a torpedo or noise maker. If the
// passed target is NULL, the new torp/noisemaker is given a
// random course and no target, leaving it to run in a striaght line.
// On success the function passes back a pointer to the new torpedo
// and on failure NULL is returned.
Submarine *Submarine::Fire_Tube(Submarine *target, char *ship_file)
{
   Submarine *my_torp;
   my_torp = new Submarine();
   if (! my_torp)
       return NULL;

   my_torp->Load_Class(ship_file);
   if (target)
   {
       my_torp->target = target;
       // set heading and desired depth
       my_torp->DesiredHeading = my_torp->Heading = BearingToTarget(target);
       my_torp->DesiredDepth = target->Depth;
       my_torp->ShipType = TYPE_TORPEDO;
   }
   else // no target, noisemaker
   {
       my_torp->target = NULL;
       // set random heading
       my_torp->Heading = rand() % 360;
       my_torp->DesiredHeading = my_torp->Heading;
       my_torp->ShipType = TYPE_NOISEMAKER;
   }
   // set current position 
   my_torp->Lat_TotalYards = Lat_TotalYards;
   my_torp->Lon_TotalYards = Lon_TotalYards;
   // set fuel
   my_torp->fuel_remaining = TORPEDO_FUEL;
   // set depth 
   my_torp->Depth = Depth;
   // set speed and desired speed
   my_torp->Speed = Speed;
   my_torp->DesiredSpeed = my_torp->MaxSpeed;
   // my_torp->ShipType = TYPE_TORPEDO;
   // my_torp->Friend = FOE;
   my_torp->Friend = Friend;
   my_torp->origin_x = Lat_TotalYards;
   my_torp->origin_y = Lon_TotalYards;
   return my_torp;
}



// This function lets us know if we can hear another ship/sub/
// It returns TRUE if we can hear it and FALSE if we can not.
int Submarine::Can_Hear(Submarine *target)
{
        float Range = DistanceToTarget(target);
        float NauticalMiles = (float)Range / 2000.0;
        float HisPassiveSonarCrosssection = target->PSCS;
        float EffectiveTargetSpeed;
        float AmbientNoise;
        float OwnShipNoise;
        float TotalNoise;
        float TargetNoise;
        float Gb;
        float Lbp;
        float NoiseFromSpeed;
        float BasisNoiseLevel;
        float value;
        float SeaState = 3.0; // Anyone want to model the weather.
        float minimum_sound = -45.0;
        int thermal_layers = 0;

        // sanity check
        if (! target)
           return FALSE;

        /* There is a special case here. If the listener is a surface
           ship then they have radar. Thus the listener can always
           detect anything on or above the surface.
        */
        if (ShipType == TYPE_SHIP)
        {
           if (target->Depth <= 0)
              return TRUE;
           if (target->using_radar)
              return TRUE;
        }

        if (ShipType == TYPE_TORPEDO)
            minimum_sound *= 2.5;
        else if (ShipType == TYPE_SHIP)
            minimum_sound *= 1.75;

        if (target->Speed <= 5.0){
             EffectiveTargetSpeed = 0.0;
        }else{
             EffectiveTargetSpeed = target->Speed - 5.0;
        }

        if (target->Speed < 20.0){
             NoiseFromSpeed = 1.30;
             BasisNoiseLevel = 0.0;
        }else{
             NoiseFromSpeed = 0.65;
             BasisNoiseLevel = 9.75;
        }
        // check just in case we didn't set the map variable
        if (map)
        {
            thermal_layers = map->Thermals_Between(Depth, target->Depth);
            #ifdef DEBUGMAP
            printf("%d thermals between ship and target\n", thermal_layers);
            #endif
        }
        AmbientNoise = 89.0 + (5.0 * SeaState);
        OwnShipNoise = RadiatedNoise();
        TotalNoise = 10.0 * log10(pow(10.0,OwnShipNoise/10.0) + pow(10.0,AmbientNoise/10.0));
        Gb = (TotalNoise - AmbientNoise) / 2.9;
        Lbp = AmbientNoise + Gb;
        TargetNoise = HisPassiveSonarCrosssection +
        ((NoiseFromSpeed * EffectiveTargetSpeed) + BasisNoiseLevel);
        if (target->pinging)
           TargetNoise += PING_NOISE;
        if (thermal_layers)
           TargetNoise -= TargetNoise * (thermal_layers * THERMAL_FILTER);
        value = TargetNoise - (20.0 * log10(NauticalMiles) + 1.1 * NauticalMiles) - Lbp;
        // if (!observer)
        //      SonarStation.flowandambientnoise = (Lbp - 34);
        if (value > minimum_sound){
                return TRUE;
        }else{
                return FALSE;
        }

}




// This function will simply return if we do not have a target.
// If we do have a target, and we can hear it, this will adjust the
// torpedo's desired heading and desired depth to match the target.
// The function returns TRUE.
int Submarine::Torpedo_AI(Submarine *all_subs)
{
   int can_hear_target = FALSE;
   Submarine *my_sub;

   // if (! target)
   //    return TRUE;
   // Noisemakers should run at full speed in a straight line
   if (ShipType == TYPE_NOISEMAKER)
       return TRUE;

   // check to see if we can hear the target
   if (target)
      can_hear_target = Can_Hear(target);
   // aim for our target if we have one
   if (can_hear_target)
   {
     DesiredHeading = BearingToTarget(target);
     DesiredDepth = target->Depth;
     DesiredSpeed = MaxSpeed;
     return TRUE;
   }

   // when we delevop active sonar, that should go here
   // as a backup to passive sonar

   // we cannot hear our target, see if we can detect a new one
   target = NULL;
   my_sub = all_subs;
   while ( (my_sub) && (! target) )
   {  
      // aim for the first thing we hear 
      if ( (my_sub->Friend != Friend) && ( Can_Hear(my_sub) ) )
         target = my_sub;
      else
         my_sub = my_sub->next; 
   }

   // what if we can't hear any target, go into search mode
   if (! target)
   {
       DesiredHeading = Heading + 90;
       DesiredSpeed = MaxSpeed / 2;
   }
   return TRUE;
}



// This function tells us what AI ships and submarines will do.
// This function returns a link to all torpedoes.
Submarine *Submarine::Sub_AI(Submarine *all_ships, Submarine *all_torpedoes, void *helicopters)
{
   int change;
   Submarine *torpedo;
   int can_hear_torpedo;
   double distance;
   int bearing;
   Submarine *target = NULL, *my_torpedoes;
   int status, found;
   int action = 0;     // 1 = running, 2 = chasing
   Submarine *track_torpedo = NULL;

   // most important thing we can do is run away from torpedoes
   if (has_sonar)
   {
       #ifdef AIDEBUG
       printf("We have sonar.\n");
       #endif
       // go through all torpedoes and see if any of them
       // chasing us
       torpedo = all_torpedoes;
       status = FALSE;   // make sure we only run away form one torpedo
       while ( (torpedo) && (! status) )
       {
           if (torpedo->ShipType == TYPE_TORPEDO) 
           {
           can_hear_torpedo = Can_Hear(torpedo);
           distance = DistanceToTarget(torpedo);
           // if we can hear a torpedo, it is close and it is aimed at us
           // then we run
           if ( (can_hear_torpedo) && (torpedo->target == this) &&
                (distance < (MAX_TORPEDO_RANGE * MILES_TO_YARDS) ) )
           {
               // status = (DesiredSpeed == MaxSpeed);
               all_torpedoes = Launch_Noisemaker(all_torpedoes, torpedo);
               bearing = (int) BearingToTarget(torpedo);
               bearing += 180;
               if (bearing >= 360)
                  bearing = bearing % 360;
               DesiredHeading = bearing;
               DesiredSpeed = MaxSpeed;
               // subs can dive too
               if (ShipType == TYPE_SUB)
               {
                 if (torpedo->Depth <= Depth)  // it is above us
                     DesiredDepth = MaxDepth;
                 else if (torpedo->Depth > Depth)  // below us
                     DesiredDepth = PERISCOPE_DEPTH;
               }
               if (mood == MOOD_CONVOY)
                  mood = MOOD_PASSIVE;
               // if (! status)
               //   return all_torpedoes;
               action = 1;  // running
               status = TRUE;
           }
           // we hear a torpedo but it is not coming after us
           else if ( (can_hear_torpedo) && (TorpedosOnBoard) )
           {
                if (ShipType == TYPE_SHIP)
                    mood = MOOD_ATTACK;
                // if we are not tracking anything, check out the torp
                if ( (! track_torpedo) && (torpedo->Friend != Friend) )
                {
                  #ifdef AIDEBUG
                  printf("Found torpedo to track.\n");
                  #endif
                  track_torpedo = torpedo;
                }
           }
          
           if ( (can_hear_torpedo) && (ShipType == TYPE_SHIP) )
              Radio_Signal(all_ships, RADIO_HEAR_TORPEDO);

           }   // end of this is a torpedo
           torpedo = torpedo->next;
        }   // end of checking out torpedoes in the water

      #ifdef AIDEBUG
      printf("Checking for targets.\n");
      #endif
      // see if we can hear a nearby enemy to shoot at
      target = Have_Enemy_Target(all_ships);
      #ifdef AIDEBUG
      if (target) printf("Found enemy.\n");
      #endif
      int count = Count_Torpedoes(all_torpedoes);
      if ( (target) && (TorpedosOnBoard > 0) ) 
           // (count < MAX_TORPEDOES_FIRED) )
      {
          int target_range = DistanceToTarget(target);
          int shooting_range;
          // we shoot at different distances, depending on mood
          if (mood == MOOD_ATTACK)
            shooting_range = TORPEDO_RANGE_ATTACK;
          else
            shooting_range = TORPEDO_RANGE_PASSIVE;
          shooting_range += (rand() % 3) - 2;  // add some randomness
          target_range *= YARDS_TO_MILES;
          #ifdef AIDEBUG
          printf("Checking range %d\n", target_range);
          #endif
          if ( (count < MAX_TORPEDOES_FIRED) && 
               (target_range < shooting_range) )
          {
          #ifdef AIDEBUG
          printf("Firing with %d torpedoes.\n", count);
          #endif
          torpedo_tube[0] = TUBE_TORPEDO;
          status = Use_Tube(FIRE_TUBE, 0);
          if (status == TUBE_ERROR_FIRE_SUCCESS)
          {
              char *ship_file, filename[] = "ships/class5.shp";
              ship_file = Find_Data_File(filename);
              torpedo = Fire_Tube(target, ship_file );
              if ( (ship_file) && (ship_file != filename) )
                   free(ship_file);
              if (torpedo)
              {
                 #ifdef AIDEBUG
                 printf("Successfully fired torpedo.\n");
                 #endif
                 torpedo->Friend = Friend;
                 torpedo->owner = this;
                 // all_torpedoes = Add_Ship(all_torpedoes, torpedo);
                 // add torpedo to linked list
                 if (! all_torpedoes)
                    all_torpedoes = torpedo;
                 else
                 {
                   my_torpedoes = all_torpedoes;
                   found = FALSE;
                   while ( (! found) && (my_torpedoes) )
                   {
                       if (my_torpedoes->next)
                          my_torpedoes = my_torpedoes->next;
                       else
                       {
                         my_torpedoes->next = torpedo;
                         found = TRUE;
                         // return all_torpedoes;
                       }
                   }
                  }  // add torpedo to list 
              }
              TorpedosOnBoard--;
              #ifdef AIDEBUG
              printf("I have %d torpedoes left.\n", TorpedosOnBoard);
              #endif
              return all_torpedoes;
          }   // torpedo firing was successful
          }   // can fire torpedo at target in range
        
          // we hear an enemy, but can't fire yet
          else if ( (mood == MOOD_ATTACK) && (! action) ) 
          {
             DesiredHeading = BearingToTarget(target); 
             if (ShipType == TYPE_SHIP)
                DesiredSpeed = (MaxSpeed / 2) + (rand() % 5) - 2;
             else  // sub
                DesiredSpeed = (MaxSpeed / 3) + (rand() % 3) + 1;
             if (ShipType == TYPE_SUB)
             {
                DesiredDepth = target->Depth;
                if (DesiredDepth < PERISCOPE_DEPTH)
                    DesiredDepth = PERISCOPE_DEPTH;
             }
             action = 2;
          }
      }    // end of we have a target and we have torpedoes on board

      // we are not tracking anything, but we have torpedoes
      // and we can hear a torpedo
      // move toward the origin of the torpedo we hear
      else if ( (TorpedosOnBoard > 0) && (track_torpedo) && (!action) )
      {
         #ifdef AIDEBUG
         printf("We hear a torpedo, tracking it.\n");
         printf("We should probably only chase torpedoes if in attack mood\n");
         #endif
         DesiredHeading = BearingToOrigin(track_torpedo);
         if (ShipType == TYPE_SUB)
            DesiredDepth = track_torpedo->Depth;
      }
      // We have no target and no torpedo to follow
      // if we are in attack mood we should slow down
      // so we can hear better
      else if ( (! action) && (!target) && (mood == MOOD_ATTACK) )
      {
          DesiredSpeed = (MaxSpeed / 3) + ( ( rand() % 5 ) - 3 );
      }
      

      // if we got this far we cannot hear a torpedo coming at us
      if ( (Speed == MaxSpeed) && (! action) )
         DesiredSpeed = MaxSpeed / 3;
   }     // I think this is the end of "we have sonar" section

   // helicopters should go low if there are helicopters
   if ( (ShipType == TYPE_SUB) && (helicopters) )
   {
       if (Depth < map->thermals[0])
          DesiredDepth = map->thermals[0] + 25;
   }
   
   // when traveling in convoy, we change course once every
   // ... twenty minutes?
   if (mood == MOOD_CONVOY)
   {
      convoy_course_change--;
      if (! convoy_course_change)
      {
          convoy_course_change = CONVOY_CHANGE_COURSE;
          DesiredHeading += 90;
          if (DesiredHeading >= 360)
              DesiredHeading = DesiredHeading % 360;
      }
   }
   else   // got nothing to do, but perhaps change course
   {
     change = rand() % CHANCE_COURSE;
     if (! change)
     {
       DesiredHeading = Heading + ( (rand() % 100) - 90);
       if (DesiredHeading >= 360)
         DesiredHeading = DesiredHeading % 360;
       DesiredSpeed = MaxSpeed / 3;
       // submarines should change depth too
       if (ShipType == TYPE_SUB)
       {
          DesiredDepth =+ rand() % 200;
          DesiredDepth /= 2;
          DesiredDepth += PERISCOPE_DEPTH;
          // DesiredDepth = ( (rand() % MaxDepth) / 2 ) + 50;
       }
     }
   }

   // check for radio message
   if ( (ShipType == TYPE_SHIP) && (radio_message) )
   {
       if ( (radio_message == RADIO_UNDER_ATTACK) && (mood == MOOD_CONVOY) )
       {
           if (TorpedosOnBoard)
             mood = MOOD_ATTACK;
           else
           {
             mood = MOOD_PASSIVE;
             DesiredHeading = rand() % 360;
           }
           DesiredSpeed = MaxSpeed;
       }
       else if ( (radio_message == RADIO_HEAR_TORPEDO) && (mood == MOOD_CONVOY) )
       {
           if (TorpedosOnBoard)
              mood = MOOD_ATTACK;
           else
              mood = MOOD_PASSIVE;
       }
   }
   radio_message = RADIO_NONE;
   if ( (ShipType == TYPE_SUB) && (Depth < PERISCOPE_DEPTH) )
       DesiredDepth = PERISCOPE_DEPTH;
   return all_torpedoes;
}  // end of Sub_AI




// This function checks on our torpedo to see how it
// is doing. If it has hit its target, we return HIT_TARGET.
// If we run out of fuel, we return OUT_OF_FUEL. Otherwise
// we return STATUS_OK.
// This function just checks status changes, it doesn't do
// anything about them.
int Submarine::Check_Status()
{
   double range;
   double delta_depth;

   // first see if we ran out of fuel
   fuel_remaining--;
   if (fuel_remaining < 1)
      return OUT_OF_FUEL;

   if (target)
   {
      range = DistanceToTarget(target);
      delta_depth = fabs(target->Depth - Depth);
      if ( (range < HITTING_RANGE) && (delta_depth < HITTING_DEPTH) )
      {
         char *full_path, *explosion = "sounds/explosion.ogg";
         full_path = Find_Data_File(explosion);
         Play_Sound(full_path);
         if ( (full_path) && (explosion) )
            free(full_path);
 
         return HIT_TARGET;
      }
   }

   return STATUS_OK;
}



// This function reduces the remaining strength of the ship's hull.
// If the ship/sub can still float, we return DAMAGE_OK, but if
// the ship is sinking, we return DAMAGE_SINK.
// If the ship sinks, we set Active to FALSE.
int Submarine::Take_Damage()
{
   hull_strength--;
   if (hull_strength < 1)
   {
      Active = FALSE;
      return DAMAGE_SINK;
   }
   else
     return DAMAGE_OK;
}



// This function counts how many torpedoes we have active
// in the water (that we fired).
int Submarine::Count_Torpedoes(Submarine *all_torp)
{
    int count = 0;
    Submarine *torp = all_torp;

    while (torp)
    {
       if ( (torp->owner == this) && (torp->ShipType == TYPE_TORPEDO) )
         count++;
       torp = torp->next;
    }
    #ifdef DEBUG
    printf("We have %d TORPEDOES in the water\n", count);
    #endif
    return count;
}


// This function counts how many noisemakers we have in the water
int Submarine::Count_Noisemakers(Submarine *all_noise)
{
   int count = 0;
   Submarine *noise = all_noise;

   while (noise)
   {
      if ( (noise->owner == this) && (noise->ShipType == TYPE_NOISEMAKER) )
         count++;
      noise = noise->next;
   }
   #ifdef DEBUG
   printf("We have %d NOISEMAKERS in the water\n", count);
   #endif
   return count;
}

   

// This function checks the list of ships to see if we have any enemies
// to shoot at.
// To get results, we must have torpedoes on board,
// We must be able to hear them clearly.
// They must be an enemy (Friend must be opposite of our FOE or FRIEND).
// In the case of multiple matches, the closest target is returned.
Submarine *Submarine::Have_Enemy_Target(Submarine *all_ships)
{
    Submarine *current, *min = NULL;
    int current_distance, min_distance = INT_MAX;
    int making_noise;

    // can we shoot?
    if (TorpedosOnBoard < 1)
        return NULL;
    // do we even have enemies?
    if ( (Friend != FRIEND) && (Friend != FOE) )
        return NULL;

    current = all_ships;
    while (current)
    {
       if ( ( (current->Friend == FOE) && (Friend == FRIEND) ) ||
            ( (current->Friend == FRIEND) && (Friend == FOE) ) )
       {
           #ifdef AIDEBUG
           printf("Examining enemy ship.\n");
           #endif
           // we do not like this ship, are they closer than our current min?
           current_distance = DistanceToTarget(current);
           if (current_distance < min_distance)
           {
               #ifdef AIDEBUG
               printf("Within range to become target.\n");
               #endif
               // they are close, can we hear them?
               making_noise = Can_Hear(current); 
               if (making_noise)
               {
                   #ifdef AIDEBUG
                   printf("We can hear this ship. Make it the target.\n");
                   #endif
                   min = current;
                   min_distance = current_distance;
                   // if the target is hostile and pinging, and
                   // we are in a convoy, switch to attack mood
                   if ( (current->pinging) && (mood == MOOD_CONVOY) )
                      mood = MOOD_ATTACK;
                  
               }
           } 
       }
       current = current->next;
    }
    return min;
}




// This function is only to be used by the AI. Here we assume
// we are being chased by a torpedo. We check to see how many
// noisemakers we have in the water. If it is less than the max
// number, we launch a noisemaker.
// This function returns a pointer to all torpedoes/noisemakers
Submarine *Submarine::Launch_Noisemaker(Submarine *all_noisemakers, Submarine *chased_by)
{
    int existing_noisemakers;
    Submarine *new_noisemaker = NULL, *my_noise;
    int status, found;

    if ( (! chased_by) || (! all_noisemakers) )
        return NULL;    // shouldn't happen, but just in case

    // do we have a noisemaker?
    if (NoiseMakers < 1)
       return all_noisemakers;

    // see if we are allowed to fire any more
    existing_noisemakers = Count_Noisemakers(all_noisemakers);
    if (existing_noisemakers >= MAX_NOISEMAKERS_FIRED)
       return all_noisemakers;

    // we are allowd to launch one
    torpedo_tube[0] = TUBE_NOISEMAKER;
    status = Use_Tube(FIRE_TUBE, 0);
    if (status == TUBE_ERROR_FIRE_NOISEMAKER)
    {
        char *ship_file, filename[] = "ships/class6.shp";
        ship_file = Find_Data_File(filename);
        new_noisemaker = Fire_Tube(NULL, ship_file );
        if ( (ship_file) && (ship_file != filename) )
             free(ship_file);
        if (new_noisemaker)
        {
           new_noisemaker->Friend = Friend;
           new_noisemaker->owner = this;
           if (! all_noisemakers)
                 all_noisemakers = new_noisemaker;
           else
           {
             my_noise = all_noisemakers;
             found = FALSE;
             while ( (!found) && (my_noise) )
             {
                 if (my_noise->next)
                     my_noise = my_noise->next;
                 else
                 {
                   my_noise->next = new_noisemaker;
                   found = TRUE;
                   // return all_noisemakers;
                 }
              }   
            }   // end of add noisemaker to list  
         }   // created new noisemaker
     }  // status
    NoiseMakers--;
    if (new_noisemaker)
    {
        new_noisemaker->DesiredHeading = chased_by->DesiredHeading;
        chased_by->Is_Distracted_By_Noisemaker(new_noisemaker);
    }
    return all_noisemakers;
}




// This function checks to see if the torpedo will get distracted
// by a given noisemaker
// Returns TRUE if distracted and FALSE if not. In the case where
// the torpedo is distracted, its target variable is reset
// to chase the noisemaker
int Submarine::Is_Distracted_By_Noisemaker(Submarine *noisemaker)
{
     int chance;

     if (! noisemaker)   // this should not happen, but just in case
        return FALSE;

     chance = rand() % DISTRACTED_CHANCE;
     if (! chance)
     {
         target = noisemaker;
         return TRUE;
         #ifdef DEBUG
         printf("Torpedo going off course to chase noisemaker.\n");
         #endif
     }

     #ifdef DEBUG
     printf("Torpedo was not distracted by noisemaker.\n");
     #endif
     return FALSE;
}



// This function sends a radio signal to all other ships of the same
// nationality.
// The function returns TRUE
int Submarine::Radio_Signal(Submarine *all_ships, int my_signal)
{
   Submarine *current_ship;

   current_ship = all_ships;
   while (current_ship)
   {
       if ( (current_ship->Friend == Friend) &&
            (current_ship->ShipType == TYPE_SHIP) )
          current_ship->radio_message = my_signal;
       
       current_ship = current_ship->next;
    }

   return TRUE;
}


// This function sends out a ping signal. We should then be
// able to "see" any ships/subs in front of us.
// This function does not detect aircraft (for obvious reasons)
// and sets "pinging" to true, which means we will be very
// noisy for a while.
int Submarine::Send_Ping(Submarine *all_ships)
{
   Submarine *current;
   int status;
   char *full_path, *ping_file = "sounds/sonar-ping.ogg";
   // some craft do not have sonar
   if (! has_sonar)
      return FALSE;

   full_path = Find_Data_File(ping_file);
   Play_Sound(full_path);
   if ( (full_path) && (full_path != ping_file) )
      free(full_path);

   pinging = 2;
   current = all_ships;
   while (current)
   {
       if (current != this)
       {
          status = ! ( InBaffles(current, 1, NULL) );
          if (status)
          {
             // got return signal from ping
             Add_Target(current, CONTACT_PING);
             
          }
       }
       current = current->next;
   }   

   return TRUE;
}



/*********************************************
        This function will return if a target is in
        the observers Baffles and therefore not
        detectable. Values for 'int sensor' are 1 for
        spherical array, 2 for towed array, 3 for port
        hull array and 4 for starboard hull array.

        Might want to move the calculations of the
        baffle angles to the Coord class so they don't
        have to be calculated all the time.
*********************************************/
int Submarine::InBaffles(Submarine *target, int sensor, TowedArray *TB16)
{
        int array_heading;
        int relative_bearing;
        int sensordeaf=1;
        int bearing_to_target;

        switch(sensor){
                case 1: //Spherical
                        sensordeaf = 0;
                        array_heading = (int)Heading;
                        bearing_to_target = (int)BearingToTarget( target);
                        if(array_heading > bearing_to_target) bearing_to_target += 360;
                        relative_bearing = bearing_to_target - array_heading;
                        if(relative_bearing > 150 && relative_bearing < 210) sensordeaf = 1;
                        if (target == this) sensordeaf = 1;
                        break;
                case 2: //Towed
                        if (! TB16)
                           sensordeaf = TRUE;
                        else
                        {
                          sensordeaf = 0;
                          array_heading = (int)TB16->ReturnHeading();
                          bearing_to_target = (int)TB16->BearingToTarget(target->Lat_TotalYards, target->Lon_TotalYards);
                          if(array_heading > bearing_to_target) bearing_to_target += 360;
                          relative_bearing = bearing_to_target - array_heading;
                          if(relative_bearing < 30 || relative_bearing > 330) sensordeaf = 1;
                        }
                        break;

                case 3: //port hull
                case 4: //sb hull
                default:
                        break;
        }
        return sensordeaf;
}

