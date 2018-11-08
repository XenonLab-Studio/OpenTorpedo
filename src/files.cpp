#include <stdio.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "files.h"

static char *data_dir = NULL;

// Find a file's full path based on the name of a file
char *Find_Data_File(char *filename)
{
   char *location;
   FILE *my_file;

   location = (char *) calloc( strlen(filename) + 64, sizeof(char) );
   if (! location)
      return filename;

   // maybe we already know the directory and don't have to search
   if (data_dir) 
   {
      sprintf(location, "%s/%s", data_dir, filename);
      return location;
   }

   sprintf(location, "%s/%s", DATADIR, filename);
   my_file = fopen(location, "r");
   if (my_file)
   {
     fclose(my_file);
     data_dir = DATADIR;
     return location;
   }

   /*
   This is the old way of guessing one location at a time. Now we
   can use the DATADIR definition to make sure we check the correct
   place.
   
   sprintf(location, "/usr/local/share/games/OpenTorpedo/%s", filename);
   my_file = fopen(location, "r");
   if (my_file)
   {
       fclose(my_file);
       data_dir = "/usr/local/share/games/OpenTorpedo";
       return location;
   }

   sprintf(location, "/usr/share/games/OpenTorpedo/%s", filename);
   my_file = fopen(location, "r");
   if (my_file)
   {
      fclose(my_file);
      data_dir = "/usr/share/games/OpenTorpedo";
      return location;
   }
   */

   // last chance, try OPTDIR
   sprintf(location, "%s/%s", OPTDIR, filename);
   my_file = fopen(location, "r");
   if (my_file)
   {
      fclose(my_file);
      data_dir = OPTDIR;
      return location;
   }
   data_dir = ".";  
   return filename;
}


SDL_Surface *Load_Image(char *filename)
{
   char *file_name;
   SDL_Surface *my_image = NULL;

   file_name = Find_Data_File(filename);
   if (file_name)
      my_image = IMG_Load(file_name);
   if ( (file_name) && (file_name != filename) )
       free(file_name);
   return my_image;

}


