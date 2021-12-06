#ifndef _SM_IO_STREAM
#define _SM_IO_STREAM
#include "IO/ParsedObject.h"
#include "Sync/Event.h"

namespace IO
{
	class Stream : public IO::ParsedObject
	{
	public:
		Stream(Text::String *sourceName);
		Stream(const UTF8Char *sourceName);
		virtual ~Stream(){};
		virtual UOSInt Read(UInt8 *buff, UOSInt size) = 0;
		virtual UOSInt Write(const UInt8 *buff, UOSInt size) = 0;

		virtual void *BeginRead(UInt8 *buff, UOSInt size, Sync::Event *evt);
		virtual UOSInt EndRead(void *reqData, Bool toWait, Bool *incomplete);
		virtual void CancelRead(void *reqData);
		virtual void *BeginWrite(const UInt8 *buff, UOSInt size, Sync::Event *evt);
		virtual UOSInt EndWrite(void *reqData, Bool toWait);
		virtual void CancelWrite(void *reqData);

		virtual Int32 Flush() = 0;
		virtual void Close() = 0;
		virtual Bool CanSeek();
		virtual Bool Recover() = 0;

		virtual IO::ParserType GetParserType();
		UInt64 ReadToEnd(IO::Stream *stm, UOSInt buffSize);
	};
}
#endif
