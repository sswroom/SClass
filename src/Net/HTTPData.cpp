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
	NN<HTTPQueue> queue;
	if (fdh->queue.SetTo(queue))
	{
		Sync::MutexUsage mutUsage(fdh->mut);
		fdh->cli = cli = queue->MakeRequest(fdh->url->ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, true);
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
			fs.GetFileTimes(nullptr, nullptr, &dt);
			cli->AddTimeHeader(CSTR("If-Modified-Since"), dt);
		}
	}
	while (cli->GetRespStatus() == 301)
	{
		Text::StringBuilderUTF8 sb;
		cli->GetRespHeader(CSTR("Location"), sb);
		if (sb.GetLength() > 0 && (sb.StartsWith(UTF8STRC("http://")) || sb.StartsWith(UTF8STRC("https://"))))
		{
			if (fdh->queue.SetTo(queue))
			{
				Sync::MutexUsage mutUsage(fdh->mut);
				queue->EndRequest(cli);
				fdh->cli = cli = queue->MakeRequest(sb.ToCString(), Net::WebUtil::RequestMethod::HTTP_GET, true);
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
				fdh->file->SetFileTimes(nullptr, nullptr, &dt);
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
					fdh->file->SetFileTimes(nullptr, nullptr, &dt);
				}
			}
		}
	}
	Sync::MutexUsage mutUsage(fdh->mut);
	if (fdh->queue.SetTo(queue))
	{
		queue->EndRequest(cli);
	}
	else
	{
		cli.Delete();
	}
	fdh->cli = nullptr;
	fdh->isLoading = false;
	mutUsage.EndUse();
	return 0;
}

Net::HTTPData::HTTPData(NN<const Net::HTTPData> fd, UInt64 offset, UInt64 length)
{
	dataOffset = offset + fd->dataOffset;
	UInt64 endOffset = fd->dataOffset + fd->GetDataSize();
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
	NN<HTTPDATAHANDLE> fdh;
	this->fdh = fd->fdh;
	if (fd->fdh.SetTo(fdh))
	{
		fdh->objectCnt++;
	}
}

Net::HTTPData::HTTPData(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Net::HTTPQueue> queue, Text::CStringNN url, Text::CStringNN localFile, Bool forceReload)
{
	UOSInt i;
	Bool needReload = forceReload;
	NN<HTTPDATAHANDLE> fdh;
	IO::Path::PathType pt = IO::Path::GetPathType(localFile);
	this->fdh = nullptr;
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
			NEW_CLASSNN(fdh, Net::HTTPData::HTTPDATAHANDLE());
			this->fdh = fdh;
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
			fdh->cli = nullptr;
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
		NEW_CLASSNN(fdh, Net::HTTPData::HTTPDATAHANDLE());
		this->fdh = fdh;
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
		fdh->cli = nullptr;
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
	NN<HTTPDATAHANDLE> fdh;
	if (!this->fdh.SetTo(fdh))
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

UInt64 Net::HTTPData::GetDataSize() const
{
	NN<HTTPDATAHANDLE> fdh;
	if (dataLength == (UOSInt)-1 && this->fdh.SetTo(fdh))
	{
		while (true)
		{
			if (fdh->fileLength != 0 || !fdh->isLoading)
			{
				((HTTPData*)this)->dataLength = fdh->fileLength;
				break;
			}
			Sync::SimpleThread::Sleep(10);
		}
	}
	return dataLength;
}

NN<Text::String> Net::HTTPData::GetFullName() const
{
	NN<HTTPDATAHANDLE> fdh;
	if (!this->fdh.SetTo(fdh))
		return Text::String::NewEmpty();
	return fdh->url;
}

Text::CString Net::HTTPData::GetShortName() const
{
	NN<HTTPDATAHANDLE> fdh;
	if (!this->fdh.SetTo(fdh))
		return nullptr;
	return fdh->fileName;
}

void Net::HTTPData::SetFullName(Text::CStringNN fullName)
{
	NN<HTTPDATAHANDLE> fdh;
	if (!this->fdh.SetTo(fdh) || fullName.leng == 0)
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

UnsafeArrayOpt<const UInt8> Net::HTTPData::GetPointer() const
{
	return nullptr;
}

NN<IO::StreamData> Net::HTTPData::GetPartialData(UInt64 offset, UInt64 length)
{
	NN<Net::HTTPData> data;
	NEW_CLASSNN(data, Net::HTTPData(*this, offset, length));
	return data;
}

Bool Net::HTTPData::IsFullFile() const
{
	return this->dataOffset == 0;
}

Bool Net::HTTPData::IsLoading() const
{
	NN<HTTPDATAHANDLE> fdh;
	if (!this->fdh.SetTo(fdh))
		return false;
	return fdh->isLoading;
}

UOSInt Net::HTTPData::GetSeekCount() const
{
	NN<HTTPDATAHANDLE> fdh;
	if (!this->fdh.SetTo(fdh))
		return 0;
	return fdh->seekCnt;
}

void Net::HTTPData::Close()
{
	NN<HTTPDATAHANDLE> fdh;
	if (this->fdh.SetTo(fdh))
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
			fdh.Delete();
		}
	}
	this->fdh = nullptr;
}

