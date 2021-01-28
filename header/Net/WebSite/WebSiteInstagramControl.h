#ifndef _SM_NET_WEBSITE_WEBSITEINSTAGRAMCONTROL
#define _SM_NET_WEBSITE_WEBSITEINSTAGRAMCONTROL
#include "Net/SocketFactory.h"
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
				const UTF8Char *shortCode;
				Int64 recTime;
				const UTF8Char *message;
				const UTF8Char *imgURL;
				const UTF8Char *videoURL;
				Bool moreImages;
			} ItemData;

			typedef struct
			{
				const UTF8Char *full_name;
				const UTF8Char *biography;
				const UTF8Char *profile_pic_url_hd;
				const UTF8Char *username;
			} ChannelInfo;
			
		private:
			Net::SocketFactory *sockf;
			Text::EncodingFactory *encFact;
			const UTF8Char *userAgent;

			Text::JSONBase *ParsePageJSON(const UTF8Char *url);
		public:
			WebSiteInstagramControl(Net::SocketFactory *sockf, Text::EncodingFactory *encFact, const UTF8Char *userAgent);
			~WebSiteInstagramControl();

			OSInt GetChannelItems(const UTF8Char *channelId, OSInt pageNo, Data::ArrayList<ItemData*> *itemList, ChannelInfo *chInfo);
			void FreeItems(Data::ArrayList<ItemData*> *itemList);
			void FreeChannelInfo(ChannelInfo *chInfo);
			OSInt GetPageImages(const UTF8Char *shortCode, Data::ArrayList<const UTF8Char*> *imageList, Data::ArrayList<const UTF8Char*> *videoList);
			const UTF8Char *GetUserAgent();
		};
	}
}
#endif
