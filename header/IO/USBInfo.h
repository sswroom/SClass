#ifndef _SM_IO_USBINFO
#define _SM_IO_USBINFO
#include "Data/ArrayListNN.h"
#include "Text/CString.h"

namespace IO
{
	class USBInfo
	{
	public:
		struct ClassData;

	private:
		NN<ClassData> clsData;
	public:
		USBInfo(NN<ClassData> info);
		~USBInfo();

		UInt16 GetVendorId();
		UInt16 GetProductId();
		UInt16 GetRevision();
		Text::CStringNN GetDispName();

		static UOSInt GetUSBList(NN<Data::ArrayListNN<USBInfo>> usbList);
	};
}

#endif
