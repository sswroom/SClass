#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "IO/StmData/FileViewData.h"

IO::StmData::FileViewData::FileViewData(const UTF8Char* fname)
{
	fdh = 0;
	IO::ViewFileBuffer *file;
	NEW_CLASS(file, IO::ViewFileBuffer(fname));
	if (file->GetPointer() == 0)
	{
		DEL_CLASS(file);
		this->dataLength = 0;
		this->dataOffset = 0;
	}
	else
	{
		fdh = MemAlloc(IO::StmData::FileViewData::FILEVIEWDATAHANDLE, 1);
		fdh->file = file;
		dataLength = fdh->fileLength = file->GetLength();
		fdh->currentOffset = 0;
		fdh->objectCnt = 1;
		fdh->fptr = (UInt8*)file->GetPointer();
		NEW_CLASS(fdh->mut, Sync::Mutex());
		dataOffset = 0;
		const UTF8Char* name2;
		const UTF8Char *name;
		UTF8Char *dname;
		name = name2 = fname;
		while (*name++)
			if (*name == IO::Path::PATH_SEPERATOR)
				fname = name;
		fdh->fileName = fdh->fullName = (dname = MemAlloc(UTF8Char, name - name2));
		while ((*dname++ = *name2++) != 0)
			if (dname[-1] == IO::Path::PATH_SEPERATOR)
				fdh->fileName = dname;
	}
}


IO::StmData::FileViewData::FileViewData(const IO::StmData::FileViewData *fd, Int64 offset, Int64 length)
{
	dataOffset = offset + fd->dataOffset;
	Int64 endOffset = fd->dataOffset + fd->dataLength;
	dataLength = length;
	if (dataOffset > endOffset)
	{
		dataOffset = endOffset;
		dataLength = 0;
	}
	else if (dataOffset + length > endOffset)
	{
		dataLength = endOffset - dataOffset;
	}
	fdh = fd->fdh;
	fdh->objectCnt++;
}

IO::StmData::FileViewData::~FileViewData()
{
	Close();
}

OSInt IO::StmData::FileViewData::GetRealData(Int64 offset, OSInt length, UInt8* buffer)
{
	if (fdh == 0)
		return 0;
	fdh->mut->Lock();
	Int64 startOfst = dataOffset + offset;
	Int64 endOfst = startOfst + length;
	if (startOfst < 0)
		return 0;
	if (startOfst >= dataOffset + dataLength)
	{
		return 0;
	}
	if (endOfst >= dataOffset + dataLength)
	{
		endOfst = dataOffset + dataLength;
	}
	MemCopyNO(buffer, &fdh->fptr[startOfst], (OSInt)(endOfst - startOfst));
	
	fdh->mut->Unlock();
	return (OSInt)(endOfst - startOfst);
}

Int64 IO::StmData::FileViewData::GetDataSize()
{
	return dataLength;
}

const UTF8Char *IO::StmData::FileViewData::GetShortName()
{
	if (fdh == 0)
		return 0;
	return fdh->fileName;
}

const UTF8Char *IO::StmData::FileViewData::GetFullName()
{
	if (fdh == 0)
		return 0;
	return fdh->fullName;
}

const UInt8 *IO::StmData::FileViewData::GetPointer()
{
	if (fdh == 0)
		return 0;
	return &fdh->fptr[this->dataOffset];
}

IO::IStreamData *IO::StmData::FileViewData::GetPartialData(Int64 offset, Int64 length)
{
	IO::StmData::FileViewData *data;
	NEW_CLASS(data, IO::StmData::FileViewData(this, offset, length));
	return data;
}

Bool IO::StmData::FileViewData::IsFullFile()
{
	return this->dataOffset == 0;
}

Bool IO::StmData::FileViewData::IsLoading()
{
	return false;
}

OSInt IO::StmData::FileViewData::GetSeekCount()
{
	return 0;
}

void IO::StmData::FileViewData::Close()
{
	if (fdh)
	{
		if (--(fdh->objectCnt) == 0)
		{
			DEL_CLASS(fdh->file);
			MemFree(fdh->fullName);
			DEL_CLASS(fdh->mut);
			MemFree(fdh);
		}
	}
	fdh = 0;
}
