#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/CDXAData.h"

IO::CDXAData::CDXAData(NN<IO::StreamData> data, UInt64 ofst, UInt64 dataSize) : IO::SectorData(data->GetFullName())
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

UIntOS IO::CDXAData::GetBytesPerSector() const
{
	return 2352;
}

Bool IO::CDXAData::ReadSector(UInt64 sectorNum, Data::ByteArray sectorBuff)
{
	return this->data->GetRealData(sectorNum * 2352LL, 2352, sectorBuff) == 2352;
}

NN<IO::SectorData> IO::CDXAData::GetPartialData(UInt64 startSector, UInt64 sectorCount) const
{
	NN<IO::SectorData> data;
	NEW_CLASSNN(data, IO::CDXAData(this->data, startSector * 2352, sectorCount * 2352));
	return data;
}

NN<IO::StreamData> IO::CDXAData::GetStreamData(UInt64 startSector, UInt64 dataSize) const
{
	return this->data->GetPartialData(startSector * 2352, dataSize);
}

UIntOS IO::CDXAData::GetSeekCount() const
{
	return this->data->GetSeekCount();
}
