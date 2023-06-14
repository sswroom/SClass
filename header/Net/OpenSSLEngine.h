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
		Net::SSLClient *CreateClientConn(void *sslObj, Socket *s, Text::CString hostName, ErrorType *err);

		static Bool SetRSAPadding(void *ctx, Crypto::Encrypt::RSACipher::Padding rsaPadding);
	public:
		OpenSSLEngine(Net::SocketFactory *sockf, Method method);
		virtual ~OpenSSLEngine();
		
		virtual Bool IsError();

		virtual Bool ServerSetCertsASN1(Crypto::Cert::X509Cert *certASN1, Crypto::Cert::X509File *keyASN1, Crypto::Cert::X509Cert *caCert);
		virtual Bool ServerSetRequireClientCert(ClientCertType cliCert);
		virtual Bool ServerSetClientCA(Text::CString clientCA);
		virtual Bool ServerAddALPNSupport(Text::CString proto);

		virtual Bool ClientSetCertASN1(Crypto::Cert::X509Cert *certASN1, Crypto::Cert::X509File *keyASN1);
		virtual void ClientSetSkipCertCheck(Bool skipCertCheck);
		virtual Net::SSLClient *ClientConnect(Text::CString hostName, UInt16 port, ErrorType *err, Data::Duration timeout);
		virtual Net::SSLClient *ClientInit(Socket *s, Text::CString hostName, ErrorType *err);

		virtual UTF8Char *GetErrorDetail(UTF8Char *sbuff);
		virtual Bool GenerateCert(Text::CString country, Text::CString company, Text::CString commonName, Crypto::Cert::X509Cert **certASN1, Crypto::Cert::X509File **keyASN1);
		virtual Crypto::Cert::X509Key *GenerateRSAKey();
		virtual Bool Signature(Crypto::Cert::X509Key *key, Crypto::Hash::HashType hashType, const UInt8 *payload, UOSInt payloadLen, UInt8 *signData, UOSInt *signLen);
		virtual Bool SignatureVerify(Crypto::Cert::X509Key *key, Crypto::Hash::HashType hashType, const UInt8 *payload, UOSInt payloadLen, const UInt8 *signData, UOSInt signLen);
		virtual UOSInt Encrypt(Crypto::Cert::X509Key *key, UInt8 *encData, const UInt8 *payload, UOSInt payloadLen, Crypto::Encrypt::RSACipher::Padding rsaPadding);
		virtual UOSInt Decrypt(Crypto::Cert::X509Key *key, UInt8 *decData, const UInt8 *payload, UOSInt payloadLen, Crypto::Encrypt::RSACipher::Padding rsaPadding);

		UOSInt RSAPublicDecrypt(Crypto::Cert::X509Key *key, UInt8 *decData, const UInt8 *payload, UOSInt payloadLen, Crypto::Encrypt::RSACipher::Padding rsaPadding);
	};
}
#endif
