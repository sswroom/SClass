#ifndef _SM_CRYPTO_CERT_X509RSAKEY
#define _SM_CRYPTO_CERT_X509RSAKEY
#include "Crypto/Cert/X509File.h"
#include "Data/ByteArray.h"

namespace Crypto
{
	namespace Cert
	{
		class X509Key : public Crypto::Cert::X509File
		{
		private:
			KeyType keyType;
		public:
			X509Key(NotNullPtr<Text::String> sourceName, Data::ByteArrayR buff, KeyType keyType);
			X509Key(Text::CStringNN sourceName, Data::ByteArrayR buff, KeyType keyType);
			virtual ~X509Key();

			virtual FileType GetFileType() const;
			virtual void ToShortName(NotNullPtr<Text::StringBuilderUTF8> sb) const;
			virtual ValidStatus IsValid(NotNullPtr<Net::SSLEngine> ssl, Optional<Crypto::Cert::CertStore> trustStore) const;
			
			virtual NotNullPtr<ASN1Data> Clone() const;
			virtual void ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const;
			virtual NotNullPtr<Net::ASN1Names> CreateNames() const;

			KeyType GetKeyType() const;
			UOSInt GetKeySizeBits() const;
			Bool IsPrivateKey() const;
			Crypto::Cert::X509Key *CreatePublicKey() const;
			Bool GetKeyId(const Data::ByteArray &keyId) const; //20 bytes

			const UInt8 *GetRSAModulus(OptOut<UOSInt> size) const;
			const UInt8 *GetRSAPublicExponent(OptOut<UOSInt> size) const;
			const UInt8 *GetRSAPrivateExponent(OptOut<UOSInt> size) const;
			const UInt8 *GetRSAPrime1(OptOut<UOSInt> size) const;
			const UInt8 *GetRSAPrime2(OptOut<UOSInt> size) const;
			const UInt8 *GetRSAExponent1(OptOut<UOSInt> size) const;
			const UInt8 *GetRSAExponent2(OptOut<UOSInt> size) const;
			const UInt8 *GetRSACoefficient(OptOut<UOSInt> size) const;

			const UInt8 *GetECPrivate(OptOut<UOSInt> size) const;
			const UInt8 *GetECPublic(OptOut<UOSInt> size) const;
			ECName GetECName() const;

			static X509Key *FromECPublicKey(const UInt8 *buff, UOSInt buffSize, const UInt8 *paramOID, UOSInt oidLen);
		};
	}
}
#endif
