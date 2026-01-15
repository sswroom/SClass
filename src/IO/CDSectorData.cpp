#include "Stdafx.h"
#include "MemTool.h"
#include "MyMemory.h"
#include "IO/CDSectorData.h"

IO::CDSectorData::CDSectorData(NN<IO::SectorData> data, UIntOS userOfst, UIntOS userDataSize, UInt64 startSector, UInt64 sectorCount) : IO::SectorData(data->GetSourceNameObj()), sectorBuff(data->GetBytesPerSector())
{
	this->data = data->GetPartialData(startSector, sectorCount);
	this->userOfst = userOfst;
	this->userDataSize = userDataSize;
}

IO::CDSectorData::CDSectorData(NN<IO::SectorData> data, UIntOS userOfst, UIntOS userDataSize) : IO::SectorData(data->GetSourceNameObj()), sectorBuff(data->GetBytesPerSector())
{
	this->data = data->GetPartialData(0, data->GetSectorCount());
	this->userOfst = userOfst;
	this->userDataSize = userDataSize;
}

IO::CDSectorData::~CDSectorData()
{
	this->data.Delete();
}

UInt64 IO::CDSectorData::GetSectorCount() const
{
	return this->data->GetSectorCount();
}

UIntOS IO::CDSectorData::GetBytesPerSector() const
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

NN<IO::SectorData> IO::CDSectorData::GetPartialData(UInt64 startSector, UInt64 sectorCount) const
{
	NN<IO::SectorData> data;
	NEW_CLASSNN(data, IO::CDSectorData(this->data, this->userOfst, this->userDataSize, startSector, sectorCount));
	return data;
}

NN<IO::StreamData> IO::CDSectorData::GetStreamData(UInt64 startSector, UInt64 dataSize) const
{
	NN<IO::StreamData> data;
	UInt64 sectorCnt = dataSize / this->userDataSize;
	if ((dataSize % this->userDataSize) != 0)
	{
		sectorCnt++;
	}
	NEW_CLASSNN(data, IO::CDSectorStreamData(this->GetPartialData(startSector, sectorCnt), 0, dataSize));
	return data;
}

UIntOS IO::CDSectorData::GetSeekCount() const
{
	return this->data->GetSeekCount();
}

IO::CDSectorStreamData::CDSectorStreamData(NN<IO::SectorData> data, UIntOS sectorOfst, UInt64 dataSize) : sectorBuff(data->GetBytesPerSector())
{
	this->data = data;
	this->sectorOfst = sectorOfst;
	this->dataSize = dataSize;
}

IO::CDSectorStreamData::~CDSectorStreamData()
{
	this->data.Delete();
}

UIntOS IO::CDSectorStreamData::GetRealData(UInt64 offset, UIntOS length, Data::ByteArray buffer)
{
	UIntOS retSize = 0;
	offset += this->sectorOfst;
	UIntOS sectorSize = this->data->GetBytesPerSector();
	UInt64 sectorStart = offset / sectorSize;
	UIntOS sectorOfst = (UIntOS)(offset % sectorSize);
	UIntOS thisSize;
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

NN<Text::String> IO::CDSectorStreamData::GetFullName() const
{
	return this->data->GetSourceNameObj();
}

Text::CString IO::CDSectorStreamData::GetShortName() const
{
	return this->data->GetSourceNameObj()->ToCString();
}

UInt64 IO::CDSectorStreamData::GetDataSize() const
{
	return this->dataSize;
}

UnsafeArrayOpt<const UInt8> IO::CDSectorStreamData::GetPointer() const
{
	return nullptr;
}

NN<IO::StreamData> IO::CDSectorStreamData::GetPartialData(UInt64 offset, UInt64 length)
{
	UInt64 endOfst = offset + length;
	if (endOfst > (this->dataSize + this->sectorOfst))
	{
		endOfst = (this->dataSize + this->sectorOfst);
	}
	UIntOS sectorSize = this->data->GetBytesPerSector();
	UInt64 rawOfst = offset + this->sectorOfst;
	UInt64 startSector = rawOfst / sectorSize;
	UInt64 endSector = (endOfst + sectorSize - 1) / sectorSize;

	NN<IO::StreamData> data;
	NEW_CLASSNN(data, CDSectorStreamData(this->data->GetPartialData(startSector, endSector - startSector), (UIntOS)(rawOfst % sectorSize), endOfst - rawOfst));
	return data;
}

Bool IO::CDSectorStreamData::IsFullFile() const
{
	return false;
}

Bool IO::CDSectorStreamData::IsLoading() const
{
	return false;
}

UIntOS IO::CDSectorStreamData::GetSeekCount() const
{
	return 0;
}
