#ifndef _SM_CRYPTO_CERT_X509FILE
#define _SM_CRYPTO_CERT_X509FILE
#include "Crypto/Hash/IHash.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListNN.h"
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
			Text::String *countryName;
			Text::String *stateOrProvinceName;
			Text::String *localityName;
			Text::String *organizationName;
			Text::String *organizationUnitName;
			Text::String *commonName;
			Text::String *emailAddress;

			static void FreeNames(CertNames *names);
		};

		struct DigestInfo
		{
			Crypto::Hash::HashType hashType;
			const UInt8 *hashVal;
			UOSInt hashLen;
		};

		struct CertExtensions
		{
			Data::ArrayListNN<Text::String> *subjectAltName;
			Data::ArrayList<Text::String *> *issuerAltName;
			Bool useSubjKeyId;
			UInt8 subjKeyId[20];
			Bool useAuthKeyId;
			UInt8 authKeyId[20];
			Bool digitalSign;
			Bool caCert;
			Int32 caCertPathLen;

			static void FreeExtensions(CertExtensions *ext);
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
				const UInt8 *signature;
				UOSInt signSize;
				const UInt8 *payload;
				UOSInt payloadSize;
				AlgType algType;
			};

		public:
			static Bool IsSigned(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path); // AuthenticationFramework
			static void AppendSigned(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName); // AuthenticationFramework
			static Bool IsTBSCertificate(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path); // AuthenticationFramework
			static void AppendTBSCertificate(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName); // AuthenticationFramework
			static Bool IsCertificate(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path); // AuthenticationFramework
			static void AppendCertificate(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName); // AuthenticationFramework
			static Bool IsTBSCertList(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path); // RFC3280
			static void AppendTBSCertList(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName); // RFC3280
			static Bool IsCertificateList(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path); // RFC3280
			static void AppendCertificateList(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName); // RFC3280
			static Bool IsPrivateKeyInfo(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path); // PKCS-8
			static void AppendPrivateKeyInfo(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, NotNullPtr<Text::StringBuilderUTF8> sb); // PKCS-8
			static Bool IsCertificateRequestInfo(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path); // PKCS-10
			static void AppendCertificateRequestInfo(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, NotNullPtr<Text::StringBuilderUTF8> sb); // PKCS-10
			static Bool IsCertificateRequest(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path); // PKCS-10
			static void AppendCertificateRequest(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, NotNullPtr<Text::StringBuilderUTF8> sb); // PKCS-8
			static Bool IsPublicKeyInfo(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path); // AuthenticationFramework
			static void AppendPublicKeyInfo(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, NotNullPtr<Text::StringBuilderUTF8> sb); // AuthenticationFramework
			static Bool IsContentInfo(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path); // RFC2315 / PKCS7
			static void AppendContentInfo(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName, ContentDataType dataType); // RFC2315 / PKCS7
			static Bool IsPFX(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path); // PKCS12
			static void AppendPFX(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName); // PKCS12

		protected:
			static void AppendVersion(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, NotNullPtr<Text::StringBuilderUTF8> sb); // AuthenticationFramework

			static void AppendAlgorithmIdentifier(const UInt8 *pdu, const UInt8 *pduEnd, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName, Bool pubKey, KeyType *keyTypeOut); // PKCS-5
			static void AppendValidity(const UInt8 *pdu, const UInt8 *pduEnd, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName); // AuthenticationFramework
			static void AppendSubjectPublicKeyInfo(const UInt8 *pdu, const UInt8 *pduEnd, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName); // AuthenticationFramework
			static void AppendName(const UInt8 *pdu, const UInt8 *pduEnd, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName); // InformationFramework
			static void AppendRelativeDistinguishedName(const UInt8 *pdu, const UInt8 *pduEnd, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName); // InformationFramework
			static void AppendAttributeTypeAndDistinguishedValue(const UInt8 *pdu, const UInt8 *pduEnd, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName); // InformationFramework
			static void AppendCRLExtensions(const UInt8 *pdu, const UInt8 *pduEnd, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName);
			static void AppendCRLExtension(const UInt8 *pdu, const UInt8 *pduEnd, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName);
			static void AppendMSOSVersion(const UInt8 *pdu, const UInt8 *pduEnd, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName);
			static void AppendMSRequestClientInfo(const UInt8 *pdu, const UInt8 *pduEnd, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName);
			static void AppendMSEnrollmentCSPProvider(const UInt8 *pdu, const UInt8 *pduEnd, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName);
			static void AppendGeneralNames(const UInt8 *pdu, const UInt8 *pduEnd, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName);
			static Bool AppendGeneralName(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName);
			static Bool AppendDistributionPoint(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName);
			static void AppendDistributionPointName(const UInt8 *pdu, const UInt8 *pduEnd, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName);
			static Bool AppendPolicyInformation(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName);
			static void AppendPKCS7SignedData(const UInt8 *pdu, const UInt8 *pduEnd, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName); // RFC2315
			static void AppendPKCS7DigestAlgorithmIdentifiers(const UInt8 *pdu, const UInt8 *pduEnd, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName); // RFC2315
			static void AppendPKCS7SignerInfos(const UInt8 *pdu, const UInt8 *pduEnd, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName); // RFC2315
			static void AppendPKCS7SignerInfo(const UInt8 *pdu, const UInt8 *pduEnd, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName); // RFC2315
			static void AppendIssuerAndSerialNumber(const UInt8 *pdu, const UInt8 *pduEnd, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName); // RFC2315
			static void AppendPKCS7Attributes(const UInt8 *pdu, const UInt8 *pduEnd, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName); // RFC2315
			static Bool AppendMacData(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName); // PKCS12
			static void AppendDigestInfo(const UInt8 *pdu, const UInt8 *pduEnd, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName); // RFC2315 / PKCS7
			static void AppendData(const UInt8 *pdu, const UInt8 *pduEnd, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName, ContentDataType dataType);
			static void AppendEncryptedData(const UInt8 *pdu, const UInt8 *pduEnd, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName, ContentDataType dataType);
			static void AppendAuthenticatedSafe(const UInt8 *pdu, const UInt8 *pduEnd, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName); //PKCS12
			static void AppendEncryptedContentInfo(const UInt8 *pdu, const UInt8 *pduEnd, NotNullPtr<Text::StringBuilderUTF8> sb, Text::CString varName, ContentDataType dataType);

			static Bool NameGetByOID(const UInt8 *pdu, const UInt8 *pduEnd, const UTF8Char *oidText, UOSInt oidTextLen, NotNullPtr<Text::StringBuilderUTF8> sb);
			static UTF8Char *NameGetByOID(const UInt8 *pdu, const UInt8 *pduEnd, const UTF8Char *oidText, UOSInt oidTextLen, UTF8Char *sbuff);
			static Bool NameGetCN(const UInt8 *pdu, const UInt8 *pduEnd, NotNullPtr<Text::StringBuilderUTF8> sb);
			static UTF8Char *NameGetCN(const UInt8 *pdu, const UInt8 *pduEnd, UTF8Char *sbuff);
			static Bool NamesGet(const UInt8 *pdu, const UInt8 *pduEnd, CertNames *names);
			static Bool ExtensionsGet(const UInt8 *pdu, const UInt8 *pduEnd, CertExtensions *ext);
			static UOSInt ExtensionsGetCRLDistributionPoints(const UInt8 *pdu, const UInt8 *pduEnd, Data::ArrayList<Text::CString> *crlDistributionPoints);
			static UOSInt DistributionPointAdd(const UInt8 *pdu, const UInt8 *pduEnd, Data::ArrayList<Text::CString> *crlDistributionPoints);
			static Crypto::Cert::X509Key *PublicKeyGetNew(const UInt8 *pdu, const UInt8 *pduEnd);

			static UOSInt KeyGetLeng(const UInt8 *pdu, const UInt8 *pduEnd, KeyType keyType);
			static KeyType KeyTypeFromOID(const UInt8 *oid, UOSInt oidLen, Bool pubKey);
			static ECName ECNameFromOID(const UInt8 *oid, UOSInt oidLen);
			static Bool AlgorithmIdentifierGet(const UInt8 *pdu, const UInt8 *pduEnd, AlgType *algType);

			X509File(NotNullPtr<Text::String> sourceName, Data::ByteArrayR buff);
			X509File(Text::CStringNN sourceName, Data::ByteArrayR buff);
		public:
			virtual ~X509File();

			virtual Net::ASN1Data::ASN1Type GetASN1Type() const;
			virtual FileType GetFileType() const = 0;
			virtual void ToShortName(NotNullPtr<Text::StringBuilderUTF8> sb) const = 0;

			virtual UOSInt GetCertCount();
			virtual Bool GetCertName(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb);
			virtual X509Cert *GetNewCert(UOSInt index);
			virtual ValidStatus IsValid(Net::SSLEngine *ssl, Crypto::Cert::CertStore *trustStore) const = 0;

			void ToShortString(NotNullPtr<Text::StringBuilderUTF8> sb) const;
			Bool IsSignatureKey(Net::SSLEngine *ssl, NotNullPtr<Crypto::Cert::X509Key> key) const;
			Bool GetSignedInfo(NotNullPtr<SignedInfo> signedInfo) const;

			static Bool ParseDigestType(DigestInfo *digestInfo, const UInt8 *pdu, const UInt8 *pduEnd);
			static Crypto::Hash::HashType GetAlgHash(AlgType algType);
			static Text::CStringNN FileTypeGetName(FileType fileType);
			static Text::CStringNN KeyTypeGetName(KeyType keyType);
			static Text::CStringNN KeyTypeGetOID(KeyType keyType);
			static Text::CStringNN ECNameGetName(ECName ecName);
			static Text::CStringNN ECNameGetOID(ECName ecName);
			static Text::CStringNN ValidStatusGetName(ValidStatus validStatus);
			static Text::CStringNN ValidStatusGetDesc(ValidStatus validStatus);
			static Crypto::Hash::HashType HashTypeFromOID(const UInt8 *oid, UOSInt oidLen);
			static X509File *CreateFromCerts(NotNullPtr<const Data::ReadingList<Crypto::Cert::Certificate *>> certs);
		};
	}
}
#endif
