#ifndef _SM_IO_CANHANDLER
#define _SM_IO_CANHANDLER
#include "Data/ByteArray.h"

namespace IO
{
	class CANHandler
	{
	public:
		virtual ~CANHandler(){};

		virtual void CANMessage(UInt32 id, Bool rtr, Data::ByteArrayR message) = 0;
	};
}
#endif
