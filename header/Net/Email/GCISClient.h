#ifndef _SM_NET_EMAIL_GCISCLIENT
#define _SM_NET_EMAIL_GCISCLIENT
#include "Crypto/Cert/X509Cert.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Text/JSONBuilder.h"

namespace Net
{
	namespace Email
	{
		class GCISClient
		{
		private:
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			NN<Text::String> notifyURL;
			NN<Crypto::Cert::X509Cert> cert;
			NN<Crypto::Cert::X509File> key;
			Optional<Crypto::Cert::X509File> svrCert;

		public:
			GCISClient(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN notifyURL, NN<Crypto::Cert::X509Cert> cert, NN<Crypto::Cert::X509File> key);
			~GCISClient();

			Bool SendMessage(Bool intranetChannel, Text::CString charset, Text::CStringNN contentType, Text::CStringNN subject, Text::CStringNN content, Text::CStringNN toList, Text::CString ccList, Text::CString bccList, Optional<Text::StringBuilderUTF8> sbError);

			Optional<Crypto::Cert::X509File> GetServerCertChain() const;
		private:
			static void ParseEmailAddresses(NN<Text::JSONBuilder> builder, Text::CStringNN toList, Text::CString ccList, Text::CString bccList);
		};
	}
}
#endif
