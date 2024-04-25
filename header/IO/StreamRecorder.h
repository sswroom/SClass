#ifndef _SM_IO_STREAMRECORDER
#define _SM_IO_STREAMRECORDER
#include "AnyType.h"
#include "IO/FileStream.h"

namespace IO
{
	class StreamRecorder
	{
	private:
		IO::FileStream destStm;
		UInt64 recordedLength;

		static void __stdcall DataHdlr(const UInt8 *buff, UOSInt buffSize, AnyType userData);
	public:
		StreamRecorder(Text::CStringNN destFile);
		~StreamRecorder();

		Bool AppendStream(NN<IO::Stream> stm);
		UInt64 GetRecordedLength();
	};
}
#endif
