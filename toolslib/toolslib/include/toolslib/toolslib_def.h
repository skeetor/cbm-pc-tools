/*******************************************************************************
 *
 * ToolsLib (c) by Gerhard W. Gruber in 2014
 *
 *******************************************************************************/

#ifndef __TOOLSLIB_DEF_H__
#define __TOOLSLIB_DEF_H__

#include <cstddef>

#ifndef UNUSED

#define UNUSED(x) (void)x

#endif

typedef unsigned __int64 uint64_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int8 uint8_t;

typedef signed __int64 int64_t;
typedef signed __int32 int32_t;
typedef signed __int16 int16_t;
typedef signed __int8 int8_t;

static const size_t invalid = (size_t)-1;
static const uint64_t invalid64u_t = (uint64_t)-1;
static const uint32_t invalid32u_t = (uint32_t)-1;
static const uint16_t invalid16u_t = (uint16_t)-1;
static const uint8_t invalid8u_t = (uint8_t)-1;

static const int64_t invalid64_t = -1;
static const int32_t invalid32_t = -1;
static const int16_t invalid16_t = -1;
static const int8_t invalid8_t = -1;

#define __STR2__(x) #x
#define TOOLSLIB_DEFINE_TO_STRING(x) __STR2__(x)
#define TOOLSLIB_WARNING __FILE__ "("TOOLSLIB_DEFINE_TO_STRING(__LINE__)") : User warning: "

#endif // __TOOLSLIB_DEF_H__
