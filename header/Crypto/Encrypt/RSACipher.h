#ifndef _SM_CRYPTO_ENCRYPT_RSACIPHER
#define _SM_CRYPTO_ENCRYPT_RSACIPHER
#include "Crypto/Hash/IHash.h"
#include "Text/CString.h"

namespace Crypto
{
	namespace Encrypt
	{
		class RSACipher
		{
		public:
			enum class Padding
			{
				PKCS1,
				NoPadding,
				PKCS1_OAEP,
				X931,
				PKCS1_PSS,
				PKCS1_WithTLS
			};

			static UOSInt PaddingAppend(UInt8 *destBuff, UOSInt destSize, const UInt8 *message, UOSInt msgSize, Padding padding);
			static UOSInt PaddingRemove(UInt8 *destBuff, const UInt8 *blockWithPadding, UOSInt blockSize, Padding padding);
			static Bool MGF1(UInt8 *destBuff, const UInt8 *seed, UOSInt seedLen, UOSInt len, Crypto::Hash::HashType hashType);
			static Text::CString PaddingGetName(Padding padding);
		};
	}
}
#endif