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
				NN<Text::String> message;
				Optional<Text::String> imgURL;
			} ItemData;

			typedef struct
			{
				Optional<Text::String> name;
				Optional<Text::String> bio;
			} ChannelInfo;
			
		private:
			NN<Net::TCPClientFactory> clif;
			Optional<Net::SSLEngine> ssl;
			Optional<Text::EncodingFactory> encFact;
			Optional<Text::String> userAgent;

		public:
			WebSiteTwitterControl(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Text::EncodingFactory> encFact, Optional<Text::String> userAgent);
			~WebSiteTwitterControl();

			UIntOS GetChannelItems(NN<Text::String> channelId, UIntOS pageNo, NN<Data::ArrayListNN<ItemData>> itemList, Optional<ChannelInfo> chInfo);
			void FreeItems(NN<Data::ArrayListNN<ItemData>> itemList);
			Optional<Text::String> GetUserAgent();
		};
	}
}
#endif
