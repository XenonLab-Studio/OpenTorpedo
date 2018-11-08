#ifndef MENU_HEADER_FILE__
#define MENU_HEADER_FILE__

#define ACTION_QUIT 1
#define ACTION_MISSION 2

#define BUTTON_PLAY 1
#define BUTTON_QUIT 2
#define BUTTON_PREV_MISSION 3
#define BUTTON_NEXT_MISSION 4
#define BUTTON_MOVE_UP 5
#define BUTTON_MOVE_DOWN 6
#define BUTTON_ACTIVATE 7

#define MISSION_PREVIOUS 1
#define MISSION_NEXT 2

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif


// Returns one of the above actions
int Main_Menu(int *mission_number, SDL_Surface *screen);



// This function selects the next/previous mission
// It checks to make sure the mission files exist.
// The new mission number is returned on success and 
// the original mission number on failure.
int New_Mission(int next_prev, int old_mission);


// This function displays the description of a mission on the
// menu screen
int Display_Mission(int mission_number);


// This function draws the important buttons and keyboard
// cursor on the screen
// The "selected" parameter tells us which button is currently
// highlighted
int Draw_Main_Menu(int selected, Uint32 menu_colour);

// This function checks to see if we have any keyboard or
// mouse input. We return one of the "button" values
// defined above
int Menu_Input();


#endif

