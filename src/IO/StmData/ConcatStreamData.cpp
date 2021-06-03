#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/StmData/ConcatStreamData.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"

IO::StmData::ConcatStreamData::ConcatStreamData(IO::StmData::ConcatStreamData::CONCATDATABASE *cdb, UInt64 dataOffset, UInt64 dataLength)
{
	this->cdb = cdb;
	this->dataOffset = dataOffset;
	this->dataLength = dataLength;
	Sync::MutexUsage mutUsage(this->cdb->mut);
	this->cdb->objectCnt++;
	mutUsage.EndUse();
}

IO::StmData::ConcatStreamData::ConcatStreamData(const UTF8Char *fileName)
{
	this->cdb = MemAlloc(CONCATDATABASE, 1);
	NEW_CLASS(this->cdb->mut, Sync::Mutex());
	NEW_CLASS(this->cdb->dataList, Data::ArrayList<IO::IStreamData*>());
	NEW_CLASS(this->cdb->ofstList, Data::ArrayListUInt64());
	this->cdb->fileName = Text::StrCopyNew(fileName);
	this->cdb->objectCnt = 1;
	this->cdb->totalSize = 0;
	this->dataOffset = 0;
	this->dataLength = (UOSInt)-1;
}

IO::StmData::ConcatStreamData::~ConcatStreamData()
{
	UInt32 cnt;
	Sync::MutexUsage mutUsage(this->cdb->mut);
	cnt = this->cdb->objectCnt--;
	mutUsage.EndUse();
	if (cnt == 1)
	{
		IO::IStreamData *data;
		UOSInt i;
		DEL_CLASS(this->cdb->ofstList);
		DEL_CLASS(this->cdb->mut);
		i = this->cdb->dataList->GetCount();
		while (i-- > 0)
		{
			data = this->cdb->dataList->GetItem(i);
			DEL_CLASS(data);
		}
		DEL_CLASS(this->cdb->dataList);
		Text::StrDelNew(this->cdb->fileName);
		MemFree(this->cdb);
	}
}

UOSInt IO::StmData::ConcatStreamData::GetRealData(UInt64 offset, UOSInt length, UInt8 *buffer)
{
	OSInt si;
	UOSInt i;
	UOSInt j;
	UInt64 startOfst;
	UInt64 endOfst = length + offset;
	UInt64 thisSize = this->dataLength;
	UOSInt readTotal = 0;
	IO::IStreamData *data;
	if ((Int64)thisSize == -1)
	{
		thisSize = this->cdb->totalSize;
	}
	if (endOfst > (UInt64)thisSize)
	{
		endOfst = thisSize;
	}
	length = (UOSInt)(endOfst - offset);

	si = this->cdb->ofstList->SortedIndexOf(offset);
	if (si < 0)
	{
		i = (UOSInt)(~si - 1);
	}
	else
	{
		i = (UOSInt)si;
	}
	startOfst = this->cdb->ofstList->GetItem(i);
	offset -= startOfst;
	j = this->cdb->dataList->GetCount();
	while (i < j)
	{
		data = this->cdb->dataList->GetItem(i);
		thisSize = data->GetDataSize() - offset;
		if (thisSize > length)
			thisSize = length;
		thisSize = data->GetRealData(offset, (UOSInt)thisSize, buffer);
		length -= (UOSInt)thisSize;
		buffer += thisSize;
		readTotal += (UOSInt)thisSize;
		if (length <= 0)
			break;
		offset = 0;
		i++;
	}
	return readTotal;
}

const UTF8Char *IO::StmData::ConcatStreamData::GetFullName()
{
	return this->cdb->fileName;
}

const UTF8Char *IO::StmData::ConcatStreamData::GetShortName()
{
	return this->cdb->fileName;
}

UInt64 IO::StmData::ConcatStreamData::GetDataSize()
{
	if (this->dataLength == (UOSInt)-1)
	{
		return this->cdb->totalSize;
	}
	else
	{
		return this->dataLength;
	}
}

const UInt8 *IO::StmData::ConcatStreamData::GetPointer()
{
	return 0;
}

IO::IStreamData *IO::StmData::ConcatStreamData::GetPartialData(UInt64 offset, UInt64 length)
{
	IO::IStreamData *data;

	UInt64 endOfst = length + offset;
	UInt64 thisSize = this->dataLength;
	if (thisSize == (UOSInt)-1)
	{
		thisSize = this->cdb->totalSize;
	}
	if (endOfst > thisSize)
	{
		endOfst = thisSize;
	}
	if (offset >= endOfst)
	{
		NEW_CLASS(data, IO::StmData::ConcatStreamData(this->cdb, 0, 0));
		return data;
	}
	else
	{
		NEW_CLASS(data, IO::StmData::ConcatStreamData(this->cdb, offset, endOfst - offset));
		return data;
	}
}

Bool IO::StmData::ConcatStreamData::IsFullFile()
{
	return false;
}

Bool IO::StmData::ConcatStreamData::IsLoading()
{
	return false;
}

UOSInt IO::StmData::ConcatStreamData::GetSeekCount()
{
	UOSInt ret = 0;
	IO::IStreamData *data;
	UOSInt i;
	i = this->cdb->dataList->GetCount();
	while (i-- > 0)
	{
		data = this->cdb->dataList->GetItem(i);
		ret += data->GetSeekCount();
	}
	return ret;
}

void IO::StmData::ConcatStreamData::AddData(IO::IStreamData *data)
{
	Sync::MutexUsage mutUsage(this->cdb->mut);
	this->cdb->dataList->Add(data);
	this->cdb->ofstList->Add(this->cdb->totalSize);
	this->cdb->totalSize += data->GetDataSize();
	mutUsage.EndUse();
}
