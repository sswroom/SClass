#ifndef _SM_CRYPTO_TOKEN_JWSIGNATURE
#define _SM_CRYPTO_TOKEN_JWSIGNATURE
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
			Net::SSLEngine *ssl;
			Algorithm alg;
			UInt8 *privateKey;
			UOSInt privateKeyLeng;
			UInt8 hashVal[256];
			UOSInt hashValSize;

		public:
			JWSignature(Net::SSLEngine *ssl, Algorithm alg, const UInt8 *privateKey, UOSInt privateKeyLeng);
			~JWSignature();

			Bool CalcHash(const UInt8 *buff, UOSInt buffSize);
			Bool VerifyHash(const UInt8 *buff, UOSInt buffSize, const UInt8 *signature, UOSInt signatureSize);
			Bool GetHashB64(NotNullPtr<Text::StringBuilderUTF8> sb) const;
			const UInt8 *GetSignature() const;
			UOSInt GetSignatureLen() const;

			static Text::CString AlgorithmGetName(Algorithm alg);
			static Algorithm AlgorithmGetByName(const UTF8Char *name);
		};
	}
}
#endif
