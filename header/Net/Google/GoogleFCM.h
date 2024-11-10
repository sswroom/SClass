#ifndef _SM_NET_GOOGLE_GOOGLEFCM
#define _SM_NET_GOOGLE_GOOGLEFCM
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	namespace Google
	{
		// Deprecated
		class GoogleFCM
		{
		private:
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			NN<Text::String> apiKey;
		public:
			GoogleFCM(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN apiKey) { this->clif = clif; this->ssl = ssl; this->apiKey = Text::String::New(apiKey); }
			~GoogleFCM() { this->apiKey->Release(); }
			Bool SendMessage(Text::CStringNN devToken, Text::CStringNN message, Optional<Text::StringBuilderUTF8> sbResult);
			Bool SendMessages(NN<Data::ArrayListNN<Text::String>> devTokens, Text::CStringNN message, Optional<Text::StringBuilderUTF8> sbResult);
		};
	}
}
#endif
