#ifndef _SM_CRYPTO_TOKEN_JWTOKEN
#define _SM_CRYPTO_TOKEN_JWTOKEN
#include "Crypto/Token/JWTParam.h"
#include "Crypto/Token/JWSignature.h"
#include "Net/SSLEngine.h"
#include "Text/StringBuilderUTF8.h"

namespace Crypto
{
	namespace Token
	{
		class JWToken
		{
		public:
			enum class VerifyType
			{
				Unknown,
				Azure,
				Password,
				Key
			};
		private:
			struct PayloadMapping
			{
				const UTF8Char *key;
				UOSInt keyLen;
				const UTF8Char *name;
				UOSInt nameLen;
			};
		private:
			JWSignature::Algorithm alg;
			Text::String *header;
			Text::String *payload;
			UInt8 *sign;
			UOSInt signSize;

			static PayloadMapping payloadNames[];

			JWToken(JWSignature::Algorithm alg);
			void SetHeader(Text::CStringNN header);
			void SetPayload(Text::CStringNN payload);
			void SetSignature(const UInt8 *sign, UOSInt signSize);
		public:
			~JWToken();

			JWSignature::Algorithm GetAlgorithm() const;
			Text::String *GetHeader() const;
			Text::String *GetPayload() const;
			VerifyType GetVerifyType(NotNullPtr<JWTParam> param) const;
			Bool SignatureValid(Net::SSLEngine *ssl, const UInt8 *key, UOSInt keyLeng, Crypto::Cert::X509Key::KeyType keyType);
			void ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const;

			Data::StringMap<Text::String*> *ParsePayload(NotNullPtr<JWTParam> param, Bool keepDefault, Text::StringBuilderUTF8 *sbErr);
			void FreeResult(Data::StringMap<Text::String*> *result);

			static JWToken *Generate(JWSignature::Algorithm alg, Text::CStringNN payload, Net::SSLEngine *ssl, const UInt8 *key, UOSInt keyLeng, Crypto::Cert::X509Key::KeyType keyType);
			static JWToken *Parse(Text::CStringNN token, Text::StringBuilderUTF8 *sbErr);
			static Text::CString PayloadName(Text::CString key);
			static Text::CString VerifyTypeGetName(VerifyType verifyType);
		};
	}
}
#endif