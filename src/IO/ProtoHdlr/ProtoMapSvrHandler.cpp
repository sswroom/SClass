#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/ProtoHdlr/ProtoMapSvrHandler.h"
#include "Sync/MutexUsage.h"

UInt16 IO::ProtoHdlr::ProtoMapSvrHandler::CalCheck(const UInt8 *buff, Int32 sz)
{
#ifndef HAS_ASM32
	UInt8 ChkDigit1;
	UInt8 ChkDigit2;
	Int32 i;
	Int32 j;
	ChkDigit1 = 0;
	ChkDigit2 = 0;
	i = 0;
	while (i < sz)
	{
		j = ChkDigit1 + (Int32)buff[i];
		ChkDigit1 = j & 255;
		ChkDigit2 = (((((Int32)ChkDigit2) << 1) | (ChkDigit2 >> 7)) + (j >> 8)) ^ buff[i];
		i++;
	}
	return ChkDigit1 | (((Int32)ChkDigit2) << 8);
#else
	_asm
	{
		mov esi,buff
		mov edx,0
		mov ecx,sz
mcclop:
		lodsb
		rol dh,1
		add dl,al
		adc dh,0
		xor dh,al
		dec ecx
		jnz mcclop
		mov ax,dx
	}
#endif
}

IO::ProtoHdlr::ProtoMapSvrHandler::ProtoMapSvrHandler(IO::IProtocolHandler::DataListener *listener)
{
	this->listener = listener;
}

IO::ProtoHdlr::ProtoMapSvrHandler::~ProtoMapSvrHandler()
{
}

void *IO::ProtoHdlr::ProtoMapSvrHandler::CreateStreamData(NotNullPtr<IO::Stream> stm)
{
	return 0;
}

void IO::ProtoHdlr::ProtoMapSvrHandler::DeleteStreamData(NotNullPtr<IO::Stream> stm, void *stmData)
{
}

UOSInt IO::ProtoHdlr::ProtoMapSvrHandler::ParseProtocol(NotNullPtr<IO::Stream> stm, void *stmObj, void *stmData, const Data::ByteArrayR &srcBuff)
{
	Bool found;
	UInt32 crcVal;
	Data::ByteArrayR buff = srcBuff;
	while (buff.GetSize() >= 8)
	{
		found = false;
		if (*(Int16*)&buff[0] == *(Int16*)"Ma")
		{
			UInt32 packetSize = *(UInt16*)&buff[2];
			if (packetSize <= 2048)
			{
				if (packetSize > buff.GetSize())
					return buff.GetSize();

				crcVal = CalCheck(buff.Ptr(), packetSize - 2);
				if ((crcVal & 0xffff) == *(UInt16*)&buff[packetSize - 2])
				{
					this->listener->DataParsed(stm, stmObj, *(UInt16*)&buff[4], 0, &buff[6], packetSize - 8);

					found = true;
					buff += packetSize;
				}
			}
		}
		else if (*(Int16*)&buff[0] == *(Int16*)"ma")
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
					this->listener->DataParsed(stm, stmObj, 0x10000 | *(UInt16*)&buff[4], 0, &buff[6], packetSize - 8);

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

UOSInt IO::ProtoHdlr::ProtoMapSvrHandler::BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, void *stmData)
{
	*(Int16*)&buff[2] = (Int16)(cmdSize + 8);
	*(Int16*)&buff[4] = cmdType;
	if (cmdSize > 0)
	{
		MemCopyNO(&buff[6], cmd, cmdSize);
	}

	if (cmdType & 0x10000)
	{
		UInt32 crcVal;
		*(Int16*)buff = *(Int16*)"ma";

		Sync::MutexUsage mutUsage(this->crcMut);
		this->crc.Clear();
		this->crc.Calc(buff, cmdSize + 6);
		this->crc.GetValue((UInt8*)&crcVal);
		mutUsage.EndUse();
		*(UInt16*)&buff[cmdSize + 6] = (UInt16)crcVal;
	}
	else
	{
		*(Int16*)buff = *(Int16*)"Ma";
		*(UInt16*)&buff[cmdSize + 6] = CalCheck(buff, (Int32)cmdSize + 6);
	}
	return cmdSize + 8;
}
