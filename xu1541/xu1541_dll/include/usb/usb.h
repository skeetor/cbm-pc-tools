#ifndef _USB_H
#define _USB_H

#include <vector>
#include <string>

#include "dllheader.h"

namespace usb
{

class USB_API USB
{
public:
	USB(void);
	virtual ~USB(void);

	std::vector<std::string> enumerateDevices(void) const;
};


}

#endif // _USB_H
