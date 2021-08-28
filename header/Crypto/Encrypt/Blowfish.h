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
			static const UInt32 origP[18];
			static const UInt32 origS[1024];

			UInt32 p[16 + 2];
			UInt32 s[4][256];
			UInt32 xl;
			UInt32 xr;

			void EncryptInt();
			void DecryptInt();
			UInt32 F(UInt32 x);
		public:
			Blowfish(const UInt8 *key, UOSInt keySize);
			virtual ~Blowfish();

			virtual UOSInt EncryptBlock(const UInt8 *inBlock, UInt8 *outBlock, void *encParam);
			virtual UOSInt DecryptBlock(const UInt8 *inBlock, UInt8 *outBlock, void *decParam);

			void SetKey(const UInt8 *key, UOSInt keySize);
		};
	}
}
#endif
