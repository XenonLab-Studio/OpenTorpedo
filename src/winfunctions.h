#ifndef WINCOMPAT_HEADER_FILE__
#define WINCOMPAT_HEADER_FILE__

#ifdef WIN32

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

char *my_strcasestr(char *haystack, char *needle);

int my_strncasecmp(char *str1, char *str2, int letters);

#endif

#endif

