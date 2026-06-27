#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/LNKFile.h"
#include "Text/MyStringW.h"

void IO::LNKFile::InitBuff(UnsafeArray<const UInt8> buff, UIntOS buffSize)
{
	if (buffSize < 76)
	{
		return;
	}
	if (ReadUInt32(&buff[0]) != 0x4C || ReadUInt32(&buff[4]) != 0x00021401 || ReadUInt32(&buff[8]) != 0 || ReadUInt32(&buff[12]) != 0xC0 || ReadUInt32(&buff[16]) != 0x46000000)
	{
		return;
	}
	UnsafeArray<UInt8> newBuff;
	this->buff = newBuff = MemAllocArr(UInt8, buffSize);
	this->buffSize = buffSize;
	MemCopyNO(newBuff.Ptr(), buff.Ptr(), buffSize);
}

IO::LNKFile::LNKFile(Text::CStringNN fileName)
{
	this->buff = nullptr;
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
	UIntOS buffSize = fs.Read(BYTEARR(buff));
	this->InitBuff(buff, buffSize);
}

IO::LNKFile::LNKFile(UnsafeArray<const UInt8> buff, UIntOS buffSize)
{
	this->buff = nullptr;
	this->buffSize = 0;
	this->InitBuff(buff, buffSize);
}

IO::LNKFile::~LNKFile()
{
	UnsafeArray<UInt8> buff;
	if (this->buff.SetTo(buff))
	{
		MemFreeArr(buff);
		this->buff = nullptr;
	}
}

Bool IO::LNKFile::IsError()
{
	return this->buff.IsNull();
}

Data::Timestamp IO::LNKFile::GetCreateTime()
{
	UnsafeArray<UInt8> buff;
	if (!this->buff.SetTo(buff))
		return Data::Timestamp(0);
	return Data::Timestamp::FromFILETIME(&buff[28], Data::DateTimeUtil::GetLocalTzQhr());
}

Data::Timestamp IO::LNKFile::GetAccessTime()
{
	UnsafeArray<UInt8> buff;
	if (!this->buff.SetTo(buff))
		return Data::Timestamp(0);
	return Data::Timestamp::FromFILETIME(&buff[36], Data::DateTimeUtil::GetLocalTzQhr());
}

Data::Timestamp IO::LNKFile::GetWriteTime()
{
	UnsafeArray<UInt8> buff;
	if (!this->buff.SetTo(buff))
		return Data::Timestamp(0);
	return Data::Timestamp::FromFILETIME(&buff[44], Data::DateTimeUtil::GetLocalTzQhr());
}

UnsafeArrayOpt<UTF8Char> IO::LNKFile::GetLocalBasePath(UnsafeArray<UTF8Char> sbuff)
{
	UnsafeArray<UInt8> buff;
	if (!this->buff.SetTo(buff))
		return nullptr;
	UInt32 flags = ReadUInt32(&buff[20]);
	if ((flags & 2) == 0)
		return nullptr;
	UIntOS ofst = 0x4C;
	if (flags & 1)
	{
		ofst += (UIntOS)ReadUInt16(&buff[ofst]) + 2;
	}
	UInt32 linkInfoFlags = ReadUInt32(&buff[ofst + 8]);
	UInt32 localBasePathOffset = ReadUInt32(&buff[ofst + 16]);
	if ((linkInfoFlags & 1) && localBasePathOffset != 0)
	{
		return Text::StrConcat(sbuff, &buff[ofst + localBasePathOffset]);
	}
	else
	{
		return nullptr;
	}
}

UnsafeArrayOpt<UTF8Char> IO::LNKFile::GetNameString(UnsafeArray<UTF8Char> sbuff)
{
	UnsafeArray<UInt8> buff;
	if (!this->buff.SetTo(buff))
		return nullptr;
	UInt32 flags = ReadUInt32(&buff[20]);
	if ((flags & 4) == 0)
		return nullptr;
	UIntOS ofst = 0x4C;
	if (flags & 1)
	{
		ofst += (UIntOS)ReadUInt16(&buff[ofst]) + 2;
	}
	if (flags & 2)
	{
		ofst += (UIntOS)ReadUInt32(&buff[ofst]);
	}
	UInt16 charCnt = ReadUInt16(&buff[ofst]);
	sbuff = Text::StrUTF16_UTF8C(sbuff, (const UTF16Char*)&buff[ofst + 2], charCnt);
	*sbuff = 0;
	return sbuff;
}

UnsafeArrayOpt<UTF8Char> IO::LNKFile::GetRelativePath(UnsafeArray<UTF8Char> sbuff)
{
	UnsafeArray<UInt8> buff;
	if (!this->buff.SetTo(buff))
		return nullptr;
	UInt32 flags = ReadUInt32(&buff[20]);
	if ((flags & 8) == 0)
		return nullptr;
	UIntOS ofst = 0x4C;
	if (flags & 1)
	{
		ofst += (UIntOS)ReadUInt16(&buff[ofst]) + 2;
	}
	if (flags & 2)
	{
		ofst += (UIntOS)ReadUInt32(&buff[ofst]);
	}
	if (flags & 4)
	{
		ofst += (UIntOS)ReadUInt16(&buff[ofst]) * 2 + 2;
	}
	UInt16 charCnt = ReadUInt16(&buff[ofst]);
	sbuff = Text::StrUTF16_UTF8C(sbuff, (const UTF16Char*)&buff[ofst + 2], charCnt);
	*sbuff = 0;
	return sbuff;
}

