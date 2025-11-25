#ifndef _SM_CRYPTO_ENCRYPT_JASYPTCONFIGFILE
#define _SM_CRYPTO_ENCRYPT_JASYPTCONFIGFILE
#include "Crypto/Encrypt/JasyptEncryptor.h"
#include "Data/FastStringMapNN.hpp"
#include "IO/ConfigFile.h"

namespace Crypto
{
	namespace Encrypt
	{
		class JasyptConfigFile : public IO::ConfigFile
		{
		private:
			NN<IO::ConfigFile> cfg;
			Data::FastStringMapNN<Data::FastStringMapNN<Text::String>> decVals;
			Crypto::Encrypt::JasyptEncryptor enc;

		public:
			JasyptConfigFile(NN<IO::ConfigFile> cfg, Crypto::Encrypt::JasyptEncryptor::KeyAlgorithm keyAlg, Crypto::Encrypt::JasyptEncryptor::CipherAlgorithm cipherAlg, Data::ByteArrayR key);
			virtual ~JasyptConfigFile();

			virtual Optional<Text::String> GetCateValue(NN<Text::String> category, NN<Text::String> name);
			virtual Optional<Text::String> GetCateValue(Text::CStringNN category, Text::CStringNN name);
		};
	}
}
#endif
