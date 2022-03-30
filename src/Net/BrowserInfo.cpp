#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/BrowserInfo.h"

Text::CString Net::BrowserInfo::GetName(BrowserType btype)
{
	switch (btype)
	{
	case BT_IE:
		return CSTR("IE");
	case BT_FIREFOX:
		return CSTR("Firefox");
	case BT_CHROME:
		return CSTR("Chrome");
	case BT_SAFARI:
		return CSTR("Safari");
	case BT_UCBROWSER:
		return CSTR("UC Browser");
	case BT_CFNETWORK:
		return CSTR("CFNetwork");
	case BT_SOGOUWEB:
		return CSTR("Sogou Web Spider");
	case BT_BAIDU:
		return CSTR("Baidu Spider");
	case BT_SEMRUSH:
		return CSTR("Semrush Bot");
	case BT_DALVIK:
		return CSTR("Dalvik");
	case BT_INDY:
		return CSTR("Indy Library");
	case BT_GOOGLEBOTS:
		return CSTR("GoogleBot (Smartphone)");
	case BT_ANDROIDWV:
		return CSTR("Android WebView");
	case BT_SAMSUNG:
		return CSTR("Samsung Browser");
	case BT_WESTWIND:
		return CSTR("West Wind Internet Protocols");
	case BT_YANDEX:
		return CSTR("Yandex Bot");
	case BT_BING:
		return CSTR("Bing Bot");
	case BT_MASSCAN:
		return CSTR("masscan");
	case BT_PYREQUESTS:
		return CSTR("Python Requests");
	case BT_ZGRAB:
		return CSTR("zgrab");
	case BT_EDGE:
		return CSTR("Edge");
	case BT_PYURLLIB:
		return CSTR("Python urllib");
	case BT_GOOGLEBOTD:
		return CSTR("GoogleBot (Desktop)");
	case BT_DOTNET:
		return CSTR(".NET Web Client");
	case BT_WINDIAG:
		return CSTR("Microsoft Windows Network Diagnostics");
	case BT_SSWR:
		return CSTR("SSWR");
	case BT_SMARTTV:
		return CSTR("SmartTV");
	case BT_BLEXBOT:
		return CSTR("BLEXBot");
	case BT_SOGOUPIC:
		return CSTR("Sogou Pic Spider");
	case BT_NUTCH:
		return CSTR("Apache Nutch");
	case BT_YISOU:
		return CSTR("Yisou Spider");
	case BT_WGET:
		return CSTR("Wget");
	case BT_SCRAPY:
		return CSTR("Scrapy");
	case BT_GOHTTP:
		return CSTR("Golang HTTP Client");
	case BT_WINHTTP:
		return CSTR("WinHTTP Client");
	case BT_NLPPROJECT:
		return CSTR("NLPProject");
	case BT_APACHEHTTP:
		return CSTR("Apache HTTP Client");
	case BT_BANNERDET:
		return CSTR("HTTP Banner Detection");
	case BT_NETCRAFTWEB:
		return CSTR("Netcraft Web Server Survey");
	case BT_NETCRAFTAG:
		return CSTR("Netcraft Survey Agent");
	case BT_AHREFSBOT:
		return CSTR("Ahrefs Bot");
	case BT_MJ12BOT:
		return CSTR("MJ12Bot");
	case BT_NETSYSRES:
		return CSTR("NetSystemsResearch");
	case BT_WHATSAPP:
		return CSTR("WhatsApp");
	case BT_CURL:
		return CSTR("Curl");
	case BT_GSA:
		return CSTR("Google Search App");
	case BT_FACEBOOK:
		return CSTR("Facebook External Hit");
	case BT_NETSEEN:
		return CSTR("NetSeen");
	case BT_MSNBOT:
		return CSTR("MSNBOT");
	case BT_LIBWWW_PERL:
		return CSTR("libwww-perl");
	case BT_UNKNOWN:
	default:
		return CSTR("Unknown");
	}
}

