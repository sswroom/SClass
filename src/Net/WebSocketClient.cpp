#include "Stdafx.h"
#include "Data/RandomBytesGenerator.h"
#include "Net/WebSocketClient.h"
#include "Text/TextBinEnc/Base64Enc.h"
#include "Sync/MutexUsage.h"

//#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

Bool Net::WebSocketClient::SendPacket(UInt8 opcode, const UInt8 *buff, UOSInt buffSize)
{
	if (this->cli == 0)
		return false;
	UInt8 packetBuff[130];
	Sync::MutexUsage mutUsage(this->sendMut);
	UInt32 mask;
	UOSInt i;
	if (buffSize < 126)
	{
		packetBuff[0] = (UInt8)(0x80 | (opcode));
		packetBuff[1] = (UInt8)(0x80 | buffSize);
		this->rand.NextBytes(&packetBuff[2], 4);
		mask = ReadNUInt32(&packetBuff[2]);
		i = 0;
		while (buffSize >= 4)
		{
			WriteNUInt32(&packetBuff[i + 6], (mask ^ ReadNUInt32(&buff[i])));
			buffSize -= 4;
			i += 4;
		}
		switch (buffSize)
		{
		case 1:
			packetBuff[i + 6] = packetBuff[2] ^ buff[i];
			i++;
			break;
		case 2:
			packetBuff[i + 6] = packetBuff[2] ^ buff[i];
			packetBuff[i + 7] = packetBuff[3] ^ buff[i + 1];
			i += 2;
			break;
		case 3:
			packetBuff[i + 6] = packetBuff[2] ^ buff[i];
			packetBuff[i + 7] = packetBuff[3] ^ buff[i + 1];
			packetBuff[i + 8] = packetBuff[4] ^ buff[i + 2];
			i += 3;
			break;
		}
		return this->cli->Write(packetBuff, i + 6) == (i + 6);
	}
	else if (buffSize < 65536)
	{
		UInt8 *packBuff = MemAlloc(UInt8, buffSize + 8);
		packBuff[0] = (UInt8)(0x80 | (opcode));
		packBuff[1] = 0xFE;
		WriteMUInt16(&packBuff[2], (UInt16)buffSize);
		this->rand.NextBytes(&packBuff[4], 4);
		mask = ReadNUInt32(&packBuff[4]);
		i = 0;
		while (buffSize >= 4)
		{
			WriteNUInt32(&packBuff[i + 8], (mask ^ ReadNUInt32(&buff[i])));
			buffSize -= 4;
			i += 4;
		}
		switch (buffSize)
		{
		case 1:
			packBuff[i + 8] = packBuff[4] ^ buff[i];
			i++;
			break;
		case 2:
			packBuff[i + 8] = packBuff[4] ^ buff[i];
			packBuff[i + 9] = packBuff[5] ^ buff[i + 1];
			i += 2;
			break;
		case 3:
			packBuff[i + 8] = packBuff[4] ^ buff[i];
			packBuff[i + 9] = packBuff[5] ^ buff[i + 1];
			packBuff[i + 10] = packBuff[6] ^ buff[i + 2];
			i += 3;
			break;
		}
		Bool succ = this->cli->Write(packBuff, i + 8) == (i + 8);
		MemFree(packBuff);
		return succ;
	}
	else
	{
		UInt8 *packBuff = MemAlloc(UInt8, buffSize + 14);
		packBuff[0] = (UInt8)(0x80 | (opcode));
		packBuff[1] = 0xFF;
		WriteMUInt64(&packBuff[2], buffSize);
		this->rand.NextBytes(&packBuff[10], 4);
		mask = ReadNUInt32(&packBuff[10]);
		i = 0;
		while (buffSize >= 4)
		{
			WriteNUInt32(&packBuff[i + 14], (mask ^ ReadNUInt32(&buff[i])));
			buffSize -= 4;
			i += 4;
		}
		switch (buffSize)
		{
		case 1:
			packBuff[i + 14] = packBuff[10] ^ buff[i];
			i++;
			break;
		case 2:
			packBuff[i + 14] = packBuff[10] ^ buff[i];
			packBuff[i + 15] = packBuff[11] ^ buff[i + 1];
			i += 2;
			break;
		case 3:
			packBuff[i + 14] = packBuff[10] ^ buff[i];
			packBuff[i + 15] = packBuff[11] ^ buff[i + 1];
			packBuff[i + 16] = packBuff[12] ^ buff[i + 2];
			i += 3;
			break;
		}
		Bool succ = this->cli->Write(packBuff, i + 14) == (i + 14);
		MemFree(packBuff);
		return succ;
	}
}

