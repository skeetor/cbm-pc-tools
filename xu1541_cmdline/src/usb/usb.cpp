
#include <windows.h>

#include <WinUSB.h>
#include <initguid.h>
#include <setupapi.h>
#include <usbiodef.h>
#include <atlbase.h>
#include <array>

#include "usb/usb.h"
#include "usb/WinUSBWrappers.h"

using namespace std;

namespace usb
{

USB::USB(void)
{
}

USB::~USB(void)
{
}

vector<string> USB::enumerateDevices(void) const
{
	vector<string> deviceInfo;
	WinUSB::StringArray deviceNames;

	BOOL bSuccess = WinUSB::CDevice::EnumerateDevices(&GUID_DEVINTERFACE_USB_DEVICE, deviceNames, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE, nullptr, [](HDEVINFO hDeviceInfoSet, _In_ PSP_DEVICE_INTERFACE_DETAIL_DATA /*pDeviceInterfaceDetailData*/, _In_ PSP_DEVINFO_DATA pDeviceInfoData, LPCTSTR pszDevicePath, DWORD_PTR /*dwItemData*/) noexcept
	{
		printf(" %s\n", pszDevicePath);
		std::array<char, 4096> buffer;
		buffer[0] = '\0';

		if (SetupDiGetDeviceRegistryProperty(hDeviceInfoSet, pDeviceInfoData, SPDRP_FRIENDLYNAME, nullptr, reinterpret_cast<PBYTE>(buffer.data()), static_cast<DWORD>(buffer.size()), nullptr))
			printf(" Friendly name:%s\n", buffer.data());
		if (SetupDiGetDeviceRegistryProperty(hDeviceInfoSet, pDeviceInfoData, SPDRP_CLASS, nullptr, reinterpret_cast<PBYTE>(buffer.data()), static_cast<DWORD>(buffer.size()), nullptr))
			printf(" Class:%s\n", buffer.data());
		if (SetupDiGetDeviceRegistryProperty(hDeviceInfoSet, pDeviceInfoData, SPDRP_CLASSGUID, nullptr, reinterpret_cast<PBYTE>(buffer.data()), static_cast<DWORD>(buffer.size()), nullptr))
			printf(" Class GUID:%s\n", buffer.data());
		if (SetupDiGetDeviceRegistryProperty(hDeviceInfoSet, pDeviceInfoData, SPDRP_DEVICEDESC, nullptr, reinterpret_cast<PBYTE>(buffer.data()), static_cast<DWORD>(buffer.size()), nullptr))
			printf(" Device Description:%s\n", buffer.data());
		if (SetupDiGetDeviceRegistryProperty(hDeviceInfoSet, pDeviceInfoData, SPDRP_DRIVER, nullptr, reinterpret_cast<PBYTE>(buffer.data()), static_cast<DWORD>(buffer.size()), nullptr))
			printf(" Driver:%s\n", buffer.data());
		if (SetupDiGetDeviceRegistryProperty(hDeviceInfoSet, pDeviceInfoData, SPDRP_ENUMERATOR_NAME, nullptr, reinterpret_cast<PBYTE>(buffer.data()), static_cast<DWORD>(buffer.size()), nullptr))
			printf(" Enumerator Name:%s\n", buffer.data());
		if (SetupDiGetDeviceRegistryProperty(hDeviceInfoSet, pDeviceInfoData, SPDRP_LOCATION_INFORMATION, nullptr, reinterpret_cast<PBYTE>(buffer.data()), static_cast<DWORD>(buffer.size()), nullptr))
			printf(" Location Information:%s\n", buffer.data());
		if (SetupDiGetDeviceRegistryProperty(hDeviceInfoSet, pDeviceInfoData, SPDRP_MFG, nullptr, reinterpret_cast<PBYTE>(buffer.data()), static_cast<DWORD>(buffer.size()), nullptr))
			printf(" Manufacturer:%s\n", buffer.data());
		if (SetupDiGetDeviceRegistryProperty(hDeviceInfoSet, pDeviceInfoData, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME, nullptr, reinterpret_cast<PBYTE>(buffer.data()), static_cast<DWORD>(buffer.size()), nullptr))
			printf(" PDO:%s\n", buffer.data());
		if (SetupDiGetDeviceRegistryProperty(hDeviceInfoSet, pDeviceInfoData, SPDRP_SECURITY_SDS, nullptr, reinterpret_cast<PBYTE>(buffer.data()), static_cast<DWORD>(buffer.size()), nullptr))
			printf(" Security Descriptor:%s\n", buffer.data());
		if (SetupDiGetDeviceRegistryProperty(hDeviceInfoSet, pDeviceInfoData, SPDRP_SERVICE, nullptr, reinterpret_cast<PBYTE>(buffer.data()), static_cast<DWORD>(buffer.size()), nullptr))
			printf(" Service:%s\n", buffer.data());
		printf("\n");

		return true;
	});

	return deviceInfo;
}

}
