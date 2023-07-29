#ifndef _SM_NET_SAMLUTIL
#define _SM_NET_SAMLUTIL
#include "Net/SSLEngine.h"
#include "Text/XMLReader.h"

namespace Net
{
	class SAMLUtil
	{
	private:
		static UOSInt DecryptEncryptedKey(Net::SSLEngine *ssl, Crypto::Cert::X509Key *key, Text::XMLReader *reader, NotNullPtr<Text::StringBuilderUTF8> sbResult, UInt8 *keyBuff);
		static UOSInt ParseKeyInfo(Net::SSLEngine *ssl, Crypto::Cert::X509Key *key, Text::XMLReader *reader, NotNullPtr<Text::StringBuilderUTF8> sbResult, UInt8 *keyBuff);
		static Bool DecryptEncryptedData(Net::SSLEngine *ssl, Crypto::Cert::X509Key *key, Text::XMLReader *reader, NotNullPtr<Text::StringBuilderUTF8> sbResult);
		static Bool DecryptAssertion(Net::SSLEngine *ssl, Crypto::Cert::X509Key *key, Text::XMLReader *reader, NotNullPtr<Text::StringBuilderUTF8> sbResult);
		static Bool DecryptResponse(Net::SSLEngine *ssl, Crypto::Cert::X509Key *key, Text::XMLReader *reader, NotNullPtr<Text::StringBuilderUTF8> sbResult);
	public:
		static Bool DecryptResponse(Net::SSLEngine *ssl, Text::EncodingFactory *encFact, Crypto::Cert::X509Key *key, Text::CString responseXML, NotNullPtr<Text::StringBuilderUTF8> sbResult);
		static Bool DecodeRequest(Text::CString requestB64, NotNullPtr<Text::StringBuilderUTF8> sbResult);
	};
}
#endif
