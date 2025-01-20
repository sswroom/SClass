#ifndef _SM_CRYPTO_HASH_BCRYPT
#define _SM_CRYPTO_HASH_BCRYPT
#include "Crypto/Hash/HashAlgorithm.h"
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

			void CalcHash(UInt32 cost, const UInt8 *salt, Text::CStringNN password, UInt8 *hashBuff) const;
		public:
			Bcrypt();
			virtual ~Bcrypt();
			
			Bool Matches(Text::CStringNN hash, Text::CStringNN password) const;
			Bool GenHash(NN<Text::StringBuilderUTF8> sb, UInt32 cost, Text::CStringNN password);
			Bool GenHash(NN<Text::StringBuilderUTF8> sb, UInt32 cost, const UInt8 *salt, Text::CStringNN password) const;
		};
	}
}
#endif
