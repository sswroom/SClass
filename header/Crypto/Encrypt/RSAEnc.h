#ifndef _SM_CRYPTO_ENCRYPT_RSAENC
#define _SM_CRYPTO_ENCRYPT_RSAENC
#include "Crypto/Cert/X509PubKey.h"

namespace Crypto
{
	namespace Encrypt
	{
		class RSAEnc
		{
		private:
			struct ClassData;
			NN<ClassData> clsData;
		public:
			RSAEnc();
			~RSAEnc();

			Bool SetPublicKey(NN<Crypto::Cert::X509PubKey> key);
			Bool LoadPublicKeyFile(Text::CString filePath);

			UnsafeArrayOpt<const UInt8> GetSessKey();
			UIntOS GetSessKeyLen();
			UnsafeArrayOpt<const UInt8> GetIV();
			UIntOS GetIVLen();


		};
	}
}
#endif
