#ifndef _SM_IO_CANHANDLER
#define _SM_IO_CANHANDLER

namespace IO
{
	class CANHandler
	{
	public:
		virtual ~CANHandler(){};

		virtual void CANMessage(UInt32 id, Bool rtr, const UInt8 *message, UOSInt msgLen) = 0;
	};
}
#endif
