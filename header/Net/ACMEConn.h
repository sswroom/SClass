#ifndef _SM_NET_ACMECONN
#define _SM_NET_ACMECONN
#include "Crypto/Cert/X509CertReq.h"
#include "Crypto/Cert/X509Key.h"
#include "Crypto/Token/JWSignature.h"
#include "Data/ArrayListNN.h"
#include "Net/HTTPClient.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Text/CString.h"
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
			Data::ArrayListNN<Text::String> *authURLs;
			Text::String *finalizeURL;
			Text::String *certificateURL;
		};

		struct Challenge
		{
			ACMEStatus status;
			AuthorizeType type;
			NotNullPtr<Text::String> url;
			NotNullPtr<Text::String> token;
		};
	private:
		Net::SocketFactory *sockf;
		Net::SSLEngine *ssl;
		Crypto::Cert::X509Key *key;
		NotNullPtr<Text::String> serverHost;
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
		static Text::String *ProtectedJWK(Text::String *nonce, NotNullPtr<Text::String> url, Crypto::Cert::X509Key *key, Crypto::Token::JWSignature::Algorithm *alg, Text::String *accountId);
		static NotNullPtr<Text::String> EncodeJWS(Net::SSLEngine *ssl, Text::CString protStr, Text::CString data, Crypto::Cert::X509Key *key, Crypto::Token::JWSignature::Algorithm alg);
		static Bool KeyHash(Crypto::Cert::X509Key *key, Text::StringBuilderUTF8 *sb);
		Net::HTTPClient *ACMEPost(NotNullPtr<Text::String> url, Text::CString data);
		Order *OrderParse(const UInt8 *buff, UOSInt buffSize);
		Challenge *ChallengeJSON(Text::JSONBase *json);
		Challenge *ChallengeParse(const UInt8 *buff, UOSInt buffSize);
	public:
		ACMEConn(Net::SocketFactory *sockf, Text::CString serverHost, UInt16 port);
		~ACMEConn();

		Bool IsError();
		Text::String *GetTermOfService();
		Text::String *GetWebsite();
		Text::String *GetAccountId();

		Bool NewNonce();
		Bool AccountNew();
		Bool AccountRetr();

		Order *OrderNew(const UTF8Char *domainNames, UOSInt namesLen); //comma seperated
		Challenge *OrderAuthorize(NotNullPtr<Text::String> authorizeURL, AuthorizeType authType);
		Order *OrderGetStatus(const UTF8Char *orderURL);
		Order *OrderFinalize(const UTF8Char *finalizeURL, Crypto::Cert::X509CertReq *csr);
		void OrderFree(Order *order);

		Challenge *ChallengeBegin(NotNullPtr<Text::String> challURL);
		Challenge *ChallengeGetStatus(NotNullPtr<Text::String> challURL);
		void ChallengeFree(Challenge *chall);

		Bool NewKey();
		Bool SetKey(Crypto::Cert::X509Key *key);
		Bool LoadKey(Text::CString fileName);
		Bool SaveKey(Text::CString fileName);

		static ACMEStatus ACMEStatusFromString(Text::String* status);
		static Text::CString AuthorizeTypeGetName(AuthorizeType authType);
		static AuthorizeType AuthorizeTypeFromString(Text::String *s);
	};
}
#endif
