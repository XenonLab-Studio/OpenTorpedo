#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include "helicopter.h"
#include "files.h"

Helicopter::Helicopter()
{
  Init();
}


Helicopter::~Helicopter()
{
  Cleanup();
}



int Helicopter::Init()
{
  MaxSpeed = 0;
  DesiredHeading = 0;
  Speed = 0;
  Heading = 0;
  current_action = ACTION_HOVER;
  mood = MOOD_PASSIVE;
  Friend = FRIEND;
  destination_x = destination_y = -1;
  ShipType = TYPE_HELICOPTER;
  ShipClass = CLASS_HELICOPTER;
  MaxDepth = LISTEN_HEIGHT;
  Rudder = HELICOPTER_RUBBER;
  hull_strength = 1;
  has_sonar = TRUE;
  PSCS = 360.0;
  firing_countdown = 0;
  next = NULL;
  return TRUE;
}



void Helicopter::Cleanup()
{
   // nothing to clean yet
}


// This function should be called right after the helicopter is created. It
// will load the ship's class specific information from the appropriate
// data file.
// The data file's format will be
// maxspeed maxdepth rudder torpedoes-carried hull_strength has_sonar, P
// SCS class-name class-type
int Helicopter::Load_Class(char *from_file)
{
   FILE *my_file;
   char line[256];
   char *status;

   if (! from_file)
      return FALSE;
   my_file = fopen(from_file, "r");
   if (! my_file)
      return FALSE;

   status = fgets(line, 256, my_file);
   if (status)
   {
       sscanf(line, "%d %d %d %d %d %d %f %s %s",
              &MaxSpeed, &MaxDepth, &Rudder, &TorpedoesOnBoard,
              &hull_strength, &has_sonar, &PSCS, ClassName, ClassType);

   }
   fclose(my_file);
   return TRUE;
}


// Helicopters do not receive mission objectives
// yet, but they may have an entry in the mission file
// for formatting. This function simply
// reads one line from the file and returns
int Helicopter::Load_Mission(FILE *from_file)
{
   char line[256];
   char *status;
   if (! from_file)
        return FALSE;
   status = fgets(line, 256, from_file);
   if (! status)
      return FALSE;
   return TRUE;
}



// This function lets us know if the helicopter is okay
// if it something has happened to make it die.
// Returns TRUE if the helicopter can still fly or
// FALSE if it has died.
// We might add checks here later for running out
// of fuel....
int Helicopter::Check_Status()
{
  if (hull_strength > 0)
    return TRUE;
  else
    return FALSE;

}



