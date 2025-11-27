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
			Text::CStringNN vendorName;
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
		} DeviceInfo;

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
		static DeviceInfo pciList[];
		static DeviceInfo btList[];
		static SDCardInfo sdCardList[];
		static SDCardInfo emmcList[];

	public:
		static Optional<const USBDeviceInfo> GetUSBInfo(UInt16 vendorId, UInt16 productId, UInt16 bcdDevice);
		static Optional<const DeviceInfo> GetPCIInfo(UInt16 vendorId, UInt16 productId);
		static Optional<const DeviceInfo> GetBTInfo(UInt16 vendorId, UInt16 productId);
		static Text::CStringNN GetUSBVendorName(UInt16 vendorId);
		static Text::CStringNN GetPCIVendorName(UInt16 vendorId);
	};
}

#endif
