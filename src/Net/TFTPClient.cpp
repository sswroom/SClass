#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Net/TFTPClient.h"
#include "Text/MyString.h"

void __stdcall Net::TFTPClient::OnDataPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData)
{
	NN<Net::TFTPClient> me = userData.GetNN<Net::TFTPClient>();
	UInt16 opcode = ReadMUInt16(&data[0]);
	if (opcode == 4)
	{
		if (ReadMUInt16(&data[2]) == me->nextId)
		{
			if (me->nextId == 0)
			{
				me->recvPort = port;
			}
			me->replyRecv = true;
			me->evt.Set();
		}
	}
	else if (opcode == 3)
	{
		NN<IO::Stream> stm;
		if (ReadMUInt16(&data[2]) == me->nextId && !me->replyRecv && me->recvStm.SetTo(stm))
		{
			if (me->nextId == 1)
			{
				me->recvPort = port;
			}
			me->recvSize = data.GetSize() - 4;
			stm->Write(Data::ByteArrayR(&data[4], data.GetSize() - 4));
			me->replyRecv = true;
			me->evt.Set();
		}
	}
	else if (opcode == 5)
	{
		me->replyError = true;
		me->replyRecv = true;
		me->evt.Set();
	}
}

Net::TFTPClient::TFTPClient(NN<Net::SocketFactory> sockf, NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, NN<IO::LogTool> log) : evt(true)
{
	this->addr = addr.Ptr()[0];
	this->port = port;
	this->recvPort = port;
	this->recvStm = nullptr;
	this->recvSize = 0;
	this->replyRecv = false;
	this->nextId = 0;
	NN<Net::UDPServer> svr;
	NEW_CLASSNN(svr, Net::UDPServer(sockf, nullptr, 0, nullptr, OnDataPacket, this, log, nullptr, 2, false));
	if (svr->IsError())
	{
		svr.Delete();
		this->svr = nullptr;
	}
	else
	{
		this->svr = svr;
	}
}

Net::TFTPClient::~TFTPClient()
{
	this->svr.Delete();
}

Bool Net::TFTPClient::IsError()
{
	return this->svr.IsNull();
}

Bool Net::TFTPClient::SendFile(UnsafeArray<const UTF8Char> fileName, NN<IO::Stream> stm)
{
	NN<Net::UDPServer> svr;
	if (!this->svr.SetTo(svr))
		return false;
	UInt8 *packet;
	UIntOS readSize;
	UIntOS blockSize = 512;
	Bool succ = false;
	packet = MemAlloc(UInt8, blockSize + 4);
	WriteMInt16(&packet[0], 2);
	readSize = (UIntOS)(Text::StrConcat(&packet[2], fileName) - packet + 1);
	readSize = (UIntOS)(Text::StrConcatC(&packet[readSize], UTF8STRC("octet")) - packet + 1);
	this->replyRecv = false;
	this->replyError = false;
	this->nextId = 0;
	this->evt.Clear();
	svr->SendTo(this->addr, this->port, packet, readSize);
	this->evt.Wait(10000);
	if (this->replyRecv && !this->replyError)
	{
		while (true)
		{
			this->nextId++;
			this->replyRecv = false;
			WriteMInt16(&packet[0], 3);
			WriteMInt16(&packet[2], this->nextId);
			readSize = stm->Read(Data::ByteArray(&packet[4], blockSize));
			this->evt.Clear();
			svr->SendTo(this->addr, this->recvPort, packet, readSize + 4);
			this->evt.Wait(10000);
			if (!this->replyRecv || this->replyError)
			{
				break;
			}
			if (readSize != blockSize)
			{
				succ = true;
				break;
			}
		}
	}
	MemFree(packet);
	return succ;
}

Bool Net::TFTPClient::RecvFile(UnsafeArray<const UTF8Char> fileName, NN<IO::Stream> stm)
{
	NN<Net::UDPServer> svr;
	if (!this->svr.SetTo(svr))
		return false;
	UInt8 packet[512];
	UIntOS readSize;
	UIntOS blockSize = 512;
	Bool succ = false;
	WriteMInt16(&packet[0], 1);
	readSize = (UIntOS)(Text::StrConcat(&packet[2], fileName) - packet + 1);
	readSize = (UIntOS)(Text::StrConcatC(&packet[readSize], UTF8STRC("octet")) - packet + 1);
	this->replyRecv = false;
	this->replyError = false;
	this->recvSize = 0;
	this->recvStm = stm;
	this->nextId = 1;
	this->evt.Clear();
	svr->SendTo(this->addr, this->port, packet, readSize);
	this->evt.Wait(10000);
	if (this->replyRecv && !this->replyError)
	{
		while (true)
		{
			this->replyRecv = false;
			WriteMInt16(&packet[0], 4);
			WriteMInt16(&packet[2], this->nextId);
			this->nextId++;
			this->evt.Clear();
			svr->SendTo(this->addr, this->recvPort, packet, 4);
			if (this->recvSize != blockSize)
			{
				succ = true;
				break;
			}
			this->evt.Wait(10000);
			if (!this->replyRecv || this->replyError)
			{
				break;
			}
		}
	}
	this->recvStm = nullptr;
	return succ;
}
