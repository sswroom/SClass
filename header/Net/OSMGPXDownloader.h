#ifndef _SM_NET_OSMGPXDOWNLOADER
#define _SM_NET_OSMGPXDOWNLOADER
#include "Net/RSSReader.h"
#include "IO/IWriter.h"

namespace Net
{
	class OSMGPXDownloader : public Net::RSSHandler
	{
	private:
		Net::RSSReader *reader;
		Net::SocketFactory *sockf;
		const WChar *storeDir;
		IO::IWriter *writer;

	public:
		OSMGPXDownloader(Net::SocketFactory *sockf, const WChar *storeDir, IO::IWriter *writer);
		virtual ~OSMGPXDownloader();

		virtual void ItemAdded(Net::RSSItem *item);
		virtual void ItemRemoved(Net::RSSItem *item);
	};
};
#endif
