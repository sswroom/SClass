#ifndef _SM_IO_BTUTIL
#define _SM_IO_BTUTIL
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	class BTUtil
	{
	public:
		typedef struct
		{
			UInt16 id;
			Text::CStringNN name;
		} NumberMap;

	private:
		static const NumberMap compId[];
		static const NumberMap uuids[];
	public:
		static Text::CStringNN GetManufacturerName(UInt16 manuf);
		static void GetAddrText(NN<Text::StringBuilderUTF8> sb, UInt8 *addr);
		static UInt64 GetAddrMAC(UInt8 *addr);
		static Text::CString GetUUIDName(UInt16 uuid);
		static void GetServiceName(NN<Text::StringBuilderUTF8> sb, void *serviceUUID);
	};
}
#endif
