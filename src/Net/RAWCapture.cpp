#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/OS.h"
#include "IO/SystemInfo.h"
#include "Net/RAWCapture.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"

void __stdcall Net::RAWCapture::DataHandler(void *userData, const UInt8 *packetData, UOSInt packetSize)
{
	Net::RAWCapture *me = (Net::RAWCapture*)userData;
	UInt8 buff[256];
	Sync::MutexUsage mutUsage(me->mut);
	me->packetCnt++;
	me->dataSize += packetSize;
	if (me->format == FF_PCAP)
	{
		Data::DateTime dt;
		Int64 t;
		dt.SetCurrTimeUTC();
		t = dt.ToTicks();
		WriteInt32(&buff[0], (Int32)(t / 1000));
		WriteInt32(&buff[4], (Int32)((t % 1000) * 1000));
		WriteInt32(&buff[8], (Int32)packetSize);
		WriteInt32(&buff[12], (Int32)packetSize);
		me->fs->Write(buff, 16);
		me->fs->Write(packetData, packetSize);
	}
	else if (me->format == FF_PCAPNG)
	{
		Data::DateTime dt;
		Int64 t;
		dt.SetCurrTimeUTC();
		t = dt.ToTicks();
		UOSInt pSize = packetSize + 32;
		if (pSize & 3)
		{
			pSize += 4 - (pSize & 3);
		}
		WriteInt32(&buff[0], 6);
		WriteInt32(&buff[4], (Int32)pSize);
		WriteInt32(&buff[8], 0);
		WriteInt32(&buff[12], (Int32)(t >> 32));
		WriteInt32(&buff[16], (Int32)(t & 0xffffffff));
		WriteInt32(&buff[20], (Int32)packetSize);
		WriteInt32(&buff[24], (Int32)packetSize);
		me->fs->Write(buff, 28);
		me->fs->Write(packetData, packetSize);
		if ((packetSize & 3) == 0)
		{
			me->fs->Write(&buff[4], 4);
		}
		else if ((packetSize & 3) == 1)
		{
			buff[1] = 0;
			buff[2] = 0;
			buff[3] = 0;
			me->fs->Write(&buff[1], 7);
		}
		else if ((packetSize & 3) == 2)
		{
			buff[2] = 0;
			buff[3] = 0;
			me->fs->Write(&buff[2], 6);
		}
		else if ((packetSize & 3) == 3)
		{
			buff[3] = 0;
			me->fs->Write(&buff[3], 5);
		}
	}
	mutUsage.EndUse();
}

Net::RAWCapture::RAWCapture(NotNullPtr<Net::SocketFactory> sockf, UInt32 adapterIP, CaptureType type, FileFormat format, Text::CStringNN fileName, Text::CString appName)
{
	if (appName.leng == 0)
	{
		appName = CSTR("RAWCapture");
	}
	this->sockf = sockf;
	this->socMon = 0;
	this->format = format;
	this->fs = 0;
	this->packetCnt = 0;
	this->dataSize = 0;
	UInt8 buff[256];

	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
	}
	else
	{
		this->fs = fs;

		if (format == FF_PCAP)
		{
			WriteUInt32(&buff[0], 0xa1b2c3d4); //magic number
			WriteInt16(&buff[4], 2); //version_major
			WriteInt16(&buff[6], 4); //version_minor
			WriteInt32(&buff[8], 0); //thiszone
			WriteInt32(&buff[12], 0); //sigfigs
			WriteInt32(&buff[16], 65536); //snaplen
			WriteInt32(&buff[20], CaptureTypeGetLinkType(type)); //network
			this->fs->Write(buff, 24);
		}
		else if (format == FF_PCAPNG)
		{
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

			this->fs->Write(buff, (UOSInt)i);

			WriteInt32(&buff[0], 1); //Block Type = IDB
			WriteInt32(&buff[4], 0); //Block Size
			WriteInt32(&buff[8], CaptureTypeGetLinkType(type)); //Link Type + Reserved
			WriteInt32(&buff[12], 65536); //snap len
			WriteInt16(&buff[16], 2);
			WriteInt16(&buff[18], 3);
			Text::StrConcatC(&buff[20], UTF8STRC("any"));
			WriteInt16(&buff[24], 9);
			WriteInt16(&buff[26], 1);
			WriteInt32(&buff[28], 3);
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

			this->fs->Write(buff, (UOSInt)i);
		}

		Socket *s = 0;
		switch (type)
		{
		case CT_RAW:
			s = sockf->CreateRAWSocket();
			break;
		case CT_IPV4:
			s = sockf->CreateRAWIPv4Socket(adapterIP);
			break;
		case CT_UDPV4:
			s = sockf->CreateUDPRAWv4Socket(adapterIP);
			break;
		case CT_ICMPV4:
			s = sockf->CreateICMPIPv4Socket(adapterIP);
			break;
		}
		if (s)
		{
			NEW_CLASS(this->socMon, Net::SocketMonitor(sockf, s, DataHandler, this, 4));
		}
	}
}

Net::RAWCapture::~RAWCapture()
{
	SDEL_CLASS(this->socMon);
	SDEL_CLASS(this->fs);
}

Bool Net::RAWCapture::IsError()
{
	return this->socMon == 0;
}

UInt64 Net::RAWCapture::GetPacketCnt()
{
	return this->packetCnt;
}

UInt64 Net::RAWCapture::GetDataSize()
{
	return this->dataSize;
}

Text::CString Net::RAWCapture::CaptureTypeGetName(CaptureType type)
{
	switch (type)
	{
	case CT_RAW:
		return CSTR("RAW");
	case CT_IPV4:
		return CSTR("IPv4");
	case CT_UDPV4:
		return CSTR("UDPv4");
	case CT_ICMPV4:
		return CSTR("ICMPv4");
	}
	return CSTR("Unknown");
}

Int32 Net::RAWCapture::CaptureTypeGetLinkType(CaptureType type)
{
	switch (type)
	{
	case CT_RAW:
		return 1;
	case CT_IPV4:
		return 101;
	case CT_UDPV4:
		return 101;
	case CT_ICMPV4:
		return 101;
	}
	return 0;
}

Text::CString Net::RAWCapture::FileFormatGetName(FileFormat format)
{
	switch (format)
	{
	case FF_PCAP:
		return CSTR("pcap");
	case FF_PCAPNG:
		return CSTR("pcapng");
	}
	return CSTR("Unknown");
}

Text::CString Net::RAWCapture::FileFormatGetExt(FileFormat format)
{
	switch (format)
	{
	case FF_PCAP:
		return CSTR("pcap");
	case FF_PCAPNG:
		return CSTR("pcapng");
	}
	return CSTR("");
}

void Net::RAWCapture::AddFilters(IO::FileSelector *selector)
{
	selector->AddFilter(CSTR("*.pcap"), CSTR("pcap"));
	selector->AddFilter(CSTR("*.pcapng"), CSTR("pcapng"));
}
