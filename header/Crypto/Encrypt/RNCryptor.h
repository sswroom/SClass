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
			static UIntOS RemovePadding(UnsafeArray<UInt8> buff, UIntOS buffSize);
		public:
			static Bool Decrypt(NN<IO::SeekableStream> srcStream, NN<IO::Stream> destStream, Text::CStringNN password);
			static Bool Encrypt(NN<IO::SeekableStream> srcStream, NN<IO::Stream> destStream, Text::CStringNN password);
		};
	}
}
#endif
