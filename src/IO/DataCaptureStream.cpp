#include "Stdafx.h"
#include "IO/DataCaptureStream.h"

IO::DataCaptureStream::DataCaptureStream(NN<IO::Stream> srcStream, DataHandler recvHdlr, DataHandler sendHdlr, AnyType hdlrObj) : Stream(srcStream->GetSourceNameObj())
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

UOSInt IO::DataCaptureStream::Read(const Data::ByteArray &buff)
{
	UOSInt ret = this->stm->Read(buff);
	if (ret > 0)
	{
		this->recvHdlr(this->hdlrObj, buff.WithSize(ret));
	}
	return ret;
}

UOSInt IO::DataCaptureStream::Write(Data::ByteArrayR buff)
{
	UOSInt ret = this->stm->Write(buff);
	if (ret > 0)
	{
		this->sendHdlr(this->hdlrObj, buff.WithSize(ret));
	}
	return ret;
}

Int32 IO::DataCaptureStream::Flush()
{
	return this->stm->Flush();
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
