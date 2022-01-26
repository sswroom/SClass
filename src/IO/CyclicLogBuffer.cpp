#include "Stdafx.h"
#include "IO/CyclicLogBuffer.h"
#include "Sync/MutexUsage.h"

IO::CyclicLogBuffer::CyclicLogBuffer(UOSInt buffSize)
{
	this->buffSize = buffSize;
	this->logBuff = MemAlloc(UTF8Char*, this->buffSize);
	this->logInd = 0;
	NEW_CLASS(this->logMut, Sync::Mutex());
	MemClear(this->logBuff, sizeof(UTF8Char*) * this->buffSize);
}

IO::CyclicLogBuffer::~CyclicLogBuffer()
{
	UOSInt i = 0;
	while (i < this->buffSize)
	{
		if (this->logBuff[i] == 0)
			break;
		MemFree(this->logBuff[i]);
		this->logBuff[i] = 0;
		i++;
	}
	MemFree(this->logBuff);
	DEL_CLASS(this->logMut);
}

void IO::CyclicLogBuffer::LogAdded(Data::DateTime *logTime, const UTF8Char *logMsg, UOSInt msgLen, LogLevel logLev)
{
	UTF8Char *strBuff = MemAlloc(UTF8Char, 25 + msgLen);
	UTF8Char *sptr = logTime->ToString(strBuff, "yyyy-MM-dd HH:mm:ss.fff");
	*sptr++ = '\t';
	Text::StrConcatC(sptr, logMsg, msgLen);

	Sync::MutexUsage mutUsage(this->logMut);
	if (this->logBuff[this->logInd])
	{
		MemFree(this->logBuff[this->logInd]);
	}
	this->logBuff[this->logInd++] = strBuff;
	if (this->logInd >= this->buffSize)
	{
		this->logInd -= this->buffSize;
	}
}

void IO::CyclicLogBuffer::LogClosed()
{

}

void IO::CyclicLogBuffer::GetLogs(Text::StringBuilderUTF8 *sb, const UTF8Char* seperator)
{
	Sync::MutexUsage mutUsage(this->logMut);
	UOSInt i = this->logInd;
	while (i-- > 0)
	{
		sb->Append(this->logBuff[i]);
		sb->Append(seperator);
	}
	i = this->buffSize - 1;
	if (this->logBuff[i])
	{
		while (i >= this->logInd)
		{
			sb->Append(this->logBuff[i]);
			sb->Append(seperator);

			i--;
		}
	}
}
