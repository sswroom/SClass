#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/FileStream.h"
#include "IO/ProtoHdlr/ProtoJMVL01Handler.h"
#include <stdio.h>

class ProtoListener : public IO::IProtocolHandler::DataListener
{
public:
	virtual void DataParsed(IO::Stream *stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize)
	{
		printf("Received cmdType 0x%x\r\n", cmdType);
	}

	virtual void DataSkipped(IO::Stream *stm, void *stmObj, const UInt8 *buff, UOSInt buffSize)
	{

	}
};

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UInt8 data[4096];
	UOSInt dataSize = 0;
	UOSInt dataLeft;
	ProtoListener listener;
	IO::ProtoHdlr::ProtoJMVL01Handler protoHdlr(&listener, 0);
	IO::FileStream fs(CSTR("/home/sswroom/Progs/Temp/20220519 JM-VL01/1652879977070_B6EF576F_62933r.dat"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	while (true)
	{
		dataLeft = fs.Read(&data[dataSize], 4096 - dataSize);
		if (dataLeft == 0)
		{
			break;
		}
		dataSize += dataLeft;
		dataLeft = protoHdlr.ParseProtocol(0, 0, 0, data, dataSize);
		if (dataLeft == dataSize)
		{

		}
		else if (dataLeft == 0)
		{
			dataSize = 0;
		}
		else
		{
			MemCopyO(data, &data[dataSize - dataLeft], dataLeft);
			dataSize = dataLeft;
		}
	}
	return 0;
}
