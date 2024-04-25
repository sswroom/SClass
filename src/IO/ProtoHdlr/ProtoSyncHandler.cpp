#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/CRC32R.h"
#include "Data/ByteTool.h"
#include "IO/ProtoHdlr/ProtoSyncHandler.h"

IO::ProtoHdlr::ProtoSyncHandler::ProtoSyncHandler(NN<IO::IProtocolHandler::DataListener> listener)
{
	this->listener = listener;
}

IO::ProtoHdlr::ProtoSyncHandler::~ProtoSyncHandler()
{
}

AnyType IO::ProtoHdlr::ProtoSyncHandler::CreateStreamData(NN<IO::Stream> stm)
{
	return 0;
}

void IO::ProtoHdlr::ProtoSyncHandler::DeleteStreamData(NN<IO::Stream> stm, AnyType stmData)
{
}

UOSInt IO::ProtoHdlr::ProtoSyncHandler::ParseProtocol(NN<IO::Stream> stm, AnyType stmObj, AnyType stmData, const Data::ByteArrayR &srcBuff)
{
	Bool found;
	UInt32 crcVal;
	Data::ByteArrayR buff = srcBuff;
	while (buff.GetSize() >= 10)
	{
		found = false;
		if (*(Int16*)&buff[0] == *(Int16*)"Sy")
		{
			UInt32 packetSize = ReadUInt16(&buff[2]);
			if (packetSize <= 8192)
			{
				if (packetSize > buff.GetSize())
					return buff.GetSize();

				crcVal = this->crc.CalcDirect(buff.Ptr(), packetSize - 2);
				if ((crcVal & 0xffff) == ReadUInt16(&buff[packetSize - 2]))
				{
					Int32 cmdType = ReadUInt16(&buff[4]);
					Int32 seqId = ReadUInt16(&buff[6]);
					this->listener->DataParsed(stm, stmObj, cmdType, seqId, &buff[8], packetSize - 10);

					found = true;
					buff += packetSize;
				}
			}
		}

		if (!found)
		{
			buff += 1;
		}
	}
	return buff.GetSize();
}

UOSInt IO::ProtoHdlr::ProtoSyncHandler::BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, AnyType stmData)
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
