#ifndef _SM_NET_OSMGPXDOWNLOADER
#define _SM_NET_OSMGPXDOWNLOADER
#include "IO/Writer.h"
#include "Net/RSSReader.h"

namespace Net
{
	class OSMGPXDownloader : public Net::RSSHandler
	{
	private:
		Net::RSSReader *reader;
		Net::SocketFactory *sockf;
		const WChar *storeDir;
		IO::Writer *writer;

	public:
		OSMGPXDownloader(Net::SocketFactory *sockf, const WChar *storeDir, IO::Writer *writer);
		virtual ~OSMGPXDownloader();

		virtual void ItemAdded(Net::RSSItem *item);
		virtual void ItemRemoved(Net::RSSItem *item);
	};
}
#endif
