#ifndef _SM_IO_CDXADATA
#define _SM_IO_CDXADATA
#include "IO/ISectorData.h"
#include "IO/IStreamData.h"

namespace IO
{
	class CDXAData : public IO::ISectorData
	{
	private:
		IO::IStreamData *data;

	public:
		CDXAData(IO::IStreamData *data, UInt64 ofst, UInt64 dataSize);
		virtual ~CDXAData();

		virtual UInt64 GetSectorCount();
		virtual UOSInt GetBytesPerSector();
		virtual Bool ReadSector(UInt64 sectorNum, UInt8 *sectorBuff);
		virtual ISectorData *GetPartialData(UInt64 startSector, UInt64 sectorCount);
 		virtual IO::IStreamData *GetStreamData(UInt64 startSector, UInt64 dataSize);
		virtual UOSInt GetSeekCount();
	};
}
#endif
