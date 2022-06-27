#ifndef _SM_CRYPTO_ENCRYPT_BASE64
#define _SM_CRYPTO_ENCRYPT_BASE64
#include "Crypto/Encrypt/ICrypto.h"

namespace Crypto
{
	namespace Encrypt
	{
		class Base64 : public ::Crypto::Encrypt::ICrypto
		{
		public:
			Base64();
			virtual ~Base64();

			virtual UOSInt Encrypt(const UInt8 *inBuff, UOSInt inSize, UInt8 *outBuff, void *encParam);
			virtual UOSInt Decrypt(const UInt8 *inBuff, UOSInt inSize, UInt8 *outBuff, void *decParam);

			virtual UOSInt GetEncBlockSize() const;
			virtual UOSInt GetDecBlockSize() const;
		};
	}
}
#endif
