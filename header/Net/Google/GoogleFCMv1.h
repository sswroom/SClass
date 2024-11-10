#ifndef _SM_NET_GOOGLE_GOOGLEFCMV1
#define _SM_NET_GOOGLE_GOOGLEFCMV1
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	namespace Google
	{
		class GoogleFCMv1
		{
		private:
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			NN<Text::String> projectId;
		public:
			GoogleFCMv1(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN projectId) { this->clif = clif; this->ssl = ssl; this->projectId = Text::String::New(projectId); }
			~GoogleFCMv1() { this->projectId->Release(); }
			Bool SendMessage(Text::CStringNN accessToken, Text::CStringNN devToken, Text::CStringNN message, Optional<Text::StringBuilderUTF8> sbResult);
		};
	}
}
#endif
