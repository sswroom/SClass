#ifndef _SM_IO_PCAPWRITER
#define _SM_IO_PCAPWRITER
#include "IO/FileStream.h"
#include "IO/PacketAnalyse.h"
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
		PcapWriter(Text::CStringNN fileName, IO::PacketAnalyse::LinkType linkType);
		virtual ~PcapWriter();

		virtual Bool IsError() const;
		virtual Bool WritePacket(Data::ByteArrayR packet);
	};
}
#endif
