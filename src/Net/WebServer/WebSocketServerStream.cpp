#include "Stdafx.h"
#include "Net/WebServer/WebSocketServerStream.h"
#include "Sync/MutexUsage.h"

Bool Net::WebServer::WebSocketServerStream::SendPacket(UInt8 opcode, Data::ByteArrayR buff)
{
	UInt8 packetBuff[130];
	Sync::MutexUsage mutUsage(this->sendMut);
	if (buff.GetSize() < 126)
	{
		packetBuff[0] = (UInt8)(0x80 | (opcode));
		packetBuff[1] = (UInt8)buff.GetSize();
		MemCopyNO(&packetBuff[2], buff.Ptr(), buff.GetSize());
		return this->resp->Write(Data::ByteArrayR(packetBuff, buff.GetSize() + 2)) == (buff.GetSize() + 2);
	}
	else if (buff.GetSize() < 65536)
	{
		UInt8 *packBuff = MemAlloc(UInt8, buff.GetSize() + 8);
		packBuff[0] = (UInt8)(0x80 | (opcode));
		packBuff[1] = 0x7E;
		WriteMUInt16(&packBuff[2], (UInt16)buff.GetSize());
		MemCopyNO(&packBuff[4], buff.Ptr(), buff.GetSize());
		Bool succ = this->resp->Write(Data::ByteArrayR(packetBuff, buff.GetSize() + 4)) == (buff.GetSize() + 4);
		MemFree(packBuff);
		return succ;
	}
	else
	{
		UInt8 *packBuff = MemAlloc(UInt8, buff.GetSize() + 10);
		packBuff[0] = (UInt8)(0x80 | (opcode));
		packBuff[1] = 0x7F;
		WriteMUInt64(&packBuff[2], buff.GetSize());
		MemCopyNO(&packBuff[10], buff.Ptr(), buff.GetSize());
		Bool succ = this->resp->Write(Data::ByteArrayR(packetBuff, buff.GetSize() + 10)) == (buff.GetSize() + 10);
		MemFree(packBuff);
		return succ;
	}
}

void Net::WebServer::WebSocketServerStream::NextPacket(UInt8 opcode, UnsafeArray<const UInt8> buff, UIntOS buffSize)
{
	switch (opcode & 15)
	{
	case 0: //continuation frame
	case 1: //text frame
	case 2: //binary frame
		return this->stmHdlr->StreamData(*this, this->stmData, Data::ByteArrayR(buff, buffSize));
	case 8: //connection close
		this->resp->ShutdownSend();
		break;
	case 9: //Ping
		this->NextPacket(10, buff, buffSize);
		break;
	case 10: //Pong
		break;
	default:
		break;
	}
}

Net::WebServer::WebSocketServerStream::WebSocketServerStream(NN<IO::StreamHandler> stmHdlr, NN<Net::WebServer::WebResponse> resp) : IO::Stream(CSTR("WebSocket"))
{
	this->stmHdlr = stmHdlr;
	this->resp = resp;
	this->stmData = this->stmHdlr->StreamCreated(*this);

	this->recvCapacity = 4096;
	this->recvBuff = MemAllocArr(UInt8, this->recvCapacity);
	this->recvSize = 0;
}

Net::WebServer::WebSocketServerStream::~WebSocketServerStream()
{
	MemFreeArr(this->recvBuff);
}

Bool Net::WebServer::WebSocketServerStream::IsDown() const
{
	return false;
}

UIntOS Net::WebServer::WebSocketServerStream::Read(const Data::ByteArray &buff)
{
	return 0;
}

UIntOS Net::WebServer::WebSocketServerStream::Write(Data::ByteArrayR buff)
{
	if (this->SendPacket(2, buff))
		return buff.GetSize();
	return 0;
}

Int32 Net::WebServer::WebSocketServerStream::Flush()
{
	return 0;
}

void Net::WebServer::WebSocketServerStream::Close()
{
	this->SendPacket(8, Data::ByteArrayR(U8STR(""), 0));
}

