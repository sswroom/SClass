#ifndef _SM_CRYPTO_CERT_CERTUTIL
#define _SM_CRYPTO_CERT_CERTUTIL
#include "Crypto/Cert/X509Cert.h"
#include "Crypto/Cert/X509CertReq.h"
#include "Crypto/Cert/X509Key.h"
#include "Crypto/Hash/HashAlgorithm.h"
#include "Net/ASN1PDUBuilder.h"
#include "Net/SSLEngine.h"

namespace Crypto
{
	namespace Cert
	{
		class CertUtil
		{
		public:
			static Bool AppendNames(NN<Net::ASN1PDUBuilder> builder, NN<const CertNames> names);
			static Bool AppendPublicKey(NN<Net::ASN1PDUBuilder> builder, NN<Crypto::Cert::X509Key> key);
			static Bool AppendExtensions(NN<Net::ASN1PDUBuilder> builder, NN<const CertExtensions> ext);
			static Bool AppendSign(NN<Net::ASN1PDUBuilder> builder, NN<Net::SSLEngine> ssl, NN<Crypto::Cert::X509Key> key, Crypto::Hash::HashType hashType);
			static Optional<Crypto::Cert::X509CertReq> CertReqCreate(NN<Net::SSLEngine> ssl, NN<const CertNames> names, NN<Crypto::Cert::X509Key> key, Optional<const CertExtensions> ext);
			static Optional<Crypto::Cert::X509Cert> SelfSignedCertCreate(NN<Net::SSLEngine> ssl, NN<const CertNames> names, NN<Crypto::Cert::X509Key> key, UIntOS validDays, Optional<const CertExtensions> ext);
			static Optional<Crypto::Cert::X509Cert> IssueCert(NN<Net::SSLEngine> ssl, NN<Crypto::Cert::X509Cert> caCert, NN<Crypto::Cert::X509Key> caKey, UIntOS validDays, NN<Crypto::Cert::X509CertReq> csr);
			static Optional<Crypto::Cert::X509Cert> FindIssuer(NN<Crypto::Cert::X509Cert> cert);
		};
	}
}
#endif
