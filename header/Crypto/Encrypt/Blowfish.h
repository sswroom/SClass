#ifndef _SM_CRYPTO_ENCRYPT_BLOWFISH
#define _SM_CRYPTO_ENCRYPT_BLOWFISH
#include "Crypto/Encrypt/BlockCipher.h"

namespace Crypto
{
	namespace Encrypt
	{
		class Blowfish : public Crypto::Encrypt::BlockCipher
		{
		private:
			struct EncryptParam
			{
				UInt32 xl;
				UInt32 xr;
			};
		private:
			static const UInt32 origP[18];
			static const UInt32 origS[1024];

			UInt32 p[16 + 2];
			UInt32 s[4][256];

			void EncryptInt(NN<EncryptParam> param) const;
			void DecryptInt(NN<EncryptParam> param) const;

			void InitPassword(UnsafeArray<const UInt8> password, UIntOS pwdLen);
			void Init();
			void Key(UnsafeArray<const UInt8> password, UIntOS pwdLen);
			void ExpandKey(UnsafeArrayOpt<const UInt8> salt, UnsafeArray<const UInt8> password, UIntOS pwdLen);
		public:
			Blowfish();
			Blowfish(UnsafeArray<const UInt8> key, UIntOS keySize);
			virtual ~Blowfish();

			virtual UIntOS EncryptBlock(UnsafeArray<const UInt8> inBlock, UnsafeArray<UInt8> outBlock) const;
			virtual UIntOS DecryptBlock(UnsafeArray<const UInt8> inBlock, UnsafeArray<UInt8> outBlock) const;

			void SetKey(UnsafeArray<const UInt8> key, UIntOS keySize);
			void EksBlowfishSetup(UInt32 cost, UnsafeArray<const UInt8> salt, Text::CStringNN password);
			void EncryptBlk(UnsafeArray<UInt32> lr);
		};
	}
}
#endif
