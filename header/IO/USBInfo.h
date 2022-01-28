#ifndef _SM_IO_USBINFO
#define _SM_IO_USBINFO
#include "Data/ArrayList.h"
#include "Text/CString.h"

namespace IO
{
	class USBInfo
	{
	private:
		struct ClassData;

		ClassData *clsData;
	public:
		USBInfo(ClassData *info);
		~USBInfo();

		UInt16 GetVendorId();
		UInt16 GetProductId();
		UInt16 GetRevision();
		Text::CString GetDispName();

		static UOSInt GetUSBList(Data::ArrayList<USBInfo*> *usbList);
	};
}

#endif
