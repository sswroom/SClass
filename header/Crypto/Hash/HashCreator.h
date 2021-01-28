#ifndef _SM_CRYPTO_HASH_HASHCREATOR
#define _SM_CRYPTO_HASH_HASHCREATOR
#include "Crypto/Hash/IHash.h"
namespace Crypto
{
	namespace Hash
	{
		class HashCreator
		{
		public:
			static IHash *CreateHash(Crypto::Hash::HashType hashType);
		};
	}
}
#endif
