#ifndef _DLLHEADER_H
#define _DLLHEADER_H

#ifdef XU1541_EXPORTS

#define XU154_API __declspec(dllexport)

#else

#define XU154_API __declspec(dllimport)

#endif

#ifdef USB_EXPORTS

#define USB_API __declspec(dllexport)

#else

#define USB_API __declspec(dllimport)

#endif

#endif // _DLLHEADER_H
