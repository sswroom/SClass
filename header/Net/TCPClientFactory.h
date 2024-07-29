#ifndef _SM_NET_TCPCLIENTFACTORY
#define _SM_NET_TCPCLIENTFACTORY
#include "Net/TCPClient.h"

namespace Net
{
	class TCPClientFactory
	{
	private:
		NN<Net::SocketFactory> sockf;
		Text::CString proxyHost;
		UInt16 proxyPort;
		Text::CString proxyUser;
		Text::CString proxyPwd;

	public:
		TCPClientFactory(NN<Net::SocketFactory> sockf);
		~TCPClientFactory();

		void SetProxy(Text::CStringNN proxyHost, UInt16 proxyPort, Text::CString proxyUser, Text::CString proxyPwd);
		NN<Net::SocketFactory> GetSocketFactory() const;

		NN<TCPClient> Create(Text::CStringNN name, UInt16 port, Data::Duration timeout);
		NN<TCPClient> Create(UInt32 ip, UInt16 port, Data::Duration timeout);
		NN<TCPClient> Create(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::Duration timeout);
	};
}
#endif
