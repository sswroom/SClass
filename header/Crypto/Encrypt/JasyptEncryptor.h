#ifndef _SM_CRYPTO_ENCRYPT_JASYPTENCRYPTOR
#define _SM_CRYPTO_ENCRYPT_JASYPTENCRYPTOR
#include "Crypto/Encrypt/Encryption.h"
#include "Crypto/Hash/HashAlgorithm.h"
#include "Data/ByteBuffer.h"
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
			Data::ByteBuffer key;
			UnsafeArrayOpt<UInt8> salt;
			UOSInt saltSize;
			UnsafeArrayOpt<UInt8> iv;
			UOSInt ivSize;
			UOSInt iterCnt;
			UOSInt dkLen;
			Data::RandomBytesGenerator random;

			UnsafeArray<const UInt8> DecGetSalt(UnsafeArray<const UInt8> buff, UnsafeArray<UInt8> salt);
			UnsafeArray<const UInt8> DecGetIV(UnsafeArray<const UInt8> buff, UnsafeArray<UInt8> iv);
			UOSInt GetEncKey(UnsafeArray<const UInt8> salt, UnsafeArray<UInt8> key);
			NN<Crypto::Encrypt::Encryption> CreateCrypto(UnsafeArray<const UInt8> iv, UnsafeArray<const UInt8> keyBuff);

		public:
			JasyptEncryptor(KeyAlgorithm keyAlg, CipherAlgorithm cipherAlg, Data::ByteArrayR key);
			~JasyptEncryptor();

			Bool Decrypt(NN<IO::ConfigFile> cfg);
			UOSInt Decrypt(UnsafeArray<const UInt8> srcBuff, UOSInt srcLen, UnsafeArray<UInt8> outBuff);
			UOSInt DecryptB64(Text::CStringNN b64Str, UnsafeArray<UInt8> outBuff);
			UOSInt EncryptAsB64(NN<Text::StringBuilderUTF8> sb, Data::ByteArrayR srcBuff);

			static Text::CStringNN KeyAlgorithmGetName(KeyAlgorithm keyAlg);
			static Text::CStringNN CipherAlgorithmGetName(CipherAlgorithm cipherAlg);
		};
	}
}
#endif
