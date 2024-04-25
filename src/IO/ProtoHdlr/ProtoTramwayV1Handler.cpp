#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/ProtoHdlr/ProtoTramwayV1Handler.h"

IO::ProtoHdlr::ProtoTramwayV1Handler::ProtoTramwayV1Handler(NN<IO::IProtocolHandler::DataListener> listener)
{
	this->listener = listener;
}

IO::ProtoHdlr::ProtoTramwayV1Handler::~ProtoTramwayV1Handler()
{
}

void *IO::ProtoHdlr::ProtoTramwayV1Handler::CreateStreamData(NN<IO::Stream> stm)
{
	ProtocolStatus *stat = MemAlloc(ProtocolStatus, 1);
	stat->buffSize = 0;
	stat->packetBuff = MemAlloc(UInt8, 0x7fffff);
	return stat;
}

void IO::ProtoHdlr::ProtoTramwayV1Handler::DeleteStreamData(NN<IO::Stream> stm, void *stmData)
{
	ProtocolStatus *stat = (ProtocolStatus *)stmData;;
	MemFree(stat->packetBuff);
	MemFree(stat);
}

UOSInt IO::ProtoHdlr::ProtoTramwayV1Handler::ParseProtocol(NN<IO::Stream> stm, void *stmObj, void *stmData, const Data::ByteArrayR &buff)
{
	ProtocolStatus *stat = (ProtocolStatus *)stmData;;
	UOSInt skipStart = 0;
	UOSInt i = 0;
	UOSInt j;
	UOSInt packetSize = 0;
	UInt8 v;
	while (i < buff.GetSize())
	{
		if (stat->buffSize < 4)
		{
			while (i < buff.GetSize() - 1)
			{
				if (buff[i] == 0xff && buff[i + 1] != 0xff)
				{
					break;
				}
				i++;
			}
			if (i > skipStart)
			{
				this->listener->DataSkipped(stm, stmObj, &buff[skipStart], i - skipStart);
			}
			if (i == buff.GetSize() - 1)
			{
				return 1;
			}
			if (i > buff.GetSize() - 4)
			{
				return buff.GetSize() - i;
			}
			*(Int32*)stat->packetBuff = *(Int32*)&buff[i];
			stat->buffSize = 4;
			i += 4;
		}
		packetSize = ((stat->packetBuff[1] << 16) | (stat->packetBuff[2] << 8) | stat->packetBuff[3]) + 4;
		j = stat->buffSize;
		while (j < packetSize)
		{
			if (i >= buff.GetSize())
			{
				stat->buffSize = j;
				return 0;
			}

			if ((v = buff[i]) == 0xff)
			{
				if (i == buff.GetSize() - 1)
				{
					stat->buffSize = j;
					return 1;
				}
				if (buff[i + 1] == 0xff)
				{
					stat->packetBuff[j++] = 0xff;
					i += 2;
				}
				else
				{
					///////////////////////////////
					i++;
				}
			}
			else
			{
				stat->packetBuff[j++] = v;
				i++;
			}
		}
		this->listener->DataParsed(stm, stmObj, ((stat->packetBuff[4] << 8) & 0x8000) | stat->packetBuff[6], ((stat->packetBuff[4] << 8) & 0x7f00) | stat->packetBuff[5], &stat->packetBuff[7], packetSize - 7);
		stat->buffSize = 0;
		skipStart = i;
	}
	return 0;
}

UOSInt IO::ProtoHdlr::ProtoTramwayV1Handler::BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, void *stmData)
{
	return 0;
}
