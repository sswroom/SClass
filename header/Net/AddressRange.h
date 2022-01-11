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
		AddressRange(const UTF8Char *addr, UOSInt addrLen, Bool scanBoardcast);
		~AddressRange();

		UOSInt GetCount();
		Bool GetItem(UOSInt index, Net::SocketUtil::AddressInfo *addr);
	};
}
#endif
