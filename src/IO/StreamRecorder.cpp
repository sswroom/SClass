#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/ActiveStreamReader.h"
#include "IO/StreamRecorder.h"

void __stdcall IO::StreamRecorder::DataHdlr(const UInt8 *buff, OSInt buffSize, void *userData)
{
	IO::StreamRecorder *me = (IO::StreamRecorder *)userData;
	me->recordedLength += me->destStm->Write(buff, buffSize);
}

IO::StreamRecorder::StreamRecorder(const UTF8Char *destFile)
{
	this->recordedLength = 0;
	NEW_CLASS(destStm, IO::FileStream(destFile, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
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

Int64 IO::StreamRecorder::GetRecordedLength()
{
	return this->recordedLength;
}
