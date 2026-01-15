#ifndef _SM_CRYPTO_ENCRYPT_RSACIPHER
#define _SM_CRYPTO_ENCRYPT_RSACIPHER
#include "Crypto/Hash/HashAlgorithm.h"
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

			static UIntOS PaddingAppend(UnsafeArray<UInt8> destBuff, UIntOS destSize, UnsafeArray<const UInt8> message, UIntOS msgSize, Padding padding);
			static UIntOS PaddingRemove(UnsafeArray<UInt8> destBuff, UnsafeArray<const UInt8> blockWithPadding, UIntOS blockSize, Padding padding);
			static Bool MGF1(UnsafeArray<UInt8> destBuff, UnsafeArray<const UInt8> seed, UIntOS seedLen, UIntOS len, Crypto::Hash::HashType hashType);
			static Text::CStringNN PaddingGetName(Padding padding);
		};
	}
}
#endif
