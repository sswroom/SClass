#ifndef _SM_IO_HIDINFO
#define _SM_IO_HIDINFO
#include "Data/ArrayListNN.h"
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

		NN<ClassData> clsData;

		HIDInfo(NN<ClassData> clsData);
	public:
		~HIDInfo();

		BusType GetBusType();
		UInt16 GetVendorId();
		UInt16 GetProductId();
		Text::String *GetDevPath();

		IO::Stream *OpenHID();

		static OSInt GetHIDList(NN<Data::ArrayListNN<HIDInfo>> hidList);
	};
};

#endif
