#ifndef _SM_CRYPTO_CERT_X509FILE
#define _SM_CRYPTO_CERT_X509FILE
#include "Data/ArrayList.h"
#include "Net/ASN1Data.h"

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
			const UTF8Char *countryName;
			const UTF8Char *stateOrProvinceName;
			const UTF8Char *localityName;
			const UTF8Char *organizationName;
			const UTF8Char *organizationUnitName;
			const UTF8Char *commonName;
			const UTF8Char *emailAddress;

			static void FreeNames(CertNames *names);
		};

		struct CertExtensions
		{
			Data::ArrayList<const UTF8Char *> *subjectAltName;
			Data::ArrayList<const UTF8Char *> *issuerAltName;
			Bool useSubjKeyId;
			UInt8 subjKeyId[20];
			Bool useAuthKeyId;
			UInt8 authKeyId[20];
			Bool caCert;
			Int32 caCertPathLen;

			static void FreeExtensions(CertExtensions *ext);
		};
		class X509Key;

		class X509File : public Net::ASN1Data
		{
		public:
			enum class FileType
			{
				Cert,
				Key,
				CertRequest,
				PrivateKey,
				Jks,
				PublicKey
			};

			enum class KeyType
			{
				Unknown,
				RSA,
				DSA,
				ECDSA,
				ED25519,
				RSAPublic
			};

		protected:
			static Bool IsSigned(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path); // AuthenticationFramework
			static void AppendSigned(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF *sb); // AuthenticationFramework
			static Bool IsTBSCertificate(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path); // AuthenticationFramework
			static void AppendTBSCertificate(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF *sb); // AuthenticationFramework
			static Bool IsCertificate(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path); // AuthenticationFramework
			static void AppendCertificate(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF *sb); // AuthenticationFramework
			static Bool IsPrivateKeyInfo(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path); // PKCS-8
			static void AppendPrivateKeyInfo(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF *sb); // PKCS-8
			static Bool IsCertificateRequestInfo(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path); // PKCS-10
			static void AppendCertificateRequestInfo(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF *sb); // PKCS-10
			static Bool IsCertificateRequest(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path); // PKCS-10
			static void AppendCertificateRequest(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF *sb); // PKCS-8
			static Bool IsPublicKeyInfo(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path); // AuthenticationFramework
			static void AppendPublicKeyInfo(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF *sb); // AuthenticationFramework

			static void AppendVersion(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF *sb); // AuthenticationFramework

			static void AppendAlgorithmIdentifier(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF *sb, const UTF8Char *varName); // PKCS-5
			static void AppendValidity(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF *sb, const UTF8Char *varName); // AuthenticationFramework
			static void AppendSubjectPublicKeyInfo(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF *sb, const UTF8Char *varName); // AuthenticationFramework
			static void AppendName(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF *sb, const UTF8Char *varName); // InformationFramework
			static void AppendRelativeDistinguishedName(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF *sb, const UTF8Char *varName); // InformationFramework
			static void AppendAttributeTypeAndDistinguishedValue(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF *sb, const UTF8Char *varName); // InformationFramework

			static Bool NameGetByOID(const UInt8 *pdu, const UInt8 *pduEnd, const Char *oidText, Text::StringBuilderUTF *sb);
			static Bool NameGetCN(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF *sb);
			static Bool NamesGet(const UInt8 *pdu, const UInt8 *pduEnd, CertNames *names);
			static Bool ExtensionsGet(const UInt8 *pdu, const UInt8 *pduEnd, CertExtensions *ext);

			static UOSInt KeyGetLeng(const UInt8 *pdu, const UInt8 *pduEnd, KeyType keyType);
			static KeyType KeyTypeFromOID(const UInt8 *oid, UOSInt oidLen, Bool pubKey);

			X509File(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize);
		public:
			virtual ~X509File();

			virtual Net::ASN1Data::ASN1Type GetASN1Type();
			virtual FileType GetFileType() = 0;
			virtual void ToShortName(Text::StringBuilderUTF *sb) = 0;

			void ToShortString(Text::StringBuilderUTF *sb);
			Bool IsSignatureKey(Net::SSLEngine *ssl, Crypto::Cert::X509Key *key);

			static const UTF8Char *FileTypeGetName(FileType fileType);
			static const UTF8Char *KeyTypeGetName(KeyType keyType);
			static const Char *KeyTypeGetOID(KeyType keyType);
		};
	}
}
#endif
