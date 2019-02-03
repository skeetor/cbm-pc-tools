/*******************************************************************************
 *
 * ToolsLib (c) by Gerhard W. Gruber in 2014
 *
 *******************************************************************************/

#ifndef __TOOLSLIB_DLL_API_H__
#define __TOOLSLIB_DLL_API_H__

#ifdef WINDOWS
#include <windows.h>
#endif // WINDOWS

// TOOLS_DLL must be define when using the library as a DLL.
// BUILD_TOOLS_DLL is defined internally when building the DLL.

#ifndef TOOLSLIB_DLL
	#define TOOLSLIB_API
#else
	#ifdef BUILD_TOOLSLIB_DLL
		#ifdef WINDOWS
			#define TOOLSLIB_API __declspec(dllexport)
		#endif // WINDOWS

		#ifdef LINUX
			#define TOOLSLIB_API __attribute__ ((visibility ("default")))
		#endif // LINUX
	#else							// Make use of the DLL
		#ifdef WINDOWS
			#define TOOLSLIB_API __declspec(dllimport)
		#endif // WINDOWS

		#ifdef LINUX
			#define TOOLSLIB_API
		#endif // LINUX
	#endif // TOOLSLIB_DLL
#endif

#endif // __TOOLSLIB_DLL_API_H__
