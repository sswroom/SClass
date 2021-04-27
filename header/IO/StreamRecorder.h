#ifndef _SM_IO_STREAMRECORDER
#define _SM_IO_STREAMRECORDER
#include "IO/FileStream.h"

namespace IO
{
	class StreamRecorder
	{
	private:
		IO::FileStream *destStm;
		UInt64 recordedLength;

		static void __stdcall DataHdlr(const UInt8 *buff, UOSInt buffSize, void *userData);
	public:
		StreamRecorder(const UTF8Char *destFile);
		~StreamRecorder();

		Bool AppendStream(IO::Stream *stm);
		UInt64 GetRecordedLength();
	};
}
#endif
