#ifndef _SM_NET_ACMECONN
#define _SM_NET_ACMECONN
#include "Crypto/Cert/X509Key.h"
#include "Crypto/Token/JWSignature.h"
#include "Net/HTTPClient.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"

namespace Net
{
	class ACMEConn
	{
	private:
		Net::SocketFactory *sockf;
		Net::SSLEngine *ssl;
		Crypto::Cert::X509Key *key;
		const UTF8Char *serverHost;
		UInt16 port;
		const UTF8Char *urlNewNonce;
		const UTF8Char *urlNewAccount;
		const UTF8Char *urlNewOrder;
		const UTF8Char *urlNewAuthz;
		const UTF8Char *urlRevokeCert;
		const UTF8Char *urlKeyChange;
		const UTF8Char *urlTermOfService;
		const UTF8Char *urlWebsite;
		const UTF8Char *nonce;

		static const UTF8Char *JWK(Crypto::Cert::X509Key *key, Crypto::Token::JWSignature::Algorithm *alg);
		static const UTF8Char *ProtectedJWK(const UTF8Char *nonce, const UTF8Char *url, Crypto::Cert::X509Key *key, Crypto::Token::JWSignature::Algorithm *alg);
		static const UTF8Char *EncodeJWS(Net::SSLEngine *ssl, const UTF8Char *protStr, const UTF8Char *data, Crypto::Cert::X509Key *key, Crypto::Token::JWSignature::Algorithm alg);
		Net::HTTPClient *ACMEPost(const UTF8Char *url, const Char *data);
	public:
		ACMEConn(Net::SocketFactory *sockf, const UTF8Char *serverHost, UInt16 port);
		~ACMEConn();

		Bool IsError();
		const UTF8Char *GetTermOfService();
		const UTF8Char *GetWebsite();

		Bool NewNonce();
		Bool AccountNew();
		Bool AccountRetr();

		Bool NewKey();
		Bool SetKey(Crypto::Cert::X509Key *key);
		Bool LoadKey(const UTF8Char *fileName);
		Bool SaveKey(const UTF8Char *fileName);
	};
}
#endif
