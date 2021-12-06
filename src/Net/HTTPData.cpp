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
		fdh->cli = fdh->queue->MakeRequest(fdh->url->v, "GET", true);
		mutUsage.EndUse();
	}
	else
	{
		fdh->cli = Net::HTTPClient::CreateConnect(fdh->sockf, fdh->ssl, fdh->url->v, "GET", true);
	}
	fdh->evtTmp->Set();
	if (IO::Path::GetPathType(fdh->localFile->v) == IO::Path::PathType::File)
	{
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream(fdh->localFile->v, IO::FileStream::FileMode::ReadOnly, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
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
				fdh->cli = Net::HTTPClient::CreateConnect(fdh->sockf, fdh->ssl, sb.ToString(), "GET", true);
			}
		}
		else
		{
			break;
		}
	}

	if (fdh->cli->GetRespStatus() == 304)
	{
		NEW_CLASS(fdh->file, IO::FileStream(fdh->localFile->v, IO::FileStream::FileMode::ReadOnly, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		fdh->fileLength = fdh->file->GetLength();
	}
	else if (fdh->cli->GetRespStatus() == 200)
	{
		fdh->fileLength = fdh->cli->GetContentLength();
		if (fdh->fileLength > 0)
		{
			NEW_CLASS(fdh->file, IO::FileStream(fdh->localFile->v, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyWrite, IO::FileStream::BufferType::Normal));
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
					IO::Path::DeleteFile(fdh->localFile->v);
					break;
				}
				Sync::MutexUsage mutUsage(fdh->mut);
				if (fdh->currentOffset != fdh->loadSize)
				{
					fdh->file->SeekFromBeginning(fdh->loadSize);
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
				NEW_CLASS(fdh->file, IO::FileStream(fdh->localFile->v, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyWrite, IO::FileStream::BufferType::Normal));
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
						fdh->file->SeekFromBeginning(fdh->loadSize);
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

Net::HTTPData::HTTPData(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Net::HTTPQueue *queue, const UTF8Char *url, const UTF8Char *localFile, Bool forceReload)
{
	UOSInt i;
	Bool needReload = forceReload;
	IO::Path::PathType pt = IO::Path::GetPathType(localFile);
	fdh = 0;
	if (pt == IO::Path::PathType::Directory)
	{
		this->dataLength = 0;
		this->dataOffset = 0;
		return;
	}
	else if (pt == IO::Path::PathType::Unknown)
	{
		needReload = true;
	}
	if (!needReload)
	{
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream(localFile, IO::FileStream::FileMode::ReadOnly, IO::FileStream::FileShare::DenyWrite, IO::FileStream::BufferType::Normal));
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
			fdh->url = Text::String::New(url);
			fdh->localFile = Text::String::New(localFile);
			fdh->isLoading = false;
			fdh->loadSize = 0;
			fdh->cli = 0;
			i = fdh->url->LastIndexOf('/');
			if (i != INVALID_INDEX)
			{
				fdh->fileName = &fdh->url->v[i + 1];
			}
			else
			{
				fdh->fileName = fdh->url->v;
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
		fdh->url = Text::String::New(url);
		fdh->localFile = Text::String::New(localFile);
		fdh->isLoading = true;
		fdh->loadSize = 0;
		fdh->sockf = sockf;
		fdh->ssl = ssl;
		fdh->queue = queue;
		i = fdh->url->LastIndexOf('/');
		if (i != INVALID_INDEX)
		{
			fdh->fileName = &fdh->url->v[i + 1];
		}
		else
		{
			fdh->fileName = fdh->url->v;
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
		if ((fdh->currentOffset = fdh->file->SeekFromBeginning(dataOffset + offset)) != dataOffset + offset)
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

Text::String *Net::HTTPData::GetFullName()
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
	UOSInt i;
	Sync::MutexUsage mutUsage(fdh->mut);
	SDEL_STRING(fdh->url);
	fdh->url = Text::String::New(fullName);
	i = fdh->url->LastIndexOf('/');
	if (i != INVALID_INDEX)
	{
		fdh->fileName = &fdh->url->v[i + 1];
	}
	else
	{
		fdh->fileName = fdh->url->v;
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
			fdh->url->Release();
			fdh->localFile->Release();
			DEL_CLASS(fdh->mut);
			MemFree(fdh);
		}
	}
	fdh = 0;
}

