#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Stream.h"

IO::Stream::Stream(const UTF8Char *sourceName) : IO::ParsedObject(sourceName)
{
}

void *IO::Stream::BeginRead(UInt8 *buff, UOSInt size, Sync::Event *evt)
{
	UOSInt retVal = Read(buff, size);
	evt->Set();
	return (void*)retVal;
}

UOSInt IO::Stream::EndRead(void *reqData, Bool toWait)
{
	return (OSInt)reqData;
}

void IO::Stream::CancelRead(void *reqData)
{
}

void *IO::Stream::BeginWrite(const UInt8 *buff, UOSInt size, Sync::Event *evt)
{
	OSInt retVal = Write(buff, size);
	evt->Set();
	return (void*)retVal;
}

UOSInt IO::Stream::EndWrite(void *reqData, Bool toWait)
{
	return (OSInt)reqData;
}

void IO::Stream::CancelWrite(void *reqData)
{
}

Bool IO::Stream::CanSeek()
{
	return false;
}

IO::ParsedObject::ParserType IO::Stream::GetParserType()
{
	return IO::ParsedObject::PT_STREAM;
}
