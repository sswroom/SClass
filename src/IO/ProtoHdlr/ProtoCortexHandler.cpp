#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/ProtoHdlr/ProtoCortexHandler.h"

IO::ProtoHdlr::ProtoCortexHandler::ProtoCortexHandler(NN<IO::ProtocolHandler::DataListener> listener)
{
	this->listener = listener;
}

IO::ProtoHdlr::ProtoCortexHandler::~ProtoCortexHandler()
{
}

AnyType IO::ProtoHdlr::ProtoCortexHandler::CreateStreamData(NN<IO::Stream> stm)
{
	return 0;
}

void IO::ProtoHdlr::ProtoCortexHandler::DeleteStreamData(NN<IO::Stream> stm, AnyType stmData)
{
}

UOSInt IO::ProtoHdlr::ProtoCortexHandler::ParseProtocol(NN<IO::Stream> stm, AnyType stmObj, AnyType stmData, const Data::ByteArrayR &buff)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UInt8 cmdSize;
	UInt8 chk;
	i = 0;
	j = 0;
	while (i < buff.GetSize())
	{
		if (buff[i] == 0xff)
		{
			if (i != j)
			{
				this->listener->DataSkipped(stm, stmObj, &buff[j], i - j);
				j = i;
			}
			if (i + 4 > buff.GetSize())
				return buff.GetSize() - i;
			cmdSize = buff[i + 2];
			if (i + 4 + cmdSize > buff.GetSize())
				return buff.GetSize() - i;
			k = i + 3 + cmdSize;
			chk = 0;
			while (k-- > i)
			{
				chk = (UInt8)((chk + buff[k]) & 0xff);
			}
			if (chk == buff[i + cmdSize + 3])
			{
				this->listener->DataParsed(stm, stmObj, buff[i + 1], 0, &buff[i + 3], cmdSize);

				i += cmdSize + 4;
				j = i;
			}
			else
			{
			}
		}
		i++;
	}
	if (i != j)
	{
		this->listener->DataSkipped(stm, stmObj, &buff[j], i - j);
	}
	return 0;
}

UOSInt IO::ProtoHdlr::ProtoCortexHandler::BuildPacket(UnsafeArray<UInt8> buff, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UOSInt cmdSize, AnyType stmData)
{
	UInt8 chk;
	UOSInt i;
	buff[0] = 0xff;
	buff[1] = (UInt8)(cmdType & 0xff);
	buff[2] = (UInt8)(cmdSize & 0xff);
	if (cmdSize > 0)
	{
		MemCopyNO(&buff[3], cmd.Ptr(), cmdSize);
	}
	i = 3 + cmdSize;
	chk = 0;
	while (i-- > 0)
	{
		chk = (UInt8)((chk + buff[i]) & 0xff);
	}
	buff[cmdSize + 3] = chk;
	return cmdSize + 4;
}
