#ifndef _SM_IO_STREAMRECORDER
#define _SM_IO_STREAMRECORDER
#include "IO/FileStream.h"

namespace IO
{
	class StreamRecorder
	{
	private:
		IO::FileStream *destStm;
		Int64 recordedLength;

		static void __stdcall DataHdlr(const UInt8 *buff, OSInt buffSize, void *userData);
	public:
		StreamRecorder(const UTF8Char *destFile);
		~StreamRecorder();

		Bool AppendStream(IO::Stream *stm);
		Int64 GetRecordedLength();
	};
};
#endif
