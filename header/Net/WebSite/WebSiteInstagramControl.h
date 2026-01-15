#ifndef _SM_NET_WEBSITE_WEBSITEINSTAGRAMCONTROL
#define _SM_NET_WEBSITE_WEBSITEINSTAGRAMCONTROL
#include "Data/ArrayListNN.hpp"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Text/EncodingFactory.h"
#include "Text/JSON.h"

namespace Net
{
	namespace WebSite
	{
		class WebSiteInstagramControl
		{
		public:
			typedef struct
			{
				Int64 id;
				NN<Text::String> shortCode;
				Int64 recTime;
				NN<Text::String> message;
				Optional<Text::String> imgURL;
				Optional<Text::String> videoURL;
				Bool moreImages;
			} ItemData;

			typedef struct
			{
				Optional<Text::String> full_name;
				Optional<Text::String> biography;
				Optional<Text::String> profile_pic_url_hd;
				Optional<Text::String> username;
			} ChannelInfo;
			
		private:
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			Optional<Text::EncodingFactory> encFact;
			Optional<Text::String> userAgent;

			Optional<Text::JSONBase> ParsePageJSON(Text::CStringNN url);
		public:
			WebSiteInstagramControl(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, Optional<Text::String> userAgent);
			~WebSiteInstagramControl();

			IntOS GetChannelItems(NN<Text::String> channelId, IntOS pageNo, NN<Data::ArrayListNN<ItemData>> itemList, Optional<ChannelInfo> chInfo);
			void FreeItems(NN<Data::ArrayListNN<ItemData>> itemList);
			void FreeChannelInfo(NN<ChannelInfo> chInfo);
			IntOS GetPageImages(NN<Text::String> shortCode, NN<Data::ArrayListStringNN> imageList, NN<Data::ArrayListStringNN> videoList);
			Optional<Text::String> GetUserAgent();
		};
	}
}
#endif
