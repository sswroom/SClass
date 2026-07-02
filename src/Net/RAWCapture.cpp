#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "IO/FileStream.h"
#include "IO/OS.h"
#include "IO/PcapngWriter.h"
#include "IO/PcapWriter.h"
#include "IO/SystemInfo.h"
#include "Net/RAWCapture.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"

void __stdcall Net::RAWCapture::DataHandler(AnyType userData, UnsafeArray<const UInt8> packetData, UIntOS packetSize)
{
	NN<Net::RAWCapture> me = userData.GetNN<Net::RAWCapture>();
	{
		Sync::MutexUsage mutUsage(me->mut);
		me->packetCnt++;
		me->dataSize += packetSize;
	}
	NN<IO::PacketLogWriter> writer;
	if (me->writer.SetTo(writer))
	{
		writer->WritePacket(Data::ByteArrayR(packetData, packetSize));
	}
}

Net::RAWCapture::RAWCapture(NN<Net::SocketFactory> sockf, UInt32 adapterIP, CaptureType type, FileFormat format, Text::CStringNN fileName, Text::CString appName)
{
	if (appName.leng == 0)
	{
		appName = CSTR("RAWCapture");
	}
	this->sockf = sockf;
	this->socMon = nullptr;
	this->writer = nullptr;
	this->packetCnt = 0;
	this->dataSize = 0;
	if (format == FF_PCAP)
	{
		NEW_CLASSOPT(this->writer, IO::PcapWriter(fileName, CaptureTypeGetLinkType(type)))
	}
	else if (format == FF_PCAPNG)
	{
		NEW_CLASSOPT(this->writer, IO::PcapngWriter(fileName, CaptureTypeGetLinkType(type), appName.OrEmpty()))
	}
	NN<IO::PacketLogWriter> writer;
	if (this->writer.SetTo(writer) && writer->IsError())
	{
		writer.Delete();
		this->writer = nullptr;
	}
	if (this->writer.NotNull())
	{
		Optional<Socket> s = nullptr;
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
			NEW_CLASSOPT(this->socMon, Net::SocketMonitor(sockf, nns, DataHandler, this, 4));
		}
	}
}

Net::RAWCapture::~RAWCapture()
{
	this->socMon.Delete();
	this->writer.Delete();
}

Bool Net::RAWCapture::IsError()
{
	return this->socMon.IsNull();
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

IO::PacketAnalyse::LinkType Net::RAWCapture::CaptureTypeGetLinkType(CaptureType type)
{
	switch (type)
	{
	case CT_RAW:
		return IO::PacketAnalyse::LinkType::Ethernet;
	case CT_IPV4:
		return IO::PacketAnalyse::LinkType::Linux;
	case CT_UDPV4:
		return IO::PacketAnalyse::LinkType::Linux;
	case CT_ICMPV4:
		return IO::PacketAnalyse::LinkType::Linux;
	}
	return IO::PacketAnalyse::LinkType::Null;
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
