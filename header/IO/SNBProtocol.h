#ifndef _SM_IO_SNBPROTOCOL
#define _SM_IO_SNBPROTOCOL
#include "AnyType.h"
#include "IO/Stream.h"

namespace IO
{
	class SNBProtocol
	{
	public:
		typedef void (__stdcall *ProtocolHandler)(AnyType userObj, UInt8 cmdType, UOSInt cmdSize, UInt8 *cmd);
	public:
		NN<IO::Stream> stm;
		ProtocolHandler protoHdlr;
		AnyType protoObj;

		Bool running;
		Bool toStop;

		static UInt32 __stdcall RecvThread(AnyType userObj);
	public:
		SNBProtocol(NN<IO::Stream> stm, ProtocolHandler protoHdlr, AnyType userObj);
		~SNBProtocol();

		void SendCommand(UInt8 cmdType, UOSInt cmdSize, UInt8 *cmd);
	};
}

#endif
