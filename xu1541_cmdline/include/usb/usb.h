#ifndef _USB_H
#define _USB_H

#include <vector>
#include <string>

namespace usb
{

class USB
{
public:
	USB(void);
	virtual ~USB(void);

	std::vector<std::string> enumerateDevices(void) const;
};


}

#endif // _USB_H
