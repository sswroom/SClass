#ifndef _SM_IO_HIDINFO
#define _SM_IO_HIDINFO
#include "Data/ArrayList.h"
#include "IO/Stream.h"
#include "Text/String.h"

namespace IO
{
	class HIDInfo
	{
	public:
		typedef enum
		{
			BT_UNKNOWN,
			BT_USB,
			BT_I2C
		} BusType;
	private:
		struct ClassData;

		ClassData *clsData;

		HIDInfo(ClassData *clsData);
	public:
		~HIDInfo();

		BusType GetBusType();
		UInt16 GetVendorId();
		UInt16 GetProductId();
		Text::String *GetDevPath();

		IO::Stream *OpenHID();

		static OSInt GetHIDList(Data::ArrayList<HIDInfo*> *hidList);
	};
};

#endif
