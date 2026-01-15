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
				NN<Text::String> message;
				Optional<Text::String> imgURL;
			} ItemData;

			typedef struct
			{
				Optional<Text::String> talkCode;
				Optional<Text::String> name;
				Optional<Text::String> detail;
				Optional<Text::String> imagePath;
				Int64 editDate;
				Optional<Text::String> screenName;
			} ChannelInfo;
			
		private:
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			Optional<Text::EncodingFactory> encFact;
			Optional<Text::String> userAgent;

			Optional<ItemData> ParsePost(NN<Text::JSONObject> postObj);
		public:
			WebSite7gogoControl(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, Optional<Text::String> userAgent);
			~WebSite7gogoControl();

			IntOS GetChannelItems(NN<Text::String> channelId, IntOS pageNo, NN<Data::ArrayListNN<ItemData>> itemList, Optional<ChannelInfo> chInfo);
			void FreeItems(NN<Data::ArrayListNN<ItemData>> itemList);
			void FreeChannelInfo(NN<ChannelInfo> chInfo);
			Optional<Text::String> GetUserAgent();
		};
	}
}
#endif
