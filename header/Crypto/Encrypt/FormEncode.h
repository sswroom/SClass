#ifndef _SM_CRYPTO_ENCRYPT_FORMENCODE
#define _SM_CRYPTO_ENCRYPT_FORMENCODE
#include "Crypto/Encrypt/Encryption.h"

namespace Crypto
{
	namespace Encrypt
	{
		class FormEncode : public Crypto::Encrypt::Encryption
		{
		private:
			static UInt8 URIAllow[256];
		public:
			FormEncode();
			virtual ~FormEncode();

			virtual UOSInt Encrypt(UnsafeArray<const UInt8> inBuff, UOSInt inSize, UnsafeArray<UInt8> outBuff);
			virtual UOSInt Decrypt(UnsafeArray<const UInt8> inBuff, UOSInt inSize, UnsafeArray<UInt8> outBuff);

			virtual UOSInt GetEncBlockSize() const;
			virtual UOSInt GetDecBlockSize() const;
		};
	}
}

#endif
