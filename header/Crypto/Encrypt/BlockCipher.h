#ifndef _SM_CRYPTO_ENCRYPT_BLOCKCIPHER
#define _SM_CRYPTO_ENCRYPT_BLOCKCIPHER
#include "Crypto/Encrypt/ICrypto.h"

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

			virtual UOSInt Encrypt(const UInt8 *inBuff, UOSInt inSize, UInt8 *outBuff, void *encParam);
			virtual UOSInt Decrypt(const UInt8 *inBuff, UOSInt inSize, UInt8 *outBuff, void *decParam);
			virtual UOSInt GetEncBlockSize() const;
			virtual UOSInt GetDecBlockSize() const;

			virtual UOSInt EncryptBlock(const UInt8 *inBlock, UInt8 *outBlock, void *encParam) const = 0; //return outSize
			virtual UOSInt DecryptBlock(const UInt8 *inBlock, UInt8 *outBlock, void *decParam) const = 0; //return outSize

			void SetChainMode(ChainMode cm);
			void SetIV(const UInt8 *iv);
		};
	}
}
#endif
