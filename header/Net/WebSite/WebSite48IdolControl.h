#ifndef _SM_NET_WEBSITE_WEBSITE48IDOLCONTROL
#define _SM_NET_WEBSITE_WEBSITE48IDOLCONTROL
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Text/EncodingFactory.h"

namespace Net
{
	namespace WebSite
	{
		class WebSite48IdolControl
		{
		public:
			typedef struct
			{
				Int32 id;
				Int64 recTime;
				NotNullPtr<Text::String> title;
			} ItemData;
			
		private:
			NotNullPtr<Net::SocketFactory> sockf;
			Optional<Net::SSLEngine> ssl;
			Text::EncodingFactory *encFact;
			Text::String *userAgent;

		public:
			WebSite48IdolControl(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::EncodingFactory *encFact, Text::String *userAgent);
			~WebSite48IdolControl();

			OSInt GetTVPageItems(OSInt pageNo, Data::ArrayList<ItemData*> *itemList);
			OSInt GetArcPageItems(OSInt pageNo, Data::ArrayList<ItemData*> *itemList);
			void FreeItems(Data::ArrayList<ItemData*> *itemList);

			Bool GetDownloadLink(Int32 videoId, Int32 linkId, NotNullPtr<Text::StringBuilderUTF8> link);
			Bool GetVideoName(Int32 videoId, NotNullPtr<Text::StringBuilderUTF8> name);

			static void Title2DisplayName(NotNullPtr<Text::String> title, NotNullPtr<Text::StringBuilderUTF8> dispName);
		};
	}
}
#endif
