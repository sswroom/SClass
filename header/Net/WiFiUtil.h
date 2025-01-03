#ifndef _SM_NET_WIFIUTIL
#define _SM_NET_WIFIUTIL
#include "Net/WirelessLAN.h"

namespace Net
{
	class WiFiUtil
	{
	public:
		static UnsafeArrayOpt<UTF8Char> GuessDeviceType(UnsafeArray<UTF8Char> sbuff, NN<Net::WirelessLAN::BSSInfo> bss);
	};
}
#endif
