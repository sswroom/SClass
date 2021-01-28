#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/CDSectorData.h"

IO::CDSectorData::CDSectorData(IO::ISectorData *data, UOSInt userOfst, UOSInt userDataSize, UInt64 startSector, UInt64 sectorCount) : IO::ISectorData(data->GetSourceNameObj())
{
	this->data = data->GetPartialData(startSector, sectorCount);
	this->userOfst = userOfst;
	this->userDataSize = userDataSize;
	this->sectorBuff = MemAlloc(UInt8, this->data->GetBytesPerSector());
}

IO::CDSectorData::CDSectorData(IO::ISectorData *data, UOSInt userOfst, UOSInt userDataSize) : IO::ISectorData(data->GetSourceNameObj())
{
	this->data = data->GetPartialData(0, data->GetSectorCount());
	this->userOfst = userOfst;
	this->userDataSize = userDataSize;
	this->sectorBuff = MemAlloc(UInt8, this->data->GetBytesPerSector());
}

IO::CDSectorData::~CDSectorData()
{
	DEL_CLASS(this->data);
	MemFree(this->sectorBuff);
}

UInt64 IO::CDSectorData::GetSectorCount()
{
	return this->data->GetSectorCount();
}

UOSInt IO::CDSectorData::GetBytesPerSector()
{
	return this->userDataSize;
}

Bool IO::CDSectorData::ReadSector(UInt64 sectorNum, UInt8 *sectorBuff)
{
	if (!this->data->ReadSector(sectorNum, this->sectorBuff))
		return false;
	MemCopyNO(sectorBuff, &this->sectorBuff[this->userOfst], this->userDataSize);
	return true;
}

IO::ISectorData *IO::CDSectorData::GetPartialData(UInt64 startSector, UInt64 sectorCount)
{
	IO::ISectorData *data;
	NEW_CLASS(data, IO::CDSectorData(this->data, this->userOfst, this->userDataSize, startSector, sectorCount));
	return data;
}

IO::IStreamData *IO::CDSectorData::GetStreamData(UInt64 startSector, UInt64 dataSize)
{
	IO::IStreamData *data;
	Int64 sectorCnt = dataSize / this->userDataSize;
	if ((dataSize % this->userDataSize) != 0)
	{
		sectorCnt++;
	}
	NEW_CLASS(data, IO::CDSectorStreamData(this->GetPartialData(startSector, sectorCnt), 0, dataSize));
	return data;
}

OSInt IO::CDSectorData::GetSeekCount()
{
	return this->data->GetSeekCount();
}

IO::CDSectorStreamData::CDSectorStreamData(IO::ISectorData *data, OSInt sectorOfst, Int64 dataSize)
{
	this->data = data;
	this->sectorOfst = sectorOfst;
	this->dataSize = dataSize;
	OSInt sectorSize = this->data->GetBytesPerSector();
	this->sectorBuff = MemAlloc(UInt8, sectorSize);
}

IO::CDSectorStreamData::~CDSectorStreamData()
{
	MemFree(this->sectorBuff);
	DEL_CLASS(this->data);
}

UOSInt IO::CDSectorStreamData::GetRealData(UInt64 offset, UOSInt length, UInt8 *buffer)
{
	UOSInt retSize = 0;
	offset += this->sectorOfst;
	UOSInt sectorSize = this->data->GetBytesPerSector();
	UInt64 sectorStart = offset / sectorSize;
	UOSInt sectorOfst = (UOSInt)(offset % sectorSize);
	UOSInt thisSize;
	while (length > 0)
	{
		if (!this->data->ReadSector(sectorStart, this->sectorBuff))
			break;
		if ((sectorOfst + length) > sectorSize)
		{
			thisSize = sectorSize - sectorOfst;
		}
		else
		{
			thisSize = length;
		}
		MemCopyNO(buffer, &this->sectorBuff[sectorOfst], thisSize);
		sectorOfst = 0;
		length -= thisSize;
		buffer += thisSize;
		retSize += thisSize;
		sectorStart++;
	}
	return retSize;
}

const UTF8Char *IO::CDSectorStreamData::GetFullName()
{
	return this->data->GetSourceNameObj();
}

const UTF8Char *IO::CDSectorStreamData::GetShortName()
{
	return this->data->GetSourceNameObj();
}

UInt64 IO::CDSectorStreamData::GetDataSize()
{
	return this->dataSize;
}

const UInt8 *IO::CDSectorStreamData::GetPointer()
{
	return 0;
}

IO::IStreamData *IO::CDSectorStreamData::GetPartialData(UInt64 offset, UInt64 length)
{
	UInt64 endOfst = offset + length;
	if (endOfst > (this->dataSize + this->sectorOfst))
	{
		endOfst = (this->dataSize + this->sectorOfst);
	}
	UOSInt sectorSize = this->data->GetBytesPerSector();
	UInt64 rawOfst = offset + this->sectorOfst;
	UInt64 startSector = rawOfst / sectorSize;
	UInt64 endSector = (endOfst + sectorSize - 1) / sectorSize;

	IO::IStreamData *data;
	NEW_CLASS(data, CDSectorStreamData(this->data->GetPartialData(startSector, endSector - startSector), (UOSInt)(rawOfst % sectorSize), endOfst - rawOfst));
	return data;
}

Bool IO::CDSectorStreamData::IsFullFile()
{
	return false;
}

Bool IO::CDSectorStreamData::IsLoading()
{
	return false;
}

OSInt IO::CDSectorStreamData::GetSeekCount()
{
	return 0;
}