const UInt8 *Net::WebSocketClient::NextPacket(UInt8 *opcode, UOSInt *packetSize)
{
	UInt8 pkSize;
	UInt64 usedSize;
	UOSInt ofst;
	while (true)
	{
		if (this->recvSize - this->recvParseOfst >= 2)
		{
			pkSize = this->recvBuff[this->recvParseOfst + 1] & 127;
			if (pkSize == 126)
			{
				ofst = this->recvParseOfst + 4;
				if (this->recvSize - this->recvParseOfst >= 4)
				{
					usedSize = ReadMUInt16(&this->recvBuff[this->recvParseOfst + 2]) + 4;
				}
				else
				{
					usedSize = 130;
				}
			}
			else if (pkSize == 127)
			{
				ofst = this->recvParseOfst + 10;
				if (this->recvSize - this->recvParseOfst >= 4)
				{
					usedSize = ReadMUInt64(&this->recvBuff[this->recvParseOfst + 2]) + 10;
				}
				else
				{
					usedSize = 136;
				}
			}
			else
			{
				usedSize = (UOSInt)pkSize + 2;
				ofst = this->recvParseOfst + 2;
			}
			if (this->recvBuff[this->recvParseOfst + 1] & 0x80)
			{
				usedSize += 4;
				if (usedSize <= this->recvSize - this->recvParseOfst)
				{
					UOSInt pSize = this->recvParseOfst + usedSize - ofst - 4;
					*packetSize = pSize;
					UInt8 *buff = &this->recvBuff[ofst];
					UInt32 mask = ReadNUInt32(buff);
					buff += 4;
					UOSInt i = pSize;
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
					*opcode = this->recvBuff[this->recvParseOfst];
					this->recvParseOfst += usedSize;
					return &this->recvBuff[ofst + 4];
				}
			}
			else
			{
				if (usedSize <= this->recvSize - this->recvParseOfst)
				{
					*packetSize = this->recvParseOfst + usedSize - ofst;
					*opcode = this->recvBuff[this->recvParseOfst];
					this->recvParseOfst += usedSize;
					return &this->recvBuff[ofst];
				}
			}
			if (usedSize > this->recvCapacity)
			{
				while (usedSize > this->recvCapacity)
				{
					this->recvCapacity <<= 1;
				}
				UInt8 *newBuff = MemAlloc(UInt8, this->recvCapacity);
				MemCopyNO(newBuff, &this->recvBuff[this->recvParseOfst], this->recvSize - this->recvParseOfst);
				MemFree(this->recvBuff);
				this->recvBuff = newBuff;
				this->recvSize -= this->recvParseOfst;
				this->recvParseOfst = 0;
			}
		}	

		if (this->recvParseOfst != 0)
		{
			if (this->recvParseOfst == this->recvSize)
			{
				this->recvSize = 0;
				this->recvParseOfst = 0;
			}
			else
			{
				MemCopyO(this->recvBuff, &this->recvBuff[this->recvParseOfst], this->recvSize - this->recvParseOfst);
				this->recvSize -= this->recvParseOfst;
				this->recvParseOfst = 0;
			}
		}
		UOSInt readSize = this->cli->Read(Data::ByteArray(&this->recvBuff[this->recvSize], this->recvCapacity - this->recvSize));
		if (readSize == 0)
			return 0;
		this->recvSize += readSize;
	}
}

const UInt8 *Net::WebSocketClient::NextPacket(NotNullPtr<Sync::MutexUsage> mutUsage, UOSInt *packetSize)
{
	mutUsage->ReplaceMutex(this->recvMut);
	if (this->cli == 0)
		return 0;

	UInt8 opcode;
	const UInt8 *buff;
	while (true)
	{
		buff = NextPacket(&opcode, packetSize);
		if (buff == 0)
			return 0;
		switch (opcode & 15)
		{
		case 0: //continuation frame
		case 1: //text frame
		case 2: //binary frame
			return buff;
		case 8: //connection close
			if (this->cli)
			{
				this->cli->ShutdownSend();
			}
			break;
		case 9: //Ping
			this->SendPong(buff, *packetSize);
			break;
		case 10: //Pong
			break;
		default:
			return 0;
		}
	}
}

