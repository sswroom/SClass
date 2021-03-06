#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Net/TFTPClient.h"
#include "Text/MyString.h"

void __stdcall Net::TFTPClient::OnDataPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData)
{
	Net::TFTPClient *me = (Net::TFTPClient*)userData;
	UInt16 opcode = ReadMUInt16(buff);
	if (opcode == 4)
	{
		if (ReadMUInt16(&buff[2]) == me->nextId)
		{
			if (me->nextId == 0)
			{
				me->recvPort = port;
			}
			me->replyRecv = true;
			me->evt->Set();
		}
	}
	else if (opcode == 3)
	{
		if (ReadMUInt16(&buff[2]) == me->nextId && !me->replyRecv && me->recvStm)
		{
			if (me->nextId == 1)
			{
				me->recvPort = port;
			}
			me->recvSize = dataSize - 4;
			me->recvStm->Write(&buff[4], dataSize - 4);
			me->replyRecv = true;
			me->evt->Set();
		}
	}
	else if (opcode == 5)
	{
		me->replyError = true;
		me->replyRecv = true;
		me->evt->Set();
	}
}

Net::TFTPClient::TFTPClient(Net::SocketFactory *sockf, const Net::SocketUtil::AddressInfo *addr, UInt16 port)
{
	this->addr = *addr;
	this->port = port;
	this->recvPort = port;
	this->recvStm = 0;
	this->recvSize = 0;
	this->replyRecv = false;
	this->nextId = 0;
	NEW_CLASS(this->evt, Sync::Event(true, (const UTF8Char*)"Net.TFTPClient.evt"));
	NEW_CLASS(this->svr, Net::UDPServer(sockf, 0, 0, 0, OnDataPacket, this, 0, 0, 2, false));
	if (this->svr->IsError())
	{
		SDEL_CLASS(this->svr);
	}
}

Net::TFTPClient::~TFTPClient()
{
	SDEL_CLASS(this->svr);
	DEL_CLASS(this->evt);
}

Bool Net::TFTPClient::IsError()
{
	return this->svr == 0;
}

Bool Net::TFTPClient::SendFile(const UTF8Char *fileName, IO::Stream *stm)
{
	if (this->svr == 0)
		return false;
	UInt8 *packet;
	UOSInt readSize;
	UOSInt blockSize = 512;
	Bool succ = false;
	packet = MemAlloc(UInt8, blockSize + 4);
	WriteMInt16(&packet[0], 2);
	readSize = (UOSInt)(Text::StrConcat(&packet[2], fileName) - packet + 1);
	readSize = (UOSInt)(Text::StrConcat(&packet[readSize], (const UTF8Char*)"octet") - packet + 1);
	this->replyRecv = false;
	this->replyError = false;
	this->nextId = 0;
	this->evt->Clear();
	this->svr->SendTo(&this->addr, this->port, packet, readSize);
	this->evt->Wait(10000);
	if (this->replyRecv && !this->replyError)
	{
		while (true)
		{
			this->nextId++;
			this->replyRecv = false;
			WriteMInt16(&packet[0], 3);
			WriteMInt16(&packet[2], this->nextId);
			readSize = stm->Read(&packet[4], blockSize);
			this->evt->Clear();
			this->svr->SendTo(&this->addr, this->recvPort, packet, readSize + 4);
			this->evt->Wait(10000);
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

Bool Net::TFTPClient::RecvFile(const UTF8Char *fileName, IO::Stream *stm)
{
	if (this->svr == 0)
		return false;
	UInt8 packet[512];
	UOSInt readSize;
	UOSInt blockSize = 512;
	Bool succ = false;
	WriteMInt16(&packet[0], 1);
	readSize = (UOSInt)(Text::StrConcat(&packet[2], fileName) - packet + 1);
	readSize = (UOSInt)(Text::StrConcat(&packet[readSize], (const UTF8Char*)"octet") - packet + 1);
	this->replyRecv = false;
	this->replyError = false;
	this->recvSize = 0;
	this->recvStm = stm;
	this->nextId = 1;
	this->evt->Clear();
	this->svr->SendTo(&this->addr, this->port, packet, readSize);
	this->evt->Wait(10000);
	if (this->replyRecv && !this->replyError)
	{
		while (true)
		{
			this->replyRecv = false;
			WriteMInt16(&packet[0], 4);
			WriteMInt16(&packet[2], this->nextId);
			this->nextId++;
			this->evt->Clear();
			this->svr->SendTo(&this->addr, this->recvPort, packet, 4);
			if (this->recvSize != blockSize)
			{
				succ = true;
				break;
			}
			this->evt->Wait(10000);
			if (!this->replyRecv || this->replyError)
			{
				break;
			}
		}
	}
	this->recvStm = 0;
	return succ;
}
