#include "Stdafx.h"
#include "IO/Path.h"
#include "IO/FileStream.h"
#include "Net/HTTPClient.h"
#include "Net/OSMGPXDownloader.h"
#include "Sync/ThreadUtil.h"

Net::OSMGPXDownloader::OSMGPXDownloader(NN<Net::SocketFactory> sockf, Text::CString storeDir, IO::Writer *writer)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = storeDir.ConcatTo(sbuff);
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
		*sptr = 0;
	}
	this->writer = writer;
	this->storeDir = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
	this->sockf = sockf;
	NEW_CLASS(reader, Net::RSSReader(CSTR("http://www.openstreetmap.org/traces/rss"), sockf, 0, 900, this));
}

Net::OSMGPXDownloader::~OSMGPXDownloader()
{
	DEL_CLASS(reader);
	this->storeDir->Release();
}

void Net::OSMGPXDownloader::ItemAdded(Net::RSSItem *item)
{
	UTF8Char gpxId[16];
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	UOSInt i;
	IO::Path::PathType pt;

	sptr = item->link->ConcatTo(sbuff);
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '/');
	sptr = &sbuff[i + 1];
	Text::StrConcat(gpxId, sptr);
	sptr2 = Text::StrConcatC(sbuff2, UTF8STRC("http://www.openstreetmap.org/trace/"));
	sptr2 = Text::StrConcat(sptr2, gpxId);
	sptr2 = Text::StrConcatC(sptr2, UTF8STRC("/data"));

	sptr = this->storeDir->ConcatTo(sbuff);
	sptr = item->author->ConcatTo(sptr);
	pt = IO::Path::GetPathType(sbuff, (UOSInt)(sptr - sbuff));
	if (pt == IO::Path::PathType::Unknown)
	{
		IO::Path::CreateDirectory(sbuff);
	}
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcat(sptr, gpxId);
	sptr = Text::StrConcatC(sptr, UTF8STRC(".gpx"));
	pt = IO::Path::GetPathType(sbuff, (UOSInt)(sptr - sbuff));
	if (pt == IO::Path::PathType::File)
	{
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		NN<Net::HTTPClient> cli;
		UInt8 buff[2048];
		UOSInt readSize;
		UInt64 totalSize;
		IO::FileStream *fs;
		UOSInt retryCnt = 3;

		sb.AppendC(UTF8STRC("Downloading: "));
		sb.AppendC(sbuff2, (UOSInt)(sptr2 - sbuff2));
		this->writer->WriteLine(sb.ToCString());

		while (retryCnt-- > 0)
		{
			NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
			cli = Net::HTTPClient::CreateConnect(sockf, 0, sbuff2, Net::WebUtil::RequestMethod::HTTP_GET, true);

			totalSize = 0;
			while (true)
			{
				readSize = cli->Read(buff, 2048);
				if (readSize <= 0)
					break;
				totalSize += readSize;
				fs->Write(buff, readSize);
			}
			DEL_CLASS(cli);
			DEL_CLASS(fs);

			if (totalSize > 1)
				break;
			IO::Path::DeleteFile(sbuff);
			Sync::SimpleThread::Sleep(10000);
		}
	}
}

void Net::OSMGPXDownloader::ItemRemoved(Net::RSSItem *item)
{
}
