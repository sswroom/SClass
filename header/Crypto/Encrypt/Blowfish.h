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

			void EncryptInt(EncryptParam *param) const;
			void DecryptInt(EncryptParam *param) const;

			void Init();
			void Key(const UInt8 *password, UOSInt pwdLen);
			void ExpandKey(const UInt8 *salt, const UInt8 *password, UOSInt pwdLen);
		public:
			Blowfish();
			Blowfish(const UInt8 *key, UOSInt keySize);
			virtual ~Blowfish();

			virtual UOSInt EncryptBlock(const UInt8 *inBlock, UInt8 *outBlock, void *encParam) const;
			virtual UOSInt DecryptBlock(const UInt8 *inBlock, UInt8 *outBlock, void *decParam) const;

			void SetKey(const UInt8 *key, UOSInt keySize);
			void EksBlowfishSetup(UInt32 cost, const UInt8 *salt, const UTF8Char *password, UOSInt pwdLen);
			void EncryptBlk(UInt32 *lr);
		};
	}
}
#endif