// This function will determine what the helicopeter is doing.
// Ideally we want to patrol more or less randomly, looking
// for enemy submarines. When we hear an enemy we
// will move close, confirm we can still hear it and fire.
// When we are out of torpedoes we should probably
// continue to patrol, and radio sightings
Submarine *Helicopter::Helicopter_AI(Submarine *all_ships, Submarine *all_torpedoes)
{
  double distance_to_target;
  int range, new_direction;
  Submarine *ship = NULL, *target = NULL;
  int do_what = DO_NOTHING;
  int new_heading;
  Submarine *torpedo, *my_torpedoes;
  int found;

  if (firing_countdown)
      firing_countdown--;
  #ifdef DEBUG_HELICOPTER
  printf("Current action: %d\n", current_action);
  #endif
  switch (current_action)
  {
      case ACTION_HOVER:
            destination_x = -1;
            destination_y = -1;
            DesiredSpeed = 0;
            current_action = ACTION_MOVING;
            break;
      case ACTION_MOVING:
            // no destination, set one
            if ( (destination_x < 0) || (destination_y < 0) )
            {
               // destination_x = Lat_TotalYards + ((rand() % 4000) - 2000);
               // destination_y = Lon_TotalYards + ((rand() % 4000) - 2000);
               // pick direction 0-7
               new_direction = rand() % 8;
               range = MILES_TO_YARDS * 2;
               switch (new_direction)
               {
                  case 0: destination_x = Lat_TotalYards + range;  
                          destination_y = Lon_TotalYards;
                          break; // west
                  case 1: destination_x = Lat_TotalYards + range;
                          destination_y = Lon_TotalYards - range;
                          break;  // sw
                  case 2: destination_x = Lat_TotalYards;
                          destination_y = Lon_TotalYards - range;
                          break;  // south
                  case 3: destination_x = Lat_TotalYards;
                          destination_y = Lon_TotalYards + range;
                          break;  // north
                  case 4: destination_x = Lat_TotalYards - range;
                          destination_y = Lon_TotalYards - range;
                          break; // se
                  case 5: destination_x = Lat_TotalYards - range;
                          destination_y = Lon_TotalYards;
                          break;  // east
                  case 6: destination_x = Lat_TotalYards + range;
                          destination_y = Lon_TotalYards + range;
                          break;  // nw
                  case 7: destination_x = Lat_TotalYards - range;
                          destination_y = Lon_TotalYards + range;
                          break;   // ne
               }   // end of pick new destination
               DesiredDepth = MOVE_HEIGHT;
               DesiredSpeed = MaxSpeed;
            }
            // figure out heading here
            new_heading = Bearing_To_Destination();
            if (new_heading != -1)
               DesiredHeading = new_heading;
            distance_to_target = Distance_To_Destination();
            #ifdef DEBUG_HELICOPTER
            printf("DesiredHeading: %d\n", DesiredHeading);
            printf("Distance: %lf\n", distance_to_target);
            #endif
            // near destination, stop then switch to listening
            if (distance_to_target < MILES_TO_YARDS) 
            {
               #ifdef DEBUG_HELICOPTER
               printf("Close to destination, slow down.\n");
               #endif
               current_action = ACTION_LISTEN;
               DesiredSpeed = 0;
               DesiredDepth = LISTEN_HEIGHT;
            }
            /*
            // to far away from target, pick new destination
            else if (distance_to_target > (MAX_DISTANCE * MILES_TO_YARDS) )
            {
               destination_x = -1;
               destination_y = -1;
            }
            */
            // close to target, slow down
            else if (distance_to_target < (2 * MILES_TO_YARDS) )
               DesiredSpeed = MaxSpeed / 4;
            break;

       case ACTION_LISTEN:
            if (destination_x >= 0)
               destination_x = -1;
            if (destination_y >= 0)
               destination_y = -1;
            DesiredSpeed = 0;
            DesiredDepth = LISTEN_HEIGHT;
            DesiredHeading = Heading;
            if ( (Depth == LISTEN_HEIGHT) && (Speed < 1.0) )
            {
               // check all ships subs to find if we can hear a foe
               target = Find_Target(all_ships);
               if (target)
               {
                   #ifdef DEBUG_HELICOPTER
                   printf("We can hear something, checking range.\n");
                   #endif
                   range = Distance_To_Target(target);
                   // we hear a foe and it is close, fire
                   if (range < (TORPEDO_RANGE_PASSIVE * MILES_TO_YARDS) )
                   {
                      #ifdef DEBUG_HELICOPTER
                      printf("We can shoot at a sub.\n");
                      #endif
                      do_what = DO_SHOOT;
                      // do shooting here
                      if ( (TorpedoesOnBoard > 0) && (! firing_countdown) )
                      {
                          char *ship_file, filename[] = "ships/class5.shp";
                          ship_file = Find_Data_File(filename);
                          torpedo = Fire_Torpedo(target, ship_file);
                          if ( (ship_file) && (ship_file != filename) )
                               free(ship_file);
                          if (torpedo)
                          {
                            #ifdef DEBUG_HELICOPTER
                            printf("We fired a torpedo.\n");
                            #endif
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
                                  }
                               }
                            }
                            TorpedoesOnBoard--;
                            firing_countdown = FIRING_WAIT;
                          }
                      }   // end of torpedoes on board
                      destination_x = target->Lat_TotalYards;
                      destination_y = target->Lon_TotalYards;
                      DesiredDepth = MOVE_HEIGHT;
                      DesiredSpeed = MaxSpeed;
                      current_action = ACTION_MOVING;
                      return all_torpedoes;
                   }
                   // we hear a foe and it is far away, move
                   else
                   {
                       #ifdef DEBUG_HELICOPTER
                       printf("We can hear a sub, we need to move closer.\n");
                       #endif
                       do_what = DO_MOVE;
                       destination_x = target->Lat_TotalYards;
                       destination_y = target->Lon_TotalYards;
                       DesiredDepth = MOVE_HEIGHT;
                       DesiredSpeed = MaxSpeed;
                       current_action = ACTION_MOVING;
                       return all_torpedoes;
                   }

               }    // end of we found a target

               // No target? then we should find a friendy ship
               // to get close to
               ship = Find_Closest_Friend(all_ships);
               if (ship)
               {
                  do_what = DO_MOVE;
                  destination_x = ship->Lat_TotalYards;
                  destination_y = ship->Lon_TotalYards;
                  DesiredDepth = MOVE_HEIGHT;
                  DesiredSpeed = MaxSpeed / 2;
               }

               // we do not hear anything, move randomly and listen again
               // when in doubt, we are hovering
               if (! do_what)
                  current_action = ACTION_HOVER;
            }   // end of we are listening
            break;
       default:
          current_action = ACTION_HOVER;
      
     }   // end of switch actions

  return all_torpedoes;
}



