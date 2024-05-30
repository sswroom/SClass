#ifndef _SM_NET_NETBIOSUTIL
#define _SM_NET_NETBIOSUTIL
#include "Text/CString.h"

namespace Net
{
	class NetBIOSUtil
	{
	public:
		static UnsafeArrayOpt<UTF8Char> GetName(UnsafeArray<UTF8Char> sbuff, UnsafeArray<const UTF8Char> nbName);
		static UnsafeArray<UTF8Char> SetName(UnsafeArray<UTF8Char> nbBuff, UnsafeArray<const UTF8Char> name);
		static Text::CStringNN NameTypeGetName(UInt8 nameType);
	};
}
#endif
