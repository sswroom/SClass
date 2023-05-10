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
			static Bool Decrypt(IO::SeekableStream *srcStream, IO::Stream *destStream, Text::CString password);
			static Bool Encrypt(IO::SeekableStream *srcStream, IO::Stream *destStream, Text::CString password);
		};
	}
}
#endif
