#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/CDXAData.h"

IO::CDXAData::CDXAData(NotNullPtr<IO::StreamData> data, UInt64 ofst, UInt64 dataSize) : IO::ISectorData(data->GetFullName())
{
	this->data = data->GetPartialData(ofst, dataSize);
}

IO::CDXAData::~CDXAData()
{
	this->data.Delete();
}

UInt64 IO::CDXAData::GetSectorCount() const
{
	return this->data->GetDataSize() / 2352;
}

UOSInt IO::CDXAData::GetBytesPerSector() const
{
	return 2352;
}

Bool IO::CDXAData::ReadSector(UInt64 sectorNum, Data::ByteArray sectorBuff)
{
	return this->data->GetRealData(sectorNum * 2352LL, 2352, sectorBuff) == 2352;
}

NotNullPtr<IO::ISectorData> IO::CDXAData::GetPartialData(UInt64 startSector, UInt64 sectorCount) const
{
	NotNullPtr<IO::ISectorData> data;
	NEW_CLASSNN(data, IO::CDXAData(this->data, startSector * 2352, sectorCount * 2352));
	return data;
}

NotNullPtr<IO::StreamData> IO::CDXAData::GetStreamData(UInt64 startSector, UInt64 dataSize) const
{
	return this->data->GetPartialData(startSector * 2352, dataSize);
}

UOSInt IO::CDXAData::GetSeekCount() const
{
	return this->data->GetSeekCount();
}
