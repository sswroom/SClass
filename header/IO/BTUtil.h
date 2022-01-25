#ifndef _SM_IO_BTUTIL
#define _SM_IO_BTUTIL
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	class BTUtil
	{
	public:
		typedef struct
		{
			UInt16 id;
			const Char *name;
		} NumberMap;

	private:
		static const NumberMap compId[];
		static const NumberMap uuids[];
	public:
		static const UTF8Char *GetManufacturerName(UInt16 manuf);
		static void GetAddrText(Text::StringBuilderUTF8 *sb, UInt8 *addr);
		static UInt64 GetAddrMAC(UInt8 *addr);
		static const UTF8Char *GetUUIDName(UInt16 uuid);
		static void GetServiceName(Text::StringBuilderUTF8 *sb, void *serviceUUID);
	};
}
#endif
