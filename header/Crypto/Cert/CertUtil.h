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
			static Bool AppendNames(NN<Net::ASN1PDUBuilder> builder, NN<const CertNames> names);
			static Bool AppendPublicKey(NN<Net::ASN1PDUBuilder> builder, NN<Crypto::Cert::X509Key> key);
			static Bool AppendExtensions(NN<Net::ASN1PDUBuilder> builder, NN<const CertExtensions> ext);
			static Bool AppendSign(NN<Net::ASN1PDUBuilder> builder, NN<Net::SSLEngine> ssl, NN<Crypto::Cert::X509Key> key, Crypto::Hash::HashType hashType);
			static Crypto::Cert::X509CertReq *CertReqCreate(NN<Net::SSLEngine> ssl, NN<const CertNames> names, NN<Crypto::Cert::X509Key> key, const CertExtensions *ext);
			static Crypto::Cert::X509Cert *SelfSignedCertCreate(NN<Net::SSLEngine> ssl, NN<const CertNames> names, NN<Crypto::Cert::X509Key> key, UOSInt validDays, const CertExtensions *ext);
			static Optional<Crypto::Cert::X509Cert> IssueCert(NN<Net::SSLEngine> ssl, NN<Crypto::Cert::X509Cert> caCert, NN<Crypto::Cert::X509Key> caKey, UOSInt validDays, NN<Crypto::Cert::X509CertReq> csr);
			static Optional<Crypto::Cert::X509Cert> FindIssuer(NN<Crypto::Cert::X509Cert> cert);
		};
	}
}
#endif
