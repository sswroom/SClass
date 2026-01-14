#ifndef _SM_CRYPTO_TOKEN_JWTOKEN
#define _SM_CRYPTO_TOKEN_JWTOKEN
#include "Crypto/Cert/X509PrivKey.h"
#include "Crypto/Token/JWTParam.h"
#include "Crypto/Token/JWSignature.h"
#include "Data/StringMapObj.hpp"
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
				UnsafeArray<const UTF8Char> key;
				UOSInt keyLen;
				UnsafeArray<const UTF8Char> name;
				UOSInt nameLen;
			};
		private:
			JWSignature::Algorithm alg;
			Optional<Text::String> header;
			Optional<Text::String> payload;
			UnsafeArrayOpt<UInt8> sign;
			UOSInt signSize;

			static PayloadMapping payloadNames[];

			JWToken(JWSignature::Algorithm alg);
			void SetHeader(Text::CStringNN header);
			void SetPayload(Text::CStringNN payload);
			void SetSignature(UnsafeArray<const UInt8> sign, UOSInt signSize);
		public:
			~JWToken();

			JWSignature::Algorithm GetAlgorithm() const;
			Optional<Text::String> GetHeader() const;
			Optional<Text::String> GetPayload() const;
			VerifyType GetVerifyType(NN<JWTParam> param) const;
			Bool SignatureValid(Optional<Net::SSLEngine> ssl, UnsafeArray<const UInt8> key, UOSInt keyLeng, Crypto::Cert::X509Key::KeyType keyType);
			void ToString(NN<Text::StringBuilderUTF8> sb) const;

			Optional<Data::StringMapObj<Text::String*>> ParsePayload(NN<JWTParam> param, Bool keepDefault, Optional<Text::StringBuilderUTF8> sbErr);
			void FreeResult(NN<Data::StringMapObj<Text::String*>> result);

			static Optional<JWToken> Generate(JWSignature::Algorithm alg, Text::CStringNN payload, Optional<Net::SSLEngine> ssl, UnsafeArray<const UInt8> key, UOSInt keyLeng, Crypto::Cert::X509Key::KeyType keyType);
			static Optional<JWToken> GenerateRSA(JWSignature::Algorithm alg, Text::CStringNN payload, Optional<Net::SSLEngine> ssl, Text::CStringNN keyId, NN<Crypto::Cert::X509PrivKey> key);
			static Optional<JWToken> Parse(Text::CStringNN token, Optional<Text::StringBuilderUTF8> sbErr);
			static Text::CStringNN PayloadName(Text::CStringNN key);
			static Text::CStringNN VerifyTypeGetName(VerifyType verifyType);
		};
	}
}
#endif
