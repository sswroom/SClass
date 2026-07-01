#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"

void IO::StmData::FileData::ReopenFile()
{
	NN<FILEDATAHANDLE> fdh;
	if (!this->fdh.SetTo(fdh))
		return;
	Sync::MutexUsage mutUsage(fdh->mut);
	NN<IO::FileStream> fs;
	NEW_CLASSNN(fs, IO::FileStream(fdh->filePath->ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		fs.Delete();
	}
	else
	{
		fdh->file.Delete();
		fdh->file = fs;
		fdh->currentOffset = fs->GetPosition();
	}
	mutUsage.EndUse();
}

IO::StmData::FileData::FileData(NN<const IO::StmData::FileData> fd, UInt64 offset, UInt64 length)
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
	NN<FILEDATAHANDLE> fdh;
	if (fd->fdh.SetTo(fdh))
	{
		this->fdh = fdh;
		fdh->objectCnt++;
	}
	this->fdn = fd->fdn;
	NN<FILEDATANAME> fdn;
	if (this->fdn.SetTo(fdn))
	{
		Sync::Interlocked::IncrementU32(fdn->objectCnt);
	}
}

IO::StmData::FileData::FileData(NN<Text::String> fname, Bool deleteOnClose)
{
	this->fdh = nullptr;
	NN<IO::FileStream> fs;
	this->fdn = nullptr;
	NEW_CLASSNN(fs, IO::FileStream(fname->ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		fs.Delete();
		this->dataLength = 0;
		this->dataOffset = 0;
	}
	else
	{
		NN<FILEDATAHANDLE> fdh;
		NEW_CLASSNN(fdh, IO::StmData::FileData::FILEDATAHANDLE());
		this->fdh = fdh;
		fdh->file = fs;
		fdh->filePath = fname->Clone();
		dataLength = fdh->fileLength = fs->GetLength();
		fdh->currentOffset = fs->GetPosition();
		fdh->objectCnt = 1;
		fdh->seekCnt = 0;
		dataOffset = 0;
		fdh->fullName = fdh->filePath->Clone();
		fdh->fileName.v = &fdh->fullName->v[fdh->fullName->LastIndexOf(IO::Path::PATH_SEPERATOR) + 1];
		fdh->fileName.leng = (UIntOS)(fdh->fullName->GetEndPtr() - fdh->fileName.v.Ptr());
		fdh->deleteOnClose = deleteOnClose;
	}
}

IO::StmData::FileData::FileData(Text::CStringNN fname, Bool deleteOnClose)
{
	this->fdh = nullptr;
	NN<IO::FileStream> fs;
	this->fdn = nullptr;
	NEW_CLASSNN(fs, IO::FileStream(fname, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		fs.Delete();
		this->dataLength = 0;
		this->dataOffset = 0;
	}
	else
	{
		NN<FILEDATAHANDLE> fdh;
		NEW_CLASSNN(fdh, IO::StmData::FileData::FILEDATAHANDLE());
		this->fdh = fdh;
		fdh->file = fs;
		fdh->filePath = Text::String::New(fname.v, fname.leng);
		dataLength = fdh->fileLength = fs->GetLength();
		fdh->currentOffset = fs->GetPosition();
		fdh->objectCnt = 1;
		fdh->seekCnt = 0;
		dataOffset = 0;
		fdh->fullName = fdh->filePath->Clone();
		fdh->fileName.v = &fdh->fullName->v[fdh->fullName->LastIndexOf(IO::Path::PATH_SEPERATOR) + 1];
		fdh->fileName.leng = (UIntOS)(fdh->fullName->GetEndPtr() - fdh->fileName.v.Ptr());
		fdh->deleteOnClose = deleteOnClose;
	}
}

IO::StmData::FileData::~FileData()
{
	this->Close();
	this->SetFullName(CSTR(""));
}

UIntOS IO::StmData::FileData::GetRealData(UInt64 offset, UIntOS length, Data::ByteArray buffer)
{
	NN<FILEDATAHANDLE> fdh;
	if (!this->fdh.SetTo(fdh))
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
	UIntOS totalRead = 0;
	UIntOS byteRead;
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

UInt64 IO::StmData::FileData::GetDataSize() const
{
	return dataLength;
}

Text::CString IO::StmData::FileData::GetShortName() const
{
	NN<FILEDATANAME> fdn;
	if (this->fdn.SetTo(fdn))
		return fdn->fileName;
	NN<FILEDATAHANDLE> fdh;
	if (this->fdh.SetTo(fdh))
		return fdh->fileName;
	return nullptr;
}

NN<Text::String> IO::StmData::FileData::GetFullName() const
{
	NN<FILEDATANAME> fdn;
	if (this->fdn.SetTo(fdn))
		return fdn->fullName;
	NN<FILEDATAHANDLE> fdh;
	if (this->fdh.SetTo(fdh))
		return fdh->fullName;
	return Text::String::NewEmpty();
}

void IO::StmData::FileData::SetFullName(Text::CStringNN fullName)
{
	NN<FILEDATANAME> fdn;
	if (this->fdn.SetTo(fdn))
	{
		if (Sync::Interlocked::DecrementU32(fdn->objectCnt) == 0)
		{
			fdn->fullName->Release();
			MemFreeNN(fdn);
		}
		this->fdn = nullptr;
	}
	if (fullName.leng > 0)
	{
		UIntOS i;
		this->fdn = fdn = MemAllocNN(FILEDATANAME);
		fdn->objectCnt = 1;
		fdn->fullName = Text::String::New(fullName);
		i = fdn->fullName->LastIndexOf(IO::Path::PATH_SEPERATOR);
		fdn->fileName.v = &fdn->fullName->v[i + 1];
		fdn->fileName.leng = (UIntOS)(fdn->fullName->GetEndPtr() - fdn->fileName.v.Ptr());
	}
}

UnsafeArrayOpt<const UInt8> IO::StmData::FileData::GetPointer() const
{
	return nullptr;
}

NN<IO::StreamData> IO::StmData::FileData::GetPartialData(UInt64 offset, UInt64 length)
{
	NN<IO::StmData::FileData> data;
	NEW_CLASSNN(data, IO::StmData::FileData(*this, offset, length));
	return data;
}

Bool IO::StmData::FileData::IsFullFile() const
{
	return this->dataOffset == 0;
}

NN<Text::String> IO::StmData::FileData::GetFullFileName() const
{
	NN<FILEDATAHANDLE> fdh;
	if (this->fdh.SetTo(fdh))
		return fdh->fullName;
	return Text::String::NewEmpty();
}

Bool IO::StmData::FileData::IsLoading() const
{
	return false;
}

UIntOS IO::StmData::FileData::GetSeekCount() const
{
	NN<FILEDATAHANDLE> fdh;
	if (this->fdh.SetTo(fdh))
		return fdh->seekCnt;
	return 0;
}

Optional<IO::FileStream> IO::StmData::FileData::GetFileStream()
{
	NN<FILEDATAHANDLE> fdh;
	if (this->fdh.SetTo(fdh))
	{
		return fdh->file;
	}
	return nullptr;
}

Bool IO::StmData::FileData::IsError()
{
	return this->fdh.IsNull();
}

void IO::StmData::FileData::Close()
{
	NN<FILEDATAHANDLE> fdh;
	if (this->fdh.SetTo(fdh))
	{
		if (--(fdh->objectCnt) == 0)
		{
			fdh->file.Delete();
			if (fdh->deleteOnClose)
			{
				IO::Path::DeleteFile(fdh->fullName->v);
			}
			fdh->fullName->Release();
			fdh->filePath->Release();
			fdh.Delete();
		}
	}
	this->fdh = nullptr;
}
