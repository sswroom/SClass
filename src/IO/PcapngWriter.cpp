#include "Stdafx.h"
#include "IO/OS.h"
#include "IO/PcapngWriter.h"
#include "IO/SystemInfo.h"
#include "Sync/MutexUsage.h"

IO::PcapngWriter::PcapngWriter(Text::CStringNN fileName, Int32 linkType, Text::CStringNN appName) : fs(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal)
{
	UInt8 buff[256];
	IO::SystemInfo sysInfo;
	OSInt i;
	UTF8Char *sptr;
	UTF8Char *sptr2;
	WriteUInt32(&buff[0], 0x0a0d0d0a); //block type = SHB
	WriteInt32(&buff[4], 2); //block length
	WriteInt32(&buff[8], 0x1A2B3C4D); //byte order magic
	WriteInt16(&buff[12], 1); //major version
	WriteInt16(&buff[14], 0); //minor version
	WriteInt64(&buff[16], -1); //section length
	sptr = sysInfo.GetPlatformName(&buff[28]);
	if (sptr == 0)
	{
		sptr = &buff[24];
	}
	else
	{
		i = sptr - buff - 28;
		WriteInt16(&buff[24], 2);
		WriteInt16(&buff[26], i);
		if (i & 3)
		{
			i = 4 - (i & 3);
			sptr += i;
			while (i-- > 0)
			{
				sptr[-i] = 0;
			}
		}
	}
	sptr2 = IO::OS::GetDistro(sptr + 4);
	*sptr2++ = ' ';
	sptr2 = IO::OS::GetVersion(sptr2);
	i = sptr2 - sptr - 4;
	WriteInt16(sptr, 3);
	WriteInt16(&sptr[2], i);
	sptr = sptr2;
	if (i & 3)
	{
		i = 4 - (i & 3);
		sptr += i;
		while (i-- > 0)
		{
			sptr[-i] = 0;
		}
	}

	sptr2 = appName.ConcatTo(sptr + 4);
	i = sptr2 - sptr - 4;
	WriteInt16(sptr, 3);
	WriteInt16(&sptr[2], i);
	sptr = sptr2;
	if (i & 3)
	{
		i = 4 - (i & 3);
		sptr += i;
		while (i-- > 0)
		{
			sptr[-i] = 0;
		}
	}
	WriteInt32(sptr, 0);
	sptr += 4;
	i = sptr - buff + 4;
	WriteInt32(sptr, (Int32)i);
	WriteInt32(&buff[4], (Int32)i);

	this->fs.Write(buff, (UOSInt)i);

	WriteInt32(&buff[0], 1); //Block Type = IDB
	WriteInt32(&buff[4], 0); //Block Size
	WriteInt32(&buff[8], linkType); //Link Type + Reserved
	WriteInt32(&buff[12], 65536); //snap len
	WriteInt16(&buff[16], 2);
	WriteInt16(&buff[18], 3);
	Text::StrConcatC(&buff[20], UTF8STRC("any"));
	WriteInt16(&buff[24], 9);
	WriteInt16(&buff[26], 1);
	WriteInt32(&buff[28], 9);
	sptr = &buff[32];
	sptr2 = IO::OS::GetDistro(sptr + 4);
	*sptr2++ = ' ';
	sptr2 = IO::OS::GetVersion(sptr2);
	i = sptr2 - sptr - 4;
	WriteInt16(sptr, 3);
	WriteInt16(&sptr[2], i);
	sptr = sptr2;
	if (i & 3)
	{
		i = 4 - (i & 3);
		sptr += i;
		while (i-- > 0)
		{
			sptr[-i] = 0;
		}
	}
	WriteInt32(sptr, 0);
	sptr += 4;
	i = sptr - buff + 4;
	WriteInt32(sptr, (Int32)i);
	WriteInt32(&buff[4], (Int32)i);

	this->fs.Write(buff, (UOSInt)i);
}

IO::PcapngWriter::~PcapngWriter()
{
}

Bool IO::PcapngWriter::IsError() const
{
	return this->fs.IsError();
}

Bool IO::PcapngWriter::WritePacket(Data::ByteArrayR packet)
{
	UInt8 buff[28];
	Data::Timestamp ts = Data::Timestamp::UtcNow();
	Int64 t = ts.ToEpochNS();
	UOSInt pSize = packet.GetSize() + 32;
	if (pSize & 3)
	{
		pSize += 4 - (pSize & 3);
	}
	WriteInt32(&buff[0], 6);
	WriteInt32(&buff[4], (Int32)pSize);
	WriteInt32(&buff[8], 0);
	WriteInt32(&buff[12], (Int32)(t >> 32));
	WriteInt32(&buff[16], (Int32)(t & 0xffffffff));
	WriteInt32(&buff[20], (Int32)packet.GetSize());
	WriteInt32(&buff[24], (Int32)packet.GetSize());
	Sync::MutexUsage mutUsage(this->mut);
	Bool succ = (this->fs.Write(buff, 28) == 28);
	succ = succ && (this->fs.Write(packet.Ptr(), packet.GetSize()) == packet.GetSize());
	if ((packet.GetSize() & 3) == 0)
	{
		succ = succ && (this->fs.Write(&buff[4], 4) == 4);
	}
	else if ((packet.GetSize() & 3) == 1)
	{
		buff[1] = 0;
		buff[2] = 0;
		buff[3] = 0;
		succ = succ && (this->fs.Write(&buff[1], 7) == 7);
	}
	else if ((packet.GetSize() & 3) == 2)
	{
		buff[2] = 0;
		buff[3] = 0;
		succ = succ && (this->fs.Write(&buff[2], 6) == 6);
	}
	else if ((packet.GetSize() & 3) == 3)
	{
		buff[3] = 0;
		succ = succ && (this->fs.Write(&buff[3], 5) == 5);
	}
	return succ;
}