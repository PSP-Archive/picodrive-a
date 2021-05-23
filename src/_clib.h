// clib-mimic

#ifndef _CLIB_MIMIC_H_INCLUDED
#define _CLIB_MIMIC_H_INCLUDED

#ifndef NULL
#define NULL (0)
#endif

void my_memset0(void *d,  unsigned int n);
//void my_memset(void *d,  long s, unsigned long n);
void my_memset(void *d,  int s, unsigned int n);
//int _memcmp(const void *s1, const void *s2, unsigned long n);
//static void _memcpy(void *d,  void *s, unsigned long n);

void _strcpy(char *d,const char *s);
//void _strcat(char *d,const char *s);
//void _strncpy(char *d,const char *s,int n);
//char *_strchr(const char *s, int c);
unsigned int _strlen(const char *s);
char* _strrchr(const char *src, int c);



#endif //_CLIB_MIMIC_H_INCLUDED
