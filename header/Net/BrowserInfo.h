#ifndef _SM_NET_BROWSERINFO
#define _SM_NET_BROWSERINFO
#include "Text/CString.h"
namespace Net
{
	class BrowserInfo
	{
	public:
		typedef enum
		{
			BT_UNKNOWN,
			BT_IE,
			BT_FIREFOX,
			BT_CHROME,
			BT_SAFARI,
			BT_UCBROWSER,
			BT_CFNETWORK,
			BT_SOGOUWEB,
			BT_BAIDU,
			BT_SEMRUSH,
			BT_DALVIK,
			BT_INDY,
			BT_GOOGLEBOTS,
			BT_ANDROIDWV,
			BT_SAMSUNG,
			BT_WESTWIND,
			BT_YANDEX,
			BT_BING,
			BT_MASSCAN,
			BT_PYREQUESTS,
			BT_ZGRAB,
			BT_EDGE,
			BT_PYURLLIB,
			BT_GOOGLEBOTD,
			BT_DOTNET,
			BT_WINDIAG,
			BT_SSWR,
			BT_SMARTTV,
			BT_BLEXBOT,
			BT_SOGOUPIC,
			BT_NUTCH,
			BT_YISOU,
			BT_WGET,
			BT_SCRAPY,
			BT_GOHTTP,
			BT_WINHTTP,
			BT_NLPPROJECT,
			BT_APACHEHTTP,
			BT_BANNERDET,
			BT_NETCRAFTWEB,
			BT_NETCRAFTAG,
			BT_AHREFSBOT,
			BT_MJ12BOT,
			BT_NETSYSRES,
			BT_WHATSAPP,
			BT_CURL,
			BT_GSA,
			BT_FACEBOOK,
			BT_NETSEEN,
			BT_MSNBOT,
			BT_LIBWWW_PERL,
			BT_HUAWEIBROWSER,
			BT_OPERA,
			BT_MIBROWSER,

			BT_FIRST = BT_UNKNOWN,
			BT_LAST = BT_MIBROWSER
		} BrowserType;

	public:
		static Text::CStringNN GetName(BrowserType btype);
		static Text::CStringNN GetDefName(BrowserType btype);
	};
}
#endif
