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
		static const MACEntry *GetMACInfo(UInt64 macAddr);
		static const MACEntry *GetMACInfoBuff(const UInt8 *mac);
		static const MACEntry *GetMACInfoOUI(const UInt8 *oui);
		static MACEntry *GetMACEntryList(UOSInt *cnt);
	};
}
#endif
