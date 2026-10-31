#ifndef _SM_IO_BTUTIL
#define _SM_IO_BTUTIL
#include "Data/UUID.h"
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
			UnsafeArray<const UTF8Char> name;
			UIntOS nameLen;
		} NumberMap;

	private:
		static const NumberMap compId[];
		static const NumberMap uuids[];
	public:
		static Text::CStringNN GetManufacturerName(UInt16 manuf);
		static Text::CString CompanyGetName(UInt16 compId);
		static void GetAddrText(NN<Text::StringBuilderUTF8> sb, UnsafeArray<const UInt8> addr);
		static UInt64 GetAddrMAC(UnsafeArray<const UInt8> addr);
		static Text::CStringNN GetUUIDName(UInt16 uuid);
		static void GetServiceName(NN<Text::StringBuilderUTF8> sb, NN<Data::UUID> serviceUUID);
		static Text::CStringNN CoreVersionGetName(UInt8 coreVersion);
		static Text::CStringNN DIACSGetName(UInt32 diacs);
		static Text::CStringNN ADTypesGetName(UInt8 adType);
	};
}
#endif
