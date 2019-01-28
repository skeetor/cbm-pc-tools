#ifndef _USB_H
#define _USB_H

class USB
{
public:
	USB(void);
	virtual ~USB(void);

	void enumerateDevices(void);
};

#endif // _USB_H
