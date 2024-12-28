#ifndef SNPRINTF_H
#define SNPRINTF_H

#include "u.h"

int stbsp_snprintf(char * buf, int count, char const * fmt, ... );
int stbsp_vsnprintf(char * buf, int count, char const * fmt, va_list va);

#define snprintf   stbsp_snprintf
#define vsnprintf  stbsp_vsnprintf

#endif
