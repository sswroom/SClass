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

			virtual OSInt Encrypt(const UInt8 *inBuff, OSInt inSize, UInt8 *outBuff, void *encParam);
			virtual OSInt Decrypt(const UInt8 *inBuff, OSInt inSize, UInt8 *outBuff, void *decParam);

			virtual OSInt GetEncBlockSize();
			virtual OSInt GetDecBlockSize();
		};
	};
};

#endif