// This function handles moving the helicopter. It will
// adjust the craft up/down, turn, and adjust speed.
int Helicopter::Handle()
{
  float AmountOfChange;
  float delta_heading;

   // first, handle height
   if (DesiredDepth > LISTEN_HEIGHT)
       DesiredDepth = LISTEN_HEIGHT;
   else if (DesiredDepth < MOVE_HEIGHT)
       DesiredDepth = MOVE_HEIGHT;

   if (DesiredDepth > Depth)
      Depth++;
   else if (DesiredDepth < Depth)
      Depth--;

   AmountOfChange = Rudder / 10;
   // turn toward desired heading
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

   // sanity check on course
   if (Heading < 0)
      Heading += 360;
   else if (Heading > 359)
      Heading -= 360;

   // sanity check on speed, if we need to turn a lot to
   // get to our destination, slow to half speed
   delta_heading = Heading - DesiredHeading;
   if ( (delta_heading < -45) || (delta_heading > 45) )
       DesiredSpeed = MaxSpeed / 2;
  
   // adjust speed   
   if (DesiredSpeed > Speed)
      Speed += 1.0;
   else if (DesiredSpeed < Speed)
      Speed -= 1.0;
   if (Speed < 0.0)
      Speed = 0.0;
   else if (Speed > MaxSpeed)
      Speed = MaxSpeed;
   
  return TRUE;
}



// This function returns the bearing (in degrees) to
// our destination. If no destination is set, then
// we return zero.
int Helicopter::Bearing_To_Destination()
{
    double delta_x = 0, delta_y = 0;
    double bearing = 0;

    if ( (destination_x < 0) || (destination_y < 0) )
       return -1;

   if (Lat_TotalYards > destination_x){
           delta_x = Lat_TotalYards - destination_x;
        }
        else{
           delta_x =  destination_x - Lat_TotalYards;
        }

        if (Lon_TotalYards > destination_y){
           delta_y = Lon_TotalYards - destination_y;
        }
        else{
           delta_y = destination_y - Lon_TotalYards;
    }

    if ( (Lon_TotalYards < destination_y) &&
         (Lat_TotalYards < destination_x) )
        bearing = (360 - ((atan(delta_x / delta_y) * 360) / RADIAN_RATIO));
    else if ( (Lon_TotalYards < destination_y) &&
              (Lat_TotalYards > destination_x) )
        bearing = (0 + ((atan(delta_x / delta_y) * 360) / RADIAN_RATIO));
    else if ( (Lon_TotalYards > destination_y) &&
              (Lat_TotalYards < destination_x) )
        bearing = (180 + ((atan(delta_x / delta_y) * 360) / RADIAN_RATIO) );
    else if ( (Lon_TotalYards > destination_y) &&
              (Lat_TotalYards > destination_x) )
        bearing = (180 - ((atan(delta_x / delta_y) * 360) / RADIAN_RATIO) );
    #ifdef DEBUG_HELICOPTER
    printf("Bearing to destination as double: %lf\n", bearing);
    #endif
    if (delta_y == 0)
    {
        if (Lat_TotalYards > destination_x)
            bearing = 90;
        else
            bearing = 270;
    }
    if (delta_x == 0)
    {
        if (Lon_TotalYards > destination_y)
            bearing = 180;
        else
            bearing = 0;
    }
    #ifdef DEBUG_HELICOPTER
    printf("Bearing to destination as double: %lf\n", bearing);
    #endif
    if ( (bearing > 360.0) || (bearing < 0.0) )
    {
        // something has gone wrong. wipe destination
        destination_x = -1;
        destination_y = -1;
        current_action = ACTION_MOVING;
        bearing = -1;
    }

    return (int) bearing;
}



