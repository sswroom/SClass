#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"

void IO::StmData::FileData::ReopenFile()
{
	if (fdh == 0)
		return;
	fdh->mut->Lock();
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(fdh->filePath, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
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
	fdh->mut->Unlock();
}

IO::StmData::FileData::FileData(const UTF8Char* fname, Bool deleteOnClose)
{
	fdh = 0;
	IO::FileStream *fs;
	this->fdn = 0;
	NEW_CLASS(fs, IO::FileStream(fname, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		this->dataLength = 0;
		this->dataOffset = 0;
	}
	else
	{
		fdh = MemAlloc(IO::StmData::FileData::FILEDATAHANDLE, 1);
		fdh->file = fs;
		fdh->filePath = Text::StrCopyNew(fname);
		dataLength = fdh->fileLength = fs->GetLength();
		fdh->currentOffset = fs->GetPosition();
		fdh->objectCnt = 1;
		fdh->seekCnt = 0;
		NEW_CLASS(fdh->mut, Sync::Mutex());
		dataOffset = 0;
		const UTF8Char* name2;
		UTF8Char *name;
		name = (UTF8Char*)(name2 = fname);
		while (*name++)
			if (*name == IO::Path::PATH_SEPERATOR)
				fname = name;
		fdh->fileName = fdh->fullName = (name = MemAlloc(UTF8Char, name - name2));
		fdh->deleteOnClose = deleteOnClose;
		while ((*name++ = *name2++) != 0)
			if (name[-1] == IO::Path::PATH_SEPERATOR)
				fdh->fileName = name;
	}
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
		Sync::Interlocked::Increment((Int32*)&this->fdn->objectCnt);
	}
}

IO::StmData::FileData::~FileData()
{
	this->Close();
	this->SetFullName(0);
}

UOSInt IO::StmData::FileData::GetRealData(UInt64 offset, UOSInt length, UInt8* buffer)
{
	if (fdh == 0)
		return 0;
	fdh->mut->Lock();
	if (fdh->currentOffset != dataOffset + offset)
	{
		if ((fdh->currentOffset = fdh->file->Seek(IO::SeekableStream::ST_BEGIN, dataOffset + offset)) != dataOffset + offset)
		{
			if (dataOffset + offset < fdh->fileLength)
			{
				this->ReopenFile();
			}
			fdh->mut->Unlock();
			return 0;
		}
		fdh->seekCnt++;
	}
	OSInt byteRead;
	if (length < dataLength - offset)
		byteRead = fdh->file->Read(buffer, length);
	else
		byteRead = fdh->file->Read(buffer, (UOSInt) (dataLength - offset));
	if (byteRead == -1)
	{
		fdh->mut->Unlock();
		return 0;
	}
	fdh->currentOffset += byteRead;
	fdh->mut->Unlock();
	return byteRead;
}

UInt64 IO::StmData::FileData::GetDataSize()
{
	return dataLength;
}

const UTF8Char *IO::StmData::FileData::GetShortName()
{
	if (this->fdn)
		return this->fdn->fileName;
	if (this->fdh)
		return this->fdh->fileName;
	return 0;
}

const UTF8Char *IO::StmData::FileData::GetFullName()
{
	if (this->fdn)
		return this->fdn->fullName;
	if (this->fdh)
		return this->fdh->fullName;
	return 0;
}

void IO::StmData::FileData::SetFullName(const UTF8Char *fullName)
{
	if (this->fdn)
	{
		if (Sync::Interlocked::Decrement((Int32*)&this->fdn->objectCnt) == 0)
		{
			Text::StrDelNew(this->fdn->fullName);
			MemFree(this->fdn);
		}
		this->fdn = 0;
	}
	if (fullName)
	{
		OSInt i;
		this->fdn = MemAlloc(FILEDATANAME, 1);
		this->fdn->objectCnt = 1;
		this->fdn->fullName = Text::StrCopyNew(fullName);
		i = Text::StrLastIndexOf(this->fdn->fullName, IO::Path::PATH_SEPERATOR);
		this->fdn->fileName = &this->fdn->fullName[i + 1];
	}
}

const UInt8 *IO::StmData::FileData::GetPointer()
{
	return 0;
}

IO::IStreamData *IO::StmData::FileData::GetPartialData(UInt64 offset, UInt64 length)
{
	IO::StmData::FileData *data;
	NEW_CLASS(data, IO::StmData::FileData(this, offset, length));
	return data;
}

Bool IO::StmData::FileData::IsFullFile()
{
	return this->dataOffset == 0;
}

const UTF8Char *IO::StmData::FileData::GetFullFileName()
{
	if (this->fdh)
		return this->fdh->fullName;
	return 0;
}

Bool IO::StmData::FileData::IsLoading()
{
	return false;
}

OSInt IO::StmData::FileData::GetSeekCount()
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


void IO::StmData::FileData::Close()
{
	if (fdh)
	{
		if (--(fdh->objectCnt) == 0)
		{
			DEL_CLASS(fdh->file);
			if (fdh->deleteOnClose)
			{
				IO::Path::DeleteFile(fdh->fullName);
			}
			MemFree(fdh->fullName);
			Text::StrDelNew(fdh->filePath);
			DEL_CLASS(fdh->mut);
			MemFree(fdh);
		}
	}
	fdh = 0;
}
