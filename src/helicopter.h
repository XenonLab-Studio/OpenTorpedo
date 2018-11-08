#ifndef HELICOPTER_HEADER_FILE__
#define HELICOPTER_HEADER_FILE__

#include "coord.h"
#include "submarine.h"

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif


#define ACTION_HOVER 0    // not sure, waiting in one spot
#define ACTION_MOVING 1   // moving to destination
#define ACTION_LISTEN 2   // lower sonar
#define ACTION_RETURN_TO_BASE 3   // to be implemented later
#define ACTION_REFUELING 4        // to be implemented later

#define LISTEN_HEIGHT -20
#define MOVE_HEIGHT -50
#define HELICOPTER_RUBBER 50
#define FIRING_WAIT 120    // two mintes

#define DO_NOTHING 0
#define DO_SHOOT 1
#define DO_MOVE 2


class Helicopter : public Coord
{
public:
   int MaxSpeed, DesiredSpeed;
   int Depth, DesiredDepth;
   int DesiredHeading;
   int TorpedoesOnBoard;
   int current_action;   // hovering, listening, moving, returning to base
   int mood;     // convoy, passive, attack
   int Friend;
   int destination_x, destination_y;
   int ShipType, ShipClass;
   char ClassName[CLASS_NAME_SIZE];
   char ClassType[CLASS_TYPE_SIZE];
   int MaxDepth, Rudder;
   int hull_strength;
   int has_sonar;
   int firing_countdown;  // make sure we can't rapid fire
   float PSCS;
   MAP *map;
   Helicopter *next;

   Helicopter();
   ~Helicopter();

   int Init();
   void Cleanup();
   int Can_Hear(Submarine *target);
   Submarine *Helicopter_AI(Submarine *all_ships, Submarine *all_torpedoes);
   int Handle();
   int Load_Class(char *from_file);
   int Load_Mission(FILE *from_file);
   int Check_Status();
   int Bearing_To_Destination();
   double Distance_To_Destination();   // in yards
   double Distance_To_Target(Submarine *target);
   Submarine *Find_Target(Submarine *all_ships);
   Submarine *Fire_Torpedo(Submarine *target, char *ship_file);
   Submarine *Find_Closest_Friend(Submarine *ships);
};



#endif

