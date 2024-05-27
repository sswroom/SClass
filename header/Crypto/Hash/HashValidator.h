#ifndef _SM_CRYPTO_HASH_HASHVALIDATOR
#define _SM_CRYPTO_HASH_HASHVALIDATOR

namespace Crypto
{
	namespace Hash
	{
		class HashValidatorSess;

		class HashValidator
		{
		public:
			virtual ~HashValidator(){};

			virtual NN<HashValidatorSess> CreateSess() = 0;
			virtual void DeleteSess(NN<HashValidatorSess> sess) = 0;
			virtual Bool SetHash(const UTF8Char *hash, UOSInt hashLen) = 0;
			virtual Bool IsMatch(NN<HashValidatorSess> sess, const UTF8Char *password, UOSInt pwdLen) = 0;
		};
	}
}
#endif
