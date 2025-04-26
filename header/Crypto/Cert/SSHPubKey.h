#ifndef _SM_CRYPTO_CERT_SSHPUBKEY
#define _SM_CRYPTO_CERT_SSHPUBKEY
#include "Crypto/Cert/X509Key.h"
#include "Data/ByteArray.h"
#include "Data/ByteBuffer.h"
#include "Text/String.h"

namespace Crypto
{
	namespace Cert
	{
		class SSHPubKey
		{
		private:
			NN<Text::String> sourceNameObj;
			Data::ByteBuffer buff;
		public:
			SSHPubKey(Text::CStringNN sourceName, Data::ByteArrayR buff);
			~SSHPubKey();

			UnsafeArray<const UInt8> GetArr() const;
			UOSInt GetSize() const;
			UnsafeArrayOpt<const UInt8> GetRSAModulus(OptOut<UOSInt> size) const;
			UnsafeArrayOpt<const UInt8> GetRSAPublicExponent(OptOut<UOSInt> size) const;

			Optional<Crypto::Cert::X509Key> CreateKey() const;
			static NN<SSHPubKey> CreateRSAPublicKey(Text::CStringNN name, Data::ByteArrayR modulus, Data::ByteArrayR publicExponent);
			static Bool IsValid(Data::ByteArrayR buff);
		};
	}
}
#endif
