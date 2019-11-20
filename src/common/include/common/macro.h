#ifndef _COMMON_MACRO_H
#define _COMMON_MACRO_H

#define xstr(s) str(s)
#define str(s) #s

#define ARRAY_SIZE(X) (sizeof(X) / sizeof((X)[0]))

#endif // _COMMON_MACRO_H
