#ifndef _SM_CRYPTO_CERT_CERTUTIL
#define _SM_CRYPTO_CERT_CERTUTIL
#include "Crypto/Cert/X509CertReq.h"
#include "Crypto/Cert/X509Key.h"
#include "Crypto/Hash/IHash.h"
#include "Net/ASN1PDUBuilder.h"
#include "Net/SSLEngine.h"

namespace Crypto
{
	namespace Cert
	{
		class CertUtil
		{
		public:
			struct CertNames
			{
				const UTF8Char *countryName;
				const UTF8Char *stateOrProvinceName;
				const UTF8Char *localityName;
				const UTF8Char *organizationName;
				const UTF8Char *organizationUnitName;
				const UTF8Char *commonName;
				const UTF8Char *emailAddress;
			};

			struct ReqExtensions
			{
				Data::ArrayList<const UTF8Char *> *subjectAltName;
			};
		public:
			static Bool AppendNames(Net::ASN1PDUBuilder *builder, const CertNames *names);
			static Bool AppendPublicKey(Net::ASN1PDUBuilder *builder, Crypto::Cert::X509Key *key);
			static Bool AppendReqExtensions(Net::ASN1PDUBuilder *builder, const ReqExtensions *ext);
			static Bool AppendSign(Net::ASN1PDUBuilder *builder, Net::SSLEngine *ssl, Crypto::Cert::X509Key *key, Crypto::Hash::HashType hashType);
			static Crypto::Cert::X509CertReq *CertReqCreate(Net::SSLEngine *ssl, const CertNames *names, Crypto::Cert::X509Key *key, const ReqExtensions *ext);
		};
	}
}
#endif
