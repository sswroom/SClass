#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/OS.h"
#include "IO/PcapngWriter.h"
#include "IO/PcapWriter.h"
#include "IO/SystemInfo.h"
#include "Net/RAWCapture.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"

void __stdcall Net::RAWCapture::DataHandler(AnyType userData, const UInt8 *packetData, UOSInt packetSize)
{
	NN<Net::RAWCapture> me = userData.GetNN<Net::RAWCapture>();
	{
		Sync::MutexUsage mutUsage(me->mut);
		me->packetCnt++;
		me->dataSize += packetSize;
	}
	if (me->writer)
	{
		me->writer->WritePacket(Data::ByteArrayR(packetData, packetSize));
	}
}

Net::RAWCapture::RAWCapture(NN<Net::SocketFactory> sockf, UInt32 adapterIP, CaptureType type, FileFormat format, Text::CStringNN fileName, Text::CString appName)
{
	if (appName.leng == 0)
	{
		appName = CSTR("RAWCapture");
	}
	this->sockf = sockf;
	this->socMon = 0;
	this->writer = 0;
	this->packetCnt = 0;
	this->dataSize = 0;
	if (format == FF_PCAP)
	{
		NEW_CLASS(this->writer, IO::PcapWriter(fileName, CaptureTypeGetLinkType(type)))
	}
	else if (format == FF_PCAPNG)
	{
		NEW_CLASS(this->writer, IO::PcapngWriter(fileName, CaptureTypeGetLinkType(type), appName.OrEmpty()))
	}
	if (this->writer && this->writer->IsError())
	{
		DEL_CLASS(this->writer);
		this->writer = 0;
	}
	if (this->writer)
	{
		Optional<Socket> s = 0;
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
		NN<Socket> nns;
		if (s.SetTo(nns))
		{
			NEW_CLASS(this->socMon, Net::SocketMonitor(sockf, nns, DataHandler, this, 4));
		}
	}
}

Net::RAWCapture::~RAWCapture()
{
	SDEL_CLASS(this->socMon);
	SDEL_CLASS(this->writer);
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

Text::CStringNN Net::RAWCapture::CaptureTypeGetName(CaptureType type)
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

Text::CStringNN Net::RAWCapture::FileFormatGetName(FileFormat format)
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

Text::CStringNN Net::RAWCapture::FileFormatGetExt(FileFormat format)
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

void Net::RAWCapture::AddFilters(NN<IO::FileSelector> selector)
{
	selector->AddFilter(CSTR("*.pcap"), CSTR("pcap"));
	selector->AddFilter(CSTR("*.pcapng"), CSTR("pcapng"));
}
