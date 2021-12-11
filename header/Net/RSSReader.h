#ifndef _SM_NET_RSSREADER
#define _SM_NET_RSSREADER
#include "Data/DateTime.h"
#include "Data/FastStringMap.h"
#include "Net/RSS.h"
#include "Net/SSLEngine.h"

namespace Net
{
	class RSSHandler
	{
	public:
		virtual void ItemAdded(Net::RSSItem *item) = 0;
		virtual void ItemRemoved(Net::RSSItem *item) = 0;
	};

	class RSSReader
	{
	private:
		typedef struct
		{
			Bool exist;
			Net::RSSItem *item;
		} RSSStatus;
	private:
		const UTF8Char *url;
		Net::SocketFactory *sockf;
		Net::SSLEngine *ssl;
		RSSHandler *hdlr;
		Data::FastStringMap<RSSStatus *> *currRSSMaps;
		Data::DateTime *nextDT;
		Net::RSS *lastRSS;
		UInt32 refreshSecond;

		Sync::Event *threadEvt;
		Bool threadRunning;
		Bool threadToStop;

		static UInt32 __stdcall RSSThread(void *userObj);
	public:
		RSSReader(const UTF8Char *url, Net::SocketFactory *sockf, Net::SSLEngine *ssl, UInt32 refreshSecond, RSSHandler *hdlr);
		~RSSReader();

		Bool IsError();
	};
}
#endif
