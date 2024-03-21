#ifndef _SM_CRYPTO_ENCRYPT_ICRYPTO
#define _SM_CRYPTO_ENCRYPT_ICRYPTO

namespace Crypto
{
	namespace Encrypt
	{
		class ICrypto
		{
		public:
			virtual ~ICrypto() {};

			virtual UOSInt Encrypt(const UInt8 *inBuff, UOSInt inSize, UInt8 *outBuff) = 0; //outBuff = null to get the size
			virtual UOSInt Decrypt(const UInt8 *inBuff, UOSInt inSize, UInt8 *outBuff) = 0; //outBuff = null to get the size

			virtual UOSInt GetEncBlockSize() const = 0;
			virtual UOSInt GetDecBlockSize() const = 0;
		};
	}
}
#endif
