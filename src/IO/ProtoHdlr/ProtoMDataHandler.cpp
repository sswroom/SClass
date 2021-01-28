#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/ProtoHdlr/ProtoMDataHandler.h"

IO::ProtoHdlr::ProtoMDataHandler::ProtoMDataHandler(IO::IProtocolHandler::DataListener *listener)
{
	this->listener = listener;
	NEW_CLASS(this->crcMut, Sync::Mutex());
	NEW_CLASS(this->crc, Crypto::Hash::CRC32R());
}

IO::ProtoHdlr::ProtoMDataHandler::~ProtoMDataHandler()
{
	DEL_CLASS(this->crc);
	DEL_CLASS(this->crcMut);
}

void *IO::ProtoHdlr::ProtoMDataHandler::CreateStreamData(IO::Stream *stm)
{
	return 0;
}

void IO::ProtoHdlr::ProtoMDataHandler::DeleteStreamData(IO::Stream *stm, void *stmData)
{
}

UOSInt IO::ProtoHdlr::ProtoMDataHandler::ParseProtocol(IO::Stream *stm, void *stmObj, void *stmData, const UInt8 *buff, UOSInt buffSize)
{
	Bool found;
	UInt8 crcVal[4];
	while (buffSize >= 8)
	{
		found = false;
		if (*(Int16*)buff == *(Int16*)"MD")
		{
			UInt32 packetSize = ReadUInt16(&buff[2]);
			if (packetSize <= 2048)
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
					this->listener->DataParsed(stm, stmObj, ReadUInt16(&buff[4]), 0, &buff[6], packetSize - 8);

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

UOSInt IO::ProtoHdlr::ProtoMDataHandler::BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, void *stmData)
{
	*(Int16*)buff = *(Int16*)"MD";
	WriteInt16(&buff[2], (cmdSize + 8));
	WriteInt16(&buff[4], cmdType);
	if (cmdSize > 0)
	{
		MemCopyNO(&buff[6], cmd, cmdSize);
	}
	UInt8 crcVal[4];
	this->crcMut->Lock();
	this->crc->Clear();
	this->crc->Calc(buff, cmdSize + 6);
	this->crc->GetValue(crcVal);
	this->crcMut->Unlock();
	WriteInt16(&buff[cmdSize + 6], ReadMInt32(crcVal));
	return cmdSize + 8;
}
