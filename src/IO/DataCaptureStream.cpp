#include "Stdafx.h"
#include "IO/DataCaptureStream.h"

IO::DataCaptureStream::DataCaptureStream(IO::Stream *srcStream, DataHandler recvHdlr, DataHandler sendHdlr, void *hdlrObj) : Stream(srcStream->GetSourceNameObj())
{
	this->stm = srcStream;
	this->recvHdlr = recvHdlr;
	this->sendHdlr = sendHdlr;
	this->hdlrObj = hdlrObj;
}

IO::DataCaptureStream::~DataCaptureStream()
{
}

Bool IO::DataCaptureStream::IsDown() const
{
	return this->stm->IsDown();
}

UOSInt IO::DataCaptureStream::Read(UInt8 *buff, UOSInt size)
{
	UOSInt ret = this->stm->Read(buff, size);
	if (ret > 0)
	{
		this->recvHdlr(this->hdlrObj, buff, ret);
	}
	return ret;
}

UOSInt IO::DataCaptureStream::Write(const UInt8 *buff, UOSInt size)
{
	UOSInt ret = this->stm->Write(buff, size);
	if (ret > 0)
	{
		this->sendHdlr(this->hdlrObj, buff, ret);
	}
	return ret;
}

Int32 IO::DataCaptureStream::Flush()
{
	return this->Flush();
}

void IO::DataCaptureStream::Close()
{
	this->stm->Close();
}

Bool IO::DataCaptureStream::Recover()
{
	return this->stm->Recover();
}

IO::StreamType IO::DataCaptureStream::GetStreamType() const
{
	return StreamType::DataCapture;
}