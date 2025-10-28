#ifndef _SM_NET_OSMGPXDOWNLOADER
#define _SM_NET_OSMGPXDOWNLOADER
#include "IO/Writer.h"
#include "Net/RSSReader.h"
#include "Text/String.h"

namespace Net
{
	class OSMGPXDownloader : public Net::RSSHandler
	{
	private:
		IO::LogTool log;
		NN<Net::RSSReader> reader;
		NN<Net::SocketFactory> sockf;
		NN<Text::String> storeDir;
		NN<IO::Writer> writer;

	public:
		OSMGPXDownloader(NN<Net::SocketFactory> sockf, Text::CStringNN storeDir, NN<IO::Writer> writer);
		virtual ~OSMGPXDownloader();

		virtual void ItemAdded(NN<Net::RSSItem> item);
		virtual void ItemRemoved(NN<Net::RSSItem> item);
	};
}
#endif
