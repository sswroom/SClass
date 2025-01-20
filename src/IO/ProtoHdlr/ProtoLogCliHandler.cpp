#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/ProtoHdlr/ProtoLogCliHandler.h"
#include "Sync/MutexUsage.h"

IO::ProtoHdlr::ProtoLogCliHandler::ProtoLogCliHandler(NN<IO::ProtocolHandler::DataListener> listener)
{
	this->listener = listener;
}

IO::ProtoHdlr::ProtoLogCliHandler::~ProtoLogCliHandler()
{
}

AnyType IO::ProtoHdlr::ProtoLogCliHandler::CreateStreamData(NN<IO::Stream> stm)
{
	return 0;
}

void IO::ProtoHdlr::ProtoLogCliHandler::DeleteStreamData(NN<IO::Stream> stm, AnyType stmData)
{
}

UOSInt IO::ProtoHdlr::ProtoLogCliHandler::ParseProtocol(NN<IO::Stream> stm, AnyType stmObj, AnyType stmData, const Data::ByteArrayR &buff)
{
	Bool found;
	UInt8 crcVal[4];
	Data::ByteArrayR myBuff = buff;
	while (myBuff.GetSize() >= 8)
	{
		found = false;
		if (*(Int16*)&myBuff[0] == *(Int16*)"lC")
		{
			UInt32 packetSize = ReadUInt16(&myBuff[2]);
			if (packetSize <= 4096)
			{
				if (packetSize > myBuff.GetSize())
					return myBuff.GetSize();

				Sync::MutexUsage mutUsage(this->crcMut);
				this->crc.Clear();
				this->crc.Calc(myBuff.Arr().Ptr(), packetSize - 2);
				this->crc.GetValue(crcVal);
				mutUsage.EndUse();
				if (ReadMUInt16(&crcVal[2]) == ReadUInt16(&myBuff[packetSize - 2]))
				{
					this->listener->DataParsed(stm, stmObj, ReadUInt16(&myBuff[4]), 0, &myBuff[6], packetSize - 8);

					found = true;
					myBuff += packetSize;
				}
			}
		}

		if (!found)
		{
			myBuff += 1;
		}
	}
	return myBuff.GetSize();
}

UOSInt IO::ProtoHdlr::ProtoLogCliHandler::BuildPacket(UnsafeArray<UInt8> buff, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UOSInt cmdSize, AnyType stmData)
{
	*(Int16*)&buff[0] = *(Int16*)"lC";
	WriteInt16(&buff[2], (Int16)cmdSize + 8);
	WriteInt16(&buff[4], cmdType);
	if (cmdSize > 0)
	{
		MemCopyNO(&buff[6], cmd.Ptr(), cmdSize);
	}
	UInt8 crcVal[4];
	Sync::MutexUsage mutUsage(this->crcMut);
	this->crc.Clear();
	this->crc.Calc(buff, cmdSize + 6);
	this->crc.GetValue(crcVal);
	mutUsage.EndUse();
	WriteInt16(&buff[cmdSize + 6], ReadMInt32(crcVal));
	return cmdSize + 8;
}
