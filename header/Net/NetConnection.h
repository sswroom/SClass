#ifndef _SM_NET_NETCONNECTION
#define _SM_NET_NETCONNECTION
#include "Net/SocketUtil.h"

namespace Net
{
	class NetConnection
	{
	public:
		virtual UnsafeArrayOpt<UTF8Char> GetRemoteName(UnsafeArray<UTF8Char> buff) const = 0;
		virtual UnsafeArrayOpt<UTF8Char> GetLocalName(UnsafeArray<UTF8Char> buff) const = 0;
		virtual Bool GetRemoteAddr(NN<Net::SocketUtil::AddressInfo> addr) const = 0;
		virtual UInt16 GetRemotePort() const = 0;
		virtual UInt16 GetLocalPort() const = 0;
	};
}
#endif
