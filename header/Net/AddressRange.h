#ifndef _SM_NET_ADDRESSRANGE
#define _SM_NET_ADDRESSRANGE
#include "Net/SocketUtil.h"

namespace Net
{
	class AddressRange
	{
	private:
		enum AddrType
		{
			AT_ERROR,
			AT_SINGLE,
			AT_RANGE,
			AT_MASK,
			AT_SCAN
		};
	private:
		Net::SocketUtil::AddressInfo addr1;
		UInt8 param;
		Bool skipFirst;
		Bool skipLast;
		AddrType aType;

	public:
		AddressRange(UnsafeArray<const UTF8Char> addr, UIntOS addrLen, Bool scanBoardcast);
		~AddressRange();

		UIntOS GetCount();
		Bool GetItem(UIntOS index, Net::SocketUtil::AddressInfo *addr);
	};
}
#endif
