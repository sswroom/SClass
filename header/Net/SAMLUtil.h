#ifndef _SM_NET_SAMLUTIL
#define _SM_NET_SAMLUTIL
#include "Net/SSLEngine.h"
#include "Text/XMLReader.h"

namespace Net
{
	class SAMLUtil
	{
	private:
		static UOSInt DecryptEncryptedKey(NotNullPtr<Net::SSLEngine> ssl, NotNullPtr<Crypto::Cert::X509Key> key, NotNullPtr<Text::XMLReader> reader, NotNullPtr<Text::StringBuilderUTF8> sbResult, UInt8 *keyBuff);
		static UOSInt ParseKeyInfo(NotNullPtr<Net::SSLEngine> ssl, NotNullPtr<Crypto::Cert::X509Key> key, NotNullPtr<Text::XMLReader> reader, NotNullPtr<Text::StringBuilderUTF8> sbResult, UInt8 *keyBuff);
		static Bool DecryptEncryptedData(NotNullPtr<Net::SSLEngine> ssl, NotNullPtr<Crypto::Cert::X509Key> key, NotNullPtr<Text::XMLReader> reader, NotNullPtr<Text::StringBuilderUTF8> sbResult);
		static Bool DecryptAssertion(NotNullPtr<Net::SSLEngine> ssl, NotNullPtr<Crypto::Cert::X509Key> key, NotNullPtr<Text::XMLReader> reader, NotNullPtr<Text::StringBuilderUTF8> sbResult);
		static Bool DecryptResponse(NotNullPtr<Net::SSLEngine> ssl, NotNullPtr<Crypto::Cert::X509Key> key, NotNullPtr<Text::XMLReader> reader, NotNullPtr<Text::StringBuilderUTF8> sbResult);
	public:
		static Bool DecryptResponse(NotNullPtr<Net::SSLEngine> ssl, Text::EncodingFactory *encFact, NotNullPtr<Crypto::Cert::X509Key> key, Text::CString responseXML, NotNullPtr<Text::StringBuilderUTF8> sbResult);
		static Bool DecodeRequest(Text::CString requestB64, NotNullPtr<Text::StringBuilderUTF8> sbResult);
	};
}
#endif
