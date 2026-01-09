#ifndef _SM_IO_PROTOCOLBUFFERSMESSAGE
#define _SM_IO_PROTOCOLBUFFERSMESSAGE
#include "Text/CString.h"

namespace IO
{
	class ProtocolBuffersMessage
	{
	public:
		ProtocolBuffersMessage(Text::CStringNN name);
		~ProtocolBuffersMessage();

		Bool ParseMsssage(UnsafeArray<const UInt8> buff, UOSInt buffSize);
	};
}
#endif