Net::WebSocketClient::WebSocketClient(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, Text::CString host, UInt16 port, Text::CString path, Text::CString origin, Protocol protocol, Data::Duration timeout) : Stream(CSTR("WebSocket"))
{
	this->recvCapacity = 4096;
	this->recvBuff = MemAlloc(UInt8, this->recvCapacity);
	this->recvParseOfst = 0;
	this->recvSize = 0;
	this->recvData = 0;
	this->recvDataCap = 0;
	this->recvDataOfst = 0;
	this->recvDataSize = 0;

	if (ssl)
	{
		Net::SSLEngine::ErrorType err;
		this->cli = ssl->ClientConnect(host, port, &err, timeout);
	}
	else
	{
		NEW_CLASS(this->cli, Net::TCPClient(sockf, host, port, timeout));
	}
	if (this->cli == 0)
	{
#if defined(VERBOSE)
		printf("cli = null\r\n");
#endif
	}
	else if (this->cli->IsConnectError())
	{
		DEL_CLASS(this->cli);
		this->cli = 0;
#if defined(VERBOSE)
		printf("Connect error\r\n");
#endif
	}
	else
	{
		this->cli->SetTimeout(8000);
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("GET "));
		sb.Append(path);
		sb.AppendC(UTF8STRC(" HTTP/1.1\r\n"));
		sb.AppendC(UTF8STRC("Host: "));
		sb.Append(host);
		if ((ssl && port != 443) || (ssl == 0 && port != 80))
		{
			sb.AppendUTF8Char(':');
			sb.AppendU16(port);
		}
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("User-Agent: SSWR_WebSocket/1.0\r\n"));
		sb.AppendC(UTF8STRC("Connection: Upgrade\r\n"));
		sb.AppendC(UTF8STRC("Sec-WebSocket-Key: "));
		Data::RandomBytesGenerator rand;
		rand.NextBytes(this->nonce, 16);
		Text::TextBinEnc::Base64Enc b64(Text::TextBinEnc::Base64Enc::Charset::Normal, false);
		b64.EncodeBin(&sb, this->nonce, 16);
		sb.AppendC(UTF8STRC("\r\n"));
		if (origin.leng > 0)
		{
			sb.AppendC(UTF8STRC("Origin: "));
			sb.Append(origin);
			sb.AppendC(UTF8STRC("\r\n"));
		}
		else
		{
			sb.AppendC(UTF8STRC("Origin: null\r\n"));
		}
		switch (protocol)
		{
		case Protocol::MQTT:
			sb.AppendC(UTF8STRC("Sec-WebSocket-Protocol: mqtt\r\n"));
			break;
		case Protocol::Chat:
		default:
			sb.AppendC(UTF8STRC("Sec-WebSocket-Protocol: chat\r\n"));
			break;
		}
		sb.AppendC(UTF8STRC("Sec-WebSocket-Version: 13\r\n"));
		sb.AppendC(UTF8STRC("Upgrade: websocket\r\n"));
		sb.AppendC(UTF8STRC("\r\n"));
#if defined(VERBOSE)
		printf("Data to send: %s\r\n", sb.v);
#endif
		if (this->cli->Write(sb.v, sb.leng) != sb.leng)
		{
#if defined(VERBOSE)
			printf("Error in writing to conn\r\n");
#endif
			DEL_CLASS(this->cli);
			this->cli = 0;
			return;
		}
		sb.ClearStr();
		sb.AllocLeng(4096);
		UOSInt readSize = this->cli->Read(Data::ByteArray(sb.v, 4096));
		if (readSize <= 0)
		{
#if defined(VERBOSE)
			printf("Error in reading data\r\n");
#endif
			DEL_CLASS(this->cli);
			this->cli = 0;
			return;
		}
		sb.v[readSize] = 0;
		sb.leng = readSize;
#if defined(VERBOSE)
		printf("Read: %s\r\n", sb.v);
#endif
		if (sb.StartsWith(UTF8STRC("HTTP/1.1 101 ")) && sb.EndsWith(UTF8STRC("\r\n\r\n")))
		{
			this->cli->SetTimeout(0);
		}
		else
		{
#if defined(VERBOSE)
			printf("Not valid WebSocket init response\r\n");
#endif
			DEL_CLASS(this->cli);
			this->cli = 0;
			return;
		}
	}
}

