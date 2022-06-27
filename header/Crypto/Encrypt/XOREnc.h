#ifndef _SM_CRYPTO_ENCRYPT_XORENC
#define _SM_CRYPTO_ENCRYPT_XORENC

#include "Crypto/Encrypt/ICrypto.h"

namespace Crypto
{
	namespace Encrypt
	{
		class XOREnc : public Crypto::Encrypt::ICrypto
		{
		public:
			XOREnc();
			virtual ~XOREnc();

			virtual UOSInt Encrypt(const UInt8 *inBuff, UOSInt inSize, UInt8 *outBuff, void *encParam);
			virtual UOSInt Decrypt(const UInt8 *inBuff, UOSInt inSize, UInt8 *outBuff, void *decParam);

			virtual UOSInt GetEncBlockSize() const;
			virtual UOSInt GetDecBlockSize() const;
		};
	}
}

#endif
