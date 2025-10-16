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
		UOSInt lineBuffSize;
		NN<IO::Stream> stm;
		UOSInt lineCnt;
		Crypto::Encrypt::Base64 b64;

	public:
		MailBase64Stream(NN<IO::Stream> stm);
		virtual ~MailBase64Stream();

		virtual Bool IsDown() const;
		virtual UOSInt Read(const Data::ByteArray &buff);
		virtual UOSInt Write(Data::ByteArrayR buff);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();

		UOSInt GetWriteCount();
	};
}
#endif
