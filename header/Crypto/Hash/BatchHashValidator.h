#ifndef _SM_CRYPTO_HASH_BATCHHASHVALIDATOR
#define _SM_CRYPTO_HASH_BATCHHASHVALIDATOR
#include "Crypto/Hash/HashValidator.h"
#include "Crypto/Hash/IHash.h"

namespace Crypto
{
	namespace Hash
	{
		class BatchHashValidator : public HashValidator
		{
		private:
			NN<Crypto::Hash::IHash> hash;
			Bool toRelease;
			UInt8 hashBuff[64];
			UOSInt hashSize;
		public:
			BatchHashValidator(NN<Crypto::Hash::IHash> hash, Bool toRelease);
			virtual ~BatchHashValidator();

			virtual NN<HashValidatorSess> CreateSess();
			virtual void DeleteSess(NN<HashValidatorSess> sess);
			virtual Bool SetHash(const UTF8Char *hash, UOSInt hashLen);
			virtual Bool IsMatch(NN<HashValidatorSess> sess, const UTF8Char *password, UOSInt pwdLen);
		};
	}
}
#endif
