#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "Net/HTTPData.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

UInt32 __stdcall Net::HTTPData::LoadThread(AnyType userObj)
{
	UInt8 buff[2048];
	UOSInt readSize;

	NN<Net::HTTPClient> cli;
	NN<HTTPDATAHANDLE> fdh = userObj.GetNN<HTTPDATAHANDLE>();
	if (fdh->queue)
	{
		Sync::MutexUsage mutUsage(fdh->mut);
		fdh->cli = cli = fdh->queue->MakeRequest(fdh->url->ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, true);
		mutUsage.EndUse();
	}
	else
	{
		fdh->cli = cli = Net::HTTPClient::CreateConnect(fdh->clif, fdh->ssl, fdh->url->ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, true);
	}
	fdh->evtTmp->Set();
	if (IO::Path::GetPathType(fdh->localFile->ToCString()) == IO::Path::PathType::File)
	{
		IO::FileStream fs(fdh->localFile, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!fs.IsError())
		{
			Data::DateTime dt;
			fs.GetFileTimes(0, 0, &dt);
			cli->AddTimeHeader(CSTR("If-Modified-Since"), dt);
		}
	}
	while (cli->GetRespStatus() == 301)
	{
		Text::StringBuilderUTF8 sb;
		cli->GetRespHeader(CSTR("Location"), sb);
		if (sb.GetLength() > 0 && (sb.StartsWith(UTF8STRC("http://")) || sb.StartsWith(UTF8STRC("https://"))))
		{
			if (fdh->queue)
			{
				Sync::MutexUsage mutUsage(fdh->mut);
				fdh->queue->EndRequest(cli);
				fdh->cli = cli = fdh->queue->MakeRequest(sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, true);
				mutUsage.EndUse();
			}
			else
			{
				cli.Delete();
				fdh->cli = cli = Net::HTTPClient::CreateConnect(fdh->clif, fdh->ssl, sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, true);
			}
		}
		else
		{
			break;
		}
	}

	if (cli->GetRespStatus() == 304)
	{
		NEW_CLASS(fdh->file, IO::FileStream(fdh->localFile, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		fdh->fileLength = fdh->file->GetLength();
	}
	else if (cli->GetRespStatus() == 200)
	{
		fdh->fileLength = cli->GetContentLength();
		if (fdh->fileLength > 0)
		{
			NEW_CLASS(fdh->file, IO::FileStream(fdh->localFile, IO::FileMode::Create, IO::FileShare::DenyWrite, IO::FileStream::BufferType::Normal));
			while (fdh->loadSize < fdh->fileLength)
			{
				readSize = cli->Read(BYTEARR(buff));
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
				fdh->file->Write(Data::ByteArrayR(buff, readSize));
				fdh->loadSize += readSize;
				fdh->currentOffset = fdh->loadSize;
				mutUsage.EndUse();
			}
			Data::DateTime dt;
			if (fdh->file && cli->GetLastModified(dt))
			{
				fdh->file->SetFileTimes(0, 0, &dt);
			}
		}
		else
		{
			Text::StringBuilderUTF8 sb;
			if (!cli->GetRespHeader(CSTR("Content-Length"), sb))
			{
				NN<IO::StreamReadReq> sess;
				Sync::Event readEvt(false);
				NEW_CLASS(fdh->file, IO::FileStream(fdh->localFile, IO::FileMode::Create, IO::FileShare::DenyWrite, IO::FileStream::BufferType::Normal));
				while (true)
				{
					readEvt.Clear();
					if (cli->BeginRead(BYTEARR(buff), readEvt).SetTo(sess))
					{
						Bool incomplete;
						readEvt.Wait(2000);
						readSize = cli->EndRead(sess, false, incomplete);
						if (readSize <= 0)
						{
							cli->Close();
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
					fdh->file->Write(Data::ByteArrayR(buff, readSize));
					fdh->loadSize += readSize;
					fdh->fileLength = fdh->loadSize;
					fdh->currentOffset = fdh->fileLength;
					mutUsage.EndUse();
				}
				Data::DateTime dt;
				if (fdh->file && cli->GetLastModified(dt))
				{
					fdh->file->SetFileTimes(0, 0, &dt);
				}
			}
		}
	}
	Sync::MutexUsage mutUsage(fdh->mut);
	if (fdh->queue)
	{
		fdh->queue->EndRequest(cli);
	}
	else
	{
		cli.Delete();
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

Net::HTTPData::HTTPData(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Net::HTTPQueue *queue, Text::CStringNN url, Text::CStringNN localFile, Bool forceReload)
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
		NEW_CLASS(fs, IO::FileStream(localFile, IO::FileMode::ReadOnly, IO::FileShare::DenyWrite, IO::FileStream::BufferType::Normal));
		if (fs->IsError())
		{
			DEL_CLASS(fs);
			this->dataLength = 0;
			this->dataOffset = 0;
		}
		else
		{
			NEW_CLASS(fdh, Net::HTTPData::HTTPDATAHANDLE());
			fdh->file = fs;
			dataOffset = 0;
			dataLength = fdh->fileLength = fs->GetLength();
			fdh->currentOffset = fs->GetPosition();
			fdh->objectCnt = 1;
			fdh->seekCnt = 0;
			fdh->url = Text::String::New(url);
			fdh->localFile = Text::String::New(localFile);
			fdh->isLoading = false;
			fdh->loadSize = 0;
			fdh->cli = 0;
			i = fdh->url->LastIndexOf('/');
			if (i != INVALID_INDEX)
			{
				fdh->fileName.v = &fdh->url->v[i + 1];
				fdh->fileName.leng = fdh->url->leng - i - 1;
			}
			else
			{
				fdh->fileName = fdh->url->ToCString();
			}
		}
	}
	else
	{
		dataOffset = 0;
		dataLength = (UOSInt)-1;
		NEW_CLASS(fdh, Net::HTTPData::HTTPDATAHANDLE());
		fdh->file = 0;
		fdh->fileLength = 0;
		fdh->currentOffset = 0;
		fdh->objectCnt = 1;
		fdh->seekCnt = 0;
		fdh->url = Text::String::New(url);
		fdh->localFile = Text::String::New(localFile);
		fdh->isLoading = true;
		fdh->loadSize = 0;
		fdh->clif = clif;
		fdh->ssl = ssl;
		fdh->queue = queue;
		i = fdh->url->LastIndexOf('/');
		if (i != INVALID_INDEX)
		{
			fdh->fileName.v = &fdh->url->v[i + 1];
			fdh->fileName.leng = fdh->url->leng - i - 1;
		}
		else
		{
			fdh->fileName = fdh->url->ToCString();
		}
		fdh->cli = 0;
		NEW_CLASS(fdh->evtTmp, Sync::Event(false));
		Sync::ThreadUtil::Create(LoadThread, fdh);
		while (fdh->cli.IsNull() && fdh->isLoading)
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

UOSInt Net::HTTPData::GetRealData(UInt64 offset, UOSInt length, Data::ByteArray buffer)
{
	if (fdh == 0)
		return 0;
	Sync::MutexUsage mutUsage(fdh->mut);
	while (fdh->isLoading && (dataOffset + offset + length > fdh->loadSize))
	{
		mutUsage.EndUse();
		Sync::SimpleThread::Sleep(10);
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
		byteRead = fdh->file->Read(buffer.WithSize(length));
	else
		byteRead = fdh->file->Read(buffer.WithSize((UOSInt) (dataLength - offset)));
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
				this->dataLength = fdh->fileLength;
				break;
			}
			Sync::SimpleThread::Sleep(10);
		}
	}
	return dataLength;
}

NN<Text::String> Net::HTTPData::GetFullName()
{
	if (fdh == 0)
		return Text::String::NewEmpty();
	return fdh->url;
}

Text::CString Net::HTTPData::GetShortName()
{
	if (fdh == 0)
		return CSTR_NULL;
	return fdh->fileName;
}

void Net::HTTPData::SetFullName(Text::CStringNN fullName)
{
	if (fdh == 0 || fullName.leng == 0)
		return;
	UOSInt i;
	Sync::MutexUsage mutUsage(fdh->mut);
	fdh->url->Release();
	fdh->url = Text::String::New(fullName);
	i = fdh->url->LastIndexOf('/');
	if (i != INVALID_INDEX)
	{
		fdh->fileName.v = &fdh->url->v[i + 1];
		fdh->fileName.leng =  fdh->url->leng - i - 1;
	}
	else
	{
		fdh->fileName = fdh->url->ToCString();
	}
	mutUsage.EndUse();
}

UnsafeArrayOpt<const UInt8> Net::HTTPData::GetPointer()
{
	return 0;
}

NN<IO::StreamData> Net::HTTPData::GetPartialData(UInt64 offset, UInt64 length)
{
	NN<Net::HTTPData> data;
	NEW_CLASSNN(data, Net::HTTPData(this, offset, length));
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
			NN<HTTPClient> cli;
			Sync::MutexUsage mutUsage(fdh->mut);
			if (fdh->isLoading && fdh->cli.SetTo(cli))
				cli->Close();
			mutUsage.EndUse();
			while (fdh->isLoading)
			{
				Sync::SimpleThread::Sleep(10);
			}
			DEL_CLASS(fdh->file);
			fdh->url->Release();
			fdh->localFile->Release();
			DEL_CLASS(fdh);
		}
	}
	fdh = 0;
}

