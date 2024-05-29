#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/CRC32R.h"
#include "Data/ByteTool.h"
#include "IO/ProtoHdlr/ProtoMDataHandler.h"

IO::ProtoHdlr::ProtoMDataHandler::ProtoMDataHandler(NN<IO::IProtocolHandler::DataListener> listener)
{
	this->listener = listener;
	NN<Crypto::Hash::CRC32R> crc;
	NEW_CLASSNN(crc, Crypto::Hash::CRC32R());
	NEW_CLASSNN(this->crc, Crypto::Hash::HashCalc(crc));
}

IO::ProtoHdlr::ProtoMDataHandler::~ProtoMDataHandler()
{
	this->crc.Delete();
}

AnyType IO::ProtoHdlr::ProtoMDataHandler::CreateStreamData(NN<IO::Stream> stm)
{
	return 0;
}

void IO::ProtoHdlr::ProtoMDataHandler::DeleteStreamData(NN<IO::Stream> stm, AnyType stmData)
{
}

UOSInt IO::ProtoHdlr::ProtoMDataHandler::ParseProtocol(NN<IO::Stream> stm, AnyType stmObj, AnyType stmData, const Data::ByteArrayR &srcBuff)
{
	Bool found;
	UInt8 crcVal[4];
	Data::ByteArrayR buff = srcBuff;
	while (buff.GetSize() >= 8)
	{
		found = false;
		if (*(Int16*)&buff[0] == *(Int16*)"MD")
		{
			UInt32 packetSize = ReadUInt16(&buff[2]);
			if (packetSize <= 2048)
			{
				if (packetSize > buff.GetSize())
					return buff.GetSize();

				this->crc->Calc(buff.Arr().Ptr(), packetSize - 2, crcVal);
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

UOSInt IO::ProtoHdlr::ProtoMDataHandler::BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, AnyType stmData)
{
	*(Int16*)buff = *(Int16*)"MD";
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
