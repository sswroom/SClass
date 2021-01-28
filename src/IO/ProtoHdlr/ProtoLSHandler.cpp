#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/CRC32R.h"
#include "IO/ProtoHdlr/ProtoLSHandler.h"

IO::ProtoHdlr::ProtoLSHandler::ProtoLSHandler(IO::IProtocolHandler::DataListener *listener)
{
	this->listener = listener;
}

IO::ProtoHdlr::ProtoLSHandler::~ProtoLSHandler()
{
}

void *IO::ProtoHdlr::ProtoLSHandler::CreateStreamData(IO::Stream *stm)
{
	return 0;
}

void IO::ProtoHdlr::ProtoLSHandler::DeleteStreamData(IO::Stream *stm, void *stmData)
{
}

UOSInt IO::ProtoHdlr::ProtoLSHandler::ParseProtocol(IO::Stream *stm, void *stmObj, void *stmData, UInt8 *buff, UOSInt buffSize)
{
	Bool found;
	while (buffSize >= 10)
	{
		found = false;
		if (*(Int16*)buff == *(Int16*)"MW")
		{
			UInt32 packetSize = *(UInt16*)&buff[2];
			if (packetSize <= 2048)
			{
				if (packetSize > buffSize)
					return buffSize;

				UInt16 chk = CalCheck(buff, packetSize - 2);
				if (chk == *(UInt16*)&buff[packetSize - 2])
				{
					this->listener->DataParsed(stm, stmObj, *(UInt16*)&buff[4], *(UInt16*)&buff[6], &buff[8], packetSize - 10);

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

UOSInt IO::ProtoHdlr::ProtoLSHandler::BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, UInt8 *cmd, UOSInt cmdSize, void *stmData)
{
	*(Int16*)buff = *(Int16*)"MW";
	*(Int16*)&buff[2] = (Int16)(cmdSize + 10);
	*(Int16*)&buff[4] = cmdType;
	*(Int16*)&buff[6] = seqId;
	if (cmdSize > 0)
	{
		MemCopyNO(&buff[8], cmd, cmdSize);
	}
	*(Int16*)&buff[cmdSize + 8] = CalCheck(buff, cmdSize + 8);
	return cmdSize + 10;
}

UInt16 IO::ProtoHdlr::ProtoLSHandler::CalCheck(UInt8 *buff, UOSInt buffSize)
{
	Crypto::Hash::CRC32R crc;
	UInt32 crcVal;
	crc.Calc(buff, buffSize);
	crc.GetValue((UInt8*)&crcVal);
	return (UInt16)crcVal;
}
