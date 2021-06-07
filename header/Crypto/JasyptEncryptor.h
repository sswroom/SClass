#ifndef _SM_CRYPTO_JASYPTENCRYPTOR
#define _SM_CRYPTO_JASYPTENCRYPTOR
#include "Crypto/Encrypt/ICrypto.h"
#include "Crypto/Hash/IHash.h"

namespace Crypto
{
	class JasyptEncryptor
	{
	public:
		typedef enum
		{
			KA_PBEWITHHMACSHA512
		} KeyAlgorithm;

		typedef enum
		{
			CA_AES256
		} CipherAlgorithm;

	private:
		KeyAlgorithm keyAlgorithmn;
		CipherAlgorithm cipherAlgorithm;
		UInt8 *key;
		UOSInt keyLen;
		UOSInt saltSize;
		UOSInt ivSize;
		UOSInt iterCnt;
		UOSInt dkLen;

		const UInt8 *DecGetSalt(const UInt8 *buff, UInt8 *salt);
		const UInt8 *DecGetIV(const UInt8 *buff, UInt8 *iv);
		UOSInt GetEncKey(const UInt8 *salt, UInt8 *key);
		Crypto::Encrypt::ICrypto *CreateCrypto(const UInt8 *iv, const UInt8 *keyBuff);

	public:
		JasyptEncryptor(KeyAlgorithm keyAlg, CipherAlgorithm cipherAlg, const UInt8 *key, UOSInt keyLen);
		~JasyptEncryptor();

		UOSInt Decrypt(const UInt8 *srcBuff, UOSInt srcLen, UInt8 *outBuff);
		UOSInt DecryptB64(const UTF8Char *b64Buff, UOSInt b64Len, UInt8 *outBuff);
		UOSInt DecryptB64(const UTF8Char *b64Buff, UInt8 *outBuff);
	};
}
#endif
