#ifndef _SM_NET_WEBSITE_WEBSITEINSTAGRAMCONTROL
#define _SM_NET_WEBSITE_WEBSITEINSTAGRAMCONTROL
#include "Data/ArrayListNN.h"
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
				Text::String *imgURL;
				Text::String *videoURL;
				Bool moreImages;
			} ItemData;

			typedef struct
			{
				Text::String *full_name;
				Text::String *biography;
				Text::String *profile_pic_url_hd;
				Text::String *username;
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

			OSInt GetChannelItems(NN<Text::String> channelId, OSInt pageNo, NN<Data::ArrayListNN<ItemData>> itemList, Optional<ChannelInfo> chInfo);
			void FreeItems(NN<Data::ArrayListNN<ItemData>> itemList);
			void FreeChannelInfo(NN<ChannelInfo> chInfo);
			OSInt GetPageImages(NN<Text::String> shortCode, Data::ArrayListStringNN *imageList, Data::ArrayListStringNN *videoList);
			Optional<Text::String> GetUserAgent();
		};
	}
}
#endif
