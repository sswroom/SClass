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

			virtual UIntOS Encrypt(UnsafeArray<const UInt8> inBuff, UIntOS inSize, UnsafeArray<UInt8> outBuff) = 0; //outBuff = null to get the size
			virtual UIntOS Decrypt(UnsafeArray<const UInt8> inBuff, UIntOS inSize, UnsafeArray<UInt8> outBuff) = 0; //outBuff = null to get the size

			virtual UIntOS GetEncBlockSize() const = 0;
			virtual UIntOS GetDecBlockSize() const = 0;
		};
	}
}
#endif
