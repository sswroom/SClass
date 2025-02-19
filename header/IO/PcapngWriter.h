#ifndef _SM_IO_PCAPNGWRITER
#define _SM_IO_PCAPNGWRITER
#include "IO/FileStream.h"
#include "IO/PacketAnalyse.h"
#include "IO/PacketLogWriter.h"
#include "Sync/Mutex.h"

namespace IO
{
	class PcapngWriter : public PacketLogWriter
	{
	private:
		IO::FileStream fs;
		Sync::Mutex mut;
	public:
		PcapngWriter(Text::CStringNN fileName, IO::PacketAnalyse::LinkType linkType, Text::CStringNN appName);
		virtual ~PcapngWriter();

		virtual Bool IsError() const;
		virtual Bool WritePacket(Data::ByteArrayR packet);
	};
}
#endif
