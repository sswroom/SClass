#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/CRC32R.h"
#include "Core/ByteTool_C.h"
#include "IO/ProtoHdlr/ProtoDUploadHandler.h"
#include "Sync/MutexUsage.h"

IO::ProtoHdlr::ProtoDUploadHandler::ProtoDUploadHandler(NN<IO::ProtocolHandler::DataListener> listener)
{
	this->listener = listener;
	NN<Crypto::Hash::CRC32R> crc;
	NEW_CLASSNN(crc, Crypto::Hash::CRC32R());
	NEW_CLASSNN(this->crc, Crypto::Hash::HashCalc(crc));
}

IO::ProtoHdlr::ProtoDUploadHandler::~ProtoDUploadHandler()
{
	this->crc.Delete();
}

AnyType IO::ProtoHdlr::ProtoDUploadHandler::CreateStreamData(NN<IO::Stream> stm)
{
	return 0;
}

void IO::ProtoHdlr::ProtoDUploadHandler::DeleteStreamData(NN<IO::Stream> stm, AnyType stmData)
{
}

UIntOS IO::ProtoHdlr::ProtoDUploadHandler::ParseProtocol(NN<IO::Stream> stm, AnyType stmObj, AnyType stmData, const Data::ByteArrayR &srcBuff)
{
	Bool found;
	UInt8 crcVal[4];
	Data::ByteArrayR buff = srcBuff;
	while (buff.GetSize() >= 8)
	{
		found = false;
		if (*(Int16*)&buff[0] == *(Int16*)"DU")
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

UIntOS IO::ProtoHdlr::ProtoDUploadHandler::BuildPacket(UnsafeArray<UInt8> buff, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UIntOS cmdSize, AnyType stmData)
{
	*(Int16*)&buff[0] = *(Int16*)"DU";
	WriteInt16(&buff[2], (Int16)cmdSize + 8);
	WriteInt16(&buff[4], cmdType);
	if (cmdSize > 0)
	{
		MemCopyNO(&buff[6], cmd.Ptr(), cmdSize);
	}
	UInt8 crcVal[4];
	this->crc->Calc(buff, cmdSize + 6, crcVal);
	WriteInt16(&buff[cmdSize + 6], ReadMInt32(crcVal));
	return cmdSize + 8;
}
