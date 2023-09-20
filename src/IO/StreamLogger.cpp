#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/StreamLogger.h"

IO::StreamLogger::StreamLogger(IO::Stream *srcStream, Bool needRelease, Text::CString readLogPath, Text::CString writeLogPath) : IO::Stream(srcStream->GetSourceNameObj())
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
		DEL_CLASS(this->stm);
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
		this->readLog->Write(buff.Ptr(), readCnt);
	}
	return readCnt;
}

UOSInt IO::StreamLogger::Write(const UInt8 *buff, UOSInt size)
{
	UOSInt writeCnt = this->stm->Write(buff, size);
	if (writeCnt > 0 && this->writeLog)
	{
		this->writeLog->Write(buff, writeCnt);
	}
	return writeCnt;
}

void *IO::StreamLogger::BeginRead(const Data::ByteArray &buff, Sync::Event *evt)
{
	void *reqData = this->stm->BeginRead(buff, evt);
	MyReqData *myReqData;
	if (reqData)
	{
		myReqData = MemAlloc(MyReqData, 1);
		myReqData->buff = buff.Ptr();
		myReqData->reqData = reqData;
		return myReqData;
	}
	else
	{
		return 0;
	}
}

UOSInt IO::StreamLogger::EndRead(void *reqData, Bool toWait, OutParam<Bool> incomplete)
{
	MyReqData *myReqData = (MyReqData*)reqData;
	Bool incomp;
	UOSInt readCnt = this->stm->EndRead(myReqData->reqData, toWait, incomp);
	incomplete.Set(incomp);
	if (!incomp)
	{
		if (readCnt > 0 && this->readLog)
		{
			this->readLog->Write(myReqData->buff, readCnt);
		}
		MemFree(myReqData);
	}
	return readCnt;
}

void IO::StreamLogger::CancelRead(void *reqData)
{
	MyReqData *myReqData = (MyReqData*)reqData;
	this->stm->CancelRead(reqData);
	MemFree(myReqData);
}

void *IO::StreamLogger::BeginWrite(const UInt8 *buff, UOSInt size, Sync::Event *evt)
{
	void *reqData = this->stm->BeginWrite(buff, size, evt);
	MyReqData *myReqData;
	if (reqData)
	{
		myReqData = MemAlloc(MyReqData, 1);
		myReqData->buff = buff;
		myReqData->reqData = reqData;
		return myReqData;
	}
	else
	{
		return 0;
	}
}

UOSInt IO::StreamLogger::EndWrite(void *reqData, Bool toWait)
{
	MyReqData *myReqData = (MyReqData*)reqData;
	UOSInt writeCnt = this->stm->EndWrite(myReqData->reqData, toWait);
	if (writeCnt >= 0)
	{
		if (writeCnt > 0 && this->writeLog)
		{
			this->writeLog->Write(myReqData->buff, writeCnt);
		}
		MemFree(myReqData);
	}
	return writeCnt;
}

void IO::StreamLogger::CancelWrite(void *reqData)
{
	MyReqData *myReqData = (MyReqData*)reqData;
	this->stm->CancelWrite(reqData);
	MemFree(myReqData);
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
