#ifndef _SM_NET_ACMECONN
#define _SM_NET_ACMECONN
#include "Crypto/Cert/X509CertReq.h"
#include "Crypto/Cert/X509Key.h"
#include "Crypto/Token/JWSignature.h"
#include "Data/ArrayListStringNN.h"
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
			Data::ArrayListStringNN *authURLs;
			Optional<Text::String> finalizeURL;
			Text::String *certificateURL;
		};

		struct Challenge
		{
			ACMEStatus status;
			AuthorizeType type;
			NN<Text::String> url;
			NN<Text::String> token;
		};
	private:
		NN<Net::SocketFactory> sockf;
		Optional<Net::SSLEngine> ssl;
		Optional<Crypto::Cert::X509Key> key;
		NN<Text::String> serverHost;
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

		static Optional<Text::String> JWK(NN<Crypto::Cert::X509Key> key, OutParam<Crypto::Token::JWSignature::Algorithm> alg);
		static Text::String *ProtectedJWK(Text::String *nonce, NN<Text::String> url, NN<Crypto::Cert::X509Key> key, OutParam<Crypto::Token::JWSignature::Algorithm> alg, Text::String *accountId);
		static NN<Text::String> EncodeJWS(Optional<Net::SSLEngine> ssl, Text::CStringNN protStr, Text::CStringNN data, NN<Crypto::Cert::X509Key> key, Crypto::Token::JWSignature::Algorithm alg);
		static Bool KeyHash(NN<Crypto::Cert::X509Key> key, NN<Text::StringBuilderUTF8> sb);
		Net::HTTPClient *ACMEPost(NN<Text::String> url, Text::CStringNN data);
		Order *OrderParse(const UInt8 *buff, UOSInt buffSize);
		Challenge *ChallengeJSON(Text::JSONBase *json);
		Challenge *ChallengeParse(const UInt8 *buff, UOSInt buffSize);
	public:
		ACMEConn(NN<Net::SocketFactory> sockf, Text::CStringNN serverHost, UInt16 port);
		~ACMEConn();

		Bool IsError();
		Text::String *GetTermOfService();
		Text::String *GetWebsite();
		Text::String *GetAccountId();

		Bool NewNonce();
		Bool AccountNew();
		Bool AccountRetr();

		Order *OrderNew(const UTF8Char *domainNames, UOSInt namesLen); //comma seperated
		Challenge *OrderAuthorize(NN<Text::String> authorizeURL, AuthorizeType authType);
		Order *OrderGetStatus(const UTF8Char *orderURL);
		Order *OrderFinalize(const UTF8Char *finalizeURL, Crypto::Cert::X509CertReq *csr);
		void OrderFree(Order *order);

		Challenge *ChallengeBegin(NN<Text::String> challURL);
		Challenge *ChallengeGetStatus(NN<Text::String> challURL);
		void ChallengeFree(Challenge *chall);

		Bool NewKey();
		Bool SetKey(Crypto::Cert::X509Key *key);
		Bool LoadKey(Text::CStringNN fileName);
		Bool SaveKey(Text::CStringNN fileName);

		static ACMEStatus ACMEStatusFromString(Optional<Text::String> status);
		static Text::CString AuthorizeTypeGetName(AuthorizeType authType);
		static AuthorizeType AuthorizeTypeFromString(NN<Text::String> s);
	};
}
#endif
