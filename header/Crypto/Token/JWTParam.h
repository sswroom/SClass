#ifndef _SM_CRYPTO_TOKEN_JWTPARAM
#define _SM_CRYPTO_TOKEN_JWTPARAM
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

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
			Text::String *GetIssuer() const;
			Bool IsIssuerValid(const UTF8Char *issuer, UOSInt issuerLen) const;
			void SetSubject(Text::String *subject);
			Text::String *GetSubject() const;
			void SetAudience(Text::String *audience);
			Text::String *GetAudience() const;
			void SetExpirationTime(Int64 t);
			Int64 GetExpirationTime() const;
			void SetNotBefore(Int64 t);
			Int64 GetNotBefore() const;
			void SetIssuedAt(Int64 t);
			Int64 GetIssuedAt() const;
			void SetJWTId(Text::String *id);
			Text::String *GetJWTId() const;
			Bool IsExpired(Data::Timestamp ts) const;
			void ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const;
		};
	}
}
#endif