UnsafeArrayOpt<UTF8Char> IO::LNKFile::GetWorkingDirectory(UnsafeArray<UTF8Char> sbuff)
{
	UnsafeArray<UInt8> buff;
	if (!this->buff.SetTo(buff))
		return nullptr;
	UInt32 flags = ReadUInt32(&buff[20]);
	if ((flags & 16) == 0)
		return nullptr;
	UIntOS ofst = 0x4C;
	if (flags & 1)
	{
		ofst += (UIntOS)ReadUInt16(&buff[ofst]) + 2;
	}
	if (flags & 2)
	{
		ofst += (UIntOS)ReadUInt32(&buff[ofst]);
	}
	if (flags & 4)
	{
		ofst += (UIntOS)ReadUInt16(&buff[ofst]) * 2 + 2;
	}
	if (flags & 8)
	{
		ofst += (UIntOS)ReadUInt16(&buff[ofst]) * 2 + 2;
	}
	UInt16 charCnt = ReadUInt16(&buff[ofst]);
	sbuff = Text::StrUTF16_UTF8C(sbuff, (const UTF16Char*)&buff[ofst + 2], charCnt);
	*sbuff = 0;
	return sbuff;
}

UnsafeArrayOpt<UTF8Char> IO::LNKFile::GetCommandLineArguments(UnsafeArray<UTF8Char> sbuff)
{
	UnsafeArray<UInt8> buff;
	if (!this->buff.SetTo(buff))
		return nullptr;
	UInt32 flags = ReadUInt32(&buff[20]);
	if ((flags & 32) == 0)
		return nullptr;
	UIntOS ofst = 0x4C;
	if (flags & 1)
	{
		ofst += (UIntOS)ReadUInt16(&buff[ofst]) + 2;
	}
	if (flags & 2)
	{
		ofst += (UIntOS)ReadUInt32(&buff[ofst]);
	}
	if (flags & 4)
	{
		ofst += (UIntOS)ReadUInt16(&buff[ofst]) * 2 + 2;
	}
	if (flags & 8)
	{
		ofst += (UIntOS)ReadUInt16(&buff[ofst]) * 2 + 2;
	}
	if (flags & 16)
	{
		ofst += (UIntOS)ReadUInt16(&buff[ofst]) * 2 + 2;
	}
	UInt16 charCnt = ReadUInt16(&buff[ofst]);
	sbuff = Text::StrUTF16_UTF8C(sbuff, (const UTF16Char*)&buff[ofst + 2], charCnt);
	*sbuff = 0;
	return sbuff;
}

UnsafeArrayOpt<UTF8Char> IO::LNKFile::GetIconLocation(UnsafeArray<UTF8Char> sbuff)
{
	UnsafeArray<UInt8> buff;
	if (!this->buff.SetTo(buff))
		return nullptr;
	UInt32 flags = ReadUInt32(&buff[20]);
	if ((flags & 64) == 0)
		return nullptr;
	UIntOS ofst = 0x4C;
	if (flags & 1)
	{
		ofst += (UIntOS)ReadUInt16(&buff[ofst]) + 2;
	}
	if (flags & 2)
	{
		ofst += (UIntOS)ReadUInt32(&buff[ofst]);
	}
	if (flags & 4)
	{
		ofst += (UIntOS)ReadUInt16(&buff[ofst]) * 2 + 2;
	}
	if (flags & 8)
	{
		ofst += (UIntOS)ReadUInt16(&buff[ofst]) * 2 + 2;
	}
	if (flags & 16)
	{
		ofst += (UIntOS)ReadUInt16(&buff[ofst]) * 2 + 2;
	}
	if (flags & 32)
	{
		ofst += (UIntOS)ReadUInt16(&buff[ofst]) * 2 + 2;
	}
	UInt16 charCnt = ReadUInt16(&buff[ofst]);
	sbuff = Text::StrUTF16_UTF8C(sbuff, (const UTF16Char*)&buff[ofst + 2], charCnt);
	*sbuff = 0;
	return sbuff;
}

UnsafeArrayOpt<UTF8Char> IO::LNKFile::GetTarget(UnsafeArray<UTF8Char> sbuff)
{
	UnsafeArray<UInt8> buff;
	if (!this->buff.SetTo(buff))
		return nullptr;
	UInt32 flags = ReadUInt32(&buff[20]);
	UIntOS ofst = 0x4C;
	if (flags & 1)
	{
		ofst += (UIntOS)ReadUInt16(&buff[ofst]) + 2;
	}
	if (flags & 2)
	{
		ofst += (UIntOS)ReadUInt32(&buff[ofst]);
	}
	if (flags & 4)
	{
		ofst += (UIntOS)ReadUInt16(&buff[ofst]) * 2 + 2;
	}
	if (flags & 8)
	{
		ofst += (UIntOS)ReadUInt16(&buff[ofst]) * 2 + 2;
	}
	if (flags & 16)
	{
		ofst += (UIntOS)ReadUInt16(&buff[ofst]) * 2 + 2;
	}
	if (flags & 32)
	{
		ofst += (UIntOS)ReadUInt16(&buff[ofst]) * 2 + 2;
	}
	if (flags & 64)
	{
		ofst += (UIntOS)ReadUInt16(&buff[ofst]) * 2 + 2;
	}
	while (ofst < this->buffSize)
	{
		UInt32 size = ReadUInt32(&buff[ofst]);
		if (size < 4)
		{
			return nullptr;
		}
		if (ReadUInt32(&buff[ofst + 4]) == 0xA0000001)
		{
			return Text::StrUTF16_UTF8(sbuff, (const UTF16Char*)&buff[ofst + 268]);
		}
		else
		{
			ofst += size;
		}
	}
	return nullptr;
}
