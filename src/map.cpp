#include <stdio.h>
#include <stdlib.h>
#include "map.h"


MAP::MAP()
{
   Init();
}



MAP::~MAP()
{

}



int MAP::Init()
{
   int status;
   int index;

   // init stuff ahead of time   
   for (index = 0; index < MAX_THERMALS; index++)
       thermals[index] = 0;

   status = Create_Thermal_Layers(MAX_THERMALS);
   return status;
}



// This function creates thermal layers spaced (more or less)
// eveningly. The function returns the number of thermals created. 
int MAP::Create_Thermal_Layers(int how_many)
{
   int base_range;
   int current_range;
   int index;

   base_range = MAX_THERMAL_DEPTH / how_many;
   current_range = 0;
   index = 0;
   while ( (index < MAX_THERMALS) && (index < how_many) )
   {
       current_range += base_range;
       thermals[index] = current_range + ( rand() % 150 ) - 75;
       index++;
   } 

   return index;
}


// At this point the ocean depth is the same everywhere, so
// we ignore the passed coordinaes and just return MAX_DEPTH
int MAP::Get_Ocean_Depth(int x, int y)
{
   return MAX_DEPTH;
}



// This function returns the number of thermal layers
// between depth1 and depth2
// If no thermals exist then the function returns zero.
int MAP::Thermals_Between(int depth_1, int depth_2)
{
   int start_point, end_point;
   int index, thermal_count;

   // if depths are the same, just return
   if (depth_1 == depth_2)
       return 0;

   // we need to make sure start_point is the higher depth
   if (depth_1 <= depth_2)
   {
      start_point = depth_1;
      end_point = depth_2;
   }
   else
   {
        start_point = depth_2;
        end_point = depth_1;
   }

   // move through each thermal layer and see if it lies between
   // the two depths.
   index = thermal_count = 0;
   while (index < MAX_THERMALS)
   {
      if ( (start_point < thermals[index]) && (thermals[index] < end_point) )
          thermal_count++;
      index++;
   }

   return thermal_count;
}


/*
This function finds the next themral above the current depth. If
no thermal lies above this position then we return "from_depth".
*/
int MAP::Next_Up(int from_depth)
{
   int index = MAX_THERMALS - 1;
   int found = FALSE;

   while ( (!found) && (index >= 0) )
   {
      if ( (thermals[index] > 0) && (thermals[index] < from_depth) )
         found = TRUE;
      else
         index--;
   }
   if (found)
      return thermals[index];
   else
      return from_depth;
}


/*
This function finds the next themral below the current depth. If
no thermal lies below this position then we return "from_depth".
*/
int MAP::Next_Down(int from_depth)
{
   int index = 0;
   int found = FALSE;

   while ( (!found) && (index < MAX_THERMALS) )
   {
        if ( (thermals[index] > 0) && (thermals[index] > from_depth) )
            found = TRUE;
        else
           index++;
   }
   if (found)
       return thermals[index];
   else
      return from_depth;
}







#ifdef DEBUGMAP
void MAP::Test_Map()
{
   int index;

   for (index = 0; index < MAX_THERMALS; index++)
     printf("Map termal %d: %d\n", index, thermals[index]);
   index = Thermals_Between(MIN_DEPTH, MAX_DEPTH);
   printf("Thermals between %d and %d: %d\n", MIN_DEPTH, MAX_DEPTH, index);
   index = Thermals_Between(MAX_DEPTH, MIN_DEPTH);
   printf("Thermals between %d and %d: %d\n", MAX_DEPTH, MIN_DEPTH, index);
}

#endif
