#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/BrowserInfo.h"

const UTF8Char *Net::BrowserInfo::GetName(BrowserType btype)
{
	switch (btype)
	{
	case BT_IE:
		return (const UTF8Char*)"IE";
	case BT_FIREFOX:
		return (const UTF8Char*)"Firefox";
	case BT_CHROME:
		return (const UTF8Char*)"Chrome";
	case BT_SAFARI:
		return (const UTF8Char*)"Safari";
	case BT_UCBROWSER:
		return (const UTF8Char*)"UC Browser";
	case BT_CFNETWORK:
		return (const UTF8Char*)"CFNetwork";
	case BT_SOGOUWEB:
		return (const UTF8Char*)"Sogou Web Spider";
	case BT_BAIDU:
		return (const UTF8Char*)"Baidu Spider";
	case BT_SEMRUSH:
		return (const UTF8Char*)"Semrush Bot";
	case BT_DALVIK:
		return (const UTF8Char*)"Dalvik";
	case BT_INDY:
		return (const UTF8Char*)"Indy Library";
	case BT_GOOGLEBOTS:
		return (const UTF8Char*)"GoogleBot (Smartphone)";
	case BT_ANDROIDWV:
		return (const UTF8Char*)"Android WebView";
	case BT_SAMSUNG:
		return (const UTF8Char*)"Samsung Browser";
	case BT_WESTWIND:
		return (const UTF8Char*)"West Wind Internet Protocols";
	case BT_YANDEX:
		return (const UTF8Char*)"Yandex Bot";
	case BT_BING:
		return (const UTF8Char*)"Bing Bot";
	case BT_MASSCAN:
		return (const UTF8Char*)"masscan";
	case BT_PYREQUESTS:
		return (const UTF8Char*)"Python Requests";
	case BT_ZGRAB:
		return (const UTF8Char*)"zgrab";
	case BT_EDGE:
		return (const UTF8Char*)"Edge";
	case BT_PYURLLIB:
		return (const UTF8Char*)"Python urllib";
	case BT_GOOGLEBOTD:
		return (const UTF8Char*)"GoogleBot (Desktop)";
	case BT_DOTNET:
		return (const UTF8Char*)".NET Web Client";
	case BT_WINDIAG:
		return (const UTF8Char*)"Microsoft Windows Network Diagnostics";
	case BT_SSWR:
		return (const UTF8Char*)"SSWR";
	case BT_SMARTTV:
		return (const UTF8Char*)"SmartTV";
	case BT_BLEXBOT:
		return (const UTF8Char*)"BLEXBot";
	case BT_SOGOUPIC:
		return (const UTF8Char*)"Sogou Pic Spider";
	case BT_NUTCH:
		return (const UTF8Char*)"Apache Nutch";
	case BT_YISOU:
		return (const UTF8Char*)"Yisou Spider";
	case BT_WGET:
		return (const UTF8Char*)"Wget";
	case BT_SCRAPY:
		return (const UTF8Char*)"Scrapy";
	case BT_GOHTTP:
		return (const UTF8Char*)"Golang HTTP Client";
	case BT_WINHTTP:
		return (const UTF8Char*)"WinHTTP Client";
	case BT_NLPPROJECT:
		return (const UTF8Char*)"NLPProject";
	case BT_APACHEHTTP:
		return (const UTF8Char*)"Apache HTTP Client";
	case BT_BANNERDET:
		return (const UTF8Char*)"HTTP Banner Detection";
	case BT_NETCRAFTWEB:
		return (const UTF8Char*)"Netcraft Web Server Survey";
	case BT_NETCRAFTAG:
		return (const UTF8Char*)"Netcraft Survey Agent";
	case BT_AHREFSBOT:
		return (const UTF8Char*)"Ahrefs Bot";
	case BT_MJ12BOT:
		return (const UTF8Char*)"MJ12Bot";
	case BT_NETSYSRES:
		return (const UTF8Char*)"NetSystemsResearch";
	case BT_WHATSAPP:
		return (const UTF8Char*)"WhatsApp";
	case BT_CURL:
		return (const UTF8Char*)"Curl";
	case BT_GSA:
		return (const UTF8Char*)"Google Search App";
	case BT_FACEBOOK:
		return (const UTF8Char*)"Facebook External Hit";
	case BT_NETSEEN:
		return (const UTF8Char*)"NetSeen";
	case BT_MSNBOT:
		return (const UTF8Char*)"MSNBOT";
	case BT_LIBWWW_PERL:
		return (const UTF8Char*)"libwww-perl";
	case BT_UNKNOWN:
	default:
		return (const UTF8Char*)"Unknown";
	}
}

