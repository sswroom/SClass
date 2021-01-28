#include "stdafx.h"
#include "MyMemory.h"
#include "Sync/Event.h"
#include "IO/Stream.h"
#include "IO/StreamTunnel.h"
#include <stdlib.h>

IO::StreamTunnel::StreamTunnel(IO::SeekableStream *sourceStm, IO::SeekableStream *readCapture, IO::SeekableStream *writeCapture)
{
	this->sourceStm = sourceStm;
	this->readCapture = readCapture;
	this->writeCapture = writeCapture;
}

IO::StreamTunnel::~StreamTunnel()
{
}

Int32 IO::StreamTunnel::Read(UInt8 *buff, Int32 size)
{
	Int32 retVal = this->sourceStm->Read(buff, size);
	if (retVal > 0)
	{
		this->readCapture->Write(buff, retVal);
	}
	return retVal;
}

Int32 IO::StreamTunnel::Write(const UInt8 *buff, Int32 size)
{
	writeCapture->Write(buff, size);
	return this->sourceStm->Write(buff, size);
}

void *IO::StreamTunnel::BeginRead(UInt8 *buff, Int32 size, Sync::Event *evt)
{
	Int32 *reqData = MemAlloc(Int32, 3);
	reqData[0] = (Int32)buff;
	reqData[1] = (Int32)size;
	reqData[2] = (Int32)this->sourceStm->BeginRead(buff, size, evt);
	return reqData;
}

Int32 IO::StreamTunnel::EndRead(void *reqData, Bool toWait)
{
	Int32 *data = (Int32*)reqData;
	Int32 retVal = this->sourceStm->EndRead((void*)data[2], toWait);
	if (retVal > 0)
	{
		this->readCapture->Write((UInt8*)data[0], retVal);
	}
	MemFree(data);
	return retVal;
}

void IO::StreamTunnel::CancelRead(void *reqData)
{
	Int32 *data = (Int32*)reqData;
	this->sourceStm->CancelRead((void*)data[2]);
	MemFree(data);
}

void *IO::StreamTunnel::BeginWrite(const UInt8 *buff, Int32 size, Sync::Event *evt)
{
	this->writeCapture->Write(buff, size);
	return this->sourceStm->BeginWrite(buff, size, evt);
}

Int32 IO::StreamTunnel::EndWrite(void *reqData, Bool toWait)
{
	return this->sourceStm->EndWrite(reqData, toWait);
}

void IO::StreamTunnel::CancelWrite(void *reqData)
{
	this->sourceStm->CancelWrite(reqData);
}

Int32 IO::StreamTunnel::Flush()
{
	return this->sourceStm->Flush();
}

void IO::StreamTunnel::Close()
{
	this->sourceStm->Close();
}

Int64 IO::StreamTunnel::Seek(IO::SeekableStream::SeekType origin, Int64 position)
{
	return this->sourceStm->Seek(origin, position);
}

Int64 IO::StreamTunnel::GetPosition()
{
	return this->sourceStm->GetPosition();
}
