#ifndef _SM_CRYPTO_TOKEN_JWTHANDLER
#define _SM_CRYPTO_TOKEN_JWTHANDLER
#include "Crypto/Token/JWSignature.h"
#include "Crypto/Token/JWTParam.h"
#include "Data/StringMap.h"
#include "Data/StringUTF8Map.h"
#include "Net/SSLEngine.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

namespace Crypto
{
	namespace Token
	{
		class JWTHandler
		{
		private:
			Net::SSLEngine *ssl;
			JWSignature::Algorithm alg;
			UInt8 *privateKey;
			UOSInt privateKeyLeng;

		public:
			JWTHandler(Net::SSLEngine *ssl, JWSignature::Algorithm alg, const UInt8 *privateKey, UOSInt privateKeyLeng);
			~JWTHandler();

			Bool Generate(NotNullPtr<Text::StringBuilderUTF8> sb, Data::StringMap<const UTF8Char*> *payload, JWTParam *param);
			Data::StringMap<Text::String*> *Parse(const UTF8Char *token, JWTParam *param);

			void FreeResult(Data::StringMap<Text::String*> *result);

			static JWTHandler *CreateHMAC(Net::SSLEngine *ssl, JWSignature::Algorithm alg, const UInt8 *key, UOSInt keyLeng);
		};
	}
}
#endif
