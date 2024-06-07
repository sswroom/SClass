#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/LNKFile.h"
#include "Text/MyStringW.h"

void IO::LNKFile::InitBuff(const UInt8 *buff, UOSInt buffSize)
{
	if (buffSize < 76)
	{
		return;
	}
	if (ReadUInt32(&buff[0]) != 0x4C || ReadUInt32(&buff[4]) != 0x00021401 || ReadUInt32(&buff[8]) != 0 || ReadUInt32(&buff[12]) != 0xC0 || ReadUInt32(&buff[16]) != 0x46000000)
	{
		return;
	}
	this->buff = MemAlloc(UInt8, buffSize);
	this->buffSize = buffSize;
	MemCopyNO(this->buff, buff, buffSize);
}

IO::LNKFile::LNKFile(Text::CStringNN fileName)
{
	this->buff = 0;
	this->buffSize = 0;
	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		return;
	}
	UInt8 buff[8192];
	if (fs.GetLength() >= 8192)
	{
		return;
	}
	UOSInt buffSize = fs.Read(BYTEARR(buff));
	this->InitBuff(buff, buffSize);
}

IO::LNKFile::LNKFile(const UInt8 *buff, UOSInt buffSize)
{
	this->buff = 0;
	this->buffSize = 0;
	this->InitBuff(buff, buffSize);
}

IO::LNKFile::~LNKFile()
{
	if (this->buff)
	{
		MemFree(this->buff);
		this->buff = 0;
	}
}

Bool IO::LNKFile::IsError()
{
	return this->buff == 0;
}

Data::Timestamp IO::LNKFile::GetCreateTime()
{
	if (this->buff == 0)
		return Data::Timestamp(0);
	return Data::Timestamp::FromFILETIME(&this->buff[28], Data::DateTimeUtil::GetLocalTzQhr());
}

Data::Timestamp IO::LNKFile::GetAccessTime()
{
	if (this->buff == 0)
		return Data::Timestamp(0);
	return Data::Timestamp::FromFILETIME(&this->buff[36], Data::DateTimeUtil::GetLocalTzQhr());
}

Data::Timestamp IO::LNKFile::GetWriteTime()
{
	if (this->buff == 0)
		return Data::Timestamp(0);
	return Data::Timestamp::FromFILETIME(&this->buff[44], Data::DateTimeUtil::GetLocalTzQhr());
}

UnsafeArrayOpt<UTF8Char> IO::LNKFile::GetLocalBasePath(UnsafeArray<UTF8Char> sbuff)
{
	if (this->buff == 0)
		return 0;
	UInt32 flags = ReadUInt32(&this->buff[20]);
	if ((flags & 2) == 0)
		return 0;
	UOSInt ofst = 0x4C;
	if (flags & 1)
	{
		ofst += (UOSInt)ReadUInt16(&this->buff[ofst]) + 2;
	}
	UInt32 linkInfoFlags = ReadUInt32(&this->buff[ofst + 8]);
	UInt32 localBasePathOffset = ReadUInt32(&this->buff[ofst + 16]);
	if ((linkInfoFlags & 1) && localBasePathOffset != 0)
	{
		return Text::StrConcat(sbuff, &this->buff[ofst + localBasePathOffset]);
	}
	else
	{
		return 0;
	}
}

UnsafeArrayOpt<UTF8Char> IO::LNKFile::GetNameString(UnsafeArray<UTF8Char> sbuff)
{
	if (this->buff == 0)
		return 0;
	UInt32 flags = ReadUInt32(&this->buff[20]);
	if ((flags & 4) == 0)
		return 0;
	UOSInt ofst = 0x4C;
	if (flags & 1)
	{
		ofst += (UOSInt)ReadUInt16(&this->buff[ofst]) + 2;
	}
	if (flags & 2)
	{
		ofst += (UOSInt)ReadUInt32(&this->buff[ofst]);
	}
	UInt16 charCnt = ReadUInt16(&this->buff[ofst]);
	sbuff = Text::StrUTF16_UTF8C(sbuff, (const UTF16Char*)&this->buff[ofst + 2], charCnt);
	*sbuff = 0;
	return sbuff;
}

UnsafeArrayOpt<UTF8Char> IO::LNKFile::GetRelativePath(UnsafeArray<UTF8Char> sbuff)
{
	if (this->buff == 0)
		return 0;
	UInt32 flags = ReadUInt32(&this->buff[20]);
	if ((flags & 8) == 0)
		return 0;
	UOSInt ofst = 0x4C;
	if (flags & 1)
	{
		ofst += (UOSInt)ReadUInt16(&this->buff[ofst]) + 2;
	}
	if (flags & 2)
	{
		ofst += (UOSInt)ReadUInt32(&this->buff[ofst]);
	}
	if (flags & 4)
	{
		ofst += (UOSInt)ReadUInt16(&this->buff[ofst]) * 2 + 2;
	}
	UInt16 charCnt = ReadUInt16(&this->buff[ofst]);
	sbuff = Text::StrUTF16_UTF8C(sbuff, (const UTF16Char*)&this->buff[ofst + 2], charCnt);
	*sbuff = 0;
	return sbuff;
}

