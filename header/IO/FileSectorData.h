#ifndef _SM_IO_FILESECTORDATA
#define _SM_IO_FILESECTORDATA
#include "IO/SectorData.h"
#include "IO/StreamData.h"

namespace IO
{
	class FileSectorData : public IO::SectorData
	{
	private:
		NN<IO::StreamData> data;
		UInt32 sectorSize;

	public:
		FileSectorData(NN<IO::StreamData> data, UInt64 ofst, UInt64 dataSize, UInt32 sectorSize);
		virtual ~FileSectorData();

		virtual UInt64 GetSectorCount() const;
		virtual UOSInt GetBytesPerSector() const;
		virtual Bool ReadSector(UInt64 sectorNum, Data::ByteArray sectorBuff);
		virtual NN<SectorData> GetPartialData(UInt64 startSector, UInt64 sectorCount) const;
		virtual NN<IO::StreamData> GetStreamData(UInt64 startSector, UInt64 dataSize) const;
		virtual UOSInt GetSeekCount() const;
	};
}
#endif
