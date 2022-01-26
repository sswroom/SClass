#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/BrowserInfo.h"

Text::CString Net::BrowserInfo::GetName(BrowserType btype)
{
	switch (btype)
	{
	case BT_IE:
		return {UTF8STRC("IE")};
	case BT_FIREFOX:
		return {UTF8STRC("Firefox")};
	case BT_CHROME:
		return {UTF8STRC("Chrome")};
	case BT_SAFARI:
		return {UTF8STRC("Safari")};
	case BT_UCBROWSER:
		return {UTF8STRC("UC Browser")};
	case BT_CFNETWORK:
		return {UTF8STRC("CFNetwork")};
	case BT_SOGOUWEB:
		return {UTF8STRC("Sogou Web Spider")};
	case BT_BAIDU:
		return {UTF8STRC("Baidu Spider")};
	case BT_SEMRUSH:
		return {UTF8STRC("Semrush Bot")};
	case BT_DALVIK:
		return {UTF8STRC("Dalvik")};
	case BT_INDY:
		return {UTF8STRC("Indy Library")};
	case BT_GOOGLEBOTS:
		return {UTF8STRC("GoogleBot (Smartphone)")};
	case BT_ANDROIDWV:
		return {UTF8STRC("Android WebView")};
	case BT_SAMSUNG:
		return {UTF8STRC("Samsung Browser")};
	case BT_WESTWIND:
		return {UTF8STRC("West Wind Internet Protocols")};
	case BT_YANDEX:
		return {UTF8STRC("Yandex Bot")};
	case BT_BING:
		return {UTF8STRC("Bing Bot")};
	case BT_MASSCAN:
		return {UTF8STRC("masscan")};
	case BT_PYREQUESTS:
		return {UTF8STRC("Python Requests")};
	case BT_ZGRAB:
		return {UTF8STRC("zgrab")};
	case BT_EDGE:
		return {UTF8STRC("Edge")};
	case BT_PYURLLIB:
		return {UTF8STRC("Python urllib")};
	case BT_GOOGLEBOTD:
		return {UTF8STRC("GoogleBot (Desktop)")};
	case BT_DOTNET:
		return {UTF8STRC(".NET Web Client")};
	case BT_WINDIAG:
		return {UTF8STRC("Microsoft Windows Network Diagnostics")};
	case BT_SSWR:
		return {UTF8STRC("SSWR")};
	case BT_SMARTTV:
		return {UTF8STRC("SmartTV")};
	case BT_BLEXBOT:
		return {UTF8STRC("BLEXBot")};
	case BT_SOGOUPIC:
		return {UTF8STRC("Sogou Pic Spider")};
	case BT_NUTCH:
		return {UTF8STRC("Apache Nutch")};
	case BT_YISOU:
		return {UTF8STRC("Yisou Spider")};
	case BT_WGET:
		return {UTF8STRC("Wget")};
	case BT_SCRAPY:
		return {UTF8STRC("Scrapy")};
	case BT_GOHTTP:
		return {UTF8STRC("Golang HTTP Client")};
	case BT_WINHTTP:
		return {UTF8STRC("WinHTTP Client")};
	case BT_NLPPROJECT:
		return {UTF8STRC("NLPProject")};
	case BT_APACHEHTTP:
		return {UTF8STRC("Apache HTTP Client")};
	case BT_BANNERDET:
		return {UTF8STRC("HTTP Banner Detection")};
	case BT_NETCRAFTWEB:
		return {UTF8STRC("Netcraft Web Server Survey")};
	case BT_NETCRAFTAG:
		return {UTF8STRC("Netcraft Survey Agent")};
	case BT_AHREFSBOT:
		return {UTF8STRC("Ahrefs Bot")};
	case BT_MJ12BOT:
		return {UTF8STRC("MJ12Bot")};
	case BT_NETSYSRES:
		return {UTF8STRC("NetSystemsResearch")};
	case BT_WHATSAPP:
		return {UTF8STRC("WhatsApp")};
	case BT_CURL:
		return {UTF8STRC("Curl")};
	case BT_GSA:
		return {UTF8STRC("Google Search App")};
	case BT_FACEBOOK:
		return {UTF8STRC("Facebook External Hit")};
	case BT_NETSEEN:
		return {UTF8STRC("NetSeen")};
	case BT_MSNBOT:
		return {UTF8STRC("MSNBOT")};
	case BT_LIBWWW_PERL:
		return {UTF8STRC("libwww-perl")};
	case BT_UNKNOWN:
	default:
		return {UTF8STRC("Unknown")};
	}
}

