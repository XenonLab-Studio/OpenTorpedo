#include <SDL.h>
#include "files.h"
#include "menu.h"
#include "draw.h"
#include "dfont.h"


SDL_Surface *menu_screen;

int Main_Menu(int *mission_number, SDL_Surface *screen)
{
   SDL_Rect dest;
   int command = 0;
   int selected_button = BUTTON_PLAY;
   int action;
   Uint32 menu_colour = SDL_MapRGB(screen->format, 0, 200,0);
   menu_screen = screen;

   dest.x = 400; dest.y = 0; dest.w = 624; dest.h = 768;
   Display_Mission(*mission_number);
   while (! command)
   {
       Draw_Main_Menu(selected_button, menu_colour);
       SDL_UpdateRects(menu_screen, 1, &dest);
       action = Menu_Input();
       if (action == BUTTON_ACTIVATE)
          action = selected_button;
       switch (action)
       {
          case BUTTON_PLAY: command = ACTION_MISSION; break;
          case BUTTON_QUIT: command = ACTION_QUIT; break;
          case BUTTON_PREV_MISSION:
                 selected_button = BUTTON_PREV_MISSION;
                 *mission_number = New_Mission(MISSION_PREVIOUS, *mission_number);
                 Display_Mission(*mission_number);
                 break;
          case BUTTON_NEXT_MISSION:
                 selected_button = BUTTON_NEXT_MISSION;
                 *mission_number = New_Mission(MISSION_NEXT, *mission_number);
                 Display_Mission(*mission_number);
                 break;
          case BUTTON_MOVE_UP:
                 selected_button--;
                 if (selected_button < BUTTON_PLAY)
                    selected_button = BUTTON_NEXT_MISSION;
                 break;
          case BUTTON_MOVE_DOWN:
                 selected_button++;
                 if (selected_button > BUTTON_NEXT_MISSION)
                     selected_button = BUTTON_PLAY;
                 break;
                 
       }
       SDL_Delay(100);
   }

   #ifdef DEBUG
   printf("Returning menu command: %d\n", command);
   #endif
   return command;
}



int New_Mission(int next_prev, int old_mission)
{
   FILE *mission_file;
   char *mission_path;
   char mission_filename[256];
   int new_mission;

   if (next_prev == MISSION_PREVIOUS)
   {
      new_mission = old_mission - 1;
      if (new_mission < 0)
        new_mission = 0;
   }
   else
   {
      new_mission = old_mission + 1;
   }

   #ifndef WIN32
   snprintf(mission_filename, 256, "data/mission%d.dat", new_mission);
   #else
   sprintf(mission_filename, "data/mission%d.dat", new_mission);
   #endif
   mission_path = Find_Data_File(mission_filename);
   mission_file = fopen(mission_path, "r");
   if ( (mission_path) && (mission_path != mission_filename) )
      free(mission_path);

   // found mission file, return the new number
   if (mission_file)
   {
       fclose(mission_file);
       return new_mission;
   }

   // we do not have a matching mission file, return the last good one
   return old_mission;
}



int Display_Mission(int mission_number)
{
   char *full_path, filename[256];
   FILE *mission_file;
   char line[256];
   int y = 610;
   DFont my_font("images/font.png", "data/font.dat");
   Uint32 menu_background = SDL_MapRGB(menu_screen->format, 0, 0, 0);
 
   #ifndef WIN32 
   snprintf(filename, 256, "data/orders%d.txt", mission_number);
   #else
   sprintf(filename, "data/orders%d.txt", mission_number);
   #endif
   full_path = Find_Data_File(filename);
   mission_file = fopen(full_path, "r");
   if ( (full_path) && (full_path != filename) )
      free(full_path);
   if (mission_file)
   {
      FillRectangle(menu_screen, 440, 610, 980, 730, menu_background);
      full_path = fgets(line, 256, mission_file);
      while (full_path)
      {  
         my_font.PutString(menu_screen, 440, y, line);
         full_path = fgets(line, 256, mission_file);
         y += 12;
      }
      fclose(mission_file);
   }
   return TRUE;
}



int Draw_Main_Menu(int selected, Uint32 menu_colour)
{
   Uint32 menu_background = SDL_MapRGB(menu_screen->format, 0, 0, 0);
   // wipe everything first
   DrawRectangle(menu_screen, 540, 160, 885, 235, menu_background);
   DrawRectangle(menu_screen, 540, 260, 885, 335, menu_background);
   FillCircle(menu_screen, 465, 565, 5, menu_background);
   FillCircle(menu_screen, 940, 565, 5, menu_background);

   switch (selected)
   {
      case BUTTON_PLAY:
           DrawRectangle(menu_screen, 540, 160, 885, 235, menu_colour);
           break;
      case BUTTON_QUIT:
           DrawRectangle(menu_screen, 540, 260, 885, 335, menu_colour);
           break;
      case BUTTON_PREV_MISSION:
           FillCircle(menu_screen, 465, 565, 5, menu_colour);
           break;
      case BUTTON_NEXT_MISSION:
           FillCircle(menu_screen, 940, 565, 5, menu_colour);
           break;

   }
   return TRUE;
}



int Menu_Input()
{
   int action = 0;
   SDL_Event event;
   int mousex, mousey;

   #ifdef DEBUG
   printf("About to check for event.\n");
   #endif
   while ( SDL_PollEvent(&event) )
   {
      #ifdef DEBUG
      printf("Got event.\n");
      #endif
      switch (event.type)
      {
          case SDL_MOUSEBUTTONDOWN:
                mousex = event.button.x;
                mousey = event.button.y;
                if ( (mousex > 540) && (mousex < 885) &&
                     (mousey > 160) && (mousey < 235) )
                     action = BUTTON_PLAY;
                else if ( (mousex > 540) && (mousex < 885) &&
                          (mousey > 260) && (mousey < 335) )
                     action = BUTTON_QUIT;
                else if ( (mousex > 440) && (mousex < 475) &&
                          (mousey > 550) && (mousey < 580) )
                     action = BUTTON_PREV_MISSION;
                else if ( (mousex > 920) && (mousex < 960) &&
                          (mousey > 550) && (mousey < 580) )
                     action = BUTTON_NEXT_MISSION;

          break;   // end of mouse stuff
          case SDL_KEYDOWN:
             if (event.key.keysym.sym == SDLK_UP)
                  action = BUTTON_MOVE_UP;
             else if (event.key.keysym.sym == SDLK_DOWN)
                  action = BUTTON_MOVE_DOWN;
             else if (event.key.keysym.sym == SDLK_ESCAPE)
                  action = BUTTON_QUIT;
             else if ( (event.key.keysym.sym == SDLK_SPACE) ||
                       (event.key.keysym.sym == SDLK_RETURN) )
                  action = BUTTON_ACTIVATE;
             break;
      }   // end of switch
   }      // end of which event
   
   #ifdef DEBUG
   printf("Returning action: %d\n", action);
   #endif
   return action;
}

