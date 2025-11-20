#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "IO/UDPFileLog.h"
#include "Net/SocketFactory.h"
#include "Text/Encoding.h"

IO::UDPFileLog::UDPFileLog(NN<IO::StreamData> fd) : IO::UDPLog(fd->GetFullName()), logBuff(65544)
{
	this->fd = fd->GetPartialData(0, fd->GetDataSize());

	UInt64 currPos;
	UInt64 buffPos;
	UInt64 fdSize;
	UOSInt buffSize;
	UOSInt endOfst;
	UOSInt readSize;
	UInt32 packetSize;
	UOSInt i;

	Data::ByteBuffer buff(1048576);
	fdSize = this->fd->GetDataSize();
	buffSize = 0;
	currPos = 0;

	while (currPos < fdSize)
	{
		readSize = 1048576 - buffSize;
		if (readSize > fdSize - currPos)
		{
			readSize = (UOSInt)(fdSize - currPos);
		}
		this->fd->GetRealData(currPos, readSize, buff.SubArray(buffSize));
		buffPos = currPos - buffSize;
		buffSize += readSize;
		endOfst = buffSize - 8;

		i = 0;
		while (i < endOfst)
		{
			if (buff[i] == 0xaa && buff[i + 1] == 0xbb)
			{
				packetSize = ReadUInt16(&buff[i + 2]);
				if (packetSize + 8 + i > buffSize)
					break;
				
				this->logPos.Add(buffPos + i);
				this->logSize.Add(packetSize + 8);

				i += packetSize + 8;
			}
			else
			{
				i++;
			}
		}
		if (i < buffSize)
		{
			buff.CopyInner(0, i, buffSize - i);
			buffSize -= i;
		}
		else
		{
			buffSize = 0;
		}

		currPos += readSize;
	}
}

IO::UDPFileLog::~UDPFileLog()
{
	this->fd.Delete();
}

UOSInt IO::UDPFileLog::GetCount(IO::LogHandler::LogLevel logLevel) const
{
	if (logLevel == IO::LogHandler::LogLevel::Raw)
	{
		return this->logPos.GetCount() << 1;
	}
/*	if (logLevel == IO::LogHandler::LogLevel::Raw)
	{
		return this->logPos->GetCount();
	}*/
	else
	{
		return this->logPos.GetCount();
	}
}

Bool IO::UDPFileLog::GetLogMessage(IO::LogHandler::LogLevel logLevel, UOSInt index, Data::Timestamp *ts, NN<Text::StringBuilderUTF8> sb, Text::LineBreakType lineBreak) const
{
	if (logLevel == IO::LogHandler::LogLevel::Raw)
	{
		if (index >= (this->logPos.GetCount() << 1))
			return false;
		if (index & 1)
		{
			index = index >> 1;

			UInt64 pos = this->logPos.GetItem(index);
			UInt32 size = this->logSize.GetItem(index);
			this->fd->GetRealData(pos, size, this->logBuff);
			*ts = Data::Timestamp::FromEpochSec(*(UInt32*)&this->logBuff[4], 0);
			return ParseLog(&this->logBuff[8], size - 8, sb, false);
		}
		index = index >> 1;
	}
/*	if (logLevel == IO::LogHandler::LogLevel::Raw)
	{
		if (index >= this->logPos->GetCount())
			return false;
		Int64 pos = this->logPos->GetItem(index);
		Int32 size = this->logSize->GetItem(index);
		this->fd->GetRealData(pos, size, this->logBuff);
		dt->SetUnixTimestamp(*(UInt32*)&this->logBuff[4]);
		return ParseLog(&this->logBuff[8], size - 8, sb, false);
	}*/
	else
	{
		if (index >= this->logPos.GetCount())
			return false;
	}
	UInt64 pos = this->logPos.GetItem(index);
	UInt32 size = this->logSize.GetItem(index);
	this->fd->GetRealData(pos, size, this->logBuff);
	*ts = Data::Timestamp::FromEpochSec(*(UInt32*)&this->logBuff[4], 0);
	sb->AppendHexBuff(&this->logBuff[8], size - 8, ' ', lineBreak);
	return true;
}

Bool IO::UDPFileLog::GetLogDescription(IO::LogHandler::LogLevel logLevel, UOSInt index, NN<Text::StringBuilderUTF8> sb) const
{
	if (logLevel == IO::LogHandler::LogLevel::Raw)
	{
		index = index >> 1;
	}
	if (index >= this->logPos.GetCount())
		return false;
	Data::DateTime dt;
	UInt64 pos = this->logPos.GetItem(index);
	UInt32 size = this->logSize.GetItem(index);
	this->fd->GetRealData(pos, size, this->logBuff);
	dt.SetUnixTimestamp(*(UInt32*)&this->logBuff[4]);
	return this->ParseLog(&this->logBuff[8], size - 8, sb, true);
}
