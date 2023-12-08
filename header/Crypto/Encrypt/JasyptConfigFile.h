#ifndef _SM_CRYPTO_ENCRYPT_JASYPTCONFIGFILE
#define _SM_CRYPTO_ENCRYPT_JASYPTCONFIGFILE
#include "Crypto/Encrypt/JasyptEncryptor.h"
#include "Data/FastStringMap.h"
#include "IO/ConfigFile.h"

namespace Crypto
{
	namespace Encrypt
	{
		class JasyptConfigFile : public IO::ConfigFile
		{
		private:
			IO::ConfigFile *cfg;
			Data::FastStringMap<Data::FastStringMap<Text::String *>*> decVals;
			Crypto::Encrypt::JasyptEncryptor enc;

		public:
			JasyptConfigFile(IO::ConfigFile *cfg, Crypto::Encrypt::JasyptEncryptor::KeyAlgorithm keyAlg, Crypto::Encrypt::JasyptEncryptor::CipherAlgorithm cipherAlg, const UInt8 *key, UOSInt keyLen);
			virtual ~JasyptConfigFile();

			virtual Optional<Text::String> GetCateValue(Text::String *category, NotNullPtr<Text::String> name);
			virtual Optional<Text::String> GetCateValue(NotNullPtr<Text::String> category, NotNullPtr<Text::String> name);
			virtual Optional<Text::String> GetCateValue(Text::CStringNN category, Text::CStringNN name);
		};
	}
}
#endif
