#ifndef _SM_IO_CRYPTOSTREAM
#define _SM_IO_CRYPTOSTREAM
#include "Crypto/Encrypt/Encryption.h"
#include "IO/Stream.h"

namespace IO
{
	class CryptoStream : public Stream
	{
	protected:
		NN<Crypto::Encrypt::Encryption> crypto;
		Optional<IO::Stream> stm;
		UnsafeArray<UInt8> encBuff;
		OSInt encBuffSize;
		UnsafeArray<UInt8> decBuff;
		OSInt decBuffSize;
		UnsafeArrayOpt<UInt8> tmpBuff;
		OSInt tmpBuffSize;


	public:
		CryptoStream(NN<IO::Stream> srcStream, NN<Crypto::Encrypt::Encryption> crypto);
		virtual ~CryptoStream();

		virtual UOSInt Read(const Data::ByteArray &buff);
		virtual UOSInt Write(Data::ByteArrayR buff);

		virtual Int32 Flush();
		virtual void Close();
	};
}
#endif
