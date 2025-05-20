#ifndef _SM_NET_SAMLUTIL
#define _SM_NET_SAMLUTIL
#include "Net/SAMLStatusCode.h"
#include "Net/SSLEngine.h"
#include "Text/XMLReader.h"

namespace Net
{
	class SAMLUtil
	{
	private:
		static UOSInt DecryptEncryptedKey(NN<Net::SSLEngine> ssl, NN<Crypto::Cert::X509Key> key, NN<Text::XMLReader> reader, NN<Text::StringBuilderUTF8> sbResult, UnsafeArray<UInt8> keyBuff);
		static UOSInt ParseKeyInfo(NN<Net::SSLEngine> ssl, NN<Crypto::Cert::X509Key> key, NN<Text::XMLReader> reader, NN<Text::StringBuilderUTF8> sbResult, UnsafeArray<UInt8> keyBuff);
		static Bool DecryptEncryptedData(NN<Net::SSLEngine> ssl, NN<Crypto::Cert::X509Key> key, NN<Text::XMLReader> reader, NN<Text::StringBuilderUTF8> sbResult);
		static Bool DecryptAssertion(NN<Net::SSLEngine> ssl, NN<Crypto::Cert::X509Key> key, NN<Text::XMLReader> reader, NN<Text::StringBuilderUTF8> sbResult);
		static Bool DecryptResponse(NN<Net::SSLEngine> ssl, NN<Crypto::Cert::X509Key> key, NN<Text::XMLReader> reader, NN<Text::StringBuilderUTF8> sbResult, OptOut<SAMLStatusCode> statusCode);
	public:
		static Bool DecryptResponse(NN<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, NN<Crypto::Cert::X509Key> key, Text::CStringNN responseXML, NN<Text::StringBuilderUTF8> sbResult, OptOut<SAMLStatusCode> statusCode);
		static Bool DecodeRequest(Text::CStringNN requestB64, NN<Text::StringBuilderUTF8> sbResult);
	};
}
#endif
