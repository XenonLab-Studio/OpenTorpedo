#ifndef MAP_HEADER_FILE__
#define MAP_HEADER_FILE__

/*
This file contains definitions for dealing with the depth of the
water and placing of thermal layers.
*/

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define MAX_DEPTH 5000      // deepest part of the ocean
#define MIN_DEPTH 0          // most shallow point of the ocean
#define MAX_THERMALS 3
#define MAX_THERMAL_DEPTH 900
#define THERMAL_FILTER 0.15

// MAP_MIN and MAP_MAX are also defined in the coord class
#ifndef MAP_MIN 
#define MAP_MIN 0
#endif
#ifndef MAP_MAX
#define MAP_MAX 300000
#endif

class MAP
{
public:
  int thermals[MAX_THERMALS];

  MAP();
  ~MAP();
  int Init();
  int Create_Thermal_Layers(int how_many);
 
  int Get_Ocean_Depth(int x, int y);
  int Thermals_Between(int depth_1, int depth_2);
  int Next_Up(int current_depth);    // find thermal from our position
  int Next_Down(int current_depth);  // find thermal from our position

  #ifdef DEBUGMAP
  void Test_Map();
  #endif

};



#endif

