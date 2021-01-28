#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/ProtoHdlr/ProtoSyncHandler.h"

IO::ProtoHdlr::ProtoSyncHandler::ProtoSyncHandler(IO::IProtocolHandler::DataListener *listener)
{
	this->listener = listener;
	NEW_CLASS(this->crcMut, Sync::Mutex());
	NEW_CLASS(this->crc, Crypto::Hash::CRC32R());
}

IO::ProtoHdlr::ProtoSyncHandler::~ProtoSyncHandler()
{
	DEL_CLASS(this->crc);
	DEL_CLASS(this->crcMut);
}

void *IO::ProtoHdlr::ProtoSyncHandler::CreateStreamData(IO::Stream *stm)
{
	return 0;
}

void IO::ProtoHdlr::ProtoSyncHandler::DeleteStreamData(IO::Stream *stm, void *stmData)
{
}

UOSInt IO::ProtoHdlr::ProtoSyncHandler::ParseProtocol(IO::Stream *stm, void *stmObj, void *stmData, const UInt8 *buff, UOSInt buffSize)
{
	Bool found;
	UInt8 crcVal[4];
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

				this->crcMut->Lock();
				this->crc->Clear();
				this->crc->Calc(buff, packetSize - 2);
				this->crc->GetValue(crcVal);
				this->crcMut->Unlock();
				if (ReadMUInt16(&crcVal[2]) == ReadUInt16(&buff[packetSize - 2]))
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
	UInt8 crcVal[4];
	this->crcMut->Lock();
	this->crc->Clear();
	this->crc->Calc(buff, cmdSize + 8);
	this->crc->GetValue(crcVal);
	this->crcMut->Unlock();
	WriteInt16(&buff[cmdSize + 8], ReadMInt32(crcVal));
	return cmdSize + 10;
}
