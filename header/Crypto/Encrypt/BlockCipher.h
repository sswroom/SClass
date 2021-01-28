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
			OSInt blockSize;
			ChainMode cm;

		public:
			BlockCipher(OSInt blockSize);
			virtual ~BlockCipher();

			virtual OSInt Encrypt(const UInt8 *inBuff, OSInt inSize, UInt8 *outBuff, void *encParam);
			virtual OSInt Decrypt(const UInt8 *inBuff, OSInt inSize, UInt8 *outBuff, void *decParam);
			virtual OSInt GetEncBlockSize();
			virtual OSInt GetDecBlockSize();

			virtual OSInt EncryptBlock(const UInt8 *inBlock, UInt8 *outBlock, void *encParam) = 0;
			virtual OSInt DecryptBlock(const UInt8 *inBlock, UInt8 *outBlock, void *decParam) = 0;

			void SetChainMode(ChainMode cm);
			void SetIV(UInt8 *iv);
		};
	}
};
#endif
