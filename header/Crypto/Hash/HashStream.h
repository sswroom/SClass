#ifndef _SM_CRYPTO_HASH_HASHSTREAM
#define _SM_CRYPTO_HASH_HASHSTREAM
#include "Crypto/Hash/IHash.h"
#include "IO/Stream.h"

namespace Crypto
{
	namespace Hash
	{
		class HashStream : public IO::Stream
		{
		private:
			Crypto::Hash::IHash *hash;
			NotNullPtr<IO::Stream> srcStm;

		public:
			HashStream(NotNullPtr<IO::Stream> srcStm, Crypto::Hash::IHash *hash);
			virtual ~HashStream();

			virtual Bool IsDown() const;
			virtual UOSInt Read(const Data::ByteArray &buff);
			virtual UOSInt Write(const UInt8 *buff, UOSInt size);

			virtual Int32 Flush();
			virtual void Close();
			virtual Bool Recover();
			virtual IO::StreamType GetStreamType() const;
		};
	}
}
#endif
