#ifndef _SM_CRYPTO_ENCRYPT_BLOCKCIPHER
#define _SM_CRYPTO_ENCRYPT_BLOCKCIPHER
#include "Crypto/Encrypt/Encryption.h"
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
			OFB,
			CTR,
			GCM
		};

		enum class PaddingMode
		{
			None,
			PKCS7
		};

		class BlockCipher : public Encryption
		{
		protected:
			UnsafeArray<UInt8> iv;
			UOSInt blockSize;
			ChainMode cm;
			PaddingMode pad;

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
			void SetPaddingMode(PaddingMode pad);
		};

		Text::CStringNN ChainModeGetName(ChainMode cm);
		Text::CStringNN PaddingModeGetName(PaddingMode pad);
	}
}
#endif
