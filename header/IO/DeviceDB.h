#ifndef _SM_IO_DEVICEDB
#define _SM_IO_DEVICEDB
#include "Text/CString.h"

namespace IO
{
	class DeviceDB
	{
	public:
		typedef struct
		{
			UInt16 vendorId;
			Text::CString vendorName;
		} VendorInfo;

		typedef struct
		{
			UInt16 vendorId;
			UInt16 productId;
			UInt16 bcdDevice;
			const Char *dispName;
			const Char *productName;
		} USBDeviceInfo;

		typedef struct
		{
			UInt16 vendorId;
			UInt16 productId;
			const Char *dispName;
			const Char *productName;
		} PCIDeviceInfo;

		typedef struct
		{
			const Char *cid;
			const Char *csd;
			const Char *name;
		} SDCardInfo;

	private:
		static VendorInfo usbVendors[];
		static USBDeviceInfo usbList[];
		static VendorInfo pciVendors[];
		static PCIDeviceInfo pciList[];
		static SDCardInfo sdCardList[];
		static SDCardInfo emmcList[];

	public:
		static const USBDeviceInfo *GetUSBInfo(UInt16 vendorId, UInt16 productId, UInt16 bcdDevice);
		static const PCIDeviceInfo *GetPCIInfo(UInt16 vendorId, UInt16 productId);
		static Text::CString GetUSBVendorName(UInt16 vendorId);
		static Text::CString GetPCIVendorName(UInt16 vendorId);
	};
}

#endif