UnsafeArrayOpt<UTF8Char> IO::LNKFile::GetWorkingDirectory(UnsafeArray<UTF8Char> sbuff)
{
	if (this->buff == 0)
		return 0;
	UInt32 flags = ReadUInt32(&this->buff[20]);
	if ((flags & 16) == 0)
		return 0;
	UOSInt ofst = 0x4C;
	if (flags & 1)
	{
		ofst += (UOSInt)ReadUInt16(&this->buff[ofst]) + 2;
	}
	if (flags & 2)
	{
		ofst += (UOSInt)ReadUInt32(&this->buff[ofst]);
	}
	if (flags & 4)
	{
		ofst += (UOSInt)ReadUInt16(&this->buff[ofst]) * 2 + 2;
	}
	if (flags & 8)
	{
		ofst += (UOSInt)ReadUInt16(&this->buff[ofst]) * 2 + 2;
	}
	UInt16 charCnt = ReadUInt16(&this->buff[ofst]);
	sbuff = Text::StrUTF16_UTF8C(sbuff, (const UTF16Char*)&this->buff[ofst + 2], charCnt);
	*sbuff = 0;
	return sbuff;
}

UnsafeArrayOpt<UTF8Char> IO::LNKFile::GetCommandLineArguments(UnsafeArray<UTF8Char> sbuff)
{
	if (this->buff == 0)
		return 0;
	UInt32 flags = ReadUInt32(&this->buff[20]);
	if ((flags & 32) == 0)
		return 0;
	UOSInt ofst = 0x4C;
	if (flags & 1)
	{
		ofst += (UOSInt)ReadUInt16(&this->buff[ofst]) + 2;
	}
	if (flags & 2)
	{
		ofst += (UOSInt)ReadUInt32(&this->buff[ofst]);
	}
	if (flags & 4)
	{
		ofst += (UOSInt)ReadUInt16(&this->buff[ofst]) * 2 + 2;
	}
	if (flags & 8)
	{
		ofst += (UOSInt)ReadUInt16(&this->buff[ofst]) * 2 + 2;
	}
	if (flags & 16)
	{
		ofst += (UOSInt)ReadUInt16(&this->buff[ofst]) * 2 + 2;
	}
	UInt16 charCnt = ReadUInt16(&this->buff[ofst]);
	sbuff = Text::StrUTF16_UTF8C(sbuff, (const UTF16Char*)&this->buff[ofst + 2], charCnt);
	*sbuff = 0;
	return sbuff;
}

UnsafeArrayOpt<UTF8Char> IO::LNKFile::GetIconLocation(UnsafeArray<UTF8Char> sbuff)
{
	if (this->buff == 0)
		return 0;
	UInt32 flags = ReadUInt32(&this->buff[20]);
	if ((flags & 64) == 0)
		return 0;
	UOSInt ofst = 0x4C;
	if (flags & 1)
	{
		ofst += (UOSInt)ReadUInt16(&this->buff[ofst]) + 2;
	}
	if (flags & 2)
	{
		ofst += (UOSInt)ReadUInt32(&this->buff[ofst]);
	}
	if (flags & 4)
	{
		ofst += (UOSInt)ReadUInt16(&this->buff[ofst]) * 2 + 2;
	}
	if (flags & 8)
	{
		ofst += (UOSInt)ReadUInt16(&this->buff[ofst]) * 2 + 2;
	}
	if (flags & 16)
	{
		ofst += (UOSInt)ReadUInt16(&this->buff[ofst]) * 2 + 2;
	}
	if (flags & 32)
	{
		ofst += (UOSInt)ReadUInt16(&this->buff[ofst]) * 2 + 2;
	}
	UInt16 charCnt = ReadUInt16(&this->buff[ofst]);
	sbuff = Text::StrUTF16_UTF8C(sbuff, (const UTF16Char*)&this->buff[ofst + 2], charCnt);
	*sbuff = 0;
	return sbuff;
}

UnsafeArrayOpt<UTF8Char> IO::LNKFile::GetTarget(UnsafeArray<UTF8Char> sbuff)
{
	if (this->buff == 0)
		return 0;
	UInt32 flags = ReadUInt32(&this->buff[20]);
	UOSInt ofst = 0x4C;
	if (flags & 1)
	{
		ofst += (UOSInt)ReadUInt16(&this->buff[ofst]) + 2;
	}
	if (flags & 2)
	{
		ofst += (UOSInt)ReadUInt32(&this->buff[ofst]);
	}
	if (flags & 4)
	{
		ofst += (UOSInt)ReadUInt16(&this->buff[ofst]) * 2 + 2;
	}
	if (flags & 8)
	{
		ofst += (UOSInt)ReadUInt16(&this->buff[ofst]) * 2 + 2;
	}
	if (flags & 16)
	{
		ofst += (UOSInt)ReadUInt16(&this->buff[ofst]) * 2 + 2;
	}
	if (flags & 32)
	{
		ofst += (UOSInt)ReadUInt16(&this->buff[ofst]) * 2 + 2;
	}
	if (flags & 64)
	{
		ofst += (UOSInt)ReadUInt16(&this->buff[ofst]) * 2 + 2;
	}
	while (ofst < this->buffSize)
	{
		UInt32 size = ReadUInt32(&this->buff[ofst]);
		if (size < 4)
		{
			return 0;
		}
		if (ReadUInt32(&this->buff[ofst + 4]) == 0xA0000001)
		{
			return Text::StrUTF16_UTF8(sbuff, (const UTF16Char*)&this->buff[ofst + 268]);
		}
		else
		{
			ofst += size;
		}
	}
	return 0;
}
