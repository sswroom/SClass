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

	struct SSLCipherSuite
	{
		UInt16 id;
		const UTF8Char *name;
		UOSInt nameLen;
	};

	class SSLUtil
	{
	private:
		static SSLCipherSuite csuitesObj[];
		static UInt16 csuites[];
		static UOSInt GenClientHello(UInt8 *buff, Text::CStringNN serverHost, SSLVer ver);
	public:
		static UOSInt GenSSLClientHello(UInt8 *buff, Text::CStringNN serverHost, SSLVer ver);
		static void ParseResponse(const UInt8 *buff, UOSInt packetSize, NotNullPtr<Text::StringBuilderUTF8> sb, OutParam<Crypto::Cert::X509File*> cert);
		static Bool IncompleteHandshake(const UInt8 *buff, UOSInt packetSize);

		static Text::CStringNN AlertLevelGetName(UInt8 level);
		static Text::CStringNN AlertDescGetName(UInt8 desc);
		static Text::CStringNN RecordTypeGetName(UInt8 recordType);
		static Text::CStringNN HandshakeTypeGetName(UInt8 hsType);
		static Text::CStringNN CompressionMethodGetName(UInt8 method);
		static Text::CStringNN ExtensionTypeGetName(UInt16 extType);
		static Text::CStringNN ECPointFormatGetName(UInt8 fmt);
		static SSLCipherSuite *CipherSuiteGet(UInt16 cipherSuite);
		static Text::CStringNN CipherSuiteGetName(UInt16 cipherSuite);
	};

	Text::CStringNN SSLVerGetName(UInt16 ver);
}
#endif