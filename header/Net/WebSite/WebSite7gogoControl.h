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
				const UTF8Char *message;
				const UTF8Char *imgURL;
			} ItemData;

			typedef struct
			{
				const UTF8Char *talkCode;
				const UTF8Char *name;
				const UTF8Char *detail;
				const UTF8Char *imagePath;
				Int64 editDate;
				const UTF8Char *screenName;
			} ChannelInfo;
			
		private:
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
			Text::EncodingFactory *encFact;
			const UTF8Char *userAgent;

			ItemData *ParsePost(Text::JSONObject *postObj);
		public:
			WebSite7gogoControl(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, const UTF8Char *userAgent);
			~WebSite7gogoControl();

			OSInt GetChannelItems(const UTF8Char *channelId, OSInt pageNo, Data::ArrayList<ItemData*> *itemList, ChannelInfo *chInfo);
			void FreeItems(Data::ArrayList<ItemData*> *itemList);
			void FreeChannelInfo(ChannelInfo *chInfo);
			const UTF8Char *GetUserAgent();
		};
	}
}
#endif
