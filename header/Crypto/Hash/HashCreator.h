#ifndef _SM_CRYPTO_HASH_HASHCREATOR
#define _SM_CRYPTO_HASH_HASHCREATOR
#include "Crypto/Hash/HashAlgorithm.h"
namespace Crypto
{
	namespace Hash
	{
		class HashCreator
		{
		public:
			static Optional<HashAlgorithm> CreateHash(Crypto::Hash::HashType hashType);
		};
	}
}
#endif
