#ifndef _SM_CRYPTO_ENCRYPT_BLOCKCIPHER
#define _SM_CRYPTO_ENCRYPT_BLOCKCIPHER
#include "Crypto/Encrypt/ICrypto.h"
#include "Text/CString.h"

namespace Crypto
{
	namespace Encrypt
	{
		enum class ChainMode
		{
			ECB,
			CBC,
			PCBC,
			CFB,
			OFB
		};

		class BlockCipher : public ICrypto
		{
		protected:
			UInt8 *iv;
			UOSInt blockSize;
			ChainMode cm;

		public:
			BlockCipher(UOSInt blockSize);
			virtual ~BlockCipher();

			virtual UOSInt Encrypt(UnsafeArray<const UInt8> inBuff, UOSInt inSize, UnsafeArray<UInt8> outBuff);
			virtual UOSInt Decrypt(UnsafeArray<const UInt8> inBuff, UOSInt inSize, UnsafeArray<UInt8> outBuff);
			virtual UOSInt GetEncBlockSize() const;
			virtual UOSInt GetDecBlockSize() const;

			virtual UOSInt EncryptBlock(UnsafeArray<const UInt8> inBlock, UnsafeArray<UInt8> outBlock) const = 0; //return outSize
			virtual UOSInt DecryptBlock(UnsafeArray<const UInt8> inBlock, UnsafeArray<UInt8> outBlock) const = 0; //return outSize

			void SetChainMode(ChainMode cm);
			void SetIV(UnsafeArray<const UInt8> iv);
		};

		Text::CStringNN ChainModeGetName(ChainMode cm);
	}
}
#endif
