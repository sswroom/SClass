#include "Stdafx.h"
#include "MemTool.h"
#include "MyMemory.h"
#include "IO/CDSectorData.h"

IO::CDSectorData::CDSectorData(IO::ISectorData *data, UOSInt userOfst, UOSInt userDataSize, UInt64 startSector, UInt64 sectorCount) : IO::ISectorData(data->GetSourceNameObj()), sectorBuff(data->GetBytesPerSector())
{
	this->data = data->GetPartialData(startSector, sectorCount);
	this->userOfst = userOfst;
	this->userDataSize = userDataSize;
}

IO::CDSectorData::CDSectorData(IO::ISectorData *data, UOSInt userOfst, UOSInt userDataSize) : IO::ISectorData(data->GetSourceNameObj()), sectorBuff(data->GetBytesPerSector())
{
	this->data = data->GetPartialData(0, data->GetSectorCount());
	this->userOfst = userOfst;
	this->userDataSize = userDataSize;
}

IO::CDSectorData::~CDSectorData()
{
	DEL_CLASS(this->data);
}

UInt64 IO::CDSectorData::GetSectorCount() const
{
	return this->data->GetSectorCount();
}

UOSInt IO::CDSectorData::GetBytesPerSector() const
{
	return this->userDataSize;
}

Bool IO::CDSectorData::ReadSector(UInt64 sectorNum, Data::ByteArray sectorBuff)
{
	if (!this->data->ReadSector(sectorNum, this->sectorBuff))
		return false;
	sectorBuff.CopyFrom(this->sectorBuff.SubArray(this->userOfst, this->userDataSize));
	return true;
}

IO::ISectorData *IO::CDSectorData::GetPartialData(UInt64 startSector, UInt64 sectorCount) const
{
	IO::ISectorData *data;
	NEW_CLASS(data, IO::CDSectorData(this->data, this->userOfst, this->userDataSize, startSector, sectorCount));
	return data;
}

IO::StreamData *IO::CDSectorData::GetStreamData(UInt64 startSector, UInt64 dataSize) const
{
	IO::StreamData *data;
	UInt64 sectorCnt = dataSize / this->userDataSize;
	if ((dataSize % this->userDataSize) != 0)
	{
		sectorCnt++;
	}
	NEW_CLASS(data, IO::CDSectorStreamData(this->GetPartialData(startSector, sectorCnt), 0, dataSize));
	return data;
}

UOSInt IO::CDSectorData::GetSeekCount() const
{
	return this->data->GetSeekCount();
}

IO::CDSectorStreamData::CDSectorStreamData(IO::ISectorData *data, UOSInt sectorOfst, UInt64 dataSize) : sectorBuff(data->GetBytesPerSector())
{
	this->data = data;
	this->sectorOfst = sectorOfst;
	this->dataSize = dataSize;
}

IO::CDSectorStreamData::~CDSectorStreamData()
{
	DEL_CLASS(this->data);
}

UOSInt IO::CDSectorStreamData::GetRealData(UInt64 offset, UOSInt length, Data::ByteArray buffer)
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
		buffer.CopyFrom(Data::ByteArrayR(&this->sectorBuff[sectorOfst], thisSize));
		sectorOfst = 0;
		length -= thisSize;
		buffer += thisSize;
		retSize += thisSize;
		sectorStart++;
	}
	return retSize;
}

NotNullPtr<Text::String> IO::CDSectorStreamData::GetFullName()
{
	return this->data->GetSourceNameObj();
}

Text::CString IO::CDSectorStreamData::GetShortName()
{
	return this->data->GetSourceNameObj()->ToCString();
}

UInt64 IO::CDSectorStreamData::GetDataSize()
{
	return this->dataSize;
}

const UInt8 *IO::CDSectorStreamData::GetPointer()
{
	return 0;
}

IO::StreamData *IO::CDSectorStreamData::GetPartialData(UInt64 offset, UInt64 length)
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

	IO::StreamData *data;
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

UOSInt IO::CDSectorStreamData::GetSeekCount()
{
	return 0;
}
