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

			static void CalcHash(UInt32 cost, const UInt8 *salt, const UTF8Char *password, UOSInt pwdLen, UInt8 *hashBuff);
		public:
			BcryptValidator();
			virtual ~BcryptValidator();

			virtual NN<HashValidatorSess> CreateSess();
			virtual void DeleteSess(NN<HashValidatorSess> sess);
			virtual Bool SetHash(const UTF8Char *hash, UOSInt hashLen);
			virtual Bool IsMatch(NN<HashValidatorSess> sess, const UTF8Char *password, UOSInt pwdLen);
		};
	}
}
#endif
