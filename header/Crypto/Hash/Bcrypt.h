#ifndef _SM_CRYPTO_HASH_BCRYPT
#define _SM_CRYPTO_HASH_BCRYPT
#include "Crypto/Hash/IHash.h"
#include "Text/StringBuilderUTF.h"
#include "Text/TextBinEnc/Radix64Enc.h"

namespace Crypto
{
	namespace Hash
	{
		class Bcrypt
		{
		private:
			Text::TextBinEnc::Radix64Enc *radix64;

			void CalcHash(UInt32 cost, const UInt8 *salt, const UTF8Char *password, UInt8 *hashBuff);
		public:
			Bcrypt();
			virtual ~Bcrypt();
			
			Bool Matches(const UTF8Char *hash, const UTF8Char *password);
			Bool GenHash(Text::StringBuilderUTF *sb, UInt32 cost, const UTF8Char *password);
			Bool GenHash(Text::StringBuilderUTF *sb, UInt32 cost, const UInt8 *salt, const UTF8Char *password);
		};
	}
}
#endif
