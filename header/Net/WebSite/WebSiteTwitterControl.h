#ifndef _SM_NET_WEBSITE_WEBSITETWITTERCONTROL
#define _SM_NET_WEBSITE_WEBSITETWITTERCONTROL
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Text/EncodingFactory.h"

namespace Net
{
	namespace WebSite
	{
		class WebSiteTwitterControl
		{
		public:
			typedef struct
			{
				Int64 id;
				Int64 recTime;
				Text::String *message;
				Text::String *imgURL;
			} ItemData;

			typedef struct
			{
				Text::String *name;
				Text::String *bio;
			} ChannelInfo;
			
		private:
			NN<Net::SocketFactory> sockf;
			Optional<Net::SSLEngine> ssl;
			Optional<Text::EncodingFactory> encFact;
			Optional<Text::String> userAgent;

		public:
			WebSiteTwitterControl(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, Optional<Text::String> userAgent);
			~WebSiteTwitterControl();

			UOSInt GetChannelItems(NN<Text::String> channelId, UOSInt pageNo, Data::ArrayList<ItemData*> *itemList, ChannelInfo *chInfo);
			void FreeItems(Data::ArrayList<ItemData*> *itemList);
			Optional<Text::String> GetUserAgent();
		};
	}
}
#endif
