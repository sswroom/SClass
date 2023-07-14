#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/ProtoHdlr/ProtoGPSDevInfoHandler.h"
#include "Sync/MutexUsage.h"

IO::ProtoHdlr::ProtoGPSDevInfoHandler::ProtoGPSDevInfoHandler(IO::IProtocolHandler::DataListener *listener)
{
	this->listener = listener;
}

IO::ProtoHdlr::ProtoGPSDevInfoHandler::~ProtoGPSDevInfoHandler()
{
}

void *IO::ProtoHdlr::ProtoGPSDevInfoHandler::CreateStreamData(NotNullPtr<IO::Stream> stm)
{
	return 0;
}

void IO::ProtoHdlr::ProtoGPSDevInfoHandler::DeleteStreamData(NotNullPtr<IO::Stream> stm, void *stmData)
{
}

UOSInt IO::ProtoHdlr::ProtoGPSDevInfoHandler::ParseProtocol(NotNullPtr<IO::Stream> stm, void *stmObj, void *stmData, const UInt8 *buff, UOSInt buffSize)
{
	Bool found;
	UInt8 crcVal[4];
	while (buffSize >= 8)
	{
		found = false;
		if (ReadNInt16(buff) == ReadNInt16((const UInt8*)"GD"))
		{
			UInt32 packetSize = ReadUInt16(&buff[2]);
			if (packetSize <= 14336)
			{
				if (packetSize > buffSize)
					return buffSize;

				Sync::MutexUsage mutUsage(&this->crcMut);
				this->crc.Clear();
				this->crc.Calc(buff, packetSize - 2);
				this->crc.GetValue(crcVal);
				mutUsage.EndUse();
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

UOSInt IO::ProtoHdlr::ProtoGPSDevInfoHandler::BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, void *stmData)
{
	WriteNInt16(buff, ReadNInt16((const UInt8*)"GD"));
	WriteUInt16(&buff[2], (UInt16)(cmdSize + 8));
	WriteUInt16(&buff[4], (UInt16)cmdType);
	if (cmdSize > 0)
	{
		MemCopyNO(&buff[6], cmd, cmdSize);
	}
	UInt8 crcVal[4];
	Sync::MutexUsage mutUsage(&this->crcMut);
	this->crc.Clear();
	this->crc.Calc(buff, cmdSize + 6);
	this->crc.GetValue(crcVal);
	mutUsage.EndUse();
	WriteUInt16(&buff[cmdSize + 6], (UInt16)ReadMUInt32(crcVal));
	return cmdSize + 8;
}
