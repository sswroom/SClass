#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"

void IO::StmData::FileData::ReopenFile()
{
	if (fdh == 0)
		return;
	Sync::MutexUsage mutUsage(fdh->mut);
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(fdh->filePath->ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
	}
	else
	{
		DEL_CLASS(fdh->file);
		fdh->file = fs;
		fdh->currentOffset = fs->GetPosition();
	}
	mutUsage.EndUse();
}

IO::StmData::FileData::FileData(const IO::StmData::FileData *fd, UInt64 offset, UInt64 length)
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
	this->fdn = fd->fdn;
	if (this->fdn)
	{
		Sync::Interlocked::IncrementU32(this->fdn->objectCnt);
	}
}

IO::StmData::FileData::FileData(NN<Text::String> fname, Bool deleteOnClose)
{
	fdh = 0;
	IO::FileStream *fs;
	this->fdn = 0;
	NEW_CLASS(fs, IO::FileStream(fname->ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		this->dataLength = 0;
		this->dataOffset = 0;
	}
	else
	{
		NEW_CLASS(fdh, IO::StmData::FileData::FILEDATAHANDLE());
		fdh->file = fs;
		fdh->filePath = fname->Clone();
		dataLength = fdh->fileLength = fs->GetLength();
		fdh->currentOffset = fs->GetPosition();
		fdh->objectCnt = 1;
		fdh->seekCnt = 0;
		dataOffset = 0;
		fdh->fullName = fdh->filePath->Clone();
		fdh->fileName.v = &fdh->fullName->v[fdh->fullName->LastIndexOf(IO::Path::PATH_SEPERATOR) + 1];
		fdh->fileName.leng = (UOSInt)(fdh->fullName->GetEndPtr() - fdh->fileName.v.Ptr());
		fdh->deleteOnClose = deleteOnClose;
	}
}

IO::StmData::FileData::FileData(Text::CStringNN fname, Bool deleteOnClose)
{
	fdh = 0;
	IO::FileStream *fs;
	this->fdn = 0;
	NEW_CLASS(fs, IO::FileStream(fname, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		this->dataLength = 0;
		this->dataOffset = 0;
	}
	else
	{
		NEW_CLASS(fdh, IO::StmData::FileData::FILEDATAHANDLE());
		fdh->file = fs;
		fdh->filePath = Text::String::New(fname.v, fname.leng);
		dataLength = fdh->fileLength = fs->GetLength();
		fdh->currentOffset = fs->GetPosition();
		fdh->objectCnt = 1;
		fdh->seekCnt = 0;
		dataOffset = 0;
		fdh->fullName = fdh->filePath->Clone();
		fdh->fileName.v = &fdh->fullName->v[fdh->fullName->LastIndexOf(IO::Path::PATH_SEPERATOR) + 1];
		fdh->fileName.leng = (UOSInt)(fdh->fullName->GetEndPtr() - fdh->fileName.v.Ptr());
		fdh->deleteOnClose = deleteOnClose;
	}
}

IO::StmData::FileData::~FileData()
{
	this->Close();
	this->SetFullName(CSTR_NULL);
}

UOSInt IO::StmData::FileData::GetRealData(UInt64 offset, UOSInt length, Data::ByteArray buffer)
{
	if (fdh == 0)
		return 0;
	Sync::MutexUsage mutUsage(fdh->mut);
	if (fdh->currentOffset != dataOffset + offset)
	{
		if ((fdh->currentOffset = fdh->file->SeekFromBeginning(dataOffset + offset)) != dataOffset + offset)
		{
			if (dataOffset + offset < fdh->fileLength)
			{
				this->ReopenFile();
			}
			mutUsage.EndUse();
			return 0;
		}
		fdh->seekCnt++;
	}
	UOSInt totalRead = 0;
	UOSInt byteRead;
	if (length > dataLength - offset)
		length = dataLength - offset;
	while (totalRead < length)
	{
		byteRead = fdh->file->Read(buffer.SubArray(totalRead, length - totalRead));
		if (byteRead == 0)
		{
			return totalRead;
		}
		totalRead += byteRead;
		fdh->currentOffset += byteRead;
	}
	return totalRead;
}

UInt64 IO::StmData::FileData::GetDataSize()
{
	return dataLength;
}

Text::CString IO::StmData::FileData::GetShortName()
{
	if (this->fdn)
		return this->fdn->fileName;
	if (this->fdh)
		return this->fdh->fileName;
	return CSTR_NULL;
}

NN<Text::String> IO::StmData::FileData::GetFullName()
{
	if (this->fdn)
		return this->fdn->fullName;
	if (this->fdh)
		return this->fdh->fullName;
	return Text::String::NewEmpty();
}

void IO::StmData::FileData::SetFullName(Text::CString fullName)
{
	if (this->fdn)
	{
		if (Sync::Interlocked::DecrementU32(this->fdn->objectCnt) == 0)
		{
			this->fdn->fullName->Release();
			MemFree(this->fdn);
		}
		this->fdn = 0;
	}
	if (fullName.leng > 0)
	{
		UOSInt i;
		this->fdn = MemAlloc(FILEDATANAME, 1);
		this->fdn->objectCnt = 1;
		this->fdn->fullName = Text::String::New(fullName);
		i = this->fdn->fullName->LastIndexOf(IO::Path::PATH_SEPERATOR);
		this->fdn->fileName.v = &this->fdn->fullName->v[i + 1];
		this->fdn->fileName.leng = (UOSInt)(this->fdn->fullName->GetEndPtr() - this->fdn->fileName.v.Ptr());
	}
}

const UInt8 *IO::StmData::FileData::GetPointer()
{
	return 0;
}

NN<IO::StreamData> IO::StmData::FileData::GetPartialData(UInt64 offset, UInt64 length)
{
	NN<IO::StmData::FileData> data;
	NEW_CLASSNN(data, IO::StmData::FileData(this, offset, length));
	return data;
}

Bool IO::StmData::FileData::IsFullFile()
{
	return this->dataOffset == 0;
}

NN<Text::String> IO::StmData::FileData::GetFullFileName()
{
	if (this->fdh)
		return this->fdh->fullName;
	return Text::String::NewEmpty();
}

Bool IO::StmData::FileData::IsLoading()
{
	return false;
}

UOSInt IO::StmData::FileData::GetSeekCount()
{
	if (this->fdh)
		return this->fdh->seekCnt;
	return 0;
}

IO::FileStream *IO::StmData::FileData::GetFileStream()
{
	if (fdh)
	{
		return fdh->file;
	}
	return 0;
}

Bool IO::StmData::FileData::IsError()
{
	return fdh == 0;
}

void IO::StmData::FileData::Close()
{
	if (fdh)
	{
		if (--(fdh->objectCnt) == 0)
		{
			DEL_CLASS(fdh->file);
			if (fdh->deleteOnClose)
			{
				IO::Path::DeleteFile(fdh->fullName->v);
			}
			fdh->fullName->Release();
			fdh->filePath->Release();
			DEL_CLASS(fdh);
		}
	}
	fdh = 0;
}
