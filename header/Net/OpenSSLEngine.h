#ifndef _SM_NET_OPENSSLENGINE
#define _SM_NET_OPENSSLENGINE
#include "Net/SSLEngine.h"

namespace Net
{
	class OpenSSLEngine : public Net::SSLEngine
	{
	public:
		struct ClassData;
	private:
		struct ClassData *clsData;
		Bool skipCertCheck;

		virtual Net::SSLClient *CreateServerConn(Socket *s);
		Net::SSLClient *CreateClientConn(void *sslObj, Socket *s, Text::CStringNN hostName, OptOut<ErrorType> err);

		static Bool SetRSAPadding(void *ctx, Crypto::Encrypt::RSACipher::Padding rsaPadding);
	public:
		OpenSSLEngine(NN<Net::SocketFactory> sockf, Method method);
		virtual ~OpenSSLEngine();
		
		virtual Bool IsError();

		virtual Bool ServerSetCertsASN1(NN<Crypto::Cert::X509Cert> certASN1, NN<Crypto::Cert::X509File> keyASN1, NN<Data::ArrayListNN<Crypto::Cert::X509Cert>> caCerts);
		virtual Bool ServerSetRequireClientCert(ClientCertType cliCert);
		virtual Bool ServerSetClientCA(Text::CStringNN clientCA);
		virtual Bool ServerAddALPNSupport(Text::CStringNN proto);

		virtual Bool ClientSetCertASN1(NN<Crypto::Cert::X509Cert> certASN1, NN<Crypto::Cert::X509File> keyASN1);
		virtual void ClientSetSkipCertCheck(Bool skipCertCheck);
		virtual Net::SSLClient *ClientConnect(Text::CStringNN hostName, UInt16 port, OptOut<ErrorType> err, Data::Duration timeout);
		virtual Net::SSLClient *ClientInit(Socket *s, Text::CStringNN hostName, OptOut<ErrorType> err);

		virtual UTF8Char *GetErrorDetail(UTF8Char *sbuff);
		virtual Bool GenerateCert(Text::CString country, Text::CString company, Text::CStringNN commonName, OutParam<Crypto::Cert::X509Cert*> certASN1, OutParam<Crypto::Cert::X509File*> keyASN1);
		virtual Optional<Crypto::Cert::X509Key> GenerateRSAKey();
		virtual Bool Signature(NN<Crypto::Cert::X509Key> key, Crypto::Hash::HashType hashType, const UInt8 *payload, UOSInt payloadLen, UInt8 *signData, OutParam<UOSInt> signLen);
		virtual Bool SignatureVerify(NN<Crypto::Cert::X509Key> key, Crypto::Hash::HashType hashType, const UInt8 *payload, UOSInt payloadLen, const UInt8 *signData, UOSInt signLen);
		virtual UOSInt Encrypt(NN<Crypto::Cert::X509Key> key, UInt8 *encData, const UInt8 *payload, UOSInt payloadLen, Crypto::Encrypt::RSACipher::Padding rsaPadding);
		virtual UOSInt Decrypt(NN<Crypto::Cert::X509Key> key, UInt8 *decData, const UInt8 *payload, UOSInt payloadLen, Crypto::Encrypt::RSACipher::Padding rsaPadding);

		UOSInt RSAPublicDecrypt(NN<Crypto::Cert::X509Key> key, UInt8 *decData, const UInt8 *payload, UOSInt payloadLen, Crypto::Encrypt::RSACipher::Padding rsaPadding);
	};
}
#endif
