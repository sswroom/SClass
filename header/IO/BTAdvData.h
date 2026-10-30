#ifndef _SM_IO_BTADVDATA
#define _SM_IO_BTADVDATA
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	class BTAdvData
	{
	public:
		static Bool ToString(UInt16 serviceUUID, Data::ByteArrayR serviceData, UnsafeArray<const UInt8> advData, NN<Text::StringBuilderUTF8> sb);
	};
}
#endif
