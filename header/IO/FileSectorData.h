#ifndef _SM_IO_FILESECTORDATA
#define _SM_IO_FILESECTORDATA
#include "IO/ISectorData.h"
#include "IO/StreamData.h"

namespace IO
{
	class FileSectorData : public IO::ISectorData
	{
	private:
		IO::StreamData *data;
		UInt32 sectorSize;

	public:
		FileSectorData(IO::StreamData *data, UInt64 ofst, UInt64 dataSize, UInt32 sectorSize);
		virtual ~FileSectorData();

		virtual UInt64 GetSectorCount() const;
		virtual UOSInt GetBytesPerSector() const;
		virtual Bool ReadSector(UInt64 sectorNum, UInt8 *sectorBuff);
		virtual ISectorData *GetPartialData(UInt64 startSector, UInt64 sectorCount) const;
		virtual IO::StreamData *GetStreamData(UInt64 startSector, UInt64 dataSize) const;
		virtual UOSInt GetSeekCount() const;
	};
}
#endif
