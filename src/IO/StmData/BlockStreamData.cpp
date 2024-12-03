#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/StmData/BlockStreamData.h"

IO::StmData::BlockStreamData::BlockStreamData(BlockStreamData &sd)
{
	this->sd = sd.GetPartialData(0, sd.GetDataSize());
	this->totalSize = 0;
}

IO::StmData::BlockStreamData::BlockStreamData(NN<IO::StreamData> sd)
{
	this->sd = sd->GetPartialData(0, sd->GetDataSize());
	this->totalSize = 0;
}

IO::StmData::BlockStreamData::~BlockStreamData()
{
	this->sd.Delete();
}

UOSInt IO::StmData::BlockStreamData::GetRealData(UInt64 offset, UOSInt length, Data::ByteArray buffer)
{
	OSInt i = this->dataOfstList.SortedIndexOf(offset);
	UOSInt totalLength = 0;
	UOSInt thisLength;
	UOSInt thisOfst;
	OSInt j = (OSInt)this->stmOfstList.GetCount();
	if (offset >= totalSize)
		return 0;
	if (i < 0)
	{
		i = ~i - 1;
		thisOfst = (UOSInt)(offset - this->dataOfstList.GetItem((UOSInt)i));
		thisLength = this->lengthList.GetItem((UOSInt)i) - thisOfst;
		if (thisLength > length)
		{
			thisLength = length;
		}
		totalLength += this->sd->GetRealData(this->stmOfstList.GetItem((UOSInt)i) + thisOfst, thisLength, buffer);
		buffer += thisLength;
		length -= thisLength;
		i++;
	}
	while (i < j)
	{
		if (length <= 0)
			break;

		thisLength = this->lengthList.GetItem((UOSInt)i);
		if (thisLength > length)
		{
			thisLength = length;
		}
		totalLength += this->sd->GetRealData(this->stmOfstList.GetItem((UOSInt)i), thisLength, buffer);
		buffer += thisLength;
		length -= thisLength;
		i++;
	}
	return totalLength;
}

NN<Text::String> IO::StmData::BlockStreamData::GetFullName() const
{
	return this->sd->GetFullName();
}

Text::CString IO::StmData::BlockStreamData::GetShortName() const
{
	return this->sd->GetShortName();
}

UInt64 IO::StmData::BlockStreamData::GetDataSize() const
{
	return totalSize;
}

UnsafeArrayOpt<const UInt8> IO::StmData::BlockStreamData::GetPointer() const
{
	return 0;
}

NN<IO::StreamData> IO::StmData::BlockStreamData::GetPartialData(UInt64 offset, UInt64 length)
{
	NN<IO::StmData::BlockStreamData> data;
	NEW_CLASSNN(data, IO::StmData::BlockStreamData(this->sd));
	OSInt i = this->dataOfstList.SortedIndexOf(offset);
	UInt64 totalLength = 0;
	UOSInt thisLength;
	UOSInt thisOfst;
	OSInt j = (OSInt)this->stmOfstList.GetCount();
	if (offset >= totalSize)
		return data;
	if (i < 0)
	{
		i = ~i - 1;
		thisOfst = (UOSInt)(offset - this->stmOfstList.GetItem((UOSInt)i));
		thisLength = this->lengthList.GetItem((UOSInt)i) - thisOfst;
		if (thisLength > length)
		{
			thisLength = (UOSInt)length;
		}
		data->dataOfstList.Add(totalLength);
		data->stmOfstList.Add(this->stmOfstList.GetItem((UOSInt)i) + thisOfst);
		data->lengthList.Add((UInt32)thisLength);
		length -= thisLength;
		totalLength += thisLength;
		i++;
	}
	while (i < j)
	{
		if (length <= 0)
			break;

		thisLength = this->lengthList.GetItem((UOSInt)i);
		if (thisLength > length)
		{
			thisLength = (UOSInt)length;
		}
		data->dataOfstList.Add(totalLength);
		data->stmOfstList.Add(this->stmOfstList.GetItem((UOSInt)i));
		data->lengthList.Add((UInt32)thisLength);
		length -= thisLength;
		totalLength += thisLength;
		i++;
	}
	data->totalSize = totalLength;
	return data;
}

Bool IO::StmData::BlockStreamData::IsFullFile() const
{
	return false;
}

Bool IO::StmData::BlockStreamData::IsLoading() const
{
	return this->sd->IsLoading();
}

UOSInt IO::StmData::BlockStreamData::GetSeekCount() const
{
	return this->sd->GetSeekCount();
}

void IO::StmData::BlockStreamData::Append(UInt64 ofst, UInt32 length)
{
	UInt64 startOfst = ofst;
	UInt64 endOfst = ofst + length;
	if (endOfst > sd->GetDataSize())
	{
		endOfst = sd->GetDataSize();
	}
	if (startOfst >= endOfst)
	{
		return;
	}
	this->dataOfstList.Add(totalSize);
	this->stmOfstList.Add(startOfst);
	this->lengthList.Add((UInt32)(endOfst - startOfst));
	totalSize += endOfst - startOfst;
}
