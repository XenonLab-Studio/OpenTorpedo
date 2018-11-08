#ifdef WIN32
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "winfunctions.h"

char *my_strcasestr(char *haystack, char *needle)
{
    int length, index;
    char *up_haystack, *up_needle;
    char *result, *my_substr;

    if ( (!haystack) || (! needle) )
       return NULL;

    // create buffers
    up_haystack = (char *) calloc( strlen(haystack) + 1, sizeof(char));
    if (! up_haystack)
       return NULL;
    up_needle = (char *) calloc( strlen(needle) + 1, sizeof(char) );
    if (! up_needle)
    {
       free(up_haystack);
       return NULL;
    }

    length = strlen(haystack);
    for (index = 0; index < length; index++)
       up_haystack[index] = toupper(haystack[index]);
    length = strlen(needle);
    for (index = 0; index < length; index++)
       up_needle[index] = toupper(needle[index]);

    result = strstr(up_haystack, up_needle);
    if (result)
        my_substr = (result - up_haystack) + haystack;
    else
        my_substr = NULL;

    // free buffers
    free(up_needle);
    free(up_haystack);
    return my_substr;
}



int my_strncasecmp(char *str1, char *str2, int letters)
{
   int index = 0;
   int letter1, letter2;
   int result = FALSE;

   while ( (index < letters) && (! result) )
   {
       letter1 = toupper(str1[index]);
       letter2 = toupper(str2[index]);
       if (letter1 == letter2)
          index++;
       else if (letter1 < letter2)
          result = -1;
       else if (letter1 > letter2)
          result = 1;
       
   }
   return result;
}



#endif

