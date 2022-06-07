#ifndef _SM_NET_OPENSSLENGINE
#define _SM_NET_OPENSSLENGINE
#include "Net/SSLEngine.h"

namespace Net
{
	class OpenSSLEngine : public Net::SSLEngine
	{
	private:
		struct ClassData;
		struct ClassData *clsData;
		Bool skipCertCheck;

		virtual Net::SSLClient *CreateServerConn(Socket *s);
		Net::SSLClient *CreateClientConn(void *sslObj, Socket *s, Text::CString hostName, ErrorType *err);
	public:
		OpenSSLEngine(Net::SocketFactory *sockf, Method method);
		virtual ~OpenSSLEngine();
		
		virtual void SetSkipCertCheck(Bool skipCertCheck);
		virtual Bool IsError();
		virtual Bool SetServerCertsASN1(Crypto::Cert::X509Cert *certASN1, Crypto::Cert::X509File *keyASN1, Crypto::Cert::X509Cert *caCert);
		virtual Bool SetClientCertASN1(Crypto::Cert::X509Cert *certASN1, Crypto::Cert::X509File *keyASN1);
		virtual UTF8Char *GetErrorDetail(UTF8Char *sbuff);
		virtual Net::SSLClient *Connect(Text::CString hostName, UInt16 port, ErrorType *err);
		virtual Net::SSLClient *ClientInit(Socket *s, Text::CString hostName, ErrorType *err);
		virtual Bool GenerateCert(Text::CString country, Text::CString company, Text::CString commonName, Crypto::Cert::X509Cert **certASN1, Crypto::Cert::X509File **keyASN1);
		virtual Crypto::Cert::X509Key *GenerateRSAKey();
		virtual Bool Signature(Crypto::Cert::X509Key *key, Crypto::Hash::HashType hashType, const UInt8 *payload, UOSInt payloadLen, UInt8 *signData, UOSInt *signLen);
		virtual Bool SignatureVerify(Crypto::Cert::X509Key *key, Crypto::Hash::HashType hashType, const UInt8 *payload, UOSInt payloadLen, const UInt8 *signData, UOSInt signLen);
		virtual UOSInt Encrypt(Crypto::Cert::X509Key *key, UInt8 *encData, const UInt8 *payload, UOSInt payloadLen);
	};
}
#endif
