#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/ActiveStreamReader.h"
#include "IO/StreamRecorder.h"

void __stdcall IO::StreamRecorder::DataHdlr(const UInt8 *buff, UOSInt buffSize, void *userData)
{
	IO::StreamRecorder *me = (IO::StreamRecorder *)userData;
	me->recordedLength += me->destStm.Write(buff, buffSize);
}

IO::StreamRecorder::StreamRecorder(Text::CStringNN destFile) : destStm(destFile, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal)
{
	this->recordedLength = 0;
}

IO::StreamRecorder::~StreamRecorder()
{
}

Bool IO::StreamRecorder::AppendStream(NotNullPtr<IO::Stream> stm)
{
	if (destStm.IsError())
		return false;
	IO::ActiveStreamReader reader(DataHdlr, this, 1048576);
	reader.ReadStream(stm, 0);
	return true;
}

UInt64 IO::StreamRecorder::GetRecordedLength()
{
	return this->recordedLength;
}
