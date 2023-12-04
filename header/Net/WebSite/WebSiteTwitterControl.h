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
			NotNullPtr<Net::SocketFactory> sockf;
			Optional<Net::SSLEngine> ssl;
			Text::EncodingFactory *encFact;
			Text::String *userAgent;

		public:
			WebSiteTwitterControl(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::EncodingFactory *encFact, Text::String *userAgent);
			~WebSiteTwitterControl();

			UOSInt GetChannelItems(NotNullPtr<Text::String> channelId, UOSInt pageNo, Data::ArrayList<ItemData*> *itemList, ChannelInfo *chInfo);
			void FreeItems(Data::ArrayList<ItemData*> *itemList);
			Text::String *GetUserAgent();
		};
	}
}
#endif
