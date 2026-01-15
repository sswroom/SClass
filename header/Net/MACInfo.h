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
			UIntOS nameLen;
		} MACEntry;

		enum class AddressType
		{
			UniversalUnicast,
			UniversalMulticast,
			LocalUnicast,
			LocalMulticast
		};
		
	private:
		static MACEntry macList[];
		static MACEntry uncMac;
	public:
		static NN<const MACEntry> GetMAC64Info(UInt64 macAddr);
		static NN<const MACEntry> GetMACInfoBuff(UnsafeArray<const UInt8> mac);
		static NN<const MACEntry> GetMACInfoOUI(UnsafeArray<const UInt8> oui);
		static UnsafeArray<MACEntry> GetMACEntryList(OutParam<UIntOS> cnt);
		static AddressType GetAddressType(UnsafeArray<const UInt8> mac);
		static Text::CStringNN AddressTypeGetName(AddressType addrType);
	};
}
#endif
