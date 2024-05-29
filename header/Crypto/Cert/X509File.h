#ifndef _SM_CRYPTO_CERT_X509FILE
#define _SM_CRYPTO_CERT_X509FILE
#include "Crypto/Hash/IHash.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListStringNN.h"
#include "Net/ASN1Data.h"
#include "Text/CString.h"

namespace Net
{
	class SSLEngine;
}

namespace Crypto
{
	namespace Cert
	{
		struct CertNames
		{
			Optional<Text::String> countryName;
			Optional<Text::String> stateOrProvinceName;
			Optional<Text::String> localityName;
			Optional<Text::String> organizationName;
			Optional<Text::String> organizationUnitName;
			Optional<Text::String> commonName;
			Optional<Text::String> emailAddress;

			static void FreeNames(NN<CertNames> names);
		};

		struct DigestInfo
		{
			Crypto::Hash::HashType hashType;
			UnsafeArray<const UInt8> hashVal;
			UOSInt hashLen;
		};

		struct CertExtensions
		{
			Optional<Data::ArrayListStringNN> subjectAltName;
			Optional<Data::ArrayListStringNN> issuerAltName;
			Bool useSubjKeyId;
			UInt8 subjKeyId[20];
			Bool useAuthKeyId;
			UInt8 authKeyId[20];
			Bool digitalSign;
			Bool caCert;
			Int32 caCertPathLen;

			static void FreeExtensions(NN<CertExtensions> ext);
		};
		class Certificate;
		class CertStore;
		class X509Key;
		class X509Cert;

		class X509File : public Net::ASN1Data
		{
		public:
			enum class FileType
			{
				Cert,
				Key,
				CertRequest,
				PrivateKey,
				PublicKey,
				PKCS7,
				PKCS12,
				CRL,
				FileList
			};

			enum class KeyType
			{
				Unknown,
				RSA,
				DSA,
				ECDSA,
				ED25519,
				RSAPublic,
				ECPublic
			};

			enum class ValidStatus
			{
				Valid,
				SelfSigned,
				SignatureInvalid,
				Revoked,
				FileFormatInvalid,
				UnknownIssuer,
				Expired,
				UnsupportedAlgorithm
			};

			enum class AlgType
			{
				Unknown,
				MD2WithRSAEncryption,
				MD5WithRSAEncryption,
				SHA1WithRSAEncryption,
				SHA256WithRSAEncryption,
				SHA384WithRSAEncryption,
				SHA512WithRSAEncryption,
				SHA224WithRSAEncryption,
				ECDSAWithSHA256,
				ECDSAWithSHA384
			};

			enum class ContentDataType
			{
				Unknown,
				AuthenticatedSafe
			};

			enum class ECName
			{
				Unknown,
				secp256r1,
				secp384r1,
				secp521r1
			};

			struct SignedInfo
			{
				UnsafeArray<const UInt8> signature;
				UOSInt signSize;
				UnsafeArray<const UInt8> payload;
				UOSInt payloadSize;
				AlgType algType;
			};

		public:
			static Bool IsSigned(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, const Char *path); // AuthenticationFramework
			static void AppendSigned(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, const Char *path, NN<Text::StringBuilderUTF8> sb, Text::CString varName); // AuthenticationFramework
			static Bool IsTBSCertificate(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, const Char *path); // AuthenticationFramework
			static void AppendTBSCertificate(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, const Char *path, NN<Text::StringBuilderUTF8> sb, Text::CString varName); // AuthenticationFramework
			static Bool IsCertificate(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, const Char *path); // AuthenticationFramework
			static void AppendCertificate(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, const Char *path, NN<Text::StringBuilderUTF8> sb, Text::CString varName); // AuthenticationFramework
			static Bool IsTBSCertList(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, const Char *path); // RFC3280
			static void AppendTBSCertList(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, const Char *path, NN<Text::StringBuilderUTF8> sb, Text::CString varName); // RFC3280
			static Bool IsCertificateList(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, const Char *path); // RFC3280
			static void AppendCertificateList(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, const Char *path, NN<Text::StringBuilderUTF8> sb, Text::CString varName); // RFC3280
			static Bool IsPrivateKeyInfo(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, const Char *path); // PKCS-8
			static void AppendPrivateKeyInfo(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, const Char *path, NN<Text::StringBuilderUTF8> sb); // PKCS-8
			static Bool IsCertificateRequestInfo(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, const Char *path); // PKCS-10
			static void AppendCertificateRequestInfo(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, const Char *path, NN<Text::StringBuilderUTF8> sb); // PKCS-10
			static Bool IsCertificateRequest(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, const Char *path); // PKCS-10
			static void AppendCertificateRequest(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, const Char *path, NN<Text::StringBuilderUTF8> sb); // PKCS-8
			static Bool IsPublicKeyInfo(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, const Char *path); // AuthenticationFramework
			static void AppendPublicKeyInfo(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, const Char *path, NN<Text::StringBuilderUTF8> sb); // AuthenticationFramework
			static Bool IsContentInfo(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, const Char *path); // RFC2315 / PKCS7
			static void AppendContentInfo(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, const Char *path, NN<Text::StringBuilderUTF8> sb, Text::CString varName, ContentDataType dataType); // RFC2315 / PKCS7
			static Bool IsPFX(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, const Char *path); // PKCS12
			static void AppendPFX(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, const Char *path, NN<Text::StringBuilderUTF8> sb, Text::CString varName); // PKCS12

		protected:
			static void AppendVersion(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, const Char *path, NN<Text::StringBuilderUTF8> sb); // AuthenticationFramework

