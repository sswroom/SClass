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
			const UTF8Char *iss;
			const UTF8Char *sub;
			const UTF8Char *aud;
			Int64 exp;
			Int64 nbf;
			Int64 iat;
			const UTF8Char *jti;

		public:
			JWTParam();
			~JWTParam();

			void Clear();
			void SetIssuer(const UTF8Char *issuer);
			const UTF8Char *GetIssuer();
			Bool IsIssuerValid(const UTF8Char *issuer);
			void SetSubject(const UTF8Char *subject);
			const UTF8Char *GetSubject();
			void SetAudience(const UTF8Char *audience);
			const UTF8Char *GetAudience();
			void SetExpirationTime(Int64 t);
			Int64 GetExpirationTime();
			void SetNotBefore(Int64 t);
			Int64 GetNotBefore();
			void SetIssuedAt(Int64 t);
			Int64 GetIssuedAt();
			void SetJWTId(const UTF8Char *id);
			const UTF8Char *GetJWTId();
			void ToString(Text::StringBuilderUTF *sb);
		};
	}
}
#endif
