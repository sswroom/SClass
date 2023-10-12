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
			NotNullPtr<Net::SocketFactory> sockf;
			Net::SSLEngine *ssl;
			NotNullPtr<Text::String> notifyURL;
			NotNullPtr<Crypto::Cert::X509Cert> cert;
			NotNullPtr<Crypto::Cert::X509File> key;
			Crypto::Cert::X509File *svrCert;

		public:
			GCISClient(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, Text::CStringNN notifyURL, NotNullPtr<Crypto::Cert::X509Cert> cert, NotNullPtr<Crypto::Cert::X509File> key);
			~GCISClient();

			Bool SendMessage(Bool intranetChannel, Text::CString charset, Text::CStringNN contentType, Text::CStringNN subject, Text::CStringNN content, Text::CStringNN toList, Text::CString ccList, Text::CString bccList, Text::StringBuilderUTF8 *sbError);

			Crypto::Cert::X509File *GetServerCertChain() const;
		private:
			static void ParseEmailAddresses(NotNullPtr<Text::JSONBuilder> builder, Text::CStringNN toList, Text::CString ccList, Text::CString bccList);
		};
	}
}
#endif
