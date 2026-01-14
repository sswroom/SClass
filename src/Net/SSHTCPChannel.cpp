#include "Stdafx.h"
#include "Net/SSHTCPChannel.h"

Net::SSHTCPChannel::SSHTCPChannel(NN<Net::SSHConn> conn, NN<SSHChannelHandle> channel, Text::CStringNN channelName) : IO::Stream(channelName)
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
	return this->channel.IsNull();
}

UOSInt Net::SSHTCPChannel::Read(const Data::ByteArray &buff)
{
	NN<Net::TCPClient> cli;
	NN<SSHChannelHandle> channel;
	UOSInt readSize = 0;
	if (this->channel.SetTo(channel))
	{
		while (!this->conn->ChannelTryRead(channel, buff.Arr(), buff.GetSize(), readSize))
		{
			if (!this->conn->GetTCPClient().SetTo(cli))
				return 0;
			cli->Wait(1000);
		}
	}
	return readSize;
}

UOSInt Net::SSHTCPChannel::Write(Data::ByteArrayR buff)
{
	NN<SSHChannelHandle> channel;
	if (this->channel.SetTo(channel))
	{
		return this->conn->ChannelWrite(channel, buff.Arr(), buff.GetSize());
	}
	return 0;
}

Int32 Net::SSHTCPChannel::Flush()
{
	return 0;
}

void Net::SSHTCPChannel::Close()
{
	NN<SSHChannelHandle> channel;
	if (this->channel.SetTo(channel))
	{
		this->conn->ChannelClose(channel);
		this->channel = nullptr;
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
	NN<SSHChannelHandle> channel;
	if (this->channel.SetTo(channel))
	{
		return this->conn->ChannelTryRead(channel, buff, maxSize, size);
	}
	else
	{
		size.Set(0);
		return true;
	}
}
