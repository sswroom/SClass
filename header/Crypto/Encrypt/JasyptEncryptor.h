#ifndef _SM_CRYPTO_ENCRYPT_JASYPTENCRYPTOR
#define _SM_CRYPTO_ENCRYPT_JASYPTENCRYPTOR
#include "Crypto/Encrypt/ICrypto.h"
#include "Crypto/Hash/IHash.h"
#include "Data/RandomBytesGenerator.h"
#include "IO/ConfigFile.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace Crypto
{
	namespace Encrypt
	{
		class JasyptEncryptor
		{
		public:
			enum KeyAlgorithm
			{
				KA_PBEWITHHMACSHA512,

				KA_FIRST = KA_PBEWITHHMACSHA512,
				KA_LAST = KA_PBEWITHHMACSHA512
			};

			enum CipherAlgorithm
			{
				CA_AES256,

				CA_FIRST = CA_AES256,
				CA_LAST = CA_AES256
			};

		private:
			KeyAlgorithm keyAlgorithmn;
			CipherAlgorithm cipherAlgorithm;
			UInt8 *key;
			UOSInt keyLen;
			UInt8 *salt;
			UOSInt saltSize;
			UInt8 *iv;
			UOSInt ivSize;
			UOSInt iterCnt;
			UOSInt dkLen;
			Data::RandomBytesGenerator *random;

			const UInt8 *DecGetSalt(const UInt8 *buff, UInt8 *salt);
			const UInt8 *DecGetIV(const UInt8 *buff, UInt8 *iv);
			UOSInt GetEncKey(const UInt8 *salt, UInt8 *key);
			Crypto::Encrypt::ICrypto *CreateCrypto(const UInt8 *iv, const UInt8 *keyBuff);

		public:
			JasyptEncryptor(KeyAlgorithm keyAlg, CipherAlgorithm cipherAlg, const UInt8 *key, UOSInt keyLen);
			~JasyptEncryptor();

			Bool Decrypt(IO::ConfigFile *cfg);
			UOSInt Decrypt(const UInt8 *srcBuff, UOSInt srcLen, UInt8 *outBuff);
			UOSInt DecryptB64(const UTF8Char *b64Buff, UOSInt b64Len, UInt8 *outBuff);
			UOSInt DecryptB64(const UTF8Char *b64Buff, UInt8 *outBuff);
			UOSInt EncryptAsB64(Text::StringBuilderUTF8 *sb, const UInt8 *srcBuff, UOSInt srcLen);

			static Text::CString GetKeyAlgorithmName(KeyAlgorithm keyAlg);
			static Text::CString GetCipherAlgorithmName(CipherAlgorithm cipherAlg);
		};
	}
}
#endif