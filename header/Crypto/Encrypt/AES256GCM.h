#ifndef _SM_CRYPTO_ENCRYPT_AES256GCM
#define _SM_CRYPTO_ENCRYPT_AES256GCM
#include "Crypto/Encrypt/ICrypto.h"

namespace Crypto
{
	namespace Encrypt
	{
		class AES256GCM : public ICrypto
		{
		private:
			UInt8 key[32];
			UInt8 iv[12];
		public:
			AES256GCM(UnsafeArray<const UInt8> key, UnsafeArray<const UInt8> iv);
			virtual ~AES256GCM();

			virtual UOSInt Encrypt(UnsafeArray<const UInt8> inBuff, UOSInt inSize, UnsafeArray<UInt8> outBuff);
			virtual UOSInt Decrypt(UnsafeArray<const UInt8> inBuff, UOSInt inSize, UnsafeArray<UInt8> outBuff);

			virtual UOSInt GetEncBlockSize() const;
			virtual UOSInt GetDecBlockSize() const;

			void SetIV(UnsafeArray<const UInt8> iv); //12 bytes
		};
	}
}
#endif
