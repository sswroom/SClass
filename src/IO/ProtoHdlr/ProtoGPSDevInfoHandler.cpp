#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/ProtoHdlr/ProtoGPSDevInfoHandler.h"
#include "Sync/MutexUsage.h"

IO::ProtoHdlr::ProtoGPSDevInfoHandler::ProtoGPSDevInfoHandler(NN<IO::IProtocolHandler::DataListener> listener)
{
	this->listener = listener;
}

IO::ProtoHdlr::ProtoGPSDevInfoHandler::~ProtoGPSDevInfoHandler()
{
}

AnyType IO::ProtoHdlr::ProtoGPSDevInfoHandler::CreateStreamData(NN<IO::Stream> stm)
{
	return 0;
}

void IO::ProtoHdlr::ProtoGPSDevInfoHandler::DeleteStreamData(NN<IO::Stream> stm, AnyType stmData)
{
}

UOSInt IO::ProtoHdlr::ProtoGPSDevInfoHandler::ParseProtocol(NN<IO::Stream> stm, AnyType stmObj, AnyType stmData, const Data::ByteArrayR &srcBuff)
{
	Bool found;
	UInt8 crcVal[4];
	Data::ByteArrayR buff = srcBuff;
	while (buff.GetSize() >= 8)
	{
		found = false;
		if (buff.ReadNI16(0) == ReadNInt16((const UInt8*)"GD"))
		{
			UInt32 packetSize = ReadUInt16(&buff[2]);
			if (packetSize <= 14336)
			{
				if (packetSize > buff.GetSize())
					return buff.GetSize();

				Sync::MutexUsage mutUsage(this->crcMut);
				this->crc.Clear();
				this->crc.Calc(buff.Arr().Ptr(), packetSize - 2);
				this->crc.GetValue(crcVal);
				mutUsage.EndUse();
				if (ReadMUInt16(&crcVal[2]) == ReadUInt16(&buff[packetSize - 2]))
				{
					this->listener->DataParsed(stm, stmObj, ReadUInt16(&buff[4]), 0, &buff[6], packetSize - 8);

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

UOSInt IO::ProtoHdlr::ProtoGPSDevInfoHandler::BuildPacket(UnsafeArray<UInt8> buff, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UOSInt cmdSize, AnyType stmData)
{
	WriteNInt16(&buff[0], ReadNInt16((const UInt8*)"GD"));
	WriteUInt16(&buff[2], (UInt16)(cmdSize + 8));
	WriteUInt16(&buff[4], (UInt16)cmdType);
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
	WriteUInt16(&buff[cmdSize + 6], (UInt16)ReadMUInt32(crcVal));
	return cmdSize + 8;
}
