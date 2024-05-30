#include "Stdafx.h"
#include "Net/SSHTCPChannel.h"

Net::SSHTCPChannel::SSHTCPChannel(NN<Net::SSHConn> conn, SSHChannelHandle *channel, Text::CStringNN channelName) : IO::Stream(channelName)
{
	this->conn = conn;
	this->channel = channel;
}

Net::SSHTCPChannel::~SSHTCPChannel()
{
	this->Close();
}

Bool Net::SSHTCPChannel::IsDown() const
{
	return this->channel == 0;
}

UOSInt Net::SSHTCPChannel::Read(const Data::ByteArray &buff)
{
	NN<Net::TCPClient> cli;
	UOSInt readSize = 0;
	while (!this->conn->ChannelTryRead(this->channel, buff.Arr(), buff.GetSize(), readSize))
	{
		if (!this->conn->GetTCPClient().SetTo(cli))
			return 0;
		cli->Wait(1000);
	}
	return readSize;
}

UOSInt Net::SSHTCPChannel::Write(UnsafeArray<const UInt8> buff, UOSInt size)
{
	if (this->channel)
	{
		return this->conn->ChannelWrite(this->channel, buff, size);
	}
	return 0;
}

Int32 Net::SSHTCPChannel::Flush()
{
	return 0;
}

void Net::SSHTCPChannel::Close()
{
	if (this->channel)
	{
		this->conn->ChannelClose(this->channel);
		this->channel = 0;
	}
}

Bool Net::SSHTCPChannel::Recover()
{
	return false;
}

IO::StreamType Net::SSHTCPChannel::GetStreamType() const
{
	return IO::StreamType::SSHTCPChannel;
}

Bool Net::SSHTCPChannel::TryRead(UInt8 *buff, UOSInt maxSize, OutParam<UOSInt> size)
{
	if (this->channel)
	{
		return this->conn->ChannelTryRead(this->channel, buff, maxSize, size);
	}
	else
	{
		size.Set(0);
		return true;
	}
}
