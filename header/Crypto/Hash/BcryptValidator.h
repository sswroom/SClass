#ifndef _SM_CRYPTO_HASH_BCRYPTVALIDATOR
#define _SM_CRYPTO_HASH_BCRYPTVALIDATOR
#include "Crypto/Hash/HashValidator.h"
#include "Text/TextBinEnc/Radix64Enc.h"

namespace Crypto
{
	namespace Hash
	{
		class BcryptValidator : public HashValidator
		{
		private:
			Text::TextBinEnc::Radix64Enc radix64;

			UInt8 salt[16];
			UInt8 hashCTxt[24];
			UInt32 cost;

			static void CalcHash(UInt32 cost, UnsafeArray<const UInt8> salt, Text::CStringNN password, UnsafeArray<UInt8> hashBuff);
		public:
			BcryptValidator();
			virtual ~BcryptValidator();

			virtual NN<HashValidatorSess> CreateSess();
			virtual void DeleteSess(NN<HashValidatorSess> sess);
			virtual Bool SetHash(UnsafeArray<const UTF8Char> hash, UIntOS hashLen);
			virtual Bool IsMatch(NN<HashValidatorSess> sess, UnsafeArray<const UTF8Char> password, UIntOS pwdLen);
		};
	}
}
#endif
