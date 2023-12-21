#ifndef _SM_NET_WEBSITE_WEBSITE7GOGOCONTROL
#define _SM_NET_WEBSITE_WEBSITE7GOGOCONTROL
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Text/EncodingFactory.h"
#include "Text/JSON.h"

namespace Net
{
	namespace WebSite
	{
		class WebSite7gogoControl
		{
		public:
			typedef struct
			{
				Int64 id;
				Int64 recTime;
				NotNullPtr<Text::String> message;
				Text::String *imgURL;
			} ItemData;

			typedef struct
			{
				Text::String *talkCode;
				Text::String *name;
				Text::String *detail;
				Text::String *imagePath;
				Int64 editDate;
				Text::String *screenName;
			} ChannelInfo;
			
		private:
			NotNullPtr<Net::SocketFactory> sockf;
			Optional<Net::SSLEngine> ssl;
			Text::EncodingFactory *encFact;
			Optional<Text::String> userAgent;

			ItemData *ParsePost(Text::JSONObject *postObj);
		public:
			WebSite7gogoControl(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::EncodingFactory *encFact, Optional<Text::String> userAgent);
			~WebSite7gogoControl();

			OSInt GetChannelItems(NotNullPtr<Text::String> channelId, OSInt pageNo, Data::ArrayList<ItemData*> *itemList, ChannelInfo *chInfo);
			void FreeItems(Data::ArrayList<ItemData*> *itemList);
			void FreeChannelInfo(ChannelInfo *chInfo);
			Optional<Text::String> GetUserAgent();
		};
	}
}
#endif
