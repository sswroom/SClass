#ifndef _SM_CRYPTO_CERT_X509KEY
#define _SM_CRYPTO_CERT_X509KEY
#include "Crypto/Cert/RSAKey.h"
#include "Crypto/Cert/X509File.h"
#include "Data/ByteArray.h"

namespace Crypto
{
	namespace Cert
	{
		class X509Key : public Crypto::Cert::X509File, public Crypto::Cert::RSAKey
		{
		private:
			KeyType keyType;
		public:
			X509Key(NN<Text::String> sourceName, Data::ByteArrayR buff, KeyType keyType);
			X509Key(Text::CStringNN sourceName, Data::ByteArrayR buff, KeyType keyType);
			virtual ~X509Key();

			virtual FileType GetFileType() const;
			virtual void ToShortName(NN<Text::StringBuilderUTF8> sb) const;
			virtual ValidStatus IsValid(NN<Net::SSLEngine> ssl, Optional<Crypto::Cert::CertStore> trustStore) const;
			
			virtual NN<ASN1Data> Clone() const;
			virtual void ToString(NN<Text::StringBuilderUTF8> sb) const;
			virtual NN<Net::ASN1Names> CreateNames() const;

			KeyType GetKeyType() const;
			UOSInt GetKeySizeBits() const;
			Bool IsPrivateKey() const;
			Optional<Crypto::Cert::X509Key> ExtractPublicKey() const;
			Bool GetKeyId(const Data::ByteArray &keyId) const; //20 bytes
			UOSInt GetDataBlockSize() const;

			virtual UnsafeArrayOpt<const UInt8> GetRSAModulus(OptOut<UOSInt> size) const;
			virtual UnsafeArrayOpt<const UInt8> GetRSAPublicExponent(OptOut<UOSInt> size) const;
			virtual UnsafeArrayOpt<const UInt8> GetRSAPrivateExponent(OptOut<UOSInt> size) const;
			virtual UnsafeArrayOpt<const UInt8> GetRSAPrime1(OptOut<UOSInt> size) const;
			virtual UnsafeArrayOpt<const UInt8> GetRSAPrime2(OptOut<UOSInt> size) const;
			virtual UnsafeArrayOpt<const UInt8> GetRSAExponent1(OptOut<UOSInt> size) const;
			virtual UnsafeArrayOpt<const UInt8> GetRSAExponent2(OptOut<UOSInt> size) const;
			virtual UnsafeArrayOpt<const UInt8> GetRSACoefficient(OptOut<UOSInt> size) const;

			UnsafeArrayOpt<const UInt8> GetECPrivate(OptOut<UOSInt> size) const;
			UnsafeArrayOpt<const UInt8> GetECPublic(OptOut<UOSInt> size) const;
			ECName GetECName() const;

			static NN<X509Key> FromECPublicKey(Data::ByteArrayR buff, Data::ByteArrayR paramOID);
			static NN<X509Key> CreateRSAPublicKey(Text::CStringNN name, Data::ByteArrayR modulus, Data::ByteArrayR publicExponent);
			static Optional<X509Key> FromRSAKey(NN<RSAKey> key);
		};
	}
}
#endif
