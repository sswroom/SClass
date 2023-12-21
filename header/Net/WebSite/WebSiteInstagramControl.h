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
				NotNullPtr<Text::String> shortCode;
				Int64 recTime;
				NotNullPtr<Text::String> message;
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
			NotNullPtr<Net::SocketFactory> sockf;
			Optional<Net::SSLEngine> ssl;
			Text::EncodingFactory *encFact;
			Optional<Text::String> userAgent;

			Text::JSONBase *ParsePageJSON(Text::CStringNN url);
		public:
			WebSiteInstagramControl(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::EncodingFactory *encFact, Optional<Text::String> userAgent);
			~WebSiteInstagramControl();

			OSInt GetChannelItems(NotNullPtr<Text::String> channelId, OSInt pageNo, Data::ArrayList<ItemData*> *itemList, ChannelInfo *chInfo);
			void FreeItems(Data::ArrayList<ItemData*> *itemList);
			void FreeChannelInfo(ChannelInfo *chInfo);
			OSInt GetPageImages(NotNullPtr<Text::String> shortCode, Data::ArrayListStringNN *imageList, Data::ArrayListStringNN *videoList);
			Optional<Text::String> GetUserAgent();
		};
	}
}
#endif
