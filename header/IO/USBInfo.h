#ifndef _SM_IO_USBINFO
#define _SM_IO_USBINFO
#include "Data/ArrayList.h"

namespace IO
{
	class USBInfo
	{
	private:
		void *clsData;
	public:
		USBInfo(void *info);
		~USBInfo();

		UInt16 GetVendorId();
		UInt16 GetProductId();
		UInt16 GetRevision();
		const UTF8Char *GetDispName();

		static OSInt GetUSBList(Data::ArrayList<USBInfo*> *usbList);
	};
}

#endif
