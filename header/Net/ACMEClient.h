#ifndef _SM_NET_ACMECLIENT
#define _SM_NET_ACMECLIENT
#include "Net/ACMEConn.h"

namespace Net
{
	class ACMEClient
	{
	private:
		Net::ACMEConn *acme;
		Bool keyReady;
		Bool accReady;

	public:
		ACMEClient(NotNullPtr<Net::SocketFactory> sockf, Text::CString serverHost, UInt16 port, Text::CString keyFile);
		~ACMEClient();

		Bool IsError();
		Text::String *GetTermOfService();
		Text::String *GetWebsite();
		Text::String *GetAccountId();
		Net::ACMEConn *GetConn();
	};
}
#endif
