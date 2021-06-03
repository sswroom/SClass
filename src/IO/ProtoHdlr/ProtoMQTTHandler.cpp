#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/ProtoHdlr/ProtoMQTTHandler.h"

typedef struct
{
	UInt8 packetType;
	UOSInt packetSize;
	UOSInt packetDataSize;
	UInt8 *packetBuff;
} ClientData;

IO::ProtoHdlr::ProtoMQTTHandler::ProtoMQTTHandler(IO::IProtocolHandler::DataListener *listener)
{
	this->listener = listener;
}

IO::ProtoHdlr::ProtoMQTTHandler::~ProtoMQTTHandler()
{
}

void *IO::ProtoHdlr::ProtoMQTTHandler::CreateStreamData(IO::Stream *stm)
{
	ClientData *cliData = MemAlloc(ClientData, 1);
	cliData->packetBuff = 0;
	cliData->packetSize = 0;
	cliData->packetDataSize = 0;
	cliData->packetType = 0;
	return cliData;
}

void IO::ProtoHdlr::ProtoMQTTHandler::DeleteStreamData(IO::Stream *stm, void *stmData)
{
	ClientData *cliData = (ClientData*)stmData;
	if (cliData->packetBuff)
	{
		MemFree(cliData->packetBuff);
		cliData->packetBuff = 0;
	}
	MemFree(cliData);
}

UOSInt IO::ProtoHdlr::ProtoMQTTHandler::ParseProtocol(IO::Stream *stm, void *stmObj, void *stmData, const UInt8 *buff, UOSInt buffSize)
{
	ClientData *cliData = (ClientData*)stmData;
	if (cliData->packetBuff)
	{
		if (cliData->packetSize - cliData->packetDataSize <= buffSize)
		{
			MemCopyNO(&cliData->packetBuff[cliData->packetDataSize], buff, cliData->packetSize - cliData->packetDataSize);
			this->listener->DataParsed(stm, stmObj, cliData->packetType, 0, cliData->packetBuff, cliData->packetSize);
			MemFree(cliData->packetBuff);
			cliData->packetBuff = 0;
			if (cliData->packetSize - cliData->packetDataSize == buffSize)
			{
				return 0;
			}
			buff += cliData->packetSize - cliData->packetDataSize;
			buffSize -= cliData->packetSize - cliData->packetDataSize;
		}
		else
		{
			MemCopyNO(&cliData->packetBuff[cliData->packetDataSize], buff, buffSize);
			cliData->packetDataSize += buffSize;
			return 0;
		}
	}
	UOSInt packetSize;
	UOSInt i;
	while (buffSize >= 2)
	{
		if (buff[1] & 0x80)
		{
			if (buffSize < 5)
				break;
			if (buff[2] & 0x80)
			{
				if (buff[3] & 0x80)
				{
					packetSize = (UOSInt)((buff[1] & 0x7f) | ((buff[2] & 0x7f) << 7) | ((buff[3] & 0x7f) << 14) | (buff[4] << 21));
					i = 5;
				}
				else
				{
					packetSize = (UOSInt)((buff[1] & 0x7f) | ((buff[2] & 0x7f) << 7) | (buff[3] << 14));
					i = 4;
				}
			}
			else
			{
				packetSize = (UOSInt)((buff[1] & 0x7f) | (buff[2] << 7));
				i = 3;
			}
		}
		else
		{
			packetSize = buff[1];
			i = 2;
		}
		if (buffSize >= packetSize + i)
		{
			this->listener->DataParsed(stm, stmObj, buff[0], 0, &buff[i], packetSize);
			if (buffSize == packetSize + i)
			{
				return 0;
			}
			buff += packetSize + i;
			buffSize -= packetSize + i;
		}
		else
		{
			cliData->packetBuff = MemAlloc(UInt8, packetSize);
			cliData->packetSize = packetSize;
			cliData->packetType = buff[0];
			MemCopyNO(cliData->packetBuff, &buff[i], buffSize - i);
			cliData->packetDataSize = buffSize - i;
			return 0;
		}
	}
	return buffSize;
}

UOSInt IO::ProtoHdlr::ProtoMQTTHandler::BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, void *stmData)
{
	buff[0] = (UInt8)(cmdType & 0xff);
	if (cmdSize < 128)
	{
		buff[1] = (UInt8)cmdSize;
		if (cmdSize > 0)
		{
			MemCopyNO(&buff[2], cmd, cmdSize);
		}
		return cmdSize + 2;
	}
	else if (cmdSize < 16384)
	{
		buff[1] = (UInt8)((cmdSize & 0x7F) | 0x80);
		buff[2] = (UInt8)(cmdSize >> 7);
		MemCopyNO(&buff[3], cmd, cmdSize);
		return cmdSize + 3;
	}
	else if (cmdSize < 2097152)
	{
		buff[1] = (UInt8)((cmdSize & 0x7F) | 0x80);
		cmdSize >>= 7;
		buff[2] = (UInt8)((cmdSize & 0x7F) | 0x80);
		buff[3] = (UInt8)(cmdSize >> 7);
		MemCopyNO(&buff[4], cmd, cmdSize);
		return cmdSize + 4;
	}
	else if (cmdSize < 268435456)
	{
		buff[1] = (UInt8)((cmdSize & 0x7F) | 0x80);
		cmdSize >>= 7;
		buff[2] = (UInt8)((cmdSize & 0x7F) | 0x80);
		cmdSize >>= 7;
		buff[3] = (UInt8)((cmdSize & 0x7F) | 0x80);
		buff[4] = (UInt8)(cmdSize >> 7);
		MemCopyNO(&buff[5], cmd, cmdSize);
		return cmdSize + 5;
	}
	else
	{
		return 0;
	}
}

Bool IO::ProtoHdlr::ProtoMQTTHandler::ParseUTF8Str(const UTF8Char *buff, UOSInt *index, UOSInt buffSize, Text::StringBuilderUTF *sb)
{
	UInt16 strSize;
	if ((buffSize - *index) < 2)
		return false;
	strSize = ReadMUInt16(&buff[*index]);
	if (buffSize - 2 - *index < strSize)
	{
		return false;
	}
	sb->AppendC((const UTF8Char*)&buff[2 + *index], strSize);
	*index = (UOSInt)strSize + 2 + *index;
	return true;
}
