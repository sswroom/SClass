#ifndef _SM_CRYPTO_ENCRYPT_RNCRYPTOR
#define _SM_CRYPTO_ENCRYPT_RNCRYPTOR
#include "IO/SeekableStream.h"

namespace Crypto
{
	namespace Encrypt
	{
		class RNCryptor
		{
		private:
			static UOSInt RemovePadding(UInt8 *buff, UOSInt buffSize);
		public:
			static Bool Decrypt(NotNullPtr<IO::SeekableStream> srcStream, NotNullPtr<IO::Stream> destStream, Text::CStringNN password);
			static Bool Encrypt(NotNullPtr<IO::SeekableStream> srcStream, NotNullPtr<IO::Stream> destStream, Text::CStringNN password);
		};
	}
}
#endif
