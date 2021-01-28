#ifndef _SM_IO_CRYPTOSTREAM
#define _SM_IO_CRYPTOSTREAM
#include "Crypto/Encrypt/ICrypto.h"
#include "IO/Stream.h"

namespace IO
{
	class CryptoStream : public Stream
	{
	protected:
		Crypto::Encrypt::ICrypto *crypto;
		IO::Stream *stm;
		void *encParam;
		UInt8 *encBuff;
		OSInt encBuffSize;
		UInt8 *decBuff;
		OSInt decBuffSize;
		UInt8 *tmpBuff;
		OSInt tmpBuffSize;


	public:
		CryptoStream(IO::Stream *srcStream, Crypto::Encrypt::ICrypto *crypto, void *encParam);
		virtual ~CryptoStream();

		virtual OSInt Read(UInt8 *buff, OSInt size);
		virtual OSInt Write(const UInt8 *buff, OSInt size);

		virtual Int32 Flush();
		virtual void Close();
	};
}
#endif
