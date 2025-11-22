#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "IO/ProtoHdlr/ProtoMQTTHandler.h"

typedef struct
{
	UInt8 packetType;
	UOSInt packetSize;
	UOSInt packetDataSize;
	UInt8 *packetBuff;
} ClientData;

IO::ProtoHdlr::ProtoMQTTHandler::ProtoMQTTHandler(NN<IO::ProtocolHandler::DataListener> listener)
{
	this->listener = listener;
}

IO::ProtoHdlr::ProtoMQTTHandler::~ProtoMQTTHandler()
{
}

AnyType IO::ProtoHdlr::ProtoMQTTHandler::CreateStreamData(NN<IO::Stream> stm)
{
	ClientData *cliData = MemAlloc(ClientData, 1);
	cliData->packetBuff = 0;
	cliData->packetSize = 0;
	cliData->packetDataSize = 0;
	cliData->packetType = 0;
	return cliData;
}

void IO::ProtoHdlr::ProtoMQTTHandler::DeleteStreamData(NN<IO::Stream> stm, AnyType stmData)
{
	NN<ClientData> cliData = stmData.GetNN<ClientData>();
	if (cliData->packetBuff)
	{
		MemFree(cliData->packetBuff);
		cliData->packetBuff = 0;
	}
	MemFree(cliData.Ptr());
}

UOSInt IO::ProtoHdlr::ProtoMQTTHandler::ParseProtocol(NN<IO::Stream> stm, AnyType stmObj, AnyType stmData, const Data::ByteArrayR &srcBuff)
{
	NN<ClientData> cliData = stmData.GetNN<ClientData>();
	Data::ByteArrayR buff = srcBuff;
	if (cliData->packetBuff)
	{
		if (cliData->packetSize - cliData->packetDataSize <= buff.GetSize())
		{
			MemCopyNO(&cliData->packetBuff[cliData->packetDataSize], buff.Arr().Ptr(), cliData->packetSize - cliData->packetDataSize);
			this->listener->DataParsed(stm, stmObj, cliData->packetType, 0, cliData->packetBuff, cliData->packetSize);
			MemFree(cliData->packetBuff);
			cliData->packetBuff = 0;
			if (cliData->packetSize - cliData->packetDataSize == buff.GetSize())
			{
				return 0;
			}
			buff += cliData->packetSize - cliData->packetDataSize;
		}
		else
		{
			MemCopyNO(&cliData->packetBuff[cliData->packetDataSize], buff.Arr().Ptr(), buff.GetSize());
			cliData->packetDataSize += buff.GetSize();
			return 0;
		}
	}
	UOSInt packetSize;
	UOSInt i;
	while (buff.GetSize() >= 2)
	{
		if (buff[1] & 0x80)
		{
			if (buff.GetSize() < 5)
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
		if (buff.GetSize() >= packetSize + i)
		{
			this->listener->DataParsed(stm, stmObj, buff[0], 0, &buff[i], packetSize);
			if (buff.GetSize() == packetSize + i)
			{
				return 0;
			}
			buff += packetSize + i;
		}
		else
		{
			cliData->packetBuff = MemAlloc(UInt8, packetSize);
			cliData->packetSize = packetSize;
			cliData->packetType = buff[0];
			MemCopyNO(cliData->packetBuff, &buff[i], buff.GetSize() - i);
			cliData->packetDataSize = buff.GetSize() - i;
			return 0;
		}
	}
	return buff.GetSize();
}

UOSInt IO::ProtoHdlr::ProtoMQTTHandler::BuildPacket(UnsafeArray<UInt8> buff, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UOSInt cmdSize, AnyType stmData)
{
	buff[0] = (UInt8)(cmdType & 0xff);
	if (cmdSize < 128)
	{
		buff[1] = (UInt8)cmdSize;
		if (cmdSize > 0)
		{
			MemCopyNO(&buff[2], cmd.Ptr(), cmdSize);
		}
		return cmdSize + 2;
	}
	else if (cmdSize < 16384)
	{
		buff[1] = (UInt8)((cmdSize & 0x7F) | 0x80);
		buff[2] = (UInt8)(cmdSize >> 7);
		MemCopyNO(&buff[3], cmd.Ptr(), cmdSize);
		return cmdSize + 3;
	}
	else if (cmdSize < 2097152)
	{
		buff[1] = (UInt8)((cmdSize & 0x7F) | 0x80);
		cmdSize >>= 7;
		buff[2] = (UInt8)((cmdSize & 0x7F) | 0x80);
		buff[3] = (UInt8)(cmdSize >> 7);
		MemCopyNO(&buff[4], cmd.Ptr(), cmdSize);
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
		MemCopyNO(&buff[5], cmd.Ptr(), cmdSize);
		return cmdSize + 5;
	}
	else
	{
		return 0;
	}
}

Bool IO::ProtoHdlr::ProtoMQTTHandler::ParseUTF8Str(UnsafeArray<const UTF8Char> buff, InOutParam<UOSInt> index, UOSInt buffSize, NN<Text::StringBuilderUTF8> sb)
{
	UInt16 strSize;
	if ((buffSize - index.Get()) < 2)
		return false;
	strSize = ReadMUInt16(&buff[index.Get()]);
	if (buffSize - 2 - index.Get() < strSize)
	{
		return false;
	}
	sb->AppendC((const UTF8Char*)&buff[2 + index.Get()], strSize);
	index.Set((UOSInt)strSize + 2 + index.Get());
	return true;
}
