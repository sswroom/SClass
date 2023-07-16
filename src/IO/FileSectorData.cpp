#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileSectorData.h"

IO::FileSectorData::FileSectorData(IO::StreamData *data, UInt64 ofst, UInt64 dataSize, UInt32 sectorSize) : IO::ISectorData(data->GetFullName())
{
	this->data = data->GetPartialData(ofst, dataSize);
	this->sectorSize = sectorSize;
}

IO::FileSectorData::~FileSectorData()
{
	DEL_CLASS(this->data);
}

UInt64 IO::FileSectorData::GetSectorCount() const
{
	return this->data->GetDataSize() / sectorSize;
}

UOSInt IO::FileSectorData::GetBytesPerSector() const
{
	return this->sectorSize;
}

Bool IO::FileSectorData::ReadSector(UInt64 sectorNum, Data::ByteArray sectorBuff)
{
	return this->data->GetRealData(sectorNum * sectorSize, sectorSize, sectorBuff) == sectorSize;
}

IO::ISectorData *IO::FileSectorData::GetPartialData(UInt64 startSector, UInt64 sectorCount) const
{
	IO::ISectorData *data;
	NEW_CLASS(data, IO::FileSectorData(this->data, startSector * sectorSize, sectorCount * sectorSize, sectorSize));
	return data;
}

IO::StreamData *IO::FileSectorData::GetStreamData(UInt64 startSector, UInt64 dataSize) const
{
	return this->data->GetPartialData(startSector * sectorSize, dataSize);
}

UOSInt IO::FileSectorData::GetSeekCount() const
{
	return this->data->GetSeekCount();
}