			static void AppendAlgorithmIdentifier(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CString varName, Bool pubKey, OptOut<KeyType> keyTypeOut); // PKCS-5
			static void AppendValidity(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CString varName); // AuthenticationFramework
			static void AppendSubjectPublicKeyInfo(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CString varName); // AuthenticationFramework
			static void AppendName(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CString varName); // InformationFramework
			static void AppendRelativeDistinguishedName(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CString varName); // InformationFramework
			static void AppendAttributeTypeAndDistinguishedValue(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CString varName); // InformationFramework
			static void AppendCRLExtensions(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CString varName);
			static void AppendCRLExtension(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CString varName);
			static void AppendMSOSVersion(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CString varName);
			static void AppendMSRequestClientInfo(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CString varName);
			static void AppendMSEnrollmentCSPProvider(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CString varName);
			static void AppendGeneralNames(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CString varName);
			static Bool AppendGeneralName(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, const Char *path, NN<Text::StringBuilderUTF8> sb, Text::CString varName);
			static Bool AppendDistributionPoint(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, const Char *path, NN<Text::StringBuilderUTF8> sb, Text::CString varName);
			static void AppendDistributionPointName(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CString varName);
			static Bool AppendPolicyInformation(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, const Char *path, NN<Text::StringBuilderUTF8> sb, Text::CString varName);
			static void AppendPKCS7SignedData(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CString varName); // RFC2315
			static void AppendPKCS7DigestAlgorithmIdentifiers(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CString varName); // RFC2315
			static void AppendPKCS7SignerInfos(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CString varName); // RFC2315
			static void AppendPKCS7SignerInfo(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CString varName); // RFC2315
			static void AppendIssuerAndSerialNumber(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CString varName); // RFC2315
			static void AppendPKCS7Attributes(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CString varName); // RFC2315
			static Bool AppendMacData(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, const Char *path, NN<Text::StringBuilderUTF8> sb, Text::CString varName); // PKCS12
			static void AppendDigestInfo(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CString varName); // RFC2315 / PKCS7
			static void AppendData(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CString varName, ContentDataType dataType);
			static void AppendEncryptedData(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CString varName, ContentDataType dataType);
			static void AppendAuthenticatedSafe(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CString varName); //PKCS12
			static void AppendEncryptedContentInfo(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb, Text::CString varName, ContentDataType dataType);

			static Bool NameGetByOID(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, Text::CStringNN oidText, NN<Text::StringBuilderUTF8> sb);
			static UTF8Char *NameGetByOID(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, Text::CStringNN oidText, UTF8Char *sbuff);
			static Bool NameGetCN(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<Text::StringBuilderUTF8> sb);
			static UTF8Char *NameGetCN(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, UTF8Char *sbuff);
			static Bool NamesGet(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<CertNames> names);
			static Bool ExtensionsGet(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, NN<CertExtensions> ext);
			static UOSInt ExtensionsGetCRLDistributionPoints(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, Data::ArrayList<Text::CString> *crlDistributionPoints);
			static UOSInt DistributionPointAdd(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, Data::ArrayList<Text::CString> *crlDistributionPoints);
			static Optional<Crypto::Cert::X509Key> PublicKeyGetNew(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd);

			static UOSInt KeyGetLeng(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, KeyType keyType);
			static KeyType KeyTypeFromOID(Data::ByteArrayR oid, Bool pubKey);
			static ECName ECNameFromOID(Data::ByteArrayR oid);
			static Bool AlgorithmIdentifierGet(UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd, OutParam<AlgType> algType);

			X509File(NN<Text::String> sourceName, Data::ByteArrayR buff);
			X509File(Text::CStringNN sourceName, Data::ByteArrayR buff);
		public:
			virtual ~X509File();

			virtual Net::ASN1Data::ASN1Type GetASN1Type() const;
			virtual FileType GetFileType() const = 0;
			virtual void ToShortName(NN<Text::StringBuilderUTF8> sb) const = 0;

			virtual UOSInt GetCertCount();
			virtual Bool GetCertName(UOSInt index, NN<Text::StringBuilderUTF8> sb);
			virtual Optional<X509Cert> GetNewCert(UOSInt index);
			virtual ValidStatus IsValid(NN<Net::SSLEngine> ssl, Optional<Crypto::Cert::CertStore> trustStore) const = 0;

			void ToShortString(NN<Text::StringBuilderUTF8> sb) const;
			Bool IsSignatureKey(NN<Net::SSLEngine> ssl, NN<Crypto::Cert::X509Key> key) const;
			Bool GetSignedInfo(NN<SignedInfo> signedInfo) const;

			static Bool ParseDigestType(DigestInfo *digestInfo, UnsafeArray<const UInt8> pdu, UnsafeArray<const UInt8> pduEnd);
			static Crypto::Hash::HashType GetAlgHash(AlgType algType);
			static Text::CStringNN FileTypeGetName(FileType fileType);
			static Text::CStringNN KeyTypeGetName(KeyType keyType);
			static Text::CStringNN KeyTypeGetOID(KeyType keyType);
			static Text::CStringNN ECNameGetName(ECName ecName);
			static Text::CStringNN ECNameGetOID(ECName ecName);
			static Text::CStringNN ValidStatusGetName(ValidStatus validStatus);
			static Text::CStringNN ValidStatusGetDesc(ValidStatus validStatus);
			static Crypto::Hash::HashType HashTypeFromOID(Data::ByteArrayR oid);
			static Optional<X509File> CreateFromCerts(NN<const Data::ReadingListNN<Crypto::Cert::Certificate>> certs);
			static Optional<X509File> CreateFromCertsAndClear(NN<Data::ArrayListNN<Crypto::Cert::X509Cert>> certs);
		};
	}
}
#endif
