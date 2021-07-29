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
				const UTF8Char *message;
				const UTF8Char *imgURL;
			} ItemData;

			typedef struct
			{
				const UTF8Char *name;
				const UTF8Char *bio;
			} ChannelInfo;
			
		private:
			Net::SocketFactory *sockf;
			Net::SSLEngine *ssl;
			Text::EncodingFactory *encFact;
			const UTF8Char *userAgent;

		public:
			WebSiteTwitterControl(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::EncodingFactory *encFact, const UTF8Char *userAgent);
			~WebSiteTwitterControl();

			UOSInt GetChannelItems(const UTF8Char *channelId, UOSInt pageNo, Data::ArrayList<ItemData*> *itemList, ChannelInfo *chInfo);
			void FreeItems(Data::ArrayList<ItemData*> *itemList);
			const UTF8Char *GetUserAgent();
		};
	}
}
#endif
