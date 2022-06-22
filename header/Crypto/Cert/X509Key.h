#ifndef _SM_CRYPTO_CERT_X509RSAKEY
#define _SM_CRYPTO_CERT_X509RSAKEY
#include "Crypto/Cert/X509File.h"

namespace Crypto
{
	namespace Cert
	{
		class X509Key : public Crypto::Cert::X509File
		{
		private:
			KeyType keyType;
		public:
			X509Key(Text::String *sourceName, const UInt8 *buff, UOSInt buffSize, KeyType keyType);
			X509Key(Text::CString sourceName, const UInt8 *buff, UOSInt buffSize, KeyType keyType);
			virtual ~X509Key();

			virtual FileType GetFileType();
			virtual void ToShortName(Text::StringBuilderUTF8 *sb);
			virtual ValidStatus IsValid(Net::SSLEngine *ssl, Crypto::Cert::CertStore *trustStore);
			
			virtual ASN1Data *Clone();
			virtual void ToString(Text::StringBuilderUTF8 *sb);

			KeyType GetKeyType();
			UOSInt GetKeySizeBits();
			Bool IsPrivateKey();
			Crypto::Cert::X509Key *CreatePublicKey();
			Bool GetKeyId(UInt8 *keyId); //20 bytes

			const UInt8 *GetRSAModulus(UOSInt *size);
			const UInt8 *GetRSAPublicExponent(UOSInt *size);
			const UInt8 *GetRSAPrivateExponent(UOSInt *size);
			const UInt8 *GetRSAPrime1(UOSInt *size);
			const UInt8 *GetRSAPrime2(UOSInt *size);
			const UInt8 *GetRSAExponent1(UOSInt *size);
			const UInt8 *GetRSAExponent2(UOSInt *size);
			const UInt8 *GetRSACoefficient(UOSInt *size);

			const UInt8 *GetECPublic(UOSInt *size);
		};
	}
}
#endif
