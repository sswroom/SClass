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

			virtual UIntOS Encrypt(UnsafeArray<const UInt8> inBuff, UIntOS inSize, UnsafeArray<UInt8> outBuff);
			virtual UIntOS Decrypt(UnsafeArray<const UInt8> inBuff, UIntOS inSize, UnsafeArray<UInt8> outBuff);

			virtual UIntOS GetEncBlockSize() const;
			virtual UIntOS GetDecBlockSize() const;
		};
	}
}

#endif
