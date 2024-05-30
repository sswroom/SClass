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
			Optional<Net::SSLEngine> ssl;
			JWSignature::Algorithm alg;
			UInt8 *key;
			UOSInt keyLeng;
			Crypto::Cert::X509Key::KeyType keyType;

		public:
			JWTHandler(Optional<Net::SSLEngine> ssl, JWSignature::Algorithm alg, UnsafeArray<const UInt8> key, UOSInt keyLeng, Crypto::Cert::X509Key::KeyType keyType);
			~JWTHandler();

			Bool Generate(NN<Text::StringBuilderUTF8> sb, Data::StringMap<const UTF8Char*> *payload, JWTParam *param);
		};
	}
}
#endif
