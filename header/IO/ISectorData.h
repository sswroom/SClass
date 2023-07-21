#ifndef _SM_IO_ISECTORDATA
#define _SM_IO_ISECTORDATA
#include "Data/ByteArray.h"
#include "IO/StreamData.h"
#include "IO/ParsedObject.h"

namespace IO
{
	class ISectorData : public IO::ParsedObject
	{
	public:
		ISectorData(NotNullPtr<Text::String> name) : IO::ParsedObject(name){};
		virtual ~ISectorData(){};

		virtual UInt64 GetSectorCount() const = 0;
		virtual UOSInt GetBytesPerSector() const = 0;
		virtual Bool ReadSector(UInt64 sectorNum, Data::ByteArray sectorBuff) = 0;
		virtual ISectorData *GetPartialData(UInt64 startSector, UInt64 sectorCount) const = 0;
		virtual NotNullPtr<IO::StreamData> GetStreamData(UInt64 startSector, UInt64 dataSize) const = 0;
		virtual UOSInt GetSeekCount() const = 0;

		virtual IO::ParserType GetParserType() const { return IO::ParserType::SectorData; };
	};
}
#endif
