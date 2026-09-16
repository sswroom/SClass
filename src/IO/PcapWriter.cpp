#include "Stdafx.h"
#include "IO/PcapWriter.h"
#include "Sync/MutexUsage.h"

IO::PcapWriter::PcapWriter(Text::CStringNN fileName, IO::PacketAnalyse::LinkType linkType) : fs(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal)
{
	UInt8 buff[24];
	WriteLUInt32(&buff[0], 0xa1b2c3d4); //magic number
	WriteLInt16(&buff[4], 2); //version_major
	WriteLInt16(&buff[6], 4); //version_minor
	WriteLInt32(&buff[8], 0); //thiszone
	WriteLInt32(&buff[12], 0); //sigfigs
	WriteLInt32(&buff[16], 65536); //snaplen
	WriteLInt32(&buff[20], (Int32)linkType); //network
	this->fs.Write(Data::ByteArrayR(buff, 24));
}

IO::PcapWriter::~PcapWriter()
{
}

Bool IO::PcapWriter::IsError() const
{
	return this->fs.IsError();
}

Bool IO::PcapWriter::WritePacket(Data::ByteArrayR packet)
{
	UInt8 buff[16];
	Data::Timestamp ts = Data::Timestamp::UtcNow();
	WriteLInt32(&buff[0], (Int32)(ts.inst.sec));
	WriteLInt32(&buff[4], (Int32)(ts.inst.nanosec / 1000));
	WriteLInt32(&buff[8], (Int32)packet.GetSize());
	WriteLInt32(&buff[12], (Int32)packet.GetSize());
	Sync::MutexUsage mutUsage(this->mut);
	Bool succ;
	succ = (this->fs.Write(Data::ByteArrayR(buff, 16)) == 16);
	succ = succ && (this->fs.Write(packet) == packet.GetSize());
	return succ;
}
