#ifndef _SM_CRYPTO_ENCRYPT_RC4CIPHER
#define _SM_CRYPTO_ENCRYPT_RC4CIPHER
#include "Crypto/Encrypt/ICrypto.h"

namespace Crypto
{
	namespace Encrypt
	{
		class RC4Cipher : public ICrypto
		{
		private:
			UInt8 s[256];
			UInt8 i;
			UInt8 j;

		public:
			RC4Cipher(const UInt8 *key, UOSInt keyLen);
			virtual ~RC4Cipher();

			void SetKey(const UInt8 *key, UOSInt keyLen);

			virtual UOSInt Encrypt(const UInt8 *inBuff, UOSInt inSize, UInt8 *outBuff, void *encParam); //outBuff = null to get the size
			virtual UOSInt Decrypt(const UInt8 *inBuff, UOSInt inSize, UInt8 *outBuff, void *decParam); //outBuff = null to get the size

			virtual UOSInt GetEncBlockSize();
			virtual UOSInt GetDecBlockSize();
		};
	}
}
#endif