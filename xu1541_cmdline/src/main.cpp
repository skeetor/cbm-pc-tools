// xu1541_cmdline.cpp : Defines the entry point for the console application.
//

#include "usb/usb.h"
#include "xu1541/xu1541.h"

// http://www.naughter.com/winusbwrappers.html


int main(int argc, char *argv[])
{
	usb::USB usb;

	usb.enumerateDevices();

    return 0;
}
