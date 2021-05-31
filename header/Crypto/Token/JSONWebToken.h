#ifndef _SM_CRYPTO_TOKEN_JSONWEBTOKEN
#define _SM_CRYPTO_TOKEN_JSONWEBTOKEN
#include "Text/StringBuilderUTF8.h"

namespace Crypto
{
	namespace Token
	{
		class JSONWebToken
		{
		public:
			typedef enum
			{
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
			} Algorithm;
		public:
			static Bool Generate(Text::StringBuilderUTF8 *sb, Algorithm alg, const UTF8Char *payload, const UInt8 *key, UOSInt keySize);
			static const UTF8Char *GetAlgorithmName(Algorithm alg);
		};
	}
}
#endif
