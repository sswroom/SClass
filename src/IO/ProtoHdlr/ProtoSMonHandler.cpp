#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/ProtoHdlr/ProtoSMonHandler.h"

IO::ProtoHdlr::ProtoSMonHandler::ProtoSMonHandler(IO::IProtocolHandler::DataListener *listener)
{
	this->listener = listener;
	NEW_CLASS(this->crc, Crypto::Hash::CRC32R());
	NEW_CLASS(this->crcMut, Sync::Mutex());
}

IO::ProtoHdlr::ProtoSMonHandler::~ProtoSMonHandler()
{
	DEL_CLASS(this->crcMut);
	DEL_CLASS(this->crc);
}

void *IO::ProtoHdlr::ProtoSMonHandler::CreateStreamData(IO::Stream *stm)
{
	return 0;
}

void IO::ProtoHdlr::ProtoSMonHandler::DeleteStreamData(IO::Stream *stm, void *stmData)
{
}

UOSInt IO::ProtoHdlr::ProtoSMonHandler::ParseProtocol(IO::Stream *stm, void *stmObj, void *stmData, const UInt8 *buff, UOSInt buffSize)
{
	Bool found;
	UInt8 crcVal[4];
	while (buffSize >= 10)
	{
		found = false;
		if (*(Int16*)buff == *(Int16*)"Sm" && *(Int16*)&buff[2] == *(Int16*)"SM")
		{
			UInt32 packetSize = ReadUInt16(&buff[6]);
			if (packetSize >= 10 && packetSize <= 2048)
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
					this->listener->DataParsed(stm, stmObj, ReadUInt16(&buff[4]), 0, &buff[8], packetSize - 10);

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

UOSInt IO::ProtoHdlr::ProtoSMonHandler::BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, void *stmData)
{
	UInt8 crcVal[4];
	*(Int16*)buff = *(Int16*)"Sm";
	*(Int16*)&buff[2] = *(Int16*)"SM";
	WriteInt16(&buff[4], cmdType);
	WriteInt16(&buff[6], (Int16)(cmdSize + 10));
	if (cmdSize > 0)
	{
		MemCopyNO(&buff[8], cmd, cmdSize);
	}

	this->crcMut->Lock();
	this->crc->Clear();
	this->crc->Calc(buff, cmdSize + 8);
	this->crc->GetValue(crcVal);
	this->crcMut->Unlock();
	WriteInt16(&buff[cmdSize + 8], ReadMInt32(crcVal));
	return cmdSize + 10;
}
