#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/ProtoHdlr/ProtoLBSGateHandler.h"

IO::ProtoHdlr::ProtoLBSGateHandler::ProtoLBSGateHandler(IO::IProtocolHandler::DataListener *listener)
{
	this->listener = listener;
	NEW_CLASS(this->crcMut, Sync::Mutex());
	NEW_CLASS(this->crc, Crypto::Hash::CRC32R());
}

IO::ProtoHdlr::ProtoLBSGateHandler::~ProtoLBSGateHandler()
{
	DEL_CLASS(this->crc);
	DEL_CLASS(this->crcMut);
}

void *IO::ProtoHdlr::ProtoLBSGateHandler::CreateStreamData(IO::Stream *stm)
{
	return 0;
}

void IO::ProtoHdlr::ProtoLBSGateHandler::DeleteStreamData(IO::Stream *stm, void *stmData)
{
}

UOSInt IO::ProtoHdlr::ProtoLBSGateHandler::ParseProtocol(IO::Stream *stm, void *stmObj, void *stmData, const UInt8 *buff, UOSInt buffSize)
{
	Bool found;
	UInt32 crcVal;
	while (buffSize >= 8)
	{
		found = false;
		if (*(Int16*)buff == *(Int16*)"lg")
		{
			UInt32 packetSize = *(UInt16*)&buff[2];
			if (packetSize <= 2048)
			{
				if (packetSize > buffSize)
					return buffSize;

				this->crcMut->Lock();
				this->crc->Clear();
				this->crc->Calc(buff, packetSize - 2);
				this->crc->GetValue((UInt8*)&crcVal);
				this->crcMut->Unlock();
				if ((crcVal & 0xffff) == *(UInt16*)&buff[packetSize - 2])
				{
					this->listener->DataParsed(stm, stmObj, *(UInt16*)&buff[4], 0, &buff[6], packetSize - 8);

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

UOSInt IO::ProtoHdlr::ProtoLBSGateHandler::BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, void *stmData)
{
	*(Int16*)buff = *(Int16*)"lg";
	*(Int16*)&buff[2] = (Int16)(cmdSize + 8);
	*(Int16*)&buff[4] = cmdType;
	if (cmdSize > 0)
	{
		MemCopyNO(&buff[6], cmd, cmdSize);
	}
	this->crcMut->Lock();
	this->crc->Clear();
	this->crc->Calc(buff, cmdSize + 6);
	this->crc->GetValue(&buff[cmdSize + 6]);
	this->crcMut->Unlock();
	return cmdSize + 8;
}