const UTF8Char *Net::BrowserInfo::GetDefName(BrowserType btype)
{
	switch (btype)
	{
	case BT_IE:
		return (const UTF8Char*)"BT_IE";
	case BT_FIREFOX:
		return (const UTF8Char*)"BT_FIREFOX";
	case BT_CHROME:
		return (const UTF8Char*)"BT_CHROME";
	case BT_SAFARI:
		return (const UTF8Char*)"BT_SAFARI";
	case BT_UCBROWSER:
		return (const UTF8Char*)"BT_UCBROWSER";
	case BT_CFNETWORK:
		return (const UTF8Char*)"BT_CFNETWORK";
	case BT_SOGOUWEB:
		return (const UTF8Char*)"BT_SOGOUWEB";
	case BT_BAIDU:
		return (const UTF8Char*)"BT_BAIDU";
	case BT_SEMRUSH:
		return (const UTF8Char*)"BT_SEMRUSH";
	case BT_DALVIK:
		return (const UTF8Char*)"BT_DALVIK";
	case BT_INDY:
		return (const UTF8Char*)"BT_INDY";
	case BT_GOOGLEBOTS:
		return (const UTF8Char*)"BT_GOOGLEBOTS";
	case BT_ANDROIDWV:
		return (const UTF8Char*)"BT_ANDROIDWV";
	case BT_SAMSUNG:
		return (const UTF8Char*)"BT_SAMSUNG";
	case BT_WESTWIND:
		return (const UTF8Char*)"BT_WESTWIND";
	case BT_YANDEX:
		return (const UTF8Char*)"BT_YANDEX";
	case BT_BING:
		return (const UTF8Char*)"BT_BING";
	case BT_MASSCAN:
		return (const UTF8Char*)"BT_MASSCAN";
	case BT_PYREQUESTS:
		return (const UTF8Char*)"BT_PYREQUESTS";
	case BT_ZGRAB:
		return (const UTF8Char*)"BT_ZGRAB";
	case BT_EDGE:
		return (const UTF8Char*)"BT_EDGE";
	case BT_PYURLLIB:
		return (const UTF8Char*)"BT_PYURLLIB";
	case BT_GOOGLEBOTD:
		return (const UTF8Char*)"BT_GOOGLEBOTD";
	case BT_DOTNET:
		return (const UTF8Char*)"BT_DOTNET";
	case BT_WINDIAG:
		return (const UTF8Char*)"BT_WINDIAG";
	case BT_SSWR:
		return (const UTF8Char*)"BT_SSWR";
	case BT_SMARTTV:
		return (const UTF8Char*)"BT_SMARTTV";
	case BT_BLEXBOT:
		return (const UTF8Char*)"BT_BLEXBOT";
	case BT_SOGOUPIC:
		return (const UTF8Char*)"BT_SOGOUPIC";
	case BT_NUTCH:
		return (const UTF8Char*)"BT_NUTCH";
	case BT_YISOU:
		return (const UTF8Char*)"BT_YISOU";
	case BT_WGET:
		return (const UTF8Char*)"BT_WGET";
	case BT_SCRAPY:
		return (const UTF8Char*)"BT_SCRAPY";
	case BT_GOHTTP:
		return (const UTF8Char*)"BT_GOHTTP";
	case BT_WINHTTP:
		return (const UTF8Char*)"BT_WINHTTP";
	case BT_NLPPROJECT:
		return (const UTF8Char*)"BT_NLPPROJECT";
	case BT_APACHEHTTP:
		return (const UTF8Char*)"BT_APACHEHTTP";
	case BT_BANNERDET:
		return (const UTF8Char*)"BT_BANNERDET";
	case BT_NETCRAFTWEB:
		return (const UTF8Char*)"BT_NETCRAFTWEB";
	case BT_NETCRAFTAG:
		return (const UTF8Char*)"BT_NETCRAFTAG";
	case BT_AHREFSBOT:
		return (const UTF8Char*)"BT_AHREFSBOT";
	case BT_MJ12BOT:
		return (const UTF8Char*)"BT_MJ12BOT";
	case BT_NETSYSRES:
		return (const UTF8Char*)"BT_NETSYSRES";
	case BT_WHATSAPP:
		return (const UTF8Char*)"BT_WHATSAPP";
	case BT_CURL:
		return (const UTF8Char*)"BT_CURL";
	case BT_GSA:
		return (const UTF8Char*)"BT_GSA";
	case BT_FACEBOOK:
		return (const UTF8Char*)"BT_FACEBOOK";
	case BT_NETSEEN:
		return (const UTF8Char*)"BT_NETSEEN";
	case BT_MSNBOT:
		return (const UTF8Char*)"BT_NETSEEN";
	case BT_LIBWWW_PERL:
		return (const UTF8Char*)"BT_LIBWWW_PERL";
	case BT_UNKNOWN:
	default:
		return (const UTF8Char*)"BT_UNKNOWN";
	}
}
