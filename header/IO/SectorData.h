#ifndef _SM_IO_SECTORDATA
#define _SM_IO_SECTORDATA
#include "Data/ByteArray.h"
#include "IO/StreamData.h"
#include "IO/ParsedObject.h"

namespace IO
{
	class SectorData : public IO::ParsedObject
	{
	public:
		SectorData(NN<Text::String> name) : IO::ParsedObject(name){};
		virtual ~SectorData(){};

		virtual UInt64 GetSectorCount() const = 0;
		virtual UOSInt GetBytesPerSector() const = 0;
		virtual Bool ReadSector(UInt64 sectorNum, Data::ByteArray sectorBuff) = 0;
		virtual NN<SectorData> GetPartialData(UInt64 startSector, UInt64 sectorCount) const = 0;
		virtual NN<IO::StreamData> GetStreamData(UInt64 startSector, UInt64 dataSize) const = 0;
		virtual UOSInt GetSeekCount() const = 0;

		virtual IO::ParserType GetParserType() const { return IO::ParserType::SectorData; };
	};
}
#endif
