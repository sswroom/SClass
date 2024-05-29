#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/CRC32R.h"
#include "Data/ByteTool.h"
#include "IO/ProtoHdlr/ProtoSMonHandler.h"

IO::ProtoHdlr::ProtoSMonHandler::ProtoSMonHandler(NN<IO::IProtocolHandler::DataListener> listener)
{
	this->listener = listener;
	NN<Crypto::Hash::CRC32R> crc;
	NEW_CLASSNN(crc, Crypto::Hash::CRC32R());
	NEW_CLASSNN(this->crc, Crypto::Hash::HashCalc(crc));
}

IO::ProtoHdlr::ProtoSMonHandler::~ProtoSMonHandler()
{
	this->crc.Delete();
}

AnyType IO::ProtoHdlr::ProtoSMonHandler::CreateStreamData(NN<IO::Stream> stm)
{
	return 0;
}

void IO::ProtoHdlr::ProtoSMonHandler::DeleteStreamData(NN<IO::Stream> stm, AnyType stmData)
{
}

UOSInt IO::ProtoHdlr::ProtoSMonHandler::ParseProtocol(NN<IO::Stream> stm, AnyType stmObj, AnyType stmData, const Data::ByteArrayR &srcBuff)
{
	Bool found;
	UInt8 crcVal[4];
	Data::ByteArrayR buff = srcBuff;
	while (buff.GetSize() >= 10)
	{
		found = false;
		if (*(Int16*)&buff[0] == *(Int16*)"Sm" && *(Int16*)&buff[2] == *(Int16*)"SM")
		{
			UInt32 packetSize = ReadUInt16(&buff[6]);
			if (packetSize >= 10 && packetSize <= 2048)
			{
				if (packetSize > buff.GetSize())
					return buff.GetSize();

				this->crc->Calc(buff.Arr().Ptr(), packetSize - 2, crcVal);
				if (ReadMUInt16(&crcVal[2]) == ReadUInt16(&buff[packetSize - 2]))
				{
					this->listener->DataParsed(stm, stmObj, ReadUInt16(&buff[4]), 0, &buff[8], packetSize - 10);

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

UOSInt IO::ProtoHdlr::ProtoSMonHandler::BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, AnyType stmData)
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

	this->crc->Calc(buff, cmdSize + 8, crcVal);
	WriteInt16(&buff[cmdSize + 8], ReadMInt32(crcVal));
	return cmdSize + 10;
}
