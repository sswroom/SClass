#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/CRC32R.h"
#include "Data/ByteTool.h"
#include "IO/ProtoHdlr/ProtoSyncHandler.h"

IO::ProtoHdlr::ProtoSyncHandler::ProtoSyncHandler(IO::IProtocolHandler::DataListener *listener)
{
	this->listener = listener;
}

IO::ProtoHdlr::ProtoSyncHandler::~ProtoSyncHandler()
{
}

void *IO::ProtoHdlr::ProtoSyncHandler::CreateStreamData(NotNullPtr<IO::Stream> stm)
{
	return 0;
}

void IO::ProtoHdlr::ProtoSyncHandler::DeleteStreamData(NotNullPtr<IO::Stream> stm, void *stmData)
{
}

UOSInt IO::ProtoHdlr::ProtoSyncHandler::ParseProtocol(NotNullPtr<IO::Stream> stm, void *stmObj, void *stmData, const UInt8 *buff, UOSInt buffSize)
{
	Bool found;
	UInt32 crcVal;
	while (buffSize >= 10)
	{
		found = false;
		if (*(Int16*)buff == *(Int16*)"Sy")
		{
			UInt32 packetSize = ReadUInt16(&buff[2]);
			if (packetSize <= 8192)
			{
				if (packetSize > buffSize)
					return buffSize;

				crcVal = this->crc.CalcDirect(buff, packetSize - 2);
				if ((crcVal & 0xffff) == ReadUInt16(&buff[packetSize - 2]))
				{
					Int32 cmdType = ReadUInt16(&buff[4]);
					Int32 seqId = ReadUInt16(&buff[6]);
					this->listener->DataParsed(stm, stmObj, cmdType, seqId, &buff[8], packetSize - 10);

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

UOSInt IO::ProtoHdlr::ProtoSyncHandler::BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, void *stmData)
{
	*(Int16*)buff = *(Int16*)"Sy";
	WriteInt16(&buff[2], (Int16)(cmdSize + 10));
	WriteInt16(&buff[4], cmdType);
	WriteInt16(&buff[6], seqId);
	if (cmdSize > 0)
	{
		MemCopyNO(&buff[8], cmd, cmdSize);
	}
	UInt32 crcVal;
	crcVal = this->crc.CalcDirect(buff, cmdSize + 8);
	WriteInt16(&buff[cmdSize + 8], crcVal);
	return cmdSize + 10;
}
