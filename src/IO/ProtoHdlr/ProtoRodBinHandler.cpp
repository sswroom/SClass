#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/ProtoHdlr/ProtoRodBinHandler.h"

IO::ProtoHdlr::ProtoRodBinHandler::ProtoRodBinHandler(IO::IProtocolHandler::DataListener *listener)
{
	this->listener = listener;
}

IO::ProtoHdlr::ProtoRodBinHandler::~ProtoRodBinHandler()
{
}

void *IO::ProtoHdlr::ProtoRodBinHandler::CreateStreamData(IO::Stream *stm)
{
	return 0;
}

void IO::ProtoHdlr::ProtoRodBinHandler::DeleteStreamData(IO::Stream *stm, void *stmData)
{
}

UOSInt IO::ProtoHdlr::ProtoRodBinHandler::ParseProtocol(IO::Stream *stm, void *stmObj, void *stmData, UInt8 *buff, UOSInt buffSize)
{
	Bool found;
	while (buffSize >= 10)
	{
		found = false;
		if (*(Int16*)buff == *(Int16*)"Af")
		{
			UInt32 packetSize = *(UInt16*)&buff[6];
			if (packetSize <= 2048)
			{
				if (packetSize > buffSize)
					return buffSize;

				UInt16 chk = CalCheck(buff, packetSize - 2);
				if (chk == *(UInt16*)&buff[packetSize - 2])
				{
					this->listener->DataParsed(stm, stmObj, *(UInt16*)&buff[2], *(UInt16*)&buff[4], &buff[8], packetSize - 10);

					found = true;
					buff += packetSize;
					buffSize -= packetSize;
				}
			}
		}

		if (!found)
		{
			buff++;
			buffSize--;
		}
	}
	return buffSize;
}

UOSInt IO::ProtoHdlr::ProtoRodBinHandler::BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, UInt8 *cmd, UOSInt cmdSize, void *stmData)
{
	*(Int16*)buff = *(Int16*)"Af";
	*(Int16*)&buff[2] = cmdType;
	*(Int16*)&buff[4] = seqId;
	*(Int16*)&buff[6] = (Int16)(cmdSize + 10);
	if (cmdSize > 0)
	{
		MemCopyNO(&buff[8], cmd, cmdSize);
	}
	*(Int16*)&buff[cmdSize + 8] = CalCheck(buff, cmdSize + 8);
	return cmdSize + 10;
}

UInt16 IO::ProtoHdlr::ProtoRodBinHandler::CalCheck(UInt8 *buff, UOSInt buffSize)
{
	UInt8 chkBytes[2];
	UOSInt i;
	chkBytes[0] = 0;
	chkBytes[1] = 0;
	i = 0;
	while (i < buffSize)
	{
		chkBytes[0] = chkBytes[0] + buff[i];
		chkBytes[1] = chkBytes[1] + chkBytes[0];
		i++;
	}
	return *(UInt16*)chkBytes;
}
