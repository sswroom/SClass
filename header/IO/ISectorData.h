#ifndef _SM_IO_ISECTORDATA
#define _SM_IO_ISECTORDATA
#include "IO/IStreamData.h"
#include "IO/ParsedObject.h"

namespace IO
{
	class ISectorData : public IO::ParsedObject
	{
	public:
		ISectorData(Text::String *name) : IO::ParsedObject(name){};
		virtual ~ISectorData(){};

		virtual UInt64 GetSectorCount() = 0;
		virtual UOSInt GetBytesPerSector() = 0;
		virtual Bool ReadSector(UInt64 sectorNum, UInt8 *sectorBuff) = 0;
		virtual ISectorData *GetPartialData(UInt64 startSector, UInt64 sectorCount) = 0;
		virtual IO::IStreamData *GetStreamData(UInt64 startSector, UInt64 dataSize) = 0;
		virtual UOSInt GetSeekCount() = 0;

		virtual IO::ParserType GetParserType() { return IO::ParserType::SectorData; };
	};
}
#endif
