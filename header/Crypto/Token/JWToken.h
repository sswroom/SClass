#ifndef _SM_CRYPTO_TOKEN_JWTOKEN
#define _SM_CRYPTO_TOKEN_JWTOKEN
#include "Crypto/Token/JWSignature.h"
#include "Net/SSLEngine.h"
#include "Text/StringBuilderUTF8.h"

namespace Crypto
{
	namespace Token
	{
		class JWToken
		{
		private:
			JWSignature::Algorithm alg;
			Text::String *header;
			Text::String *payload;
			UInt8 *sign;
			UOSInt signSize;

			JWToken(JWSignature::Algorithm alg);
			void SetHeader(Text::CStringNN header);
			void SetPayload(Text::CStringNN payload);
			void SetSignature(const UInt8 *sign, UOSInt signSize);
		public:
			~JWToken();

			Text::String *GetHeader() const;
			Text::String *GetPayload() const;

			Bool SignatureValid(Net::SSLEngine *ssl, const UInt8 *key, UOSInt keyLeng);
			void ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const;

			static JWToken *Generate(JWSignature::Algorithm alg, Text::CStringNN payload, Net::SSLEngine *ssl, const UInt8 *privateKey, UOSInt privateKeyLeng);
			static JWToken *Parse(Text::CStringNN token, Text::StringBuilderUTF8 *sbErr);
		};
	}
}
#endif
