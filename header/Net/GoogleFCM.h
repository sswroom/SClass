#ifndef _SM_NET_GOOGLEFCM
#define _SM_NET_GOOGLEFCM
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	class GoogleFCM
	{
	public:
		static Bool SendMessage(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN apiKey, Text::CStringNN devToken, Text::CStringNN message, Optional<Text::StringBuilderUTF8> sbResult);
		static Bool SendMessages(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN apiKey, NN<Data::ArrayListNN<Text::String>> devTokens, Text::CStringNN message, Optional<Text::StringBuilderUTF8> sbResult);
	};
}
#endif
