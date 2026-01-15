#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/StreamLogger.h"

IO::StreamLogger::StreamLogger(NN<IO::Stream> srcStream, Bool needRelease, Text::CString readLogPath, Text::CString writeLogPath) : IO::Stream(srcStream->GetSourceNameObj())
{
	this->stm = srcStream;
	this->needRelease = needRelease;
	if (readLogPath.leng > 0)
	{
		NEW_CLASSOPT(this->readLog, IO::FileStream(readLogPath.OrEmpty(), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	}
	else
	{
		this->readLog = nullptr;
	}
	if (writeLogPath.leng > 0)
	{
		NEW_CLASSOPT(this->writeLog, IO::FileStream(writeLogPath.OrEmpty(), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	}
	else
	{
		this->writeLog = nullptr;
	}
}

IO::StreamLogger::~StreamLogger()
{
	if (this->needRelease)
	{
		this->stm.Delete();
	}
	this->writeLog.Delete();
	this->readLog.Delete();
}

Bool IO::StreamLogger::IsDown() const
{
	return this->stm->IsDown();
}

UIntOS IO::StreamLogger::Read(const Data::ByteArray &buff)
{
	NN<IO::FileStream> readLog;
	UIntOS readCnt = this->stm->Read(buff);
	if (readCnt > 0 && this->readLog.SetTo(readLog))
	{
		readLog->Write(buff.WithSize(readCnt));
	}
	return readCnt;
}

UIntOS IO::StreamLogger::Write(Data::ByteArrayR buff)
{
	NN<IO::FileStream> writeLog;
	UIntOS writeCnt = this->stm->Write(buff);
	if (writeCnt > 0 && this->writeLog.SetTo(writeLog))
	{
		writeLog->Write(buff.WithSize(writeCnt));
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
		return nullptr;
	}
}

UIntOS IO::StreamLogger::EndRead(NN<IO::StreamReadReq> reqData, Bool toWait, OutParam<Bool> incomplete)
{
	NN<IO::FileStream> readLog;
	NN<MyReqData> myReqData = NN<MyReqData>::ConvertFrom(reqData);
	Bool incomp;
	UIntOS readCnt = this->stm->EndRead(NN<IO::StreamReadReq>::ConvertFrom(myReqData->reqData), toWait, incomp);
	incomplete.Set(incomp);
	if (!incomp)
	{
		if (readCnt > 0 && this->readLog.SetTo(readLog))
		{
			readLog->Write(Data::ByteArrayR(myReqData->buff, readCnt));
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
		return nullptr;
	}
}

UIntOS IO::StreamLogger::EndWrite(NN<IO::StreamWriteReq> reqData, Bool toWait)
{
	NN<IO::FileStream> writeLog;
	NN<MyReqData> myReqData = NN<MyReqData>::ConvertFrom(reqData);
	UIntOS writeCnt = this->stm->EndWrite(myReqData->reqData, toWait);
	if (writeCnt >= 0)
	{
		if (writeCnt > 0 && this->writeLog.SetTo(writeLog))
		{
			writeLog->Write(Data::ByteArrayR(myReqData->buff, writeCnt));
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
