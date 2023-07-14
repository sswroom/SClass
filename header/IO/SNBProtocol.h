#ifndef _SM_IO_SNBPROTOCOL
#define _SM_IO_SNBPROTOCOL
#include "IO/Stream.h"

namespace IO
{
	class SNBProtocol
	{
	public:
		typedef void (__stdcall *ProtocolHandler)(void *userObj, UInt8 cmdType, UOSInt cmdSize, UInt8 *cmd);
	public:
		NotNullPtr<IO::Stream> stm;
		ProtocolHandler protoHdlr;
		void *protoObj;

		Bool running;
		Bool toStop;

		static UInt32 __stdcall RecvThread(void *userObj);
	public:
		SNBProtocol(NotNullPtr<IO::Stream> stm, ProtocolHandler protoHdlr, void *userObj);
		~SNBProtocol();

		void SendCommand(UInt8 cmdType, UOSInt cmdSize, UInt8 *cmd);
	};
}

#endif
