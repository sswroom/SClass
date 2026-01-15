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
			Optional<Text::String> iss;
			Optional<Text::String> sub;
			Optional<Text::String> aud;
			Int64 exp;
			Int64 nbf;
			Int64 iat;
			Optional<Text::String> jti;

		public:
			JWTParam();
			~JWTParam();

			void Clear();
			void SetIssuer(Optional<Text::String> issuer);
			Optional<Text::String> GetIssuer() const;
			Bool IsIssuerValid(UnsafeArray<const UTF8Char> issuer, UIntOS issuerLen) const;
			void SetSubject(Optional<Text::String> subject);
			Optional<Text::String> GetSubject() const;
			void SetAudience(Optional<Text::String> audience);
			Optional<Text::String> GetAudience() const;
			void SetExpirationTime(Int64 t);
			Int64 GetExpirationTime() const;
			void SetNotBefore(Int64 t);
			Int64 GetNotBefore() const;
			void SetIssuedAt(Int64 t);
			Int64 GetIssuedAt() const;
			void SetJWTId(Optional<Text::String> id);
			Optional<Text::String> GetJWTId() const;
			Bool IsExpired(Data::Timestamp ts) const;
			void ToString(NN<Text::StringBuilderUTF8> sb) const;
		};
	}
}
#endif
