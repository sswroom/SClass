#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/CRC16.h"
#include "Data/ByteTool.h"
#include "IO/ProtoHdlr/ProtoJMVL01Handler.h"

IO::ProtoHdlr::ProtoJMVL01Handler::ProtoJMVL01Handler(IO::IProtocolHandler::DataListener *listener, UInt64 devId) : crc(Crypto::Hash::CRC16::GetPolynomialCCITT())
{
	this->listener = listener;
	this->devId = devId;
}

IO::ProtoHdlr::ProtoJMVL01Handler::~ProtoJMVL01Handler()
{
}

void *IO::ProtoHdlr::ProtoJMVL01Handler::CreateStreamData(NotNullPtr<IO::Stream> stm)
{
	return (void*)-1;
}

void IO::ProtoHdlr::ProtoJMVL01Handler::DeleteStreamData(NotNullPtr<IO::Stream> stm, void *stmData)
{
}

UOSInt IO::ProtoHdlr::ProtoJMVL01Handler::ParseProtocol(NotNullPtr<IO::Stream> stm, void *stmObj, void *stmData, const UInt8 *buff, UOSInt buffSize)
{
	UInt16 crcVal;
	UInt16 len;
	UOSInt i;
	i = 0;
	while (i < buffSize - 1)
	{
		if (buff[i] == 0x78 && buff[i + 1] == 0x78)
		{
			if (i + 11 > buffSize)
			{
				return buffSize - i;
			}
			len = buff[i + 2];
			if (len + i + 5 > buffSize)
			{
				return buffSize - i;
			}
			crcVal = this->crc.CalcDirect(&buff[i + 2], (UOSInt)len - 1);
			if (crcVal == ReadMUInt16(&buff[i + 1 + len]) && buff[i + 3 + len] == 13 && buff[i + 4 + len] == 10)
			{
				this->listener->DataParsed(stm, stmObj, buff[i + 3], ReadMUInt16(&buff[i - 1 + len]), &buff[i + 4], (UOSInt)len - 5);
				i += (UOSInt)len + 4;
			}
		}
		else if (buff[i] == 0x79 && buff[i + 1] == 0x79)
		{
			if (i + 12 > buffSize)
			{
				return buffSize - i;
			}
			len = ReadMUInt16(&buff[i + 2]);
			if (len + i + 6 > buffSize)
			{
				return buffSize - i;
			}
			crcVal = this->crc.CalcDirect(&buff[i + 2], len);
			if (crcVal == ReadMUInt16(&buff[i + 2 + len]) && buff[i + 4 + len] == 13 && buff[i + 5 + len] == 10)
			{
				this->listener->DataParsed(stm, stmObj, buff[i + 4], ReadMUInt16(&buff[i + len]), &buff[i + 5], (UOSInt)len - 5);
				i += (UOSInt)len + 5;
			}
		}
		i++;
	}
	return buffSize - i;
}

UOSInt IO::ProtoHdlr::ProtoJMVL01Handler::BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, void *stmData)
{
	if (cmdSize >= 1024)
		return 0;
	if (cmdSize > 250)
	{
		buff[0] = 0x79;
		buff[1] = 0x79;
		WriteMUInt16(&buff[2], (cmdSize + 5));
		buff[4] = (UInt8)cmdType;
		MemCopyNO(&buff[5], cmd, cmdSize);
		WriteMUInt16(&buff[5 + cmdSize], seqId);
		UInt16 crc = this->crc.CalcDirect(&buff[2], 5 + cmdSize);
		WriteMUInt16(&buff[7 + cmdSize], crc);
		buff[9 + cmdSize] = 13;
		buff[10 + cmdSize] = 10;
		return 11 + cmdSize;
	}
	else
	{
		buff[0] = 0x78;
		buff[1] = 0x78;
		buff[2] = (UInt8)(cmdSize + 5);
		buff[3] = (UInt8)cmdType;
		MemCopyNO(&buff[4], cmd, cmdSize);
		WriteMUInt16(&buff[4 + cmdSize], seqId);
		UInt16 crc = this->crc.CalcDirect(&buff[2], 4 + cmdSize);
		WriteMUInt16(&buff[6 + cmdSize], crc);
		buff[8 + cmdSize] = 13;
		buff[9 + cmdSize] = 10;
		return 10 + cmdSize;
	}
}
