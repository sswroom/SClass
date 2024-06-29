#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/StreamLogger.h"

IO::StreamLogger::StreamLogger(NN<IO::Stream> srcStream, Bool needRelease, Text::CString readLogPath, Text::CString writeLogPath) : IO::Stream(srcStream->GetSourceNameObj())
{
	this->stm = srcStream;
	this->needRelease = needRelease;
	if (readLogPath.leng > 0)
	{
		NEW_CLASS(this->readLog, IO::FileStream(readLogPath.OrEmpty(), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	}
	else
	{
		this->readLog = 0;
	}
	if (writeLogPath.leng > 0)
	{
		NEW_CLASS(this->writeLog, IO::FileStream(writeLogPath.OrEmpty(), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	}
	else
	{
		this->writeLog = 0;
	}
}

IO::StreamLogger::~StreamLogger()
{
	if (this->needRelease)
	{
		this->stm.Delete();
	}
	SDEL_CLASS(this->writeLog);
	SDEL_CLASS(this->readLog);
}

Bool IO::StreamLogger::IsDown() const
{
	return this->stm->IsDown();
}

UOSInt IO::StreamLogger::Read(const Data::ByteArray &buff)
{
	UOSInt readCnt = this->stm->Read(buff);
	if (readCnt > 0 && this->readLog)
	{
		this->readLog->Write(buff.WithSize(readCnt));
	}
	return readCnt;
}

UOSInt IO::StreamLogger::Write(Data::ByteArrayR buff)
{
	UOSInt writeCnt = this->stm->Write(buff);
	if (writeCnt > 0 && this->writeLog)
	{
		this->writeLog->Write(buff.WithSize(writeCnt));
	}
	return writeCnt;
}

Optional<IO::StreamReadReq> IO::StreamLogger::BeginRead(const Data::ByteArray &buff, NN<Sync::Event> evt)
{
	NN<MyReqData> myReqData;
	NN<IO::StreamReadReq> reqData;
	if (this->stm->BeginRead(buff, evt).SetTo(reqData))
	{
		myReqData = MemAllocNN(MyReqData);
		myReqData->buff = buff.Arr();
		myReqData->reqData = NN<IO::StreamWriteReq>::ConvertFrom(reqData);
		return NN<IO::StreamReadReq>::ConvertFrom(myReqData);
	}
	else
	{
		return 0;
	}
}

UOSInt IO::StreamLogger::EndRead(NN<IO::StreamReadReq> reqData, Bool toWait, OutParam<Bool> incomplete)
{
	NN<MyReqData> myReqData = NN<MyReqData>::ConvertFrom(reqData);
	Bool incomp;
	UOSInt readCnt = this->stm->EndRead(NN<IO::StreamReadReq>::ConvertFrom(myReqData->reqData), toWait, incomp);
	incomplete.Set(incomp);
	if (!incomp)
	{
		if (readCnt > 0 && this->readLog)
		{
			this->readLog->Write(Data::ByteArrayR(myReqData->buff, readCnt));
		}
		MemFreeNN(myReqData);
	}
	return readCnt;
}

void IO::StreamLogger::CancelRead(NN<IO::StreamReadReq> reqData)
{
	NN<MyReqData> myReqData = NN<MyReqData>::ConvertFrom(reqData);
	this->stm->CancelRead(NN<IO::StreamReadReq>::ConvertFrom(reqData));
	MemFreeNN(myReqData);
}

Optional<IO::StreamWriteReq> IO::StreamLogger::BeginWrite(Data::ByteArrayR buff, NN<Sync::Event> evt)
{
	NN<MyReqData> myReqData;
	NN<IO::StreamWriteReq> reqData;
	if (this->stm->BeginWrite(buff, evt).SetTo(reqData))
	{
		myReqData = MemAllocNN(MyReqData);
		myReqData->buff = buff.Arr();
		myReqData->reqData = reqData;
		return NN<IO::StreamWriteReq>::ConvertFrom(myReqData);
	}
	else
	{
		return 0;
	}
}

UOSInt IO::StreamLogger::EndWrite(NN<IO::StreamWriteReq> reqData, Bool toWait)
{
	NN<MyReqData> myReqData = NN<MyReqData>::ConvertFrom(reqData);
	UOSInt writeCnt = this->stm->EndWrite(myReqData->reqData, toWait);
	if (writeCnt >= 0)
	{
		if (writeCnt > 0 && this->writeLog)
		{
			this->writeLog->Write(Data::ByteArrayR(myReqData->buff, writeCnt));
		}
		MemFreeNN(myReqData);
	}
	return writeCnt;
}

void IO::StreamLogger::CancelWrite(NN<IO::StreamWriteReq> reqData)
{
	NN<MyReqData> myReqData = NN<MyReqData>::ConvertFrom(reqData);
	this->stm->CancelWrite(reqData);
	MemFreeNN(myReqData);
}

Int32 IO::StreamLogger::Flush()
{
	return this->stm->Flush();
}

void IO::StreamLogger::Close()
{
	this->stm->Close();
}

Bool IO::StreamLogger::Recover()
{
	return this->stm->Recover();
}

IO::StreamType IO::StreamLogger::GetStreamType() const
{
	return IO::StreamType::StreamLogger;
}
