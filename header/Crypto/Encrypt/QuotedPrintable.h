#ifndef _SM_CRYPTO_ENCRYPT_QUOTEDPRINTABLE
#define _SM_CRYPTO_ENCRYPT_QUOTEDPRINTABLE
#include "Crypto/Encrypt/ICrypto.h"

namespace Crypto
{
	namespace Encrypt
	{
		class QuotedPrintable : public Crypto::Encrypt::ICrypto
		{
		public:
			QuotedPrintable();
			virtual ~QuotedPrintable();

			virtual Int32 Encrypt(const UInt8 *inBuff, Int32 inSize, UInt8 *outBuff, void *encParam);
			virtual Int32 Decrypt(const UInt8 *inBuff, Int32 inSize, UInt8 *outBuff, void *decParam);
		};
	};
};

#endif
