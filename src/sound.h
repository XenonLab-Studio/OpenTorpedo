#ifndef SOUND_HEADER_FILE__
#define SOUND_HEADER_FILE__

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

int Init_Audio(int enable_sound);

void Sound_Done();   // call back for when the music is finished playing

int Play_Sound(char *sound_file);

void Clean_Up_Audio();




#endif

