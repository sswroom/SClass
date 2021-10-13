#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/ActiveStreamReader.h"
#include "IO/StreamRecorder.h"

void __stdcall IO::StreamRecorder::DataHdlr(const UInt8 *buff, UOSInt buffSize, void *userData)
{
	IO::StreamRecorder *me = (IO::StreamRecorder *)userData;
	me->recordedLength += me->destStm->Write(buff, buffSize);
}

IO::StreamRecorder::StreamRecorder(const UTF8Char *destFile)
{
	this->recordedLength = 0;
	NEW_CLASS(destStm, IO::FileStream(destFile, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
}

IO::StreamRecorder::~StreamRecorder()
{
	DEL_CLASS(destStm);
}

Bool IO::StreamRecorder::AppendStream(IO::Stream *stm)
{
	if (destStm->IsError())
		return false;
	IO::ActiveStreamReader *reader;
	NEW_CLASS(reader, IO::ActiveStreamReader(DataHdlr, this, stm, 1048576));
	reader->ReadStream(0);
	DEL_CLASS(reader);
	return true;
}

UInt64 IO::StreamRecorder::GetRecordedLength()
{
	return this->recordedLength;
}
