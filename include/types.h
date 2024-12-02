#ifndef __TYPES_H__
#define __TYPES_H__ 1

#define COALENGINE_WIN32 

#if defined(COALENGINE_WIN32)
// Windows 32 bits--------------------------------------
#include<stdint.h>
typedef ::int8_t int8;
typedef ::uint8_t uint8;
typedef ::int16_t int16;
typedef ::uint16_t uint16;
typedef ::int32_t int32;
typedef ::uint32_t uint32;

#elif defined(COALENGINE_PS4)
//---
#elif defined(COALENGINE_RASPBERRY)
//---
#else
//#error Missing Platform
#endif

#endif
