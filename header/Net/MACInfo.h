#ifndef _SM_NET_MACINFO
#define _SM_NET_MACINFO
#include "Text/CString.h"

namespace Net
{
	class MACInfo
	{
	public:
		typedef struct
		{
			UInt64 rangeStart;
			UInt64 rangeEnd;
			const UTF8Char *name;
			UOSInt nameLen;
		} MACEntry;
		
	private:
		static MACEntry macList[];
		static MACEntry uncMac;
	public:
		static NN<const MACEntry> GetMACInfo(UInt64 macAddr);
		static NN<const MACEntry> GetMACInfoBuff(UnsafeArray<const UInt8> mac);
		static NN<const MACEntry> GetMACInfoOUI(UnsafeArray<const UInt8> oui);
		static UnsafeArray<MACEntry> GetMACEntryList(OutParam<UOSInt> cnt);
	};
}
#endif
