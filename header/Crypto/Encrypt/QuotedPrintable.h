#ifndef _SM_CRYPTO_ENCRYPT_QUOTEDPRINTABLE
#define _SM_CRYPTO_ENCRYPT_QUOTEDPRINTABLE
#include "Crypto/Encrypt/Encryption.h"

namespace Crypto
{
	namespace Encrypt
	{
		class QuotedPrintable : public Crypto::Encrypt::Encryption
		{
		public:
			QuotedPrintable();
			virtual ~QuotedPrintable();

			virtual Int32 Encrypt(UnsafeArray<const UInt8> inBuff, Int32 inSize, UnsafeArray<UInt8> outBuff);
			virtual Int32 Decrypt(UnsafeArray<const UInt8> inBuff, Int32 inSize, UnsafeArray<UInt8> outBuff);
		};
	};
};

#endif
