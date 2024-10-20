#ifndef _SM_IO_CDSECTORDATA
#define _SM_IO_CDSECTORDATA
#include "Data/ByteBuffer.h"
#include "IO/ISectorData.h"
#include "IO/StreamData.h"

namespace IO
{
	class CDSectorData : public IO::ISectorData
	{
	private:
		NN<IO::ISectorData> data;
		UOSInt userOfst;
		UOSInt userDataSize;
		Data::ByteBuffer sectorBuff;

		CDSectorData(NN<IO::ISectorData> data, UOSInt userOfst, UOSInt userDataSize, UInt64 startSector, UInt64 sectorCount);
	public:
		CDSectorData(NN<IO::ISectorData> data, UOSInt userOfst, UOSInt userDataSize);
		virtual ~CDSectorData();

		virtual UInt64 GetSectorCount() const;
		virtual UOSInt GetBytesPerSector() const;
		virtual Bool ReadSector(UInt64 sectorNum, Data::ByteArray sectorBuff);
		virtual NN<ISectorData> GetPartialData(UInt64 startSector, UInt64 sectorCount) const;
		virtual NN<IO::StreamData> GetStreamData(UInt64 startSector, UInt64 dataSize) const;
		virtual UOSInt GetSeekCount() const;
	};

	class CDSectorStreamData : public IO::StreamData
	{
	private:
		NN<IO::ISectorData> data;
		UOSInt sectorOfst;
		UInt64 dataSize;
		Data::ByteBuffer sectorBuff;

	public:
		CDSectorStreamData(NN<IO::ISectorData> data, UOSInt sectorOfst, UInt64 dataSize);
		virtual ~CDSectorStreamData();
		virtual UOSInt GetRealData(UInt64 offset, UOSInt length, Data::ByteArray buffer);
		virtual NN<Text::String> GetFullName();
		virtual Text::CString GetShortName();
		virtual UInt64 GetDataSize();
		virtual UnsafeArrayOpt<const UInt8> GetPointer();

		virtual NN<StreamData> GetPartialData(UInt64 offset, UInt64 length);
		virtual Bool IsFullFile();
		virtual Bool IsLoading();
		virtual UOSInt GetSeekCount();
	};
}
#endif
