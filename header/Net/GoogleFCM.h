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
		static Bool SendMessage(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CString apiKey, Text::CString devToken, Text::CString message, Text::StringBuilderUTF8 *sbResult);
		static Bool SendMessages(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CString apiKey, NN<Data::ArrayListNN<Text::String>> devTokens, Text::CString message, Text::StringBuilderUTF8 *sbResult);
	};
}
#endif
