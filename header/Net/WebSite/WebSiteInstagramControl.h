#ifndef _SM_NET_WEBSITE_WEBSITEINSTAGRAMCONTROL
#define _SM_NET_WEBSITE_WEBSITEINSTAGRAMCONTROL
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
				Text::String *shortCode;
				Int64 recTime;
				Text::String *message;
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
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
			Text::EncodingFactory *encFact;
			Text::String *userAgent;

			Text::JSONBase *ParsePageJSON(const UTF8Char *url, UOSInt urlLen);
		public:
			WebSiteInstagramControl(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, Text::String *userAgent);
			~WebSiteInstagramControl();

			OSInt GetChannelItems(Text::String *channelId, OSInt pageNo, Data::ArrayList<ItemData*> *itemList, ChannelInfo *chInfo);
			void FreeItems(Data::ArrayList<ItemData*> *itemList);
			void FreeChannelInfo(ChannelInfo *chInfo);
			OSInt GetPageImages(Text::String *shortCode, Data::ArrayList<Text::String*> *imageList, Data::ArrayList<Text::String*> *videoList);
			Text::String *GetUserAgent();
		};
	}
}
#endif
