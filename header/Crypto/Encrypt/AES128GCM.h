#ifndef _SM_CRYPTO_ENCRYPT_AES128GCM
#define _SM_CRYPTO_ENCRYPT_AES128GCM
#include "Crypto/Encrypt/Encryption.h"

namespace Crypto
{
	namespace Encrypt
	{
		class AES128GCM : public Encryption
		{
		private:
			UInt8 key[16];
			UInt8 iv[12];
		public:
			AES128GCM(UnsafeArray<const UInt8> key, UnsafeArray<const UInt8> iv);
			virtual ~AES128GCM();

			virtual UOSInt Encrypt(UnsafeArray<const UInt8> inBuff, UOSInt inSize, UnsafeArray<UInt8> outBuff);
			virtual UOSInt Decrypt(UnsafeArray<const UInt8> inBuff, UOSInt inSize, UnsafeArray<UInt8> outBuff);

			virtual UOSInt GetEncBlockSize() const;
			virtual UOSInt GetDecBlockSize() const;
		};
	}
}
#endif
