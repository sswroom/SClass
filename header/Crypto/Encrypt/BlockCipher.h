#ifndef _SM_CRYPTO_ENCRYPT_BLOCKCIPHER
#define _SM_CRYPTO_ENCRYPT_BLOCKCIPHER
#include "Crypto/Encrypt/ICrypto.h"

namespace Crypto
{
	namespace Encrypt
	{
		class BlockCipher : public ICrypto
		{
		public:
			typedef enum
			{
				CM_ECB,
				CM_CBC,
				CM_PCBC,
				CM_CFB,
				CM_OFB
			} ChainMode;

		protected:
			UInt8 *iv;
			UOSInt blockSize;
			ChainMode cm;

		public:
			BlockCipher(UOSInt blockSize);
			virtual ~BlockCipher();

			virtual UOSInt Encrypt(const UInt8 *inBuff, UOSInt inSize, UInt8 *outBuff, void *encParam);
			virtual UOSInt Decrypt(const UInt8 *inBuff, UOSInt inSize, UInt8 *outBuff, void *decParam);
			virtual UOSInt GetEncBlockSize();
			virtual UOSInt GetDecBlockSize();

			virtual OSInt EncryptBlock(const UInt8 *inBlock, UInt8 *outBlock, void *encParam) = 0;
			virtual OSInt DecryptBlock(const UInt8 *inBlock, UInt8 *outBlock, void *decParam) = 0;

			void SetChainMode(ChainMode cm);
			void SetIV(const UInt8 *iv);
		};
	}
}
#endif
