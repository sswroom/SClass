#ifndef _SM_IO_FILESECTORDATA
#define _SM_IO_FILESECTORDATA
#include "IO/ISectorData.h"
#include "IO/IStreamData.h"

namespace IO
{
	class FileSectorData : public IO::ISectorData
	{
	private:
		IO::IStreamData *data;
		UInt32 sectorSize;

	public:
		FileSectorData(IO::IStreamData *data, UInt64 ofst, UInt64 dataSize, UInt32 sectorSize);
		virtual ~FileSectorData();

		virtual UInt64 GetSectorCount();
		virtual UOSInt GetBytesPerSector();
		virtual Bool ReadSector(UInt64 sectorNum, UInt8 *sectorBuff);
		virtual ISectorData *GetPartialData(UInt64 startSector, UInt64 sectorCount);
		virtual IO::IStreamData *GetStreamData(UInt64 startSector, UInt64 dataSize);
		virtual OSInt GetSeekCount();
	};
}
#endif
