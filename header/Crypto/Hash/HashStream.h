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
			IO::Stream *srcStm;

		public:
			HashStream(IO::Stream *srcStm, Crypto::Hash::IHash *hash);
			virtual ~HashStream();

			virtual UOSInt Read(UInt8 *buff, UOSInt size);
			virtual UOSInt Write(const UInt8 *buff, UOSInt size);

			virtual Int32 Flush();
			virtual void Close();
			virtual Bool Recover();
		};
	}
}
#endif
