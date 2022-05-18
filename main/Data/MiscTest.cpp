#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/FileStream.h"
#include "IO/ProtoHdlr/ProtoJMVL01Handler.h"

class ProtoListener : public IO::IProtocolHandler::DataListener
{
public:
	virtual void DataParsed(IO::Stream *stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize)
	{

	}

	virtual void DataSkipped(IO::Stream *stm, void *stmObj, const UInt8 *buff, UOSInt buffSize)
	{

	}
};

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UInt8 data[128];
	UOSInt dataSize = IO::FileStream::LoadFile(CSTR("/home/sswroom/Progs/Temp/1652878633008_B6EF576F_62443r.dat"), data, sizeof(data));
	ProtoListener listener;
	IO::ProtoHdlr::ProtoJMVL01Handler protoHdlr(&listener, 0);
	protoHdlr.ParseProtocol(0, 0, 0, data, dataSize);
	return 0;
}
