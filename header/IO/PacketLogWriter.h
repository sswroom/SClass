#ifndef _SM_IO_PACKETLOGWRITER
#define _SM_IO_PACKETLOGWRITER
#include "Data/ByteArray.h"

namespace IO
{
	class PacketLogWriter
	{
	public:
		virtual ~PacketLogWriter() {};

		virtual Bool IsError() const = 0;
		virtual Bool WritePacket(Data::ByteArrayR packet) = 0;
	};
}
#endif
