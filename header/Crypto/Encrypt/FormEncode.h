#ifndef _SM_CRYPTO_ENCRYPT_FORMENCODE
#define _SM_CRYPTO_ENCRYPT_FORMENCODE
#include "Crypto/Encrypt/ICrypto.h"

namespace Crypto
{
	namespace Encrypt
	{
		class FormEncode : public Crypto::Encrypt::ICrypto
		{
		private:
			static UInt8 URIAllow[256];
		public:
			FormEncode();
			virtual ~FormEncode();

			virtual UOSInt Encrypt(const UInt8 *inBuff, UOSInt inSize, UInt8 *outBuff, void *encParam);
			virtual UOSInt Decrypt(const UInt8 *inBuff, UOSInt inSize, UInt8 *outBuff, void *decParam);

			virtual UOSInt GetEncBlockSize() const;
			virtual UOSInt GetDecBlockSize() const;
		};
	};
};

#endif
