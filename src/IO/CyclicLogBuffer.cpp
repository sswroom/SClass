#include "Stdafx.h"
#include "IO/CyclicLogBuffer.h"
#include "Sync/MutexUsage.h"

IO::CyclicLogBuffer::CyclicLogBuffer(UIntOS buffSize)
{
	this->buffSize = buffSize;
	this->logBuff = MemAllocArr(UnsafeArrayOpt<UTF8Char>, this->buffSize);
	this->logLeng = MemAlloc(UIntOS, this->buffSize);
	this->logInd = 0;
	MemClear(this->logBuff.Ptr(), sizeof(UnsafeArray<UTF8Char>) * this->buffSize);
}

IO::CyclicLogBuffer::~CyclicLogBuffer()
{
	UIntOS i = 0;
	while (i < this->buffSize)
	{
		if (this->logBuff[i].IsNull())
			break;
		MemFreeArr(this->logBuff[i]);
		this->logBuff[i] = nullptr;
		i++;
	}
	MemFreeArr(this->logBuff);
	MemFree(this->logLeng);
}

void IO::CyclicLogBuffer::LogAdded(const Data::Timestamp &logTime, Text::CStringNN logMsg, LogLevel logLev)
{
	UnsafeArray<UTF8Char> strBuff = MemAllocArr(UTF8Char, 25 + logMsg.leng);
	UnsafeArray<UTF8Char> sptr = logTime.ToString(strBuff, "yyyy-MM-dd HH:mm:ss.fff");
	*sptr++ = '\t';
	sptr = logMsg.ConcatTo(sptr);

	Sync::MutexUsage mutUsage(this->logMut);
	if (this->logBuff[this->logInd].NotNull())
	{
		MemFreeArr(this->logBuff[this->logInd]);
	}
	this->logLeng[this->logInd] = (UIntOS)(sptr - strBuff);
	this->logBuff[this->logInd++] = strBuff;
	if (this->logInd >= this->buffSize)
	{
		this->logInd -= this->buffSize;
	}
}

void IO::CyclicLogBuffer::LogClosed()
{

}

void IO::CyclicLogBuffer::GetLogs(NN<Text::StringBuilderUTF8> sb, Text::CStringNN seperator)
{
	Sync::MutexUsage mutUsage(this->logMut);
	UIntOS i = this->logInd;
	while (i-- > 0)
	{
		sb->AppendC(UnsafeArrayOpt<const UTF8Char>(this->logBuff[i]).Or(U8STR("")), this->logLeng[i]);
		sb->Append(seperator);
	}
	i = this->buffSize - 1;
	UnsafeArray<UTF8Char> nns;
	if (this->logBuff[i].SetTo(nns))
	{
		while (i >= this->logInd)
		{
			sb->AppendC(nns, this->logLeng[i]);
			sb->Append(seperator);

			i--;
		}
	}
}
