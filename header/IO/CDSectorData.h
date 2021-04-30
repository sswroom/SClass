#ifndef _SM_IO_CDSECTORDATA
#define _SM_IO_CDSECTORDATA
#include "IO/ISectorData.h"
#include "IO/IStreamData.h"

namespace IO
{
	class CDSectorData : public IO::ISectorData
	{
	private:
		IO::ISectorData *data;
		UOSInt userOfst;
		UOSInt userDataSize;
		UInt8 *sectorBuff;

		CDSectorData(IO::ISectorData *data, UOSInt userOfst, UOSInt userDataSize, UInt64 startSector, UInt64 sectorCount);
	public:
		CDSectorData(IO::ISectorData *data, UOSInt userOfst, UOSInt userDataSize);
		virtual ~CDSectorData();

		virtual UInt64 GetSectorCount();
		virtual UOSInt GetBytesPerSector();
		virtual Bool ReadSector(UInt64 sectorNum, UInt8 *sectorBuff);
		virtual ISectorData *GetPartialData(UInt64 startSector, UInt64 sectorCount);
		virtual IO::IStreamData *GetStreamData(UInt64 startSector, UInt64 dataSize);
		virtual UOSInt GetSeekCount();
	};

	class CDSectorStreamData : public IO::IStreamData
	{
	private:
		IO::ISectorData *data;
		UOSInt sectorOfst;
		UInt64 dataSize;
		UInt8 *sectorBuff;

	public:
		CDSectorStreamData(IO::ISectorData *data, UOSInt sectorOfst, UInt64 dataSize);
		virtual ~CDSectorStreamData();
		virtual UOSInt GetRealData(UInt64 offset, UOSInt length, UInt8 *buffer);
		virtual const UTF8Char *GetFullName();
		virtual const UTF8Char *GetShortName();
		virtual UInt64 GetDataSize();
		virtual const UInt8 *GetPointer();

		virtual IStreamData *GetPartialData(UInt64 offset, UInt64 length);
		virtual Bool IsFullFile();
		virtual Bool IsLoading();
		virtual UOSInt GetSeekCount();
	};
}
#endif