// This function lets us know how far away
// we are (in yards) from out destination.
// If no destination is set, we return zero.
double Helicopter::Distance_To_Destination()
{
   int delta_x, delta_y;
   if ( (destination_x < 0) || (destination_y < 0) )
      return 0;

   delta_x = destination_x - Lat_TotalYards;
   if (delta_x < 0)
      delta_x = -delta_x;
   delta_y = destination_y - Lon_TotalYards;
   if (delta_y < 0)
      delta_y = -delta_y;
   
   return sqrt( (delta_x * delta_x) + (delta_y * delta_y) );
}



// This function finds out how far away a target is
// from our helicopter in yards.
double Helicopter::Distance_To_Target(Submarine *Target)
{
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




// This function will go through the list of existing
// subs. It checks to see if we can hear
// any foes. If so, this function returns the closest
// enemy. If nothing is found, the function returns
// NULL.
Submarine *Helicopter::Find_Target(Submarine *all_ships)
{
    Submarine *ship;
    Submarine *best_target = NULL;
    double min_range = INT_MAX;
    double current_range;
    int status;

    ship = all_ships;
    while (ship)
    {
       // check to see if it is a sub
       if (ship->ShipType == TYPE_SUB)
       {
         // check to see if it is a foe
         if ( ((ship->Friend == FOE) && (Friend == FRIEND)) ||
            ( (ship->Friend == FRIEND) && (Friend == FOE)) )
         {
            // check to see if it is close
            current_range = Distance_To_Target(ship);
            if (current_range < min_range)
            {
                // check to see if we can hear it
                status = Can_Hear(ship);
                if (status)
                {
                    min_range = current_range;
                    best_target = ship;
                }   // end of we can hear this one
            }    // end of closer
         }   // endo f is enemy
       }   // end of proper type
       ship = ship->next;
    }
    return best_target;
}



// This function will let us know if we can hear a given ship/sub
// The function returns TRUE if we can hear the target or FALSE
// if we cannot.
int Helicopter::Can_Hear(Submarine *target)
{
        float Range = Distance_To_Target(target);
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
        // OwnShipNoise = RadiatedNoise();
        OwnShipNoise = 0.0;
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






// This function launches a torpedo or noise maker. If the
// passed target is NULL, the new torp/noisemaker is given a
// random course and no target, leaving it to run in a striaght line.
// On success the function passes back a pointer to the new torpedo
// and on failure NULL is returned.
Submarine *Helicopter::Fire_Torpedo(Submarine *target, char *ship_file)
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
       my_torp->DesiredHeading = my_torp->Heading = my_torp->BearingToTarget(target);
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



// This function tries to locate the nearest friendly surface ship.
// If we find a friendly surface vessel that ship's pointer is
// returned. Otherwise we return NULL.
Submarine *Helicopter::Find_Closest_Friend(Submarine *ships)
{
    double nearest = INT_MAX;
    double range;
    Submarine *best_fit = NULL;
    Submarine *current;
    
    current = ships;
    while (current)
    {
       if ( (current->ShipType == TYPE_SHIP) && (current->Friend == Friend) )
       {
          range = Distance_To_Target(current);
          if (range < nearest)
          {
              best_fit = current;
              nearest = range;
          }
       }
       current = current->next;
    }
    return best_fit;
}

