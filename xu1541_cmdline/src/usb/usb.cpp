
#include <windows.h>

#include <WinUSB.h>
#include <initguid.h>
#include <setupapi.h>
#include <usbiodef.h>
#include <atlbase.h>
#include <string>
#include <vector>

#include "usb/usb.h"
#include "usb/WinUSBWrappers.h"

USB::USB(void)
{
}

USB::~USB(void)
{
}

void USB::enumerateDevices(void)
{
	WinUSB::StringArray deviceNames;

	BOOL bSuccess = WinUSB::CDevice::EnumerateDevices(&GUID_DEVINTERFACE_USB_DEVICE, deviceNames, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE, nullptr, [](HDEVINFO hDeviceInfoSet, _In_ PSP_DEVICE_INTERFACE_DETAIL_DATA /*pDeviceInterfaceDetailData*/, _In_ PSP_DEVINFO_DATA pDeviceInfoData, LPCTSTR pszDevicePath, DWORD_PTR /*dwItemData*/) noexcept
	{
		_tprintf(_T(" %s\n"), pszDevicePath);
		std::array<TCHAR, 4096> buffer;
#pragma warning(suppress: 26446)
		buffer[0] = _T('\0');
#pragma warning(suppress: 26472 26490)
		if (SetupDiGetDeviceRegistryProperty(hDeviceInfoSet, pDeviceInfoData, SPDRP_FRIENDLYNAME, nullptr, reinterpret_cast<PBYTE>(buffer.data()), static_cast<DWORD>(buffer.size()), nullptr))
			_tprintf(_T(" Friendly name:%s\n"), buffer.data());
#pragma warning(suppress: 26472 26490)
		if (SetupDiGetDeviceRegistryProperty(hDeviceInfoSet, pDeviceInfoData, SPDRP_CLASS, nullptr, reinterpret_cast<PBYTE>(buffer.data()), static_cast<DWORD>(buffer.size()), nullptr))
			_tprintf(_T(" Class:%s\n"), buffer.data());
#pragma warning(suppress: 26472 26490)
		if (SetupDiGetDeviceRegistryProperty(hDeviceInfoSet, pDeviceInfoData, SPDRP_CLASSGUID, nullptr, reinterpret_cast<PBYTE>(buffer.data()), static_cast<DWORD>(buffer.size()), nullptr))
			_tprintf(_T(" Class GUID:%s\n"), buffer.data());
#pragma warning(suppress: 26472 26490)
		if (SetupDiGetDeviceRegistryProperty(hDeviceInfoSet, pDeviceInfoData, SPDRP_DEVICEDESC, nullptr, reinterpret_cast<PBYTE>(buffer.data()), static_cast<DWORD>(buffer.size()), nullptr))
			_tprintf(_T(" Device Description:%s\n"), buffer.data());
#pragma warning(suppress: 26472 26490)
		if (SetupDiGetDeviceRegistryProperty(hDeviceInfoSet, pDeviceInfoData, SPDRP_DRIVER, nullptr, reinterpret_cast<PBYTE>(buffer.data()), static_cast<DWORD>(buffer.size()), nullptr))
			_tprintf(_T(" Driver:%s\n"), buffer.data());
#pragma warning(suppress: 26472 26490)
		if (SetupDiGetDeviceRegistryProperty(hDeviceInfoSet, pDeviceInfoData, SPDRP_ENUMERATOR_NAME, nullptr, reinterpret_cast<PBYTE>(buffer.data()), static_cast<DWORD>(buffer.size()), nullptr))
			_tprintf(_T(" Enumerator Name:%s\n"), buffer.data());
#pragma warning(suppress: 26472 26490)
		if (SetupDiGetDeviceRegistryProperty(hDeviceInfoSet, pDeviceInfoData, SPDRP_LOCATION_INFORMATION, nullptr, reinterpret_cast<PBYTE>(buffer.data()), static_cast<DWORD>(buffer.size()), nullptr))
			_tprintf(_T(" Location Information:%s\n"), buffer.data());
#pragma warning(suppress: 26472 26490)
		if (SetupDiGetDeviceRegistryProperty(hDeviceInfoSet, pDeviceInfoData, SPDRP_MFG, nullptr, reinterpret_cast<PBYTE>(buffer.data()), static_cast<DWORD>(buffer.size()), nullptr))
			_tprintf(_T(" Manufacturer:%s\n"), buffer.data());
#pragma warning(suppress: 26472 26490)
		if (SetupDiGetDeviceRegistryProperty(hDeviceInfoSet, pDeviceInfoData, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME, nullptr, reinterpret_cast<PBYTE>(buffer.data()), static_cast<DWORD>(buffer.size()), nullptr))
			_tprintf(_T(" PDO:%s\n"), buffer.data());
#pragma warning(suppress: 26472 26490)
		if (SetupDiGetDeviceRegistryProperty(hDeviceInfoSet, pDeviceInfoData, SPDRP_SECURITY_SDS, nullptr, reinterpret_cast<PBYTE>(buffer.data()), static_cast<DWORD>(buffer.size()), nullptr))
			_tprintf(_T(" Security Descriptor:%s\n"), buffer.data());
#pragma warning(suppress: 26472 26490)
		if (SetupDiGetDeviceRegistryProperty(hDeviceInfoSet, pDeviceInfoData, SPDRP_SERVICE, nullptr, reinterpret_cast<PBYTE>(buffer.data()), static_cast<DWORD>(buffer.size()), nullptr))
			_tprintf(_T(" Service:%s\n"), buffer.data());
		_tprintf(_T("\n"));

		return true;
	});
}

