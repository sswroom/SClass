#ifndef _SM_CRYPTO_CERT_X509FILE
#define _SM_CRYPTO_CERT_X509FILE
#include "Net/ASN1Data.h"

namespace Crypto
{
	namespace Cert
	{
		class X509File : public Net::ASN1Data
		{
		public:
			typedef enum
			{
				FT_CERT,
				FT_RSA_KEY,
				FT_CERT_REQ,
				FT_PRIV_KEY,
				FT_JKS
			} FileType;
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

			static void AppendVersion(const UInt8 *pdu, const UInt8 *pduEnd, const Char *path, Text::StringBuilderUTF *sb); // AuthenticationFramework

			static void AppendAlgorithmIdentifier(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF *sb, const UTF8Char *varName); // PKCS-5
			static void AppendValidity(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF *sb, const UTF8Char *varName); // AuthenticationFramework
			static void AppendSubjectPublicKeyInfo(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF *sb, const UTF8Char *varName); // AuthenticationFramework
			static void AppendName(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF *sb, const UTF8Char *varName); // InformationFramework
			static void AppendRelativeDistinguishedName(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF *sb, const UTF8Char *varName); // InformationFramework
			static void AppendAttributeTypeAndDistinguishedValue(const UInt8 *pdu, const UInt8 *pduEnd, Text::StringBuilderUTF *sb, const UTF8Char *varName); // InformationFramework

			X509File(const UTF8Char *sourceName, const UInt8 *buff, UOSInt buffSize);
		public:
			virtual ~X509File();

			virtual Net::ASN1Data::ASN1Type GetASN1Type();
			virtual FileType GetFileType() = 0;
		};
	}
}
#endif
