#ifndef _SM_CRYPTO_HASH_BCRYPT
#define _SM_CRYPTO_HASH_BCRYPT
#include "Crypto/Hash/IHash.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/TextBinEnc/Radix64Enc.h"

namespace Crypto
{
	namespace Hash
	{
		class Bcrypt
		{
		private:
			Text::TextBinEnc::Radix64Enc radix64;

			void CalcHash(UInt32 cost, const UInt8 *salt, const UTF8Char *password, UOSInt pwdLen, UInt8 *hashBuff);
		public:
			Bcrypt();
			virtual ~Bcrypt();
			
			Bool Matches(const UTF8Char *hash, UOSInt hashLen, const UTF8Char *password, UOSInt pwdLen);
			Bool GenHash(Text::StringBuilderUTF8 *sb, UInt32 cost, const UTF8Char *password, UOSInt pwdLen);
			Bool GenHash(Text::StringBuilderUTF8 *sb, UInt32 cost, const UInt8 *salt, const UTF8Char *password, UOSInt pwdLen);
		};
	}
}
#endif
