#ifndef _SM_NET_ACMECONN
#define _SM_NET_ACMECONN
#include "Crypto/Cert/X509CertReq.h"
#include "Crypto/Cert/X509Key.h"
#include "Crypto/Token/JWSignature.h"
#include "Net/HTTPClient.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Text/JSON.h"
#include "Text/String.h"

namespace Net
{
	class ACMEConn
	{
	public:
		enum class ACMEStatus
		{
			Unknown,
			Pending,
			Ready,
			Processing,
			Valid,
			Invalid
		};

		enum class AuthorizeType
		{
			Unknown,
			HTTP_01,
			DNS_01,
			TLS_ALPN_01
		};

		struct Order
		{
			ACMEStatus status;
			Int64 expires;
			const UTF8Char *orderURL;
			Data::ArrayList<const UTF8Char*> *authURLs;
			const UTF8Char *finalizeURL;
			const UTF8Char *certificateURL;
		};

		struct Challenge
		{
			ACMEStatus status;
			AuthorizeType type;
			const UTF8Char *url;
			const UTF8Char *token;
		};
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
		const UTF8Char *accountId;

		static Text::String *JWK(Crypto::Cert::X509Key *key, Crypto::Token::JWSignature::Algorithm *alg);
		static Text::String *ProtectedJWK(const UTF8Char *nonce, const UTF8Char *url, Crypto::Cert::X509Key *key, Crypto::Token::JWSignature::Algorithm *alg, const UTF8Char *accountId);
		static Text::String *EncodeJWS(Net::SSLEngine *ssl, const UTF8Char *protStr, const UTF8Char *data, Crypto::Cert::X509Key *key, Crypto::Token::JWSignature::Algorithm alg);
		static Bool KeyHash(Crypto::Cert::X509Key *key, Text::StringBuilderUTF *sb);
		Net::HTTPClient *ACMEPost(const UTF8Char *url, const Char *data);
		Order *OrderParse(const UInt8 *buff, UOSInt buffSize);
		Challenge *ChallengeJSON(Text::JSONBase *json);
		Challenge *ChallengeParse(const UInt8 *buff, UOSInt buffSize);
	public:
		ACMEConn(Net::SocketFactory *sockf, const UTF8Char *serverHost, UInt16 port);
		~ACMEConn();

		Bool IsError();
		const UTF8Char *GetTermOfService();
		const UTF8Char *GetWebsite();
		const UTF8Char *GetAccountId();

		Bool NewNonce();
		Bool AccountNew();
		Bool AccountRetr();

		Order *OrderNew(const UTF8Char *domainNames); //comma seperated
		Challenge *OrderAuthorize(const UTF8Char *authorizeURL, AuthorizeType authType);
		Order *OrderGetStatus(const UTF8Char *orderURL);
		Order *OrderFinalize(const UTF8Char *finalizeURL, Crypto::Cert::X509CertReq *csr);
		void OrderFree(Order *order);

		Challenge *ChallengeBegin(const UTF8Char *challURL);
		Challenge *ChallengeGetStatus(const UTF8Char *challURL);
		void ChallengeFree(Challenge *chall);

		Bool NewKey();
		Bool SetKey(Crypto::Cert::X509Key *key);
		Bool LoadKey(const UTF8Char *fileName);
		Bool SaveKey(const UTF8Char *fileName);

		static ACMEStatus ACMEStatusFromString(const UTF8Char* status);
		static const UTF8Char *AuthorizeTypeGetName(AuthorizeType authType);
		static AuthorizeType AuthorizeTypeFromString(const UTF8Char *s);
	};
}
#endif