Net::WebSocketClient::~WebSocketClient()
{
	SDEL_CLASS(this->cli);
	MemFree(this->recvBuff);
	if (this->recvData)
	{
		MemFree(this->recvData);
	}
}

UTF8Char *Net::WebSocketClient::GetRemoteName(UTF8Char *buff) const
{
	if (this->cli)
	{
		return this->cli->GetRemoteName(buff);
	}
	*buff = 0;
	return 0;
}

UTF8Char *Net::WebSocketClient::GetLocalName(UTF8Char *buff) const
{
	if (this->cli)
	{
		return this->cli->GetLocalName(buff);
	}
	*buff = 0;
	return 0;
}

Bool Net::WebSocketClient::GetRemoteAddr(Net::SocketUtil::AddressInfo *addr) const
{
	if (this->cli)
	{
		return this->cli->GetRemoteAddr(addr);
	}
	addr->addrType = Net::AddrType::Unknown;
	return false;
}

UInt16 Net::WebSocketClient::GetRemotePort() const
{
	if (this->cli)
	{
		return this->cli->GetRemotePort();
	}
	return 0;
}

UInt16 Net::WebSocketClient::GetLocalPort() const
{
	if (this->cli)
	{
		return this->cli->GetLocalPort();
	}
	return 0;
}

Bool Net::WebSocketClient::IsDown() const
{
	if (this->cli)
		return this->cli->IsDown();
	return true;
}

UOSInt Net::WebSocketClient::Read(const Data::ByteArray &buff)
{
	Sync::MutexUsage mutUsage(this->recvMut);
	if (this->recvDataOfst < this->recvDataSize)
	{
		if (this->recvDataSize - this->recvDataOfst > buff.GetSize())
		{
			buff.CopyFrom(Data::ByteArrayR(&this->recvData[this->recvDataOfst], buff.GetSize()));
			this->recvDataOfst += buff.GetSize();
			return buff.GetSize();
		}
		else
		{
			UOSInt size = this->recvDataSize - this->recvDataOfst;
			buff.CopyFrom(Data::ByteArrayR(&this->recvData[this->recvDataOfst], size));
			this->recvDataOfst = this->recvDataSize;
			return size;
		}
	}
	UOSInt packetSize;
	const UInt8 *packetBuff = this->NextPacket(mutUsage, &packetSize);
	if (packetBuff == 0)
		return 0;
	if (packetSize <= buff.GetSize())
	{
		buff.CopyFrom(Data::ByteArrayR(packetBuff, packetSize));
		return packetSize;
	}
	buff.CopyFrom(Data::ByteArrayR(packetBuff, buff.GetSize()));
	if (packetSize - buff.GetSize() > this->recvDataCap)
	{
		if (this->recvData)
			MemFree(this->recvData);
		this->recvDataCap = packetSize - buff.GetSize();
		this->recvData = MemAlloc(UInt8, this->recvDataCap);
	}
	this->recvDataSize = packetSize - buff.GetSize();
	MemCopyNO(this->recvData, &packetBuff[buff.GetSize()], this->recvDataSize);
	this->recvDataOfst = 0;
	return buff.GetSize();
}

UOSInt Net::WebSocketClient::Write(const UInt8 *buff, UOSInt size)
{
	if (SendPacket(2, buff, size))
		return size;
	return 0;
}

Int32 Net::WebSocketClient::Flush()
{
	if (this->cli)
		return this->cli->Flush();
	return 0;
}

void Net::WebSocketClient::Close()
{
	if (this->cli)
	{
		this->Shutdown();
	}
}

Bool Net::WebSocketClient::Recover()
{
	return false;
}

IO::StreamType Net::WebSocketClient::GetStreamType() const
{
	return IO::StreamType::WebSocket;
}

Bool Net::WebSocketClient::Shutdown()
{
	if (this->cli == 0)
		return false;

	if (this->SendClose(0, 0))
	{
		this->cli->ShutdownSend();
		return true;
	}
	return false;
}

Bool Net::WebSocketClient::SendPing(const UInt8 *buff, UOSInt buffSize)
{
	return this->SendPacket(9, buff, buffSize);
}

Bool Net::WebSocketClient::SendPong(const UInt8 *buff, UOSInt buffSize)
{
	return this->SendPacket(10, buff, buffSize);
}

Bool Net::WebSocketClient::SendClose(const UInt8 *buff, UOSInt buffSize)
{
	return this->SendPacket(8, buff, buffSize);
}

