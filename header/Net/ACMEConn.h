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
			Text::String *orderURL;
			Data::ArrayList<Text::String*> *authURLs;
			Text::String *finalizeURL;
			Text::String *certificateURL;
		};

		struct Challenge
		{
			ACMEStatus status;
			AuthorizeType type;
			Text::String *url;
			Text::String *token;
		};
	private:
		Net::SocketFactory *sockf;
		Net::SSLEngine *ssl;
		Crypto::Cert::X509Key *key;
		Text::String *serverHost;
		UInt16 port;
		Text::String *urlNewNonce;
		Text::String *urlNewAccount;
		Text::String *urlNewOrder;
		Text::String *urlNewAuthz;
		Text::String *urlRevokeCert;
		Text::String *urlKeyChange;
		Text::String *urlTermOfService;
		Text::String *urlWebsite;
		Text::String *nonce;
		Text::String *accountId;

		static Text::String *JWK(Crypto::Cert::X509Key *key, Crypto::Token::JWSignature::Algorithm *alg);
		static Text::String *ProtectedJWK(Text::String *nonce, Text::String *url, Crypto::Cert::X509Key *key, Crypto::Token::JWSignature::Algorithm *alg, Text::String *accountId);
		static Text::String *EncodeJWS(Net::SSLEngine *ssl, const UTF8Char *protStr, const UTF8Char *data, Crypto::Cert::X509Key *key, Crypto::Token::JWSignature::Algorithm alg);
		static Bool KeyHash(Crypto::Cert::X509Key *key, Text::StringBuilderUTF *sb);
		Net::HTTPClient *ACMEPost(Text::String *url, const Char *data);
		Order *OrderParse(const UInt8 *buff, UOSInt buffSize);
		Challenge *ChallengeJSON(Text::JSONBase *json);
		Challenge *ChallengeParse(const UInt8 *buff, UOSInt buffSize);
	public:
		ACMEConn(Net::SocketFactory *sockf, const UTF8Char *serverHost, UInt16 port);
		~ACMEConn();

		Bool IsError();
		Text::String *GetTermOfService();
		Text::String *GetWebsite();
		Text::String *GetAccountId();

		Bool NewNonce();
		Bool AccountNew();
		Bool AccountRetr();

		Order *OrderNew(const UTF8Char *domainNames); //comma seperated
		Challenge *OrderAuthorize(Text::String *authorizeURL, AuthorizeType authType);
		Order *OrderGetStatus(const UTF8Char *orderURL);
		Order *OrderFinalize(const UTF8Char *finalizeURL, Crypto::Cert::X509CertReq *csr);
		void OrderFree(Order *order);

		Challenge *ChallengeBegin(Text::String *challURL);
		Challenge *ChallengeGetStatus(Text::String *challURL);
		void ChallengeFree(Challenge *chall);

		Bool NewKey();
		Bool SetKey(Crypto::Cert::X509Key *key);
		Bool LoadKey(const UTF8Char *fileName);
		Bool SaveKey(const UTF8Char *fileName);

		static ACMEStatus ACMEStatusFromString(Text::String* status);
		static Text::CString AuthorizeTypeGetName(AuthorizeType authType);
		static AuthorizeType AuthorizeTypeFromString(Text::String *s);
	};
}
#endif
