#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/CRC32R.h"
#include "Data/ByteTool.h"
#include "IO/ProtoHdlr/ProtoRevGeoHandler.h"

IO::ProtoHdlr::ProtoRevGeoHandler::ProtoRevGeoHandler(IO::IProtocolHandler::DataListener *listener)
{
	this->listener = listener;
	Crypto::Hash::CRC32R *crc;
	NEW_CLASS(crc, Crypto::Hash::CRC32R());
	NEW_CLASS(this->crc, Crypto::Hash::HashCalc(crc));
}

IO::ProtoHdlr::ProtoRevGeoHandler::~ProtoRevGeoHandler()
{
	DEL_CLASS(this->crc);
}

void *IO::ProtoHdlr::ProtoRevGeoHandler::CreateStreamData(NotNullPtr<IO::Stream> stm)
{
	return 0;
}

void IO::ProtoHdlr::ProtoRevGeoHandler::DeleteStreamData(NotNullPtr<IO::Stream> stm, void *stmData)
{
}

UOSInt IO::ProtoHdlr::ProtoRevGeoHandler::ParseProtocol(NotNullPtr<IO::Stream> stm, void *stmObj, void *stmData, const Data::ByteArrayR &srcBuff)
{
	Bool found;
	UInt8 crcVal[4];
	Data::ByteArrayR buff = srcBuff;
	while (buff.GetSize() >= 8)
	{
		found = false;
		if (*(Int16*)&buff[0] == *(Int16*)"RG")
		{
			UInt32 packetSize = ReadUInt16(&buff[2]);
			if (packetSize <= 2048)
			{
				if (packetSize > buff.GetSize())
					return buff.GetSize();

				this->crc->Calc(buff.Ptr(), packetSize - 2, crcVal);
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

UOSInt IO::ProtoHdlr::ProtoRevGeoHandler::BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, void *stmData)
{
	*(Int16*)buff = *(Int16*)"RG";
	WriteInt16(&buff[2], (cmdSize + 8));
	WriteInt16(&buff[4], cmdType);
	if (cmdSize > 0)
	{
		MemCopyNO(&buff[6], cmd, cmdSize);
	}
	UInt8 crcVal[4];
	this->crc->Calc(buff, cmdSize + 6, crcVal);
	WriteInt16(&buff[cmdSize + 6], ReadMInt32(crcVal));
	return cmdSize + 8;
}
