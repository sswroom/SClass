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
			ClassData *clsData;
		public:
			RSAEnc();
			~RSAEnc();

			Bool SetPublicKey(Crypto::Cert::X509PubKey *key);
			Bool LoadPublicKeyFile(Text::CString filePath);

			UnsafeArrayOpt<const UInt8> GetSessKey();
			UOSInt GetSessKeyLen();
			UnsafeArrayOpt<const UInt8> GetIV();
			UOSInt GetIVLen();


		};
	}
}
#endif
