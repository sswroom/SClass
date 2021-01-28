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

			virtual OSInt Encrypt(const UInt8 *inBuff, OSInt inSize, UInt8 *outBuff, void *encParam);
			virtual OSInt Decrypt(const UInt8 *inBuff, OSInt inSize, UInt8 *outBuff, void *decParam);

			virtual OSInt GetEncBlockSize();
			virtual OSInt GetDecBlockSize();
		};
	};
};

#endif
