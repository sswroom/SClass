#ifndef _SM_DATA_DWGUTIL
#define _SM_DATA_DWGUTIL
#include "UnsafeArray.h"

namespace Data
{
	class DWGUtil
	{
	public:
		static void HeaderDecrypt(UnsafeArray<const UInt8> srcPtr, UnsafeArray<UInt8> destPtr, UIntOS size);
	};
}
#endif
