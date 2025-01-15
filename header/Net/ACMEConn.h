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
			Optional<Text::String> orderURL;
			Optional<Data::ArrayListStringNN> authURLs;
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
		NN<Net::TCPClientFactory> clif;
		Optional<Net::SSLEngine> ssl;
		Optional<Crypto::Cert::X509Key> key;
		NN<Text::String> serverHost;
		UInt16 port;
		Optional<Text::String> urlNewNonce;
		Optional<Text::String> urlNewAccount;
		Optional<Text::String> urlNewOrder;
		Optional<Text::String> urlNewAuthz;
		Optional<Text::String> urlRevokeCert;
		Optional<Text::String> urlKeyChange;
		Optional<Text::String> urlTermOfService;
		Optional<Text::String> urlWebsite;
		Optional<Text::String> nonce;
		Optional<Text::String> accountId;

		static Optional<Text::String> JWK(NN<Crypto::Cert::X509Key> key, OutParam<Crypto::Token::JWSignature::Algorithm> alg);
		static Optional<Text::String> ProtectedJWK(NN<Text::String> nonce, NN<Text::String> url, NN<Crypto::Cert::X509Key> key, OutParam<Crypto::Token::JWSignature::Algorithm> alg, Optional<Text::String> accountId);
		static NN<Text::String> EncodeJWS(Optional<Net::SSLEngine> ssl, Text::CStringNN protStr, Text::CStringNN data, NN<Crypto::Cert::X509Key> key, Crypto::Token::JWSignature::Algorithm alg);
		static Bool KeyHash(NN<Crypto::Cert::X509Key> key, NN<Text::StringBuilderUTF8> sb);
		Optional<Net::HTTPClient> ACMEPost(NN<Text::String> url, Text::CStringNN data);
		Optional<Order> OrderParse(UnsafeArray<const UInt8> buff, UOSInt buffSize);
		Optional<Challenge> ChallengeJSON(NN<Text::JSONBase> json);
		Optional<Challenge> ChallengeParse(UnsafeArray<const UInt8> buff, UOSInt buffSize);
	public:
		ACMEConn(NN<Net::TCPClientFactory> clif, Text::CStringNN serverHost, UInt16 port);
		~ACMEConn();

		Bool IsError();
		Optional<Text::String> GetTermOfService();
		Optional<Text::String> GetWebsite();
		Optional<Text::String> GetAccountId();

		Bool NewNonce();
		Bool AccountNew();
		Bool AccountRetr();

		Optional<Order> OrderNew(UnsafeArray<const UTF8Char> domainNames, UOSInt namesLen); //comma seperated
		Optional<Challenge> OrderAuthorize(NN<Text::String> authorizeURL, AuthorizeType authType);
		Optional<Order> OrderGetStatus(UnsafeArray<const UTF8Char> orderURL);
		Optional<Order> OrderFinalize(UnsafeArray<const UTF8Char> finalizeURL, NN<Crypto::Cert::X509CertReq> csr);
		void OrderFree(NN<Order> order);

		Optional<Challenge> ChallengeBegin(NN<Text::String> challURL);
		Optional<Challenge> ChallengeGetStatus(NN<Text::String> challURL);
		void ChallengeFree(NN<Challenge> chall);

		Bool NewKey();
		Bool SetKey(NN<Crypto::Cert::X509Key> key);
		Bool LoadKey(Text::CStringNN fileName);
		Bool SaveKey(Text::CStringNN fileName);

		static ACMEStatus ACMEStatusFromString(Optional<Text::String> status);
		static Text::CString AuthorizeTypeGetName(AuthorizeType authType);
		static AuthorizeType AuthorizeTypeFromString(NN<Text::String> s);
	};
}
#endif
