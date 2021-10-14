#ifndef _SM_CRYPTO_CERT_CERTIFICATE
#define _SM_CRYPTO_CERT_CERTIFICATE
#include "Crypto/Cert/X509CertReq.h"
#include "Crypto/Cert/X509Key.h"
#include "Crypto/Hash/IHash.h"
#include "Net/ASN1PDUBuilder.h"

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
				const UTF8Char *stateProvinceName;
				const UTF8Char *localityName;
				const UTF8Char *organizationName;
				const UTF8Char *organizationUnitName;
				const UTF8Char *commonName;
				const UTF8Char *emailAddress;
			};
		public:
			static Bool AppendNames(Net::ASN1PDUBuilder *builder, const CertNames *names);
			static Bool AppendPublicKey(Net::ASN1PDUBuilder *builder, Crypto::Cert::X509Key *key);
			static Bool AppendSign(Net::ASN1PDUBuilder *builder, Crypto::Cert::X509Key *key, Crypto::Hash::HashType hashType);
			static Crypto::Cert::X509CertReq *CertReqCreate(const CertNames *names, Crypto::Cert::X509Key *key);
		};
	}
}
#endif
