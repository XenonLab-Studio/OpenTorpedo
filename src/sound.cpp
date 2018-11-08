#include <stdio.h>
#include <SDL.h>
#include <SDL/SDL_mixer.h>
#include "sound.h"

Mix_Music *my_music = NULL;
int play_sounds = FALSE;

int Init_Audio(int allow_sounds)
{
   my_music = NULL;
   int audio_rate = 22050;
   Uint16 audio_format = AUDIO_S16; /* 16-bit stereo */
   int audio_channels = 2;
   int audio_buffers = 4096;

   if (allow_sounds)
     Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers);
     
   play_sounds = allow_sounds;
   
   return TRUE;
}


void Sound_Done()
{
    if (my_music)
    {
      Mix_HaltMusic();
      Mix_FreeMusic(my_music);
      my_music = NULL;
    }
}



int Play_Sound(char *sound_file)
{
   // maybe sound if disabled?
   if (! play_sounds)
      return FALSE;

   // something else is playing
   if (my_music)
      return FALSE;

   // bad pointer?
   if (! sound_file)
      return FALSE;

   my_music = Mix_LoadMUS(sound_file);
   if (my_music)
   {
       Mix_PlayMusic(my_music, 0);
       Mix_HookMusicFinished(Sound_Done);
       return TRUE;
   }
   return FALSE;
}



void Clean_Up_Audio()
{
    // sound was disabled, nothing to clean
    if (! play_sounds)
       return;

    Mix_HaltMusic();
    if (my_music)
       Mix_FreeMusic(my_music);
    my_music = NULL;
    Mix_CloseAudio();
}

