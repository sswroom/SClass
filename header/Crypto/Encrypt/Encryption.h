#ifndef _SM_CRYPTO_ENCRYPT_ENCRYPTION
#define _SM_CRYPTO_ENCRYPT_ENCRYPTION

namespace Crypto
{
	namespace Encrypt
	{
		class Encryption
		{
		public:
			virtual ~Encryption() {};

			virtual UOSInt Encrypt(UnsafeArray<const UInt8> inBuff, UOSInt inSize, UnsafeArray<UInt8> outBuff) = 0; //outBuff = null to get the size
			virtual UOSInt Decrypt(UnsafeArray<const UInt8> inBuff, UOSInt inSize, UnsafeArray<UInt8> outBuff) = 0; //outBuff = null to get the size

			virtual UOSInt GetEncBlockSize() const = 0;
			virtual UOSInt GetDecBlockSize() const = 0;
		};
	}
}
#endif
