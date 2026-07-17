#ifndef _SM_CRYPTO_ENCRYPT_BLOCKCIPHER
#define _SM_CRYPTO_ENCRYPT_BLOCKCIPHER
#include "SIMD.h"
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
			UIntOS blockSize;
			UnsafeArray<UInt8> iv;
			UIntOS ivSize;
			ChainMode cm;
			PaddingMode pad;
			UnsafeArrayOpt<UInt8> aad;
			UIntOS aadSize;

		public:
			BlockCipher(UIntOS blockSize);
			BlockCipher(UIntOS blockSize, UIntOS ivSize);
			virtual ~BlockCipher();

			virtual UIntOS Encrypt(UnsafeArray<const UInt8> inBuff, UIntOS inSize, UnsafeArray<UInt8> outBuff);
			virtual UIntOS Decrypt(UnsafeArray<const UInt8> inBuff, UIntOS inSize, UnsafeArray<UInt8> outBuff);
			virtual UIntOS GetEncBlockSize() const;
			virtual UIntOS GetDecBlockSize() const;
			UIntOS GetIVSize() const;

			virtual UIntOS EncryptBlock(UnsafeArray<const UInt8> inBlock, UnsafeArray<UInt8> outBlock) const = 0; //return outSize
			virtual UIntOS DecryptBlock(UnsafeArray<const UInt8> inBlock, UnsafeArray<UInt8> outBlock) const = 0; //return outSize

			void SetChainMode(ChainMode cm);
			void SetIV(UnsafeArray<const UInt8> iv);
			void SetPaddingMode(PaddingMode pad);
			void SetAAD(UnsafeArray<const UInt8> aad, UIntOS aadSize);

			static void GFMult(UnsafeArray<const UInt8> a, UnsafeArray<const UInt8> b, UnsafeArray<UInt8> c);
			static void GhashUpdateBlock(UnsafeArray<UInt8> gcmBlk, UnsafeArray<const UInt8> inBuff);
			static void GhashUpdateBlockPadding(UnsafeArray<UInt8> gcmBlk, UnsafeArray<const UInt8> inBuff, UIntOS inSize);
			static void GhashUpdate(UnsafeArray<UInt8> gcmBlk, UnsafeArray<const UInt8> inBuff, UIntOS inSize);

			static UInt8x16 GFMultSIMD(UInt8x16 a, UInt8x16 b);
			static UInt8x16 GhashUpdateBlockSIMD(UInt8x16 hash, UInt8x16 hKey, UnsafeArray<const UInt8> inBuff);
			static UInt8x16 GhashUpdateBlockPaddingSIMD(UInt8x16 hash, UInt8x16 hKey, UnsafeArray<const UInt8> inBuff, UIntOS inSize);
			static UInt8x16 GhashUpdateSIMD(UInt8x16 hash, UInt8x16 hKey, UnsafeArray<const UInt8> inBuff, UIntOS inSize);
		};

		Text::CStringNN ChainModeGetName(ChainMode cm);
		Text::CStringNN PaddingModeGetName(PaddingMode pad);
	}
}
#endif
