#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/ProtoHdlr/ProtoLBSGateHandler.h"
#include "Sync/MutexUsage.h"

IO::ProtoHdlr::ProtoLBSGateHandler::ProtoLBSGateHandler(NN<IO::IProtocolHandler::DataListener> listener)
{
	this->listener = listener;
}

IO::ProtoHdlr::ProtoLBSGateHandler::~ProtoLBSGateHandler()
{
}

AnyType IO::ProtoHdlr::ProtoLBSGateHandler::CreateStreamData(NN<IO::Stream> stm)
{
	return 0;
}

void IO::ProtoHdlr::ProtoLBSGateHandler::DeleteStreamData(NN<IO::Stream> stm, AnyType stmData)
{
}

UOSInt IO::ProtoHdlr::ProtoLBSGateHandler::ParseProtocol(NN<IO::Stream> stm, AnyType stmObj, AnyType stmData, const Data::ByteArrayR &srcBuff)
{
	Bool found;
	UInt32 crcVal;
	Data::ByteArrayR buff = srcBuff;
	while (buff.GetSize() >= 8)
	{
		found = false;
		if (*(Int16*)&buff[0] == *(Int16*)"lg")
		{
			UInt32 packetSize = *(UInt16*)&buff[2];
			if (packetSize <= 2048)
			{
				if (packetSize > buff.GetSize())
					return buff.GetSize();

				Sync::MutexUsage mutUsage(this->crcMut);
				this->crc.Clear();
				this->crc.Calc(buff.Ptr(), packetSize - 2);
				this->crc.GetValue((UInt8*)&crcVal);
				mutUsage.EndUse();
				if ((crcVal & 0xffff) == *(UInt16*)&buff[packetSize - 2])
				{
					this->listener->DataParsed(stm, stmObj, *(UInt16*)&buff[4], 0, &buff[6], packetSize - 8);

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

UOSInt IO::ProtoHdlr::ProtoLBSGateHandler::BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, AnyType stmData)
{
	*(Int16*)buff = *(Int16*)"lg";
	*(Int16*)&buff[2] = (Int16)(cmdSize + 8);
	*(Int16*)&buff[4] = cmdType;
	if (cmdSize > 0)
	{
		MemCopyNO(&buff[6], cmd, cmdSize);
	}
	Sync::MutexUsage mutUsage(this->crcMut);
	this->crc.Clear();
	this->crc.Calc(buff, cmdSize + 6);
	this->crc.GetValue(&buff[cmdSize + 6]);
	return cmdSize + 8;
}
