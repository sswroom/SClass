#ifndef _SM_CRYPTO_TOKEN_JWSIGNATURE
#define _SM_CRYPTO_TOKEN_JWSIGNATURE
#include "Crypto/Cert/X509Key.h"
#include "Crypto/Token/JWTParam.h"
#include "Net/SSLEngine.h"
#include "Text/StringBuilderUTF8.h"

namespace Crypto
{
	namespace Token
	{
		class JWSignature
		{
		public:
			enum class Algorithm
			{
				Unknown,
				HS256,
				HS384,
				HS512,
				PS256,
				PS384,
				PS512,
				RS256,
				RS384,
				RS512,
				ES256,
				ES256K,
				ES384,
				ES512,
				EDDSA
			};

		private:
			Optional<Net::SSLEngine> ssl;
			Algorithm alg;
			Crypto::Cert::X509Key::KeyType keyType;
			UnsafeArray<UInt8> privateKey;
			UOSInt privateKeyLeng;
			UInt8 hashVal[256];
			UOSInt hashValSize;

		public:
			JWSignature(Optional<Net::SSLEngine> ssl, Algorithm alg, UnsafeArray<const UInt8> privateKey, UOSInt privateKeyLeng, Crypto::Cert::X509Key::KeyType keyType);
			~JWSignature();

			Bool CalcHash(UnsafeArray<const UInt8> buff, UOSInt buffSize);
			Bool VerifyHash(UnsafeArray<const UInt8> buff, UOSInt buffSize, UnsafeArray<const UInt8> signature, UOSInt signatureSize);
			Bool GetHashB64(NN<Text::StringBuilderUTF8> sb) const;
			UnsafeArray<const UInt8> GetSignature() const;
			UOSInt GetSignatureLen() const;

			static Text::CStringNN AlgorithmGetName(Algorithm alg);
			static Algorithm AlgorithmGetByName(UnsafeArray<const UTF8Char> name);
		};
	}
}
#endif
