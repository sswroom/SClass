#ifndef _SM_IO_CDSECTORDATA
#define _SM_IO_CDSECTORDATA
#include "Data/ByteBuffer.h"
#include "IO/SectorData.h"
#include "IO/StreamData.h"

namespace IO
{
	class CDSectorData : public IO::SectorData
	{
	private:
		NN<IO::SectorData> data;
		UOSInt userOfst;
		UOSInt userDataSize;
		Data::ByteBuffer sectorBuff;

		CDSectorData(NN<IO::SectorData> data, UOSInt userOfst, UOSInt userDataSize, UInt64 startSector, UInt64 sectorCount);
	public:
		CDSectorData(NN<IO::SectorData> data, UOSInt userOfst, UOSInt userDataSize);
		virtual ~CDSectorData();

		virtual UInt64 GetSectorCount() const;
		virtual UOSInt GetBytesPerSector() const;
		virtual Bool ReadSector(UInt64 sectorNum, Data::ByteArray sectorBuff);
		virtual NN<SectorData> GetPartialData(UInt64 startSector, UInt64 sectorCount) const;
		virtual NN<IO::StreamData> GetStreamData(UInt64 startSector, UInt64 dataSize) const;
		virtual UOSInt GetSeekCount() const;
	};

	class CDSectorStreamData : public IO::StreamData
	{
	private:
		NN<IO::SectorData> data;
		UOSInt sectorOfst;
		UInt64 dataSize;
		Data::ByteBuffer sectorBuff;

	public:
		CDSectorStreamData(NN<IO::SectorData> data, UOSInt sectorOfst, UInt64 dataSize);
		virtual ~CDSectorStreamData();
		virtual UOSInt GetRealData(UInt64 offset, UOSInt length, Data::ByteArray buffer);
		virtual NN<Text::String> GetFullName() const;
		virtual Text::CString GetShortName() const;
		virtual UInt64 GetDataSize() const;
		virtual UnsafeArrayOpt<const UInt8> GetPointer() const;

		virtual NN<StreamData> GetPartialData(UInt64 offset, UInt64 length);
		virtual Bool IsFullFile() const;
		virtual Bool IsLoading() const;
		virtual UOSInt GetSeekCount() const;
	};
}
#endif
