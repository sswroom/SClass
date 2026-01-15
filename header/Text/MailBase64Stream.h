#ifndef _SM_TEXT_MAILBASE64STREAM
#define _SM_TEXT_MAILBASE64STREAM
#include "Crypto/Encrypt/Base64.h"
#include "IO/Stream.h"

namespace Text
{
	class MailBase64Stream : public IO::Stream
	{
	private:
		UInt8 lineBuff[54];
		UIntOS lineBuffSize;
		NN<IO::Stream> stm;
		UIntOS lineCnt;
		Crypto::Encrypt::Base64 b64;

	public:
		MailBase64Stream(NN<IO::Stream> stm);
		virtual ~MailBase64Stream();

		virtual Bool IsDown() const;
		virtual UIntOS Read(const Data::ByteArray &buff);
		virtual UIntOS Write(Data::ByteArrayR buff);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();

		UIntOS GetWriteCount();
	};
}
#endif
