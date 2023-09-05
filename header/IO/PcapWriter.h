#ifndef _SM_IO_PCAPWRITER
#define _SM_IO_PCAPWRITER
#include "IO/FileStream.h"
#include "IO/PacketLogWriter.h"
#include "Sync/Mutex.h"

namespace IO
{
	class PcapWriter : public PacketLogWriter
	{
	private:
		IO::FileStream fs;
		Sync::Mutex mut;
	public:
		PcapWriter(Text::CStringNN fileName, Int32 linkType);
		virtual ~PcapWriter();

		virtual Bool IsError() const;
		virtual Bool WritePacket(Data::ByteArrayR packet);
	};
}
#endif
