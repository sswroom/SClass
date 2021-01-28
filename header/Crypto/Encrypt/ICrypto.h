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

			virtual OSInt Encrypt(const UInt8 *inBuff, OSInt inSize, UInt8 *outBuff, void *encParam) = 0; //outBuff = null to get the size
			virtual OSInt Decrypt(const UInt8 *inBuff, OSInt inSize, UInt8 *outBuff, void *decParam) = 0; //outBuff = null to get the size

			virtual OSInt GetEncBlockSize() = 0;
			virtual OSInt GetDecBlockSize() = 0;
		};
	}
}
#endif
