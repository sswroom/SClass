#include "Stdafx.h"
#include "IO/Path.h"
#include "IO/FileStream.h"
#include "Net/HTTPClient.h"
#include "Net/OSMGPXDownloader.h"
#include "Sync/Thread.h"

Net::OSMGPXDownloader::OSMGPXDownloader(Net::SocketFactory *sockf, const UTF8Char *storeDir, IO::Writer *writer)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = Text::StrConcat(sbuff, storeDir);
	if (sptr[-1] != '\\')
	{
		*sptr++ = '\\';
		*sptr = 0;
	}
	this->writer = writer;
	this->storeDir = Text::StrCopyNew(sbuff);
	this->sockf = sockf;
	NEW_CLASS(reader, Net::RSSReader((const UTF8Char*)"http://www.openstreetmap.org/traces/rss", sockf, 900, this));
}

Net::OSMGPXDownloader::~OSMGPXDownloader()
{
	DEL_CLASS(reader);
	Text::StrDelNew(storeDir);
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

	Text::StrConcat(sbuff, item->link);
	i = Text::StrLastIndexOf(sbuff, '/');
	sptr = &sbuff[i + 1];
	Text::StrConcat(gpxId, sptr);
	sptr2 = Text::StrConcat(sbuff2, (const UTF8Char*)"http://www.openstreetmap.org/trace/");
	sptr2 = Text::StrConcat(sptr2, gpxId);
	sptr2 = Text::StrConcat(sptr2, (const UTF8Char*)"/data");

	sptr = Text::StrConcat(sbuff, this->storeDir);
	sptr = Text::StrConcat(sptr, item->author);
	pt = IO::Path::GetPathType(sbuff);
	if (pt == IO::Path::PathType::Unknown)
	{
		IO::Path::CreateDirectory(sbuff);
	}
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"\\");
	sptr = Text::StrConcat(sptr, gpxId);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)".gpx");
	pt = IO::Path::GetPathType(sbuff);
	if (pt == IO::Path::PathType::File)
	{
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		Net::HTTPClient *cli;
		UInt8 buff[2048];
		UOSInt readSize;
		UInt64 totalSize;
		IO::FileStream *fs;
		UOSInt retryCnt = 3;

		sb.Append((const UTF8Char*)"Downloading: ");
		sb.Append(sbuff2);
		this->writer->WriteLine(sb.ToString());

		while (retryCnt-- > 0)
		{
			NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
			cli = Net::HTTPClient::CreateConnect(sockf, sbuff2, "GET", true);

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
			Sync::Thread::Sleep(10000);
		}
	}
}

void Net::OSMGPXDownloader::ItemRemoved(Net::RSSItem *item)
{
}
