#include "Stdafx.h"
#include "IO/SMTCWriter.h"
#include "Sync/MutexUsage.h"

IO::SMTCWriter::SMTCWriter(Text::CStringNN fileName) : fs(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal)
{
	this->fs.Write(UTF8STRC("SmTC"));
}

IO::SMTCWriter::~SMTCWriter()
{
}

Bool IO::SMTCWriter::IsError() const
{
	return this->fs.IsError();
}

void IO::SMTCWriter::TCPConnect(NN<Net::TCPClient> cli)
{
	Data::Timestamp ts = Data::Timestamp::UtcNow();
	UInt8 buff[21];
	WriteInt64(&buff[0], ts.inst.sec);
	WriteUInt32(&buff[8], ts.inst.nanosec);
	WriteUInt64(&buff[12], cli->GetCliId());
	buff[20] = 0;
	Sync::MutexUsage mutUsage(this->mut);
	this->fs.Write(buff, 21);
}

void IO::SMTCWriter::TCPDisconnect(NN<Net::TCPClient> cli)
{
	Data::Timestamp ts = Data::Timestamp::UtcNow();
	UInt8 buff[21];
	WriteInt64(&buff[0], ts.inst.sec);
	WriteUInt32(&buff[8], ts.inst.nanosec);
	WriteUInt64(&buff[12], cli->GetCliId());
	buff[20] = 1;
	Sync::MutexUsage mutUsage(this->mut);
	this->fs.Write(buff, 21);
}

void IO::SMTCWriter::TCPSend(NN<Net::TCPClient> cli, UnsafeArray<const UInt8> data, UOSInt size)
{
	Data::Timestamp ts = Data::Timestamp::UtcNow();
	UInt8 buff[23];
	WriteInt64(&buff[0], ts.inst.sec);
	WriteUInt32(&buff[8], ts.inst.nanosec);
	WriteUInt64(&buff[12], cli->GetCliId());
	buff[20] = 3;
	WriteUInt16(&buff[21], (UInt16)size);
	Sync::MutexUsage mutUsage(this->mut);
	this->fs.Write(buff, 23);
	this->fs.Write(data, size);
}

void IO::SMTCWriter::TCPRecv(NN<Net::TCPClient> cli, UnsafeArray<const UInt8> data, UOSInt size)
{
	Data::Timestamp ts = Data::Timestamp::UtcNow();
	UInt8 buff[23];
	WriteInt64(&buff[0], ts.inst.sec);
	WriteUInt32(&buff[8], ts.inst.nanosec);
	WriteUInt64(&buff[12], cli->GetCliId());
	buff[20] = 2;
	WriteUInt16(&buff[21], (UInt16)size);
	Sync::MutexUsage mutUsage(this->mut);
	this->fs.Write(buff, 23);
	this->fs.Write(data, size);
}
