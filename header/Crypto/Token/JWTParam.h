#ifndef _SM_CRYPTO_TOKEN_JWTPARAM
#define _SM_CRYPTO_TOKEN_JWTPARAM
#include "Text/StringBuilderUTF.h"

namespace Crypto
{
	namespace Token
	{
		class JWTParam
		{
		private:
			Text::String *iss;
			Text::String *sub;
			Text::String *aud;
			Int64 exp;
			Int64 nbf;
			Int64 iat;
			Text::String *jti;

		public:
			JWTParam();
			~JWTParam();

			void Clear();
			void SetIssuer(Text::String *issuer);
			Text::String *GetIssuer();
			Bool IsIssuerValid(const UTF8Char *issuer, UOSInt issuerLen);
			void SetSubject(Text::String *subject);
			Text::String *GetSubject();
			void SetAudience(Text::String *audience);
			Text::String *GetAudience();
			void SetExpirationTime(Int64 t);
			Int64 GetExpirationTime();
			void SetNotBefore(Int64 t);
			Int64 GetNotBefore();
			void SetIssuedAt(Int64 t);
			Int64 GetIssuedAt();
			void SetJWTId(Text::String *id);
			Text::String *GetJWTId();
			void ToString(Text::StringBuilderUTF *sb);
		};
	}
}
#endif
