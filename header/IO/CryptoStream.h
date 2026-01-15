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
		IntOS encBuffSize;
		UnsafeArray<UInt8> decBuff;
		IntOS decBuffSize;
		UnsafeArrayOpt<UInt8> tmpBuff;
		IntOS tmpBuffSize;


	public:
		CryptoStream(NN<IO::Stream> srcStream, NN<Crypto::Encrypt::Encryption> crypto);
		virtual ~CryptoStream();

		virtual UIntOS Read(const Data::ByteArray &buff);
		virtual UIntOS Write(Data::ByteArrayR buff);

		virtual Int32 Flush();
		virtual void Close();
	};
}
#endif
