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
			static Bool AppendNames(NotNullPtr<Net::ASN1PDUBuilder> builder, const CertNames *names);
			static Bool AppendPublicKey(NotNullPtr<Net::ASN1PDUBuilder> builder, NotNullPtr<Crypto::Cert::X509Key> key);
			static Bool AppendExtensions(NotNullPtr<Net::ASN1PDUBuilder> builder, const CertExtensions *ext);
			static Bool AppendSign(NotNullPtr<Net::ASN1PDUBuilder> builder, Net::SSLEngine *ssl, NotNullPtr<Crypto::Cert::X509Key> key, Crypto::Hash::HashType hashType);
			static Crypto::Cert::X509CertReq *CertReqCreate(Net::SSLEngine *ssl, const CertNames *names, NotNullPtr<Crypto::Cert::X509Key> key, const CertExtensions *ext);
			static Crypto::Cert::X509Cert *SelfSignedCertCreate(Net::SSLEngine *ssl, const CertNames *names, NotNullPtr<Crypto::Cert::X509Key> key, UOSInt validDays, const CertExtensions *ext);
			static Crypto::Cert::X509Cert *IssueCert(Net::SSLEngine *ssl, Crypto::Cert::X509Cert *caCert, NotNullPtr<Crypto::Cert::X509Key> caKey, UOSInt validDays, Crypto::Cert::X509CertReq *csr);
			static Crypto::Cert::X509Cert *FindIssuer(NotNullPtr<Crypto::Cert::X509Cert> cert);
		};
	}
}
#endif
