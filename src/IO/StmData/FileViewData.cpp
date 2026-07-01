#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "IO/StmData/FileViewData.h"
#include "Sync/MutexUsage.h"

IO::StmData::FileViewData::FileViewData(UnsafeArray<const UTF8Char> fname)
{
	fdh = 0;
	NN<IO::ViewFileBuffer> file;
	NEW_CLASSNN(file, IO::ViewFileBuffer(fname));
	if (file->GetPointer().IsNull())
	{
		file.Delete();
		this->dataLength = 0;
		this->dataOffset = 0;
	}
	else
	{
		NN<FILEVIEWDATAHANDLE> fdh;
		NEW_CLASSNN(fdh, IO::StmData::FileViewData::FILEVIEWDATAHANDLE());
		this->fdh = fdh;
		fdh->file = file;
		dataLength = fdh->fileLength = file->GetLength();
		fdh->currentOffset = 0;
		fdh->objectCnt = 1;
		fdh->fptr = (UInt8*)file->GetPointer().Ptr();
		dataOffset = 0;
		UnsafeArray<const UTF8Char> name2;
		UnsafeArray<const UTF8Char> name;
		UnsafeArray<UTF8Char> dname;
		name = name2 = fname;
		while (*name++)
			if (*name == IO::Path::PATH_SEPERATOR)
				fname = name;
		fdh->fullName = Text::String::New((UIntOS)(name - name2 - 1));
		fdh->fileName.v = dname = fdh->fullName->v;
		while ((*dname++ = *name2++) != 0)
			if (dname[-1] == IO::Path::PATH_SEPERATOR)
				fdh->fileName.v = dname;
		fdh->fileName.leng = (UIntOS)(fdh->fullName->GetEndPtr() - fdh->fileName.v.Ptr());
	}
}


IO::StmData::FileViewData::FileViewData(NN<const IO::StmData::FileViewData> fd, UInt64 offset, UInt64 length)
{
	this->fdh = nullptr;
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
	NN<FILEVIEWDATAHANDLE> fdh;
	if (fd->fdh.SetTo(fdh))
	{
		this->fdh = fdh;
		fdh->objectCnt++;
	}
}

IO::StmData::FileViewData::~FileViewData()
{
	Close();
}

UIntOS IO::StmData::FileViewData::GetRealData(UInt64 offset, UIntOS length, Data::ByteArray buffer)
{
	NN<FILEVIEWDATAHANDLE> fdh;
	if (!this->fdh.SetTo(fdh))
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
	MemCopyNO(buffer.Arr().Ptr(), &fdh->fptr[startOfst], (UIntOS)(endOfst - startOfst));
	
	return (UIntOS)(endOfst - startOfst);
}

UInt64 IO::StmData::FileViewData::GetDataSize() const
{
	return dataLength;
}

Text::CString IO::StmData::FileViewData::GetShortName() const
{
	NN<FILEVIEWDATAHANDLE> fdh;
	if (!this->fdh.SetTo(fdh))
		return nullptr;
	return fdh->fileName;
}

NN<Text::String> IO::StmData::FileViewData::GetFullName() const
{
	NN<FILEVIEWDATAHANDLE> fdh;
	if (!this->fdh.SetTo(fdh))
		return Text::String::NewEmpty();
	return fdh->fullName;
}

UnsafeArrayOpt<const UInt8> IO::StmData::FileViewData::GetPointer() const
{
	NN<FILEVIEWDATAHANDLE> fdh;
	if (!this->fdh.SetTo(fdh))
		return nullptr;
	return &fdh->fptr[this->dataOffset];
}

NN<IO::StreamData> IO::StmData::FileViewData::GetPartialData(UInt64 offset, UInt64 length)
{
	NN<IO::StmData::FileViewData> data;
	NEW_CLASSNN(data, IO::StmData::FileViewData(*this, offset, length));
	return data;
}

Bool IO::StmData::FileViewData::IsFullFile() const
{
	return this->dataOffset == 0;
}

Bool IO::StmData::FileViewData::IsLoading() const
{
	return false;
}

UIntOS IO::StmData::FileViewData::GetSeekCount() const
{
	return 0;
}

void IO::StmData::FileViewData::Close()
{
	NN<FILEVIEWDATAHANDLE> fdh;
	if (this->fdh.SetTo(fdh))
	{
		if (--(fdh->objectCnt) == 0)
		{
			fdh->file.Delete();
			fdh->fullName->Release();
			fdh.Delete();
		}
	}
	this->fdh = nullptr;
}
