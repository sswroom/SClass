#ifndef _SM_CRYPTO_HASH_HASHSTREAM
#define _SM_CRYPTO_HASH_HASHSTREAM
#include "Crypto/Hash/HashAlgorithm.h"
#include "IO/Stream.h"

namespace Crypto
{
	namespace Hash
	{
		class HashStream : public IO::Stream
		{
		private:
			NN<Crypto::Hash::HashAlgorithm> hash;
			NN<IO::Stream> srcStm;

		public:
			HashStream(NN<IO::Stream> srcStm, NN<Crypto::Hash::HashAlgorithm> hash);
			virtual ~HashStream();

			virtual Bool IsDown() const;
			virtual UOSInt Read(const Data::ByteArray &buff);
			virtual UOSInt Write(Data::ByteArrayR buff);

			virtual Int32 Flush();
			virtual void Close();
			virtual Bool Recover();
			virtual IO::StreamType GetStreamType() const;
		};
	}
}
#endif
