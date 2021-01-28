#include "Stdafx.h"
#include "Sync/Mutex.h"
#include "Sync/Event.h"
#include "IO/Stream.h"
#include "IO/FileStream.h"
#include "IO/IStreamData.h"
#include "IO/StmData/FileData.h"
#include "IO/DataSegment.h"

IO::DataSegment::DataSegment(IO::StmData::FileData *fd)
{
	this->fd = (IO::StmData::FileData*)fd->GetPartialData(0, fd->GetDataSize());
	datas = new DATASEG[capacity = 10];
	dataCount = 0;
}

IO::DataSegment::~DataSegment()
{
	delete[] datas;
	delete fd;
	datas = 0;
}

IO::StmData::FileData* IO::DataSegment::GetFd()
{
	return fd;
}

void IO::DataSegment::Add(Int64 offset, Int32 length)
{
	if (capacity <= dataCount)
	{
		DATASEG *tmp = new DATASEG[capacity = (capacity << 1)];
		long i = dataCount;
		tmp[dataCount].offset = offset;
		tmp[dataCount++].length = length;
		while (i--)
		{
			tmp[i].offset = datas[i].offset;
			tmp[i].length = datas[i].length;
		}
		delete[] datas;
		datas = tmp;
	}
	else
	{
		datas[dataCount].offset = offset;
		datas[dataCount++].length = length;
	}
}

Int32 IO::DataSegment::GetCount()
{
	return dataCount;
}

Int64 IO::DataSegment::GetOffset(Int32 i)
{
	return datas[i].offset;
}

Int32 IO::DataSegment::GetLength(Int32 i)
{
	return datas[i].length;
}
