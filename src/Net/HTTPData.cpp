#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Net/HTTPData.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

UInt32 __stdcall Net::HTTPData::LoadThread(void *userObj)
{
	UInt8 buff[2048];
	UTF8Char u8buff[64];
	UOSInt readSize;

	HTTPDATAHANDLE *fdh = (HTTPDATAHANDLE*)userObj;
	if (fdh->queue)
	{
		Sync::MutexUsage mutUsage(fdh->mut);
		fdh->cli = fdh->queue->MakeRequest(fdh->url, "GET", true);
		mutUsage.EndUse();
	}
	else
	{
		fdh->cli = Net::HTTPClient::CreateConnect(fdh->sockf, fdh->url, "GET", true);
	}
	fdh->evtTmp->Set();
	if (IO::Path::GetPathType(fdh->localFile) == IO::Path::PT_FILE)
	{
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream(fdh->localFile, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
		if (!fs->IsError())
		{
			Data::DateTime dt;
			fs->GetFileTimes(0, 0, &dt);
			Net::HTTPClient::Date2Str(u8buff, &dt);
			fdh->cli->AddHeader((const UTF8Char*)"If-Modified-Since", u8buff);
		}
		DEL_CLASS(fs);
	}
	while (fdh->cli->GetRespStatus() == 301)
	{
		Text::StringBuilderUTF8 sb;
		fdh->cli->GetRespHeader((const UTF8Char*)"Location", &sb);
		if (sb.GetLength() > 0 && (sb.StartsWith((const UTF8Char*)"http://") || sb.StartsWith((const UTF8Char*)"https://")))
		{
			if (fdh->queue)
			{
				Sync::MutexUsage mutUsage(fdh->mut);
				fdh->queue->EndRequest(fdh->cli);
				fdh->cli = fdh->queue->MakeRequest(sb.ToString(), "GET", true);
				mutUsage.EndUse();
			}
			else
			{
				DEL_CLASS(fdh->cli);
				fdh->cli = Net::HTTPClient::CreateConnect(fdh->sockf, sb.ToString(), "GET", true);
			}
		}
		else
		{
			break;
		}
	}

	if (fdh->cli->GetRespStatus() == 304)
	{
		NEW_CLASS(fdh->file, IO::FileStream(fdh->localFile, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
		fdh->fileLength = fdh->file->GetLength();
	}
	else if (fdh->cli->GetRespStatus() == 200)
	{
		fdh->fileLength = fdh->cli->GetContentLength();
		if (fdh->fileLength > 0)
		{
			NEW_CLASS(fdh->file, IO::FileStream(fdh->localFile, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_WRITE, IO::FileStream::BT_NORMAL));
			while (fdh->loadSize < fdh->fileLength)
			{
				readSize = fdh->cli->Read(buff, 2048);
				if (readSize == 0)
				{
					Sync::MutexUsage mutUsage(fdh->mut);
					DEL_CLASS(fdh->file);
					fdh->file = 0;
					fdh->fileLength = 0;
					mutUsage.EndUse();
					IO::Path::DeleteFile(fdh->localFile);
					break;
				}
				Sync::MutexUsage mutUsage(fdh->mut);
				if (fdh->currentOffset != fdh->loadSize)
				{
					fdh->file->Seek(IO::SeekableStream::ST_BEGIN, (Int64)fdh->loadSize);
					fdh->seekCnt++;
					fdh->currentOffset = fdh->loadSize;
				}
				fdh->file->Write(buff, readSize);
				fdh->loadSize += readSize;
				fdh->currentOffset = fdh->loadSize;
				mutUsage.EndUse();
			}
			Data::DateTime dt;
			if (fdh->file && fdh->cli->GetLastModified(&dt))
			{
				fdh->file->SetFileTimes(0, 0, &dt);
			}
		}
		else
		{
			Text::StringBuilderUTF8 sb;
			if (!fdh->cli->GetRespHeader((const UTF8Char*)"Content-Length", &sb))
			{
				Sync::Event *readEvt;
				void *sess;

				NEW_CLASS(readEvt, Sync::Event(false, (const UTF8Char*)"Net.HTTPData.LoadThread.readEvt"));
				NEW_CLASS(fdh->file, IO::FileStream(fdh->localFile, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_WRITE, IO::FileStream::BT_NORMAL));
				while (true)
				{
					readEvt->Clear();
					sess = fdh->cli->BeginRead(buff, 2048, readEvt);
					if (sess)
					{
						Bool incomplete;
						readEvt->Wait(2000);
						readSize = fdh->cli->EndRead(sess, false, &incomplete);
						if (readSize <= 0)
						{
							fdh->cli->Close();
							break;
						}
					}
					else
					{
						break;
					}
					
					Sync::MutexUsage mutUsage(fdh->mut);
					if (fdh->currentOffset != fdh->loadSize)
					{
						fdh->file->Seek(IO::SeekableStream::ST_BEGIN, (Int64)fdh->loadSize);
						fdh->seekCnt++;
					}
					fdh->file->Write(buff, readSize);
					fdh->loadSize += readSize;
					fdh->fileLength = fdh->loadSize;
					fdh->currentOffset = fdh->fileLength;
					mutUsage.EndUse();
				}
				DEL_CLASS(readEvt);
				Data::DateTime dt;
				if (fdh->file && fdh->cli->GetLastModified(&dt))
				{
					fdh->file->SetFileTimes(0, 0, &dt);
				}
			}
		}
	}
	Sync::MutexUsage mutUsage(fdh->mut);
	if (fdh->queue)
	{
		fdh->queue->EndRequest(fdh->cli);
	}
	else
	{
		DEL_CLASS(fdh->cli);
	}
	fdh->cli = 0;
	fdh->isLoading = false;
	mutUsage.EndUse();
	return 0;
}

Net::HTTPData::HTTPData(const Net::HTTPData *fd, UInt64 offset, UInt64 length)
{
	dataOffset = offset + fd->dataOffset;
	UInt64 endOffset = fd->dataOffset + ((Net::HTTPData*)fd)->GetDataSize();
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

Net::HTTPData::HTTPData(Net::SocketFactory *sockf, Net::HTTPQueue *queue, const UTF8Char *url, const UTF8Char *localFile, Bool forceReload)
{
	UOSInt i;
	Bool needReload = forceReload;
	IO::Path::PathType pt = IO::Path::GetPathType(localFile);
	fdh = 0;
	if (pt == IO::Path::PT_DIRECTORY)
	{
		this->dataLength = 0;
		this->dataOffset = 0;
		return;
	}
	else if (pt == IO::Path::PT_UNKNOWN)
	{
		needReload = true;
	}
	if (!needReload)
	{
		OSInt i;
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream(localFile, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_WRITE, IO::FileStream::BT_NORMAL));
		if (fs->IsError())
		{
			DEL_CLASS(fs);
			this->dataLength = 0;
			this->dataOffset = 0;
		}
		else
		{
			fdh = MemAlloc(Net::HTTPData::HTTPDATAHANDLE, 1);
			fdh->file = fs;
			dataOffset = 0;
			dataLength = fdh->fileLength = fs->GetLength();
			fdh->currentOffset = fs->GetPosition();
			fdh->objectCnt = 1;
			fdh->seekCnt = 0;
			NEW_CLASS(fdh->mut, Sync::Mutex());
			fdh->url = Text::StrCopyNew(url);
			fdh->localFile = Text::StrCopyNew(localFile);
			fdh->isLoading = false;
			fdh->loadSize = 0;
			fdh->cli = 0;
			i = Text::StrLastIndexOf(fdh->url, '/');
			if (i >= 0)
			{
				fdh->fileName = &fdh->url[i + 1];
			}
			else
			{
				fdh->fileName = fdh->url;
			}
		}
	}
	else
	{
		dataOffset = 0;
		dataLength = (UOSInt)-1;
		fdh = MemAlloc(Net::HTTPData::HTTPDATAHANDLE, 1);
		fdh->file = 0;
		fdh->fileLength = 0;
		fdh->currentOffset = 0;
		fdh->objectCnt = 1;
		fdh->seekCnt = 0;
		NEW_CLASS(fdh->mut, Sync::Mutex());
		fdh->url = Text::StrCopyNew(url);
		fdh->localFile = Text::StrCopyNew(localFile);
		fdh->isLoading = true;
		fdh->loadSize = 0;
		fdh->sockf = sockf;
		fdh->queue = queue;
		i = Text::StrLastIndexOf(fdh->url, '/');
		if (i >= 0)
		{
			fdh->fileName = &fdh->url[i + 1];
		}
		else
		{
			fdh->fileName = fdh->url;
		}
		fdh->cli = 0;
		NEW_CLASS(fdh->evtTmp, Sync::Event(false, (const UTF8Char*)"Net.HTTPData.fdh.evtTmp"));
		Sync::Thread::Create(LoadThread, fdh);
		while (fdh->cli == 0 && fdh->isLoading)
		{
			fdh->evtTmp->Wait(10);
		}
		DEL_CLASS(fdh->evtTmp);
	}
}

Net::HTTPData::~HTTPData()
{
	Close();
}

UOSInt Net::HTTPData::GetRealData(UInt64 offset, UOSInt length, UInt8* buffer)
{
	if (fdh == 0)
		return 0;
	Sync::MutexUsage mutUsage(fdh->mut);
	while (fdh->isLoading && (dataOffset + offset + length > fdh->loadSize))
	{
		mutUsage.EndUse();
		Sync::Thread::Sleep(10);
		mutUsage.BeginUse();
	}
	if (fdh->currentOffset != dataOffset + offset)
	{
		if ((fdh->currentOffset = fdh->file->Seek(IO::SeekableStream::ST_BEGIN, dataOffset + offset)) != dataOffset + offset)
		{
			mutUsage.EndUse();
			return 0;
		}
		fdh->seekCnt++;
	}
	UOSInt byteRead;
	if (length < this->GetDataSize() - offset)
		byteRead = fdh->file->Read(buffer, length);
	else
		byteRead = fdh->file->Read(buffer, (UOSInt) (dataLength - offset));
	if (byteRead == 0)
	{
		mutUsage.EndUse();
		return 0;
	}
	fdh->currentOffset += byteRead;
	mutUsage.EndUse();
	return byteRead;
}

UInt64 Net::HTTPData::GetDataSize()
{
	if (dataLength == (UOSInt)-1)
	{
		while (true)
		{
			if (fdh->fileLength != 0 || !fdh->isLoading)
			{
				dataLength = fdh->fileLength;
				break;
			}
			Sync::Thread::Sleep(10);
		}
	}
	return dataLength;
}

const UTF8Char *Net::HTTPData::GetFullName()
{
	if (fdh == 0)
		return 0;
	return fdh->url;
}

const UTF8Char *Net::HTTPData::GetShortName()
{
	if (fdh == 0)
		return 0;
	return fdh->fileName;
}

void Net::HTTPData::SetFullName(const UTF8Char *fullName)
{
	if (fdh == 0 || fullName == 0)
		return;
	OSInt i;
	Sync::MutexUsage mutUsage(fdh->mut);
	SDEL_TEXT(fdh->url);
	fdh->url = Text::StrCopyNew(fullName);
	i = Text::StrLastIndexOf(fdh->url, '/');
	if (i >= 0)
	{
		fdh->fileName = &fdh->url[i + 1];
	}
	else
	{
		fdh->fileName = fdh->url;
	}
	mutUsage.EndUse();
}

const UInt8 *Net::HTTPData::GetPointer()
{
	return 0;
}

IO::IStreamData *Net::HTTPData::GetPartialData(UInt64 offset, UInt64 length)
{
	Net::HTTPData *data;
	NEW_CLASS(data, Net::HTTPData(this, offset, length));
	return data;
}

Bool Net::HTTPData::IsFullFile()
{
	return this->dataOffset == 0;
}

Bool Net::HTTPData::IsLoading()
{
	if (fdh == 0)
		return false;
	return fdh->isLoading;
}

UOSInt Net::HTTPData::GetSeekCount()
{
	if (fdh == 0)
		return 0;
	return fdh->seekCnt;
}

void Net::HTTPData::Close()
{
	if (fdh)
	{
		if (--(fdh->objectCnt) == 0)
		{
			Sync::MutexUsage mutUsage(fdh->mut);
			if (fdh->isLoading)
				fdh->cli->Close();
			mutUsage.EndUse();
			while (fdh->isLoading)
			{
				Sync::Thread::Sleep(10);
			}
			DEL_CLASS(fdh->file);
			Text::StrDelNew(fdh->url);
			Text::StrDelNew(fdh->localFile);
			DEL_CLASS(fdh->mut);
			MemFree(fdh);
		}
	}
	fdh = 0;
}

