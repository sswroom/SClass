#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "IO/StmData/FileViewData.h"
#include "Sync/MutexUsage.h"

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
		NEW_CLASS(fdh, IO::StmData::FileViewData::FILEVIEWDATAHANDLE());
		fdh->file = file;
		dataLength = fdh->fileLength = file->GetLength();
		fdh->currentOffset = 0;
		fdh->objectCnt = 1;
		fdh->fptr = (UInt8*)file->GetPointer();
		dataOffset = 0;
		const UTF8Char* name2;
		const UTF8Char *name;
		UTF8Char *dname;
		name = name2 = fname;
		while (*name++)
			if (*name == IO::Path::PATH_SEPERATOR)
				fname = name;
		fdh->fullName = Text::String::New((UOSInt)(name - name2 - 1));
		fdh->fileName.v = dname = fdh->fullName->v;
		while ((*dname++ = *name2++) != 0)
			if (dname[-1] == IO::Path::PATH_SEPERATOR)
				fdh->fileName.v = dname;
		fdh->fileName.leng = (UOSInt)(fdh->fullName->GetEndPtr() - fdh->fileName.v);
	}
}


IO::StmData::FileViewData::FileViewData(const IO::StmData::FileViewData *fd, UInt64 offset, UInt64 length)
{
	dataOffset = offset + fd->dataOffset;
	UInt64 endOffset = fd->dataOffset + fd->dataLength;
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

UOSInt IO::StmData::FileViewData::GetRealData(UInt64 offset, UOSInt length, Data::ByteArray buffer)
{
	if (fdh == 0)
		return 0;
	Sync::MutexUsage mutUsage(fdh->mut);
	UInt64 startOfst = dataOffset + offset;
	UInt64 endOfst = startOfst + length;
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
	MemCopyNO(buffer.Ptr(), &fdh->fptr[startOfst], (UOSInt)(endOfst - startOfst));
	
	return (UOSInt)(endOfst - startOfst);
}

UInt64 IO::StmData::FileViewData::GetDataSize()
{
	return dataLength;
}

Text::CString IO::StmData::FileViewData::GetShortName()
{
	if (fdh == 0)
		return CSTR_NULL;
	return fdh->fileName;
}

NotNullPtr<Text::String> IO::StmData::FileViewData::GetFullName()
{
	if (fdh == 0)
		return Text::String::NewEmpty();
	return fdh->fullName;
}

const UInt8 *IO::StmData::FileViewData::GetPointer()
{
	if (fdh == 0)
		return 0;
	return &fdh->fptr[this->dataOffset];
}

NotNullPtr<IO::StreamData> IO::StmData::FileViewData::GetPartialData(UInt64 offset, UInt64 length)
{
	NotNullPtr<IO::StmData::FileViewData> data;
	NEW_CLASSNN(data, IO::StmData::FileViewData(this, offset, length));
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

UOSInt IO::StmData::FileViewData::GetSeekCount()
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
			fdh->fullName->Release();
			DEL_CLASS(fdh);
		}
	}
	fdh = 0;
}