Text::CString Net::BrowserInfo::GetDefName(BrowserType btype)
{
	switch (btype)
	{
	case BT_IE:
		return {UTF8STRC("BT_IE")};
	case BT_FIREFOX:
		return {UTF8STRC("BT_FIREFOX")};
	case BT_CHROME:
		return {UTF8STRC("BT_CHROME")};
	case BT_SAFARI:
		return {UTF8STRC("BT_SAFARI")};
	case BT_UCBROWSER:
		return {UTF8STRC("BT_UCBROWSER")};
	case BT_CFNETWORK:
		return {UTF8STRC("BT_CFNETWORK")};
	case BT_SOGOUWEB:
		return {UTF8STRC("BT_SOGOUWEB")};
	case BT_BAIDU:
		return {UTF8STRC("BT_BAIDU")};
	case BT_SEMRUSH:
		return {UTF8STRC("BT_SEMRUSH")};
	case BT_DALVIK:
		return {UTF8STRC("BT_DALVIK")};
	case BT_INDY:
		return {UTF8STRC("BT_INDY")};
	case BT_GOOGLEBOTS:
		return {UTF8STRC("BT_GOOGLEBOTS")};
	case BT_ANDROIDWV:
		return {UTF8STRC("BT_ANDROIDWV")};
	case BT_SAMSUNG:
		return {UTF8STRC("BT_SAMSUNG")};
	case BT_WESTWIND:
		return {UTF8STRC("BT_WESTWIND")};
	case BT_YANDEX:
		return {UTF8STRC("BT_YANDEX")};
	case BT_BING:
		return {UTF8STRC("BT_BING")};
	case BT_MASSCAN:
		return {UTF8STRC("BT_MASSCAN")};
	case BT_PYREQUESTS:
		return {UTF8STRC("BT_PYREQUESTS")};
	case BT_ZGRAB:
		return {UTF8STRC("BT_ZGRAB")};
	case BT_EDGE:
		return {UTF8STRC("BT_EDGE")};
	case BT_PYURLLIB:
		return {UTF8STRC("BT_PYURLLIB")};
	case BT_GOOGLEBOTD:
		return {UTF8STRC("BT_GOOGLEBOTD")};
	case BT_DOTNET:
		return {UTF8STRC("BT_DOTNET")};
	case BT_WINDIAG:
		return {UTF8STRC("BT_WINDIAG")};
	case BT_SSWR:
		return {UTF8STRC("BT_SSWR")};
	case BT_SMARTTV:
		return {UTF8STRC("BT_SMARTTV")};
	case BT_BLEXBOT:
		return {UTF8STRC("BT_BLEXBOT")};
	case BT_SOGOUPIC:
		return {UTF8STRC("BT_SOGOUPIC")};
	case BT_NUTCH:
		return {UTF8STRC("BT_NUTCH")};
	case BT_YISOU:
		return {UTF8STRC("BT_YISOU")};
	case BT_WGET:
		return {UTF8STRC("BT_WGET")};
	case BT_SCRAPY:
		return {UTF8STRC("BT_SCRAPY")};
	case BT_GOHTTP:
		return {UTF8STRC("BT_GOHTTP")};
	case BT_WINHTTP:
		return {UTF8STRC("BT_WINHTTP")};
	case BT_NLPPROJECT:
		return {UTF8STRC("BT_NLPPROJECT")};
	case BT_APACHEHTTP:
		return {UTF8STRC("BT_APACHEHTTP")};
	case BT_BANNERDET:
		return {UTF8STRC("BT_BANNERDET")};
	case BT_NETCRAFTWEB:
		return {UTF8STRC("BT_NETCRAFTWEB")};
	case BT_NETCRAFTAG:
		return {UTF8STRC("BT_NETCRAFTAG")};
	case BT_AHREFSBOT:
		return {UTF8STRC("BT_AHREFSBOT")};
	case BT_MJ12BOT:
		return {UTF8STRC("BT_MJ12BOT")};
	case BT_NETSYSRES:
		return {UTF8STRC("BT_NETSYSRES")};
	case BT_WHATSAPP:
		return {UTF8STRC("BT_WHATSAPP")};
	case BT_CURL:
		return {UTF8STRC("BT_CURL")};
	case BT_GSA:
		return {UTF8STRC("BT_GSA")};
	case BT_FACEBOOK:
		return {UTF8STRC("BT_FACEBOOK")};
	case BT_NETSEEN:
		return {UTF8STRC("BT_NETSEEN")};
	case BT_MSNBOT:
		return {UTF8STRC("BT_NETSEEN")};
	case BT_LIBWWW_PERL:
		return {UTF8STRC("BT_LIBWWW_PERL")};
	case BT_UNKNOWN:
	default:
		return {UTF8STRC("BT_UNKNOWN")};
	}
}