Text::CString Net::BrowserInfo::GetDefName(BrowserType btype)
{
	switch (btype)
	{
	case BT_IE:
		return CSTR("BT_IE");
	case BT_FIREFOX:
		return CSTR("BT_FIREFOX");
	case BT_CHROME:
		return CSTR("BT_CHROME");
	case BT_SAFARI:
		return CSTR("BT_SAFARI");
	case BT_UCBROWSER:
		return CSTR("BT_UCBROWSER");
	case BT_CFNETWORK:
		return CSTR("BT_CFNETWORK");
	case BT_SOGOUWEB:
		return CSTR("BT_SOGOUWEB");
	case BT_BAIDU:
		return CSTR("BT_BAIDU");
	case BT_SEMRUSH:
		return CSTR("BT_SEMRUSH");
	case BT_DALVIK:
		return CSTR("BT_DALVIK");
	case BT_INDY:
		return CSTR("BT_INDY");
	case BT_GOOGLEBOTS:
		return CSTR("BT_GOOGLEBOTS");
	case BT_ANDROIDWV:
		return CSTR("BT_ANDROIDWV");
	case BT_SAMSUNG:
		return CSTR("BT_SAMSUNG");
	case BT_WESTWIND:
		return CSTR("BT_WESTWIND");
	case BT_YANDEX:
		return CSTR("BT_YANDEX");
	case BT_BING:
		return CSTR("BT_BING");
	case BT_MASSCAN:
		return CSTR("BT_MASSCAN");
	case BT_PYREQUESTS:
		return CSTR("BT_PYREQUESTS");
	case BT_ZGRAB:
		return CSTR("BT_ZGRAB");
	case BT_EDGE:
		return CSTR("BT_EDGE");
	case BT_PYURLLIB:
		return CSTR("BT_PYURLLIB");
	case BT_GOOGLEBOTD:
		return CSTR("BT_GOOGLEBOTD");
	case BT_DOTNET:
		return CSTR("BT_DOTNET");
	case BT_WINDIAG:
		return CSTR("BT_WINDIAG");
	case BT_SSWR:
		return CSTR("BT_SSWR");
	case BT_SMARTTV:
		return CSTR("BT_SMARTTV");
	case BT_BLEXBOT:
		return CSTR("BT_BLEXBOT");
	case BT_SOGOUPIC:
		return CSTR("BT_SOGOUPIC");
	case BT_NUTCH:
		return CSTR("BT_NUTCH");
	case BT_YISOU:
		return CSTR("BT_YISOU");
	case BT_WGET:
		return CSTR("BT_WGET");
	case BT_SCRAPY:
		return CSTR("BT_SCRAPY");
	case BT_GOHTTP:
		return CSTR("BT_GOHTTP");
	case BT_WINHTTP:
		return CSTR("BT_WINHTTP");
	case BT_NLPPROJECT:
		return CSTR("BT_NLPPROJECT");
	case BT_APACHEHTTP:
		return CSTR("BT_APACHEHTTP");
	case BT_BANNERDET:
		return CSTR("BT_BANNERDET");
	case BT_NETCRAFTWEB:
		return CSTR("BT_NETCRAFTWEB");
	case BT_NETCRAFTAG:
		return CSTR("BT_NETCRAFTAG");
	case BT_AHREFSBOT:
		return CSTR("BT_AHREFSBOT");
	case BT_MJ12BOT:
		return CSTR("BT_MJ12BOT");
	case BT_NETSYSRES:
		return CSTR("BT_NETSYSRES");
	case BT_WHATSAPP:
		return CSTR("BT_WHATSAPP");
	case BT_CURL:
		return CSTR("BT_CURL");
	case BT_GSA:
		return CSTR("BT_GSA");
	case BT_FACEBOOK:
		return CSTR("BT_FACEBOOK");
	case BT_NETSEEN:
		return CSTR("BT_NETSEEN");
	case BT_MSNBOT:
		return CSTR("BT_NETSEEN");
	case BT_LIBWWW_PERL:
		return CSTR("BT_LIBWWW_PERL");
	case BT_UNKNOWN:
	default:
		return CSTR("BT_UNKNOWN");
	}
}