Bool Net::WebServer::WebSocketServerStream::Recover()
{
	return false;
}

IO::StreamType Net::WebServer::WebSocketServerStream::GetStreamType() const
{
	return IO::StreamType::Unknown;
}

void Net::WebServer::WebSocketServerStream::ProtocolData(UnsafeArray<const UInt8> data, UIntOS dataSize)
{
	if (this->recvSize + dataSize > this->recvCapacity)
	{
		while (this->recvSize + dataSize > this->recvCapacity)
		{
			this->recvCapacity <<= 1;
		}
		UnsafeArray<UInt8> newBuff = MemAllocArr(UInt8, this->recvCapacity);
		if (this->recvSize > 0)
		{
			MemCopyNO(&newBuff[0], &this->recvBuff[0], this->recvSize);
		}
		MemFreeArr(this->recvBuff);
		this->recvBuff = newBuff;
	}
	MemCopyNO(&this->recvBuff[this->recvSize], &data[0], dataSize);
	this->recvSize += dataSize;

	UInt8 pkSize;
	UInt64 usedSize;
	UIntOS ofst;
	UIntOS parseOfst = 0;
	while (this->recvSize - parseOfst >= 2)
	{
		pkSize = this->recvBuff[parseOfst + 1] & 127;
		if (pkSize == 126)
		{
			ofst = parseOfst + 4;
			if (this->recvSize - parseOfst >= 4)
			{
				usedSize = (UInt64)(ReadMUInt16(&this->recvBuff[parseOfst + 2]) + 4);
			}
			else
			{
				usedSize = 130;
			}
		}
		else if (pkSize == 127)
		{
			ofst = parseOfst + 10;
			if (this->recvSize - parseOfst >= 4)
			{
				usedSize = ReadMUInt64(&this->recvBuff[parseOfst + 2]) + 10;
			}
			else
			{
				usedSize = 136;
			}
		}
		else
		{
			usedSize = (UIntOS)pkSize + 2;
			ofst = parseOfst + 2;
		}
		if (this->recvBuff[parseOfst + 1] & 0x80)
		{
			usedSize += 4;
			if (usedSize <= this->recvSize - parseOfst)
			{
				UIntOS pSize = parseOfst + usedSize - ofst - 4;
				UInt8 *buff = &this->recvBuff[ofst];
				UInt32 mask = ReadNUInt32(buff);
				buff += 4;
				UIntOS i = pSize;
				while (i >= 4)
				{
					WriteNUInt32(buff, mask ^ ReadNUInt32(buff));
					buff += 4;
					i -= 4;
				}
				switch (i)
				{
				case 3:
					buff[2] = this->recvBuff[ofst + 2] ^ buff[2];
				case 2:
					buff[1] = this->recvBuff[ofst + 1] ^ buff[1];
				case 1:
					buff[0] = this->recvBuff[ofst] ^ buff[0];
					break;
				}
				this->NextPacket(this->recvBuff[parseOfst], &this->recvBuff[ofst + 4], pSize);
				parseOfst += usedSize;
			}
			else
			{
				break;
			}
		}
		else
		{
			if (usedSize <= this->recvSize - parseOfst)
			{
				this->NextPacket(this->recvBuff[parseOfst], &this->recvBuff[ofst], parseOfst + usedSize - ofst);
				parseOfst += usedSize;
			}
			else
			{
				break;
			}
		}
	}
	if (parseOfst == this->recvSize)
	{
		this->recvSize = 0;
	}
	else if (parseOfst > 0)
	{
		MemCopyO(&this->recvBuff[0], &this->recvBuff[parseOfst], this->recvSize - parseOfst);
		this->recvSize -= parseOfst;
	}
}

void Net::WebServer::WebSocketServerStream::ConnectionClosed()
{
	this->stmHdlr->StreamClosed(*this, this->stmData);
	DEL_CLASS(this);
}
