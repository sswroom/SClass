#ifndef _SM_CRYPTO_TOKEN_JWTHANDLER
#define _SM_CRYPTO_TOKEN_JWTHANDLER
#include "Crypto/Token/JWTParam.h"
#include "Data/StringUTF8Map.h"
#include "Text/StringBuilderUTF8.h"

namespace Crypto
{
	namespace Token
	{
		class JWTHandler
		{
		public:
			typedef enum
			{
				UNKNOWN,
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

		private:
			Algorithm alg;
			UInt8 *privateKey;
			UOSInt privateKeyLeng;

		private:
			JWTHandler(Algorithm alg, const UInt8 *privateKey, UOSInt privateKeyLeng);
		public:
			~JWTHandler();

			Bool Generate(Text::StringBuilderUTF8 *sb, Data::StringUTF8Map<const UTF8Char*> *payload, JWTParam *param);
			Data::StringUTF8Map<const UTF8Char*> *Parse(const UTF8Char *token, JWTParam *param);

			void FreeResult(Data::StringUTF8Map<const UTF8Char*> *result);

			static JWTHandler *CreateHMAC(Algorithm alg, const UInt8 *key, UOSInt keyLeng);
			static const UTF8Char *GetAlgorithmName(Algorithm alg);
			static Algorithm GetAlgorithmByName(const UTF8Char *name);
		};
	}
}
#endif
