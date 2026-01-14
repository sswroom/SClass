#ifndef _SM_NET_RSSREADER
#define _SM_NET_RSSREADER
#include "AnyType.h"
#include "Data/DateTime.h"
#include "Data/FastStringMapNN.hpp"
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
		NN<Net::TCPClientFactory> clif;
		Optional<Net::SSLEngine> ssl;
		NN<IO::LogTool> log;
		NN<RSSHandler> hdlr;
		Data::FastStringMapNN<RSSStatus> currRSSMaps;
		Data::DateTime nextDT;
		Optional<Net::RSS> lastRSS;
		UInt32 refreshSecond;
		Data::Duration timeout;

		Sync::Event threadEvt;
		Bool threadRunning;
		Bool threadToStop;

		static UInt32 __stdcall RSSThread(AnyType userObj);
	public:
		RSSReader(Text::CStringNN url, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, UInt32 refreshSecond, NN<RSSHandler> hdlr, Data::Duration timeout, NN<IO::LogTool> log);
		~RSSReader();

		Bool IsError();
	};
}
#endif
