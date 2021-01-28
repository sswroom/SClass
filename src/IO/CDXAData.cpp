#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/CDXAData.h"

IO::CDXAData::CDXAData(IO::IStreamData *data, UInt64 ofst, UInt64 dataSize) : IO::ISectorData(data->GetFullName())
{
	this->data = data->GetPartialData(ofst, dataSize);
}

IO::CDXAData::~CDXAData()
{
	DEL_CLASS(this->data);
}

UInt64 IO::CDXAData::GetSectorCount()
{
	return this->data->GetDataSize() / 2352;
}

UOSInt IO::CDXAData::GetBytesPerSector()
{
	return 2352;
}

Bool IO::CDXAData::ReadSector(UInt64 sectorNum, UInt8 *sectorBuff)
{
	return this->data->GetRealData(sectorNum * 2352LL, 2352, sectorBuff) == 2352;
}

IO::ISectorData *IO::CDXAData::GetPartialData(UInt64 startSector, UInt64 sectorCount)
{
	IO::ISectorData *data;
	NEW_CLASS(data, IO::CDXAData(this->data, startSector * 2352, sectorCount * 2352));
	return data;
}

IO::IStreamData *IO::CDXAData::GetStreamData(UInt64 startSector, UInt64 dataSize)
{
	return this->data->GetPartialData(startSector * 2352, dataSize);
}

OSInt IO::CDXAData::GetSeekCount()
{
	return this->data->GetSeekCount();
}
