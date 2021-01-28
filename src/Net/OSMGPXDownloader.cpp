#include "Stdafx.h"
#include "IO/Path.h"
#include "IO/FileStream.h"
#include "Net/HTTPClient.h"
#include "Net/OSMGPXDownloader.h"
#include "Sync/Thread.h"

Net::OSMGPXDownloader::OSMGPXDownloader(Net::SocketFactory *sockf, const WChar *storeDir, IO::IWriter *writer)
{
	WChar sbuff[512];
	WChar *sptr;
	sptr = Text::StrConcat(sbuff, storeDir);
	if (sptr[-1] != '\\')
	{
		*sptr++ = '\\';
		*sptr = 0;
	}
	this->writer = writer;
	this->storeDir = Text::StrCopyNew(sbuff);
	this->sockf = sockf;
	NEW_CLASS(reader, Net::RSSReader(L"http://www.openstreetmap.org/traces/rss", sockf, 900, this));
}

Net::OSMGPXDownloader::~OSMGPXDownloader()
{
	DEL_CLASS(reader);
	Text::StrDelNew(storeDir);
}

void Net::OSMGPXDownloader::ItemAdded(Net::RSSItem *item)
{
	WChar gpxId[16];
	WChar sbuff[512];
	WChar sbuff2[512];
	WChar *sptr;
	WChar *sptr2;
	OSInt i;
	IO::Path::PathType pt;

	Text::StrConcat(sbuff, item->link);
	i = Text::StrLastIndexOf(sbuff, '/');
	sptr = &sbuff[i + 1];
	Text::StrConcat(gpxId, sptr);
	sptr2 = Text::StrConcat(sbuff2, L"http://www.openstreetmap.org/trace/");
	sptr2 = Text::StrConcat(sptr2, gpxId);
	sptr2 = Text::StrConcat(sptr2, L"/data");

	sptr = Text::StrConcat(sbuff, this->storeDir);
	sptr = Text::StrConcat(sptr, item->author);
	pt = IO::Path::GetPathType(sbuff);
	if (pt == IO::Path::PT_UNKNOWN)
	{
		IO::Path::CreateDirectory(sbuff);
	}
	sptr = Text::StrConcat(sptr, L"\\");
	sptr = Text::StrConcat(sptr, gpxId);
	sptr = Text::StrConcat(sptr, L".gpx");
	pt = IO::Path::GetPathType(sbuff);
	if (pt == IO::Path::PT_FILE)
	{
	}
	else
	{
		Text::StringBuilder sb;
		Net::HTTPClient *cli;
		UInt8 buff[2048];
		OSInt readSize;
		Int64 totalSize;
		IO::FileStream *fs;
		OSInt retryCnt = 3;

		sb.Append(L"Downloading: ");
		sb.Append(sbuff2);
		this->writer->WriteLine(sb.ToString());

		while (retryCnt-- > 0)
		{
			NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NO_WRITE_BUFFER));
			NEW_CLASS(cli, Net::HTTPClient(sockf, sbuff2, L"GET", true));

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
