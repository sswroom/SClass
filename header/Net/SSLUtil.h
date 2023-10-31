#ifndef _SM_NET_SSLUTIL
#define _SM_NET_SSLUTIL
#include "Crypto/Cert/X509File.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	enum class SSLVer
	{
		SSL3_0 = 0x300,
		TLS1_0 = 0x301,
		TLS1_1 = 0x302,
		TLS1_2 = 0x303,
		TLS1_3 = 0x304
	};

	class SSLUtil
	{
	private:
		static UInt16 csuites[];
		static UOSInt GenClientHello(UInt8 *buff, Text::CStringNN serverHost, SSLVer ver);
	public:
		static UOSInt GenSSLClientHello(UInt8 *buff, Text::CStringNN serverHost, SSLVer ver);
		static void ParseResponse(const UInt8 *buff, UOSInt packetSize, NotNullPtr<Text::StringBuilderUTF8> sb, OutParam<Crypto::Cert::X509File*> cert);
		static Bool IncompleteHandshake(const UInt8 *buff, UOSInt packetSize);
	};
}
#endif