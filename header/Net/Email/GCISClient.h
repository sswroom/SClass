#ifndef _SM_NET_EMAIL_GCISCLIENT
#define _SM_NET_EMAIL_GCISCLIENT
#include "Crypto/Cert/X509Cert.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"

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

		public:
			GCISClient(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, Text::CStringNN notifyURL, NotNullPtr<Crypto::Cert::X509Cert> cert, NotNullPtr<Crypto::Cert::X509File> key);
			~GCISClient();

			Bool SendMessage(Bool internalChannel, Text::CString charset, Text::CStringNN contentType, Text::CStringNN subject, Data::ByteArrayR content, Text::CStringNN toList, Text::CString ccList, Text::StringBuilderUTF8 *sbError);
		};
	}
}
#endif
