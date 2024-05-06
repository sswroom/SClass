#ifndef _SM_NET_RSSREADER
#define _SM_NET_RSSREADER
#include "AnyType.h"
#include "Data/DateTime.h"
#include "Data/FastStringMap.h"
#include "Net/RSS.h"
#include "Net/SSLEngine.h"

namespace Net
{
	class RSSHandler
	{
	public:
		virtual void ItemAdded(NN<Net::RSSItem> item) = 0;
		virtual void ItemRemoved(NN<Net::RSSItem> item) = 0;
	};

	class RSSReader
	{
	private:
		typedef struct
		{
			Bool exist;
			NN<Net::RSSItem> item;
		} RSSStatus;
	private:
		NN<Text::String> url;
		NN<Net::SocketFactory> sockf;
		Optional<Net::SSLEngine> ssl;
		NN<IO::LogTool> log;
		RSSHandler *hdlr;
		Data::FastStringMapNN<RSSStatus> currRSSMaps;
		Data::DateTime nextDT;
		Net::RSS *lastRSS;
		UInt32 refreshSecond;
		Data::Duration timeout;

		Sync::Event *threadEvt;
		Bool threadRunning;
		Bool threadToStop;

		static UInt32 __stdcall RSSThread(AnyType userObj);
	public:
		RSSReader(Text::CString url, NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, UInt32 refreshSecond, RSSHandler *hdlr, Data::Duration timeout, NN<IO::LogTool> log);
		~RSSReader();

		Bool IsError();
	};
}
#endif
