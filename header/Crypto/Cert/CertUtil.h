#ifndef _SM_CRYPTO_CERT_CERTUTIL
#define _SM_CRYPTO_CERT_CERTUTIL
#include "Crypto/Cert/X509Cert.h"
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
			struct ReqExtensions
			{
				Data::ArrayList<const UTF8Char *> *subjectAltName;
				Bool useSubjKeyId;
				UInt8 subjKeyId[20];
				Bool useAuthKeyId;
				UInt8 authKeyId[20];
			};
		public:
			static Bool AppendNames(Net::ASN1PDUBuilder *builder, const CertNames *names);
			static Bool AppendPublicKey(Net::ASN1PDUBuilder *builder, Crypto::Cert::X509Key *key);
			static Bool AppendReqExtensions(Net::ASN1PDUBuilder *builder, const ReqExtensions *ext);
			static Bool AppendSign(Net::ASN1PDUBuilder *builder, Net::SSLEngine *ssl, Crypto::Cert::X509Key *key, Crypto::Hash::HashType hashType);
			static Crypto::Cert::X509CertReq *CertReqCreate(Net::SSLEngine *ssl, const CertNames *names, Crypto::Cert::X509Key *key, const ReqExtensions *ext);
			static Crypto::Cert::X509Cert *SelfSignedCertCreate(Net::SSLEngine *ssl, const CertNames *names, Crypto::Cert::X509Key *key, UOSInt validDays, const ReqExtensions *ext);
		};
	}
}
#endif
