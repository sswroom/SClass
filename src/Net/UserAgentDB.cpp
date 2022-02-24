#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Net/UserAgentDB.h"

Net::UserAgentDB::UAEntry Net::UserAgentDB::uaList[] = {
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("\"Mozilla/5.0")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("537.36"),        Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.1"),       0, 0,                 UTF8STRC("\"Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) baidu.sogo.uc.Chrome/36.0.1985.125 Safari/537.36\"")},
	{Net::BrowserInfo::BT_NLPPROJECT, 0, 0,                      Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("\"nlpproject.info research\"")},
	{Net::BrowserInfo::BT_APACHEHTTP, UTF8STRC("4.5.2"),         Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Apache-HttpClient/4.5.2 (Java/1.8.0_151)")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Cloud mapping experiment. Contact research@pdrlabs.net")},
	{Net::BrowserInfo::BT_DALVIK,     UTF8STRC("2.1.0"),         Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-C9000"), UTF8STRC("Dalvik/2.1.0 (Linux; U; Android 8.0.0; SM-C9000 Build/R16NW)")},
	{Net::BrowserInfo::BT_DALVIK,     UTF8STRC("2.1.0"),         Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("MI 8"),     UTF8STRC("Dalvik/2.1.0 (Linux; U; Android 9; MI 8 MIUI/V11.0.6.0.PEAMIXM)")},
	{Net::BrowserInfo::BT_DALVIK,     UTF8STRC("2.1.0"),         Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-G975U"), UTF8STRC("Dalvik/2.1.0 (Linux; U; Android 9; SM-G975U Build/PPR1.180610.011)")},
	{Net::BrowserInfo::BT_DALVIK,     UTF8STRC("2.1.0"),         Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-G9550"), UTF8STRC("Dalvik/2.1.0 (Linux; U; Android 9; SM-G9550 Build/PPR1.180610.011)")},
	{Net::BrowserInfo::BT_DALVIK,     UTF8STRC("2.1.0"),         Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-G9730"), UTF8STRC("Dalvik/2.1.0 (Linux; U; Android 9; SM-G9730 Build/PPR1.180610.011)")},
	{Net::BrowserInfo::BT_DALVIK,     UTF8STRC("2.1.0"),         Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-G9750"), UTF8STRC("Dalvik/2.1.0 (Linux; U; Android 9; SM-G9750 Build/PPR1.180610.011)")},
	{Net::BrowserInfo::BT_DALVIK,     UTF8STRC("2.1.0"),         Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-N9500"), UTF8STRC("Dalvik/2.1.0 (Linux; U; Android 9; SM-N9500 Build/PPR1.180610.011)")},
	{Net::BrowserInfo::BT_DALVIK,     UTF8STRC("2.1.0"),         Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("SM-G9730"), UTF8STRC("Dalvik/2.1.0 (Linux; U; Android 10; SM-G9730 Build/QP1A.190711.020)")},
	{Net::BrowserInfo::BT_DALVIK,     UTF8STRC("2.1.0"),         Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("SM-G9750"), UTF8STRC("Dalvik/2.1.0 (Linux; U; Android 10; SM-G9750 Build/QP1A.190711.020)")},
	{Net::BrowserInfo::BT_GOHTTP,     UTF8STRC("1.1"),           Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Go-http-client/1.1")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Googlebot-Image/1.0")},
	{Net::BrowserInfo::BT_BANNERDET,  0, 0,                      Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("HTTP Banner Detection (https://security.ipip.net)")},
	{Net::BrowserInfo::BT_WINDIAG,    0, 0,                      Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Microsoft Windows Network Diagnostics")},
	{Net::BrowserInfo::BT_CFNETWORK,  UTF8STRC("902.2"),         Manage::OSInfo::OT_DARWIN,       UTF8STRC("17.7.0"),    0, 0,                 UTF8STRC("MobileSafari/604.1 CFNetwork/902.2 Darwin/17.7.0")},
	{Net::BrowserInfo::BT_CFNETWORK,  UTF8STRC("974.2.1"),       Manage::OSInfo::OT_DARWIN,       UTF8STRC("18.0.0"),    0, 0,                 UTF8STRC("MobileSafari/604.1 CFNetwork/974.2.1 Darwin/18.0.0")},
	{Net::BrowserInfo::BT_CFNETWORK,  UTF8STRC("978.0.7"),       Manage::OSInfo::OT_DARWIN,       UTF8STRC("18.7.0"),    0, 0,                 UTF8STRC("MobileSafari/604.1 CFNetwork/978.0.7 Darwin/18.7.0")},
	{Net::BrowserInfo::BT_CFNETWORK,  UTF8STRC("1107.1"),        Manage::OSInfo::OT_DARWIN,       UTF8STRC("19.0.0"),    0, 0,                 UTF8STRC("MobileSafari/604.1 CFNetwork/1107.1 Darwin/19.0.0")},
	{Net::BrowserInfo::BT_CFNETWORK,  UTF8STRC("1120"),          Manage::OSInfo::OT_DARWIN,       UTF8STRC("19.0.0"),    0, 0,                 UTF8STRC("MobileSafari/604.1 CFNetwork/1120 Darwin/19.0.0")},
	{Net::BrowserInfo::BT_CFNETWORK,  UTF8STRC("1121.2.2"),      Manage::OSInfo::OT_DARWIN,       UTF8STRC("19.2.0"),    0, 0,                 UTF8STRC("MobileSafari/604.1 CFNetwork/1121.2.2 Darwin/19.2.0")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Mozilla")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Mozilla/3.0 (compatible; Indy Library)")},
	{Net::BrowserInfo::BT_IE,         UTF8STRC("5.01"),          Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Mozilla/4.0 (compatible; MSIE 5.01; Windows NT)")},
	{Net::BrowserInfo::BT_DOTNET,     UTF8STRC("4.0.30319.42000"),Manage::OSInfo::OT_UNKNOWN,     0, 0,                  0, 0,                 UTF8STRC("Mozilla/4.0 (compatible; MSIE 6.0; MS Web Services Client Protocol 4.0.30319.42000)")},
	{Net::BrowserInfo::BT_IE,         UTF8STRC("6.0"),           Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("5.1"),       0, 0,                 UTF8STRC("Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)")},
	{Net::BrowserInfo::BT_IE,         UTF8STRC("7.0"),           Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("5.1"),       0, 0,                 UTF8STRC("Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 5.1; SE 2.X MetaSr 1.0; SE 2.X MetaSr 1.0; .NET CLR 2.0.50727; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729; .NET CLR 1.1.4322; CIBA;  InfoPath.2; SE 2.X MetaSr 1.0; AskTB5.6; SE 2.X MetaSr 1.0)")},
	{Net::BrowserInfo::BT_IE,         UTF8STRC("7.0"),           Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.0"),       0, 0,                 UTF8STRC("Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.0)")},
	{Net::BrowserInfo::BT_IE,         UTF8STRC("7.0"),           Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.1; WOW64; Trident/7.0; SLCC2; .NET CLR 2.0.50727; .NET CLR 3.5.30729; .NET CLR 3.0.30729; Media Center PC 6.0; .NET4.0C; .NET4.0E)")},
	{Net::BrowserInfo::BT_IE,         UTF8STRC("7.0"),           Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.1; WOW64; Trident/7.0; SLCC2; .NET CLR 2.0.50727; .NET CLR 3.5.30729; .NET CLR 3.0.30729; Media Center PC 6.0; .NET4.0C; .NET4.0E; ms-office)")},
	{Net::BrowserInfo::BT_IE,         UTF8STRC("7.0"),           Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.2"),       0, 0,                 UTF8STRC("Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.2; WOW64; Trident/7.0; .NET4.0C; .NET4.0E; .NET CLR 2.0.50727; .NET CLR 3.0.30729; .NET CLR 3.5.30729; InfoPath.3)")},
	{Net::BrowserInfo::BT_IE,         UTF8STRC("8.0"),           Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("5.1"),       0, 0,                 UTF8STRC("Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; SV1; .NET CLR 2.0.50727) CometHTTP")},
	{Net::BrowserInfo::BT_IE,         UTF8STRC("8.0"),           Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("5.1"),       0, 0,                 UTF8STRC("Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0)")},
	{Net::BrowserInfo::BT_IE,         UTF8STRC("8.0"),           Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("5.1"),       0, 0,                 UTF8STRC("Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET4.0C)")},
	{Net::BrowserInfo::BT_IE,         UTF8STRC("8.0"),           Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("5.2"),       0, 0,                 UTF8STRC("Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.2; Trident/4.0)")},
	{Net::BrowserInfo::BT_IE,         UTF8STRC("8.0"),           Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.0"),       0, 0,                 UTF8STRC("Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.0; Trident/4.0)")},
	{Net::BrowserInfo::BT_IE,         UTF8STRC("8.0"),           Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.1; WOW64; Trident/4.0)")},
	{Net::BrowserInfo::BT_IE,         UTF8STRC("8.0"),           Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.1; Win64; x64; Trident/4.0)")},
	{Net::BrowserInfo::BT_IE,         UTF8STRC("9.0"),           Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/4.0 (compatible; MSIE 9.0; Windows NT 6.1)")},
	{Net::BrowserInfo::BT_IE,         UTF8STRC("9.0"),           Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/4.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)")},
	{Net::BrowserInfo::BT_NETCRAFTWEB,0, 0,                      Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Mozilla/4.0 (compatible; Netcraft Web Server Survey)")},
	{Net::BrowserInfo::BT_WINHTTP,    UTF8STRC("5"),             Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Mozilla/4.0 (compatible; Win32; WinHttp.WinHttpRequest.5)")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Mozilla/4.0+(compatible;+MSIE+6.0;+Windows+NT+5.1)")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("67.0"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("5.1.1"),     UTF8STRC("rv:67.0"),  UTF8STRC("Mozilla/5.0 (Android 5.1.1; Mobile; rv:67.0) Gecko/67.0 Firefox/67.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("68.0"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("rv:68.0"),  UTF8STRC("Mozilla/5.0 (Android 8.1.0; Mobile; rv:68.0) Gecko/68.0 Firefox/68.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("68.0"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("rv:68.0"),  UTF8STRC("Mozilla/5.0 (Android 9; Mobile; rv:68.0) Gecko/68.0 Firefox/68.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("68.0"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("rv:68.0"),  UTF8STRC("Mozilla/5.0 (Android 10; Mobile; rv:68.0) Gecko/68.0 Firefox/68.0")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("38.0.1025.166"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("4.2.1"),     UTF8STRC("Nexus 5"),  UTF8STRC("Mozilla/5.0 (Linux; Android 4.2.1; en-us; Nexus 5 Build/JOP40D) AppleWebKit/535.19 (KHTML, like Gecko; googleweblight) Chrome/38.0.1025.166 Mobile Safari/535.19")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("74.0.3729.157"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("4.4.2"),     UTF8STRC("CHC-U01"),  UTF8STRC("Mozilla/5.0 (Linux; Android 4.4.2; CHC-U01) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.157 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("30.0.0.0"),      Manage::OSInfo::OT_ANDROID,      UTF8STRC("4.4.2"),     UTF8STRC("IM-A860K"), UTF8STRC("Mozilla/5.0 (Linux; Android 4.4.2; IM-A860K Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/30.0.0.0 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("30.0.0.0"),      Manage::OSInfo::OT_ANDROID,      UTF8STRC("4.4.2"),     UTF8STRC("Nexus 5"),  UTF8STRC("Mozilla/5.0 (Linux; Android 4.4.2; Nexus 5 Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/30.0.0.0 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("30.0.0.0"),      Manage::OSInfo::OT_ANDROID,      UTF8STRC("4.4.2"),     UTF8STRC("SM-N9005"), UTF8STRC("Mozilla/5.0 (Linux; Android 4.4.2; SM-N9005 Build/KOT49H) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/30.0.0.0 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("33.0.0.0"),      Manage::OSInfo::OT_ANDROID,      UTF8STRC("4.4.4"),     UTF8STRC("Coolpad Y82-520"),UTF8STRC("Mozilla/5.0 (Linux; Android 4.4.4; Coolpad Y82-520 Build/KTU84P) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/33.0.0.0 Mobile Safari/537.36 MicroMessenger/6.5.4.1000 NetType/4G Language/zh_CN")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("53.0.2785.49"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("4.4.4"),     UTF8STRC("ALE-CL00"), UTF8STRC("Mozilla/5.0 (Linux; Android 4.4.4; HUAWEI ALE-CL00 Build/HuaweiALE-CL00; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/53.0.2785.49 Mobile MQQBrowser/6.2 TBS/043024 Safari/537.36 MicroMessenger/6.5.4.1000 NetType/4G Language/zh_CN")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("5.0.2"),     UTF8STRC("LG-D802"),  UTF8STRC("Mozilla/5.0 (Linux; Android 5.0.2; LG-D802) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("77.0.3865.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("5.0"),       UTF8STRC("ASUS_Z00AD"),UTF8STRC("Mozilla/5.0 (Linux; Android 5.0; ASUS_Z00AD) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/77.0.3865.116 Mobile Safari/537.36 OPR/55.2.2719.50740")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("70.0.3538.110"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("5.0"),       UTF8STRC("E2303"),    UTF8STRC("Mozilla/5.0 (Linux; Android 5.0; E2303) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/70.0.3538.110 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("53.0.2785.49"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("5.1.1"),     UTF8STRC("R7Plusm"),  UTF8STRC("Mozilla/5.0 (Linux; Android 5.1.1; R7Plusm Build/LMY47V; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/53.0.2785.49 Mobile MQQBrowser/6.2 TBS/043220 Safari/537.36 MicroMessenger/6.5.7.1041 NetType/WIFI Language/zh_CN")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("69.0.3497.100"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("5.1.1"),     UTF8STRC("SM-J120M"), UTF8STRC("Mozilla/5.0 (Linux; Android 5.1.1; SM-J120M Build/LMY47X) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/69.0.3497.100 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("53.0.2785.49"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("5.1.1"),     UTF8STRC("SM801"),    UTF8STRC("Mozilla/5.0 (Linux; Android 5.1.1; SM801 Build/LMY47V; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/53.0.2785.49 Mobile MQQBrowser/6.2 TBS/043024 Safari/537.36 MicroMessenger/6.5.4.1000 NetType/4G Language/zh_CN")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("50.0.2661.94"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("6.0.1"),     UTF8STRC("CPH1611"),  UTF8STRC("Mozilla/5.0 (Linux; Android 6.0.1; CPH1611 Build/MMB29M) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/50.0.2661.94 Mobile Safari/537.36 OPR/37.0.2192.112031")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("6.0.1"),     UTF8STRC("LG-K535"),  UTF8STRC("Mozilla/5.0 (Linux; Android 6.0.1; LG-K535) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("54.0.2840.68"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("6.0.1"),     UTF8STRC("MiTV4-ANSM0"),UTF8STRC("Mozilla/5.0 (Linux; Android 6.0.1; MiTV4-ANSM0 Build/MHC19J) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/54.0.2840.68 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_GOOGLEBOTS, UTF8STRC("2.1"),           Manage::OSInfo::OT_ANDROID,      UTF8STRC("6.0.1"),     UTF8STRC("Nexus 5X"), UTF8STRC("Mozilla/5.0 (Linux; Android 6.0.1; Nexus 5X Build/MMB29P) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2272.96 Mobile Safari/537.36 (compatible; Googlebot/2.1; +http://www.google.com/bot.html)")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("37.0.0.0"),      Manage::OSInfo::OT_ANDROID,      UTF8STRC("6.0.1"),     UTF8STRC("OPPO R9s"), UTF8STRC("Mozilla/5.0 (Linux; Android 6.0.1; OPPO R9s Build/MMB29M) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/37.0.0.0 Mobile MQQBrowser/6.9 TBS/036906 Safari/537.36 MicroMessenger/6.5.4.1000 NetType/4G Language/zh_CN")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("10.2"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("6.0.1"),     UTF8STRC("SM-A9000"), UTF8STRC("Mozilla/5.0 (Linux; Android 6.0.1; SAMSUNG SM-A9000) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/10.2 Chrome/71.0.3578.99 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("4.2"),           Manage::OSInfo::OT_ANDROID,      UTF8STRC("6.0.1"),     UTF8STRC("SM-C7010"), UTF8STRC("Mozilla/5.0 (Linux; Android 6.0.1; SAMSUNG SM-C7010 Build/MMB29M) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/4.2 Chrome/44.0.2403.133 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("10.1"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("6.0.1"),     UTF8STRC("SM-J5108"), UTF8STRC("Mozilla/5.0 (Linux; Android 6.0.1; SAMSUNG SM-J5108) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/10.1 Chrome/71.0.3578.99 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("78.0.3904.108"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("6.0.1"),     UTF8STRC("SM-A9000"), UTF8STRC("Mozilla/5.0 (Linux; Android 6.0.1; SM-A9000 Build/MMB29M; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/78.0.3904.108 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("6.0.1"),     UTF8STRC("SM-A9000"), UTF8STRC("Mozilla/5.0 (Linux; Android 6.0.1; SM-A9000 Build/MMB29M; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("6.0.1"),     UTF8STRC("SM-A9000"), UTF8STRC("Mozilla/5.0 (Linux; Android 6.0.1; SM-A9000 Build/MMB29M; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("6.0.1"),     UTF8STRC("SM-A9000"), UTF8STRC("Mozilla/5.0 (Linux; Android 6.0.1; SM-A9000 Build/MMB29M; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("6.0.1"),     UTF8STRC("SM-A9000"), UTF8STRC("Mozilla/5.0 (Linux; Android 6.0.1; SM-A9000) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("6.0.1"),     UTF8STRC("SM-A9000"), UTF8STRC("Mozilla/5.0 (Linux; Android 6.0.1; SM-A9000) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("57.0.2987.132"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("6.0.1"),     UTF8STRC("SM-C7010"), UTF8STRC("Mozilla/5.0 (Linux; Android 6.0.1; SM-C7010 Build/MMB29M; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/57.0.2987.132 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("6.0.1"),     UTF8STRC("SM-G610Y"), UTF8STRC("Mozilla/5.0 (Linux; Android 6.0.1; SM-G610Y) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("52.0.2743.98"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("6.0.1"),     UTF8STRC("SM-G920V"), UTF8STRC("Mozilla/5.0 (Linux; Android 6.0.1; SM-G920V Build/MMB29K) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/52.0.2743.98 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("6.0.1"),     UTF8STRC("SM-G6100"), UTF8STRC("Mozilla/5.0 (Linux; Android 6.0.1; SM-G6100) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("55.0.2883.91"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("6.0.1"),     UTF8STRC("SM-N910U"), UTF8STRC("Mozilla/5.0 (Linux; Android 6.0.1; SM-N910U Build/MMB29K; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/55.0.2883.91 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("6.0.1"),     UTF8STRC("ZTE BLADE A512"),UTF8STRC("Mozilla/5.0 (Linux; Android 6.0.1; ZTE BLADE A512) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("48.0.2542.0"),   Manage::OSInfo::OT_ANDROID,      UTF8STRC("6.0"),       UTF8STRC("8S89H_E3"), UTF8STRC("Mozilla/5.0 (Linux; Android 6.0; 8S89H_E3 Build/MRA58K; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/48.0.2542.0 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("6.0"),       UTF8STRC("HTC One X9 dual sim"),UTF8STRC("Mozilla/5.0 (Linux; Android 6.0; HTC One X9 dual sim) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("6.0"),       UTF8STRC("HTC_U-2u"), UTF8STRC("Mozilla/5.0 (Linux; Android 6.0; HTC_U-2u) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("48.0.2542.0"),   Manage::OSInfo::OT_ANDROID,      UTF8STRC("6.0"),       UTF8STRC("MStar Android TV"),UTF8STRC("Mozilla/5.0 (Linux; Android 6.0; MStar Android TV Build/MRA58K; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/48.0.2542.0 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.117"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("6.0"),       UTF8STRC("Nexus 5"),  UTF8STRC("Mozilla/5.0 (Linux; Android 6.0; Nexus 5 Build/MRA58N) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.117 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("59.0.3071.125"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.0"),       UTF8STRC("A75B"),     UTF8STRC("Mozilla/5.0 (Linux; Android 7.0; A75B Build/NRD90M; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/59.0.3071.125 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.0"),       UTF8STRC("ASUS_X018D"),UTF8STRC("Mozilla/5.0 (Linux; Android 7.0; ASUS_X018D) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("63.0.3239.111"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.0"),       UTF8STRC("ASUS_Z012DA"),UTF8STRC("Mozilla/5.0 (Linux; Android 7.0; ASUS_Z012DA Build/NRD90M) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.111 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.0"),       UTF8STRC("EVA-L19"),  UTF8STRC("Mozilla/5.0 (Linux; Android 7.0; EVA-L19) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("53.0.2785.49"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.0"),       UTF8STRC("FRD-AL00"), UTF8STRC("Mozilla/5.0 (Linux; Android 7.0; FRD-AL00 Build/HUAWEIFRD-AL00; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/53.0.2785.49 Mobile MQQBrowser/6.2 TBS/043602 Safari/537.36 MicroMessenger/6.5.16.1120 NetType/WIFI Language/zh_CN")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.0"),       UTF8STRC("HTC 10 evo"),UTF8STRC("Mozilla/5.0 (Linux; Android 7.0; HTC 10 evo) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("78.0.3904.108"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.0"),       UTF8STRC("MLA-AL10"), UTF8STRC("Mozilla/5.0 (Linux; Android 7.0; HUAWEI MLA-AL10 Build/HUAWEIMLA-AL10; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/78.0.3904.108 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.0"),       UTF8STRC("MLA-AL10"), UTF8STRC("Mozilla/5.0 (Linux; Android 7.0; HUAWEI MLA-AL10) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.0"),       UTF8STRC("MLA-AL10"), UTF8STRC("Mozilla/5.0 (Linux; Android 7.0; HUAWEI MLA-AL10) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.0"),       UTF8STRC("VNS-L31"),  UTF8STRC("Mozilla/5.0 (Linux; Android 7.0; HUAWEI VNS-L31) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("66.0.3359.126"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.0"),       UTF8STRC("JMM-AL10"), UTF8STRC("Mozilla/5.0 (Linux; Android 7.0; JMM-AL10 Build/HONORJMM-AL10; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/66.0.3359.126 MQQBrowser/6.2 TBS/045018 Mobile Safari/537.36 MMWEBID/4954 MicroMessenger/7.0.3.1400(0x27000334) Process/tools NetType/WIFI Language/zh_CN")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("66.0.3359.126"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.0"),       UTF8STRC("JMM-AL10"), UTF8STRC("Mozilla/5.0 (Linux; Android 7.0; JMM-AL10 Build/HONORJMM-AL10; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/66.0.3359.126 MQQBrowser/6.2 TBS/045114 Mobile Safari/537.36 V1_AND_SQ_7.3.0_758_YYB_D QQ/7.3.0.3340 NetType/WIFI WebP/0.3.0 Pixel/720")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("73.0.3683.90"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.0"),       UTF8STRC("LG-H860"),  UTF8STRC("Mozilla/5.0 (Linux; Android 7.0; LG-H860 Build/NRD90U; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/73.0.3683.90 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.0"),       UTF8STRC("LG-H860"),  UTF8STRC("Mozilla/5.0 (Linux; Android 7.0; LG-H860 Build/NRD90U; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.0"),       UTF8STRC("LG-H860"),  UTF8STRC("Mozilla/5.0 (Linux; Android 7.0; LG-H860 Build/NRD90U; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.0"),       UTF8STRC("LG-H860"),  UTF8STRC("Mozilla/5.0 (Linux; Android 7.0; LG-H860 Build/NRD90U; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.0"),       UTF8STRC("LG-H961N"), UTF8STRC("Mozilla/5.0 (Linux; Android 7.0; LG-H961N Build/NRD90U; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.0"),       UTF8STRC("LG-H961N"), UTF8STRC("Mozilla/5.0 (Linux; Android 7.0; LG-H961N Build/NRD90U; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.0"),       UTF8STRC("LG-H961N"), UTF8STRC("Mozilla/5.0 (Linux; Android 7.0; LG-H961N Build/NRD90U; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("74.0.3729.157"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.0"),       UTF8STRC("LG-H961N"), UTF8STRC("Mozilla/5.0 (Linux; Android 7.0; LG-H961N) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.157 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.0"),       UTF8STRC("LG-H990"),  UTF8STRC("Mozilla/5.0 (Linux; Android 7.0; LG-H990) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.0"),       UTF8STRC("LG-K535"),  UTF8STRC("Mozilla/5.0 (Linux; Android 7.0; LG-K535) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.0"),       UTF8STRC("Redmi Note 4"),UTF8STRC("Mozilla/5.0 (Linux; Android 7.0; Redmi Note 4) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("10.2"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.0"),       UTF8STRC("SM-G9287"), UTF8STRC("Mozilla/5.0 (Linux; Android 7.0; SAMSUNG SM-G9287) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/10.2 Chrome/71.0.3578.99 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("68.0.3440.91"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.0"),       UTF8STRC("SM-N9200"), UTF8STRC("Mozilla/5.0 (Linux; Android 7.0; SM-N9200 Build/NRD90M; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/68.0.3440.91 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.0"),       UTF8STRC("SM-T813"),  UTF8STRC("Mozilla/5.0 (Linux; Android 7.0; SM-T813) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.0"),       UTF8STRC("TRT-TL10"), UTF8STRC("Mozilla/5.0 (Linux; Android 7.0; TRT-TL10) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("72.0.3626.105"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.0"),       UTF8STRC("VIE-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 7.0; VIE-L29) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/72.0.3626.105 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.1.1"),     UTF8STRC("E6633"),    UTF8STRC("Mozilla/5.0 (Linux; Android 7.1.1; E6633) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("66.0.3359.126"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.1.1"),     UTF8STRC("GIONEE S10CL"),UTF8STRC("Mozilla/5.0 (Linux; Android 7.1.1; GIONEE S10CL Build/NMF26F; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/66.0.3359.126 MQQBrowser/6.2 TBS/45016 Mobile Safari/537.36 MMWEBID/5391 MicroMessenger/7.0.8.1540(0x27000833) Process/tools NetType/WIFI Language/en ABI/arm64")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.1.1"),     UTF8STRC("LG-M700"),  UTF8STRC("Mozilla/5.0 (Linux; Android 7.1.1; LG-M700 Build/NMF26X; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("78.0.3904.96"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.1.1"),     UTF8STRC("MI MAX 2"), UTF8STRC("Mozilla/5.0 (Linux; Android 7.1.1; MI MAX 2 Build/NMF26F; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/78.0.3904.96 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.79"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.1.1"),     UTF8STRC("MI MAX 2"), UTF8STRC("Mozilla/5.0 (Linux; Android 7.1.1; MI MAX 2 Build/NMF26F; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.79 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.1.1"),     UTF8STRC("MI MAX 2"), UTF8STRC("Mozilla/5.0 (Linux; Android 7.1.1; MI MAX 2 Build/NMF26F; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.1.1"),     UTF8STRC("MI MAX 2"), UTF8STRC("Mozilla/5.0 (Linux; Android 7.1.1; MI MAX 2 Build/NMF26F; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.1.1"),     UTF8STRC("MI MAX 2"), UTF8STRC("Mozilla/5.0 (Linux; Android 7.1.1; MI MAX 2 Build/NMF26F; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("56.0.2924.87"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.1.1"),     UTF8STRC("Moto E (4"),UTF8STRC("Mozilla/5.0 (Linux; Android 7.1.1; Moto E (4) Plus Build/NMA26.42-152) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/56.0.2924.87 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("10.1"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.1.1"),     UTF8STRC("SM-C9000"), UTF8STRC("Mozilla/5.0 (Linux; Android 7.1.1; SAMSUNG SM-C9000) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/10.1 Chrome/71.0.3578.99 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.1.2"),     UTF8STRC("Redmi Note 5A"),UTF8STRC("Mozilla/5.0 (Linux; Android 7.1.2; Redmi Note 5A Build/N2G47H; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.1.2"),     UTF8STRC("Redmi Note 5A"),UTF8STRC("Mozilla/5.0 (Linux; Android 7.1.2; Redmi Note 5A Build/N2G47H; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.1.2"),     UTF8STRC("Redmi Note 5A"),UTF8STRC("Mozilla/5.0 (Linux; Android 7.1.2; Redmi Note 5A Build/N2G47H; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("ASUS_Z012DA"),UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; ASUS_Z012DA) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("G3116"),    UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; G3116) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("G3116"),    UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; G3116) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("G8232"),    UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; G8232) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("4.0"),           Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("LDN-AL00"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; LDN-AL00 Build/HUAWEILDN-AL00) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("LDN-LX2"),  UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; LDN-LX2 Build/HUAWEILDN-LX2; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("LDN-LX2"),  UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; LDN-LX2 Build/HUAWEILDN-LX2; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("LDN-LX2"),  UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; LDN-LX2 Build/HUAWEILDN-LX2; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("80.0.3987.87"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("LDN-LX2"),  UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; LDN-LX2 Build/HUAWEILDN-LX2; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/80.0.3987.87 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("LG-H870DS"),UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; LG-H870DS Build/OPR1.170623.032; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36 MMWEBID/7764 MicroMessenger/7.0.9.1545(0x27000943) Process/tools NetType/4G Language/zh_HK ABI/arm64")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("LG-H870DS"),UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; LG-H870DS Build/OPR1.170623.032; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36 MMWEBID/7764 MicroMessenger/7.0.9.1545(0x27000943) Process/tools NetType/WIFI Language/zh_HK ABI/arm64")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("LG-H870DS"),UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; LG-H870DS Build/OPR1.170623.032; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36 MMWEBID/7764 MicroMessenger/7.0.9.1545(0x27000943) Process/tools NetType/4G Language/zh_HK ABI/arm64")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("LG-H870DS"),UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; LG-H870DS Build/OPR1.170623.032; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36 MMWEBID/7764 MicroMessenger/7.0.9.1545(0x27000943) Process/tools NetType/WIFI Language/zh_HK ABI/arm64")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("LG-H870DS"),UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; LG-H870DS Build/OPR1.170623.032; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36 MMWEBID/7764 MicroMessenger/7.0.10.1561(0x27000A41) Process/tools NetType/4G Language/zh_HK ABI/arm64 GPVersion/1")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("LG-H870DS"),UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; LG-H870DS Build/OPR1.170623.032; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36 MMWEBID/7764 MicroMessenger/7.0.10.1561(0x27000A41) Process/tools NetType/4G Language/zh_HK ABI/arm64 GPVersion/1")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("LG-H870DS"),UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; LG-H870DS Build/OPR1.170623.032; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36 MMWEBID/7764 MicroMessenger/7.0.10.1561(0x27000A41) Process/tools NetType/WIFI Language/zh_HK ABI/arm64 GPVersion/1")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("LG-H870DS"),UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; LG-H870DS) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("66.0.3359.126"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("LG-H930"),  UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; LG-H930 Build/OPR1.170623.026; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/66.0.3359.126 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("LG-H930"),  UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; LG-H930 Build/OPR1.170623.026; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("LG-H930"),  UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; LG-H930 Build/OPR1.170623.026; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("LG-H930"),  UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; LG-H930) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("LG-H990"),  UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; LG-H990 Build/OPR1.170623.032; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("78.0.3904.108"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("LM-G710"),  UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; LM-G710 Build/OPR1.170623.032; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/78.0.3904.108 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("MI 5"),     UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; MI 5) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("MI 5"),     UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; MI 5) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("MI 5"),     UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; MI 5) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("77.0.9805.620"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("Pixel 2 XL"),UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; Pixel 2 XL Build/OPD1.170816.004) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/77.0.9805.620 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("RNE-L22"),  UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; RNE-L22) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("S41"),      UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; S41) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("9.4"),           Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-A9200"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SAMSUNG SM-A9200 Build/R16NW) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/9.4 Chrome/67.0.3396.87 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("10.2"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-C9000"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SAMSUNG SM-C9000) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/10.2 Chrome/71.0.3578.99 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("10.1"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-G9350"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SAMSUNG SM-G9350) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/10.1 Chrome/71.0.3578.99 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("10.2"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-G9550"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SAMSUNG SM-G9550) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/10.2 Chrome/71.0.3578.99 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-A520F"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-A520F) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-A520F"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-A520F) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-A730F"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-A730F Build/R16NW; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-A730F"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-A730F Build/R16NW; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-C5010"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-C5010 Build/R16NW; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-C5010"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-C5010 Build/R16NW; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-C5010"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-C5010 Build/R16NW; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-C5010"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-C5010) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-C7000"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-C7000 Build/R16NW; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-C7000"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-C7000 Build/R16NW; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-C7000"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-C7000 Build/R16NW; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("78.0.3904.62"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-C7000"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-C7000) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.62 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-C7000"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-C7000) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-C7010"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-C7010 Build/R16NW; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-C7010"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-C7010 Build/R16NW; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-C7010"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-C7010 Build/R16NW; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("70.0.3538.110"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-C9000"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-C9000 Build/R16NW; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/70.0.3538.110 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("78.0.3904.108"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-C9000"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-C9000 Build/R16NW; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/78.0.3904.108 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-C9000"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-C9000 Build/R16NW; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-C9000"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-C9000 Build/R16NW; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-C9000"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-C9000 Build/R16NW; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-C9000"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-C9000) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-C9000"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-C9000) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-C9000"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-C9000) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("69.0.3497.100"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-G930F"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-G930F Build/R16NW) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/69.0.3497.100 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-G6100"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-G6100) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-G6100"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-G6100) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("78.0.3904.108"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-G9350"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-G9350 Build/R16NW; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/78.0.3904.108 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-G9350"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-G9350 Build/R16NW; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-G9350"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-G9350 Build/R16NW; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-G9350"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-G9350 Build/R16NW; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-G9550"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-G9550 Build/R16NW; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-G9550"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-G9550 Build/R16NW; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-G9550"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-G9550 Build/R16NW; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("78.0.3904.108"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-G9550"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-G9550) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.108 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-G9550"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-G9550) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-G9550"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-G9550) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-G9550"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-G9550) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-J600G"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-J600G Build/R16NW; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-N9500"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-N9500 Build/R16NW; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-N9500"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-N9500 Build/R16NW; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("SM-N9500"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; SM-N9500 Build/R16NW; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("VKY-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; VKY-L29) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("VKY-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; VKY-L29) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0.0"),     UTF8STRC("WAS-TL10"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.0.0; WAS-TL10) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("16th"),     UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; 16th Build/OPM1.171019.026; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36 [FB_IAB/FB4A;FBAV/251.0.0.31.111;]")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("ASUS_X00RD"),UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; ASUS_X00RD Build/OPM1.171019.019) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("CPH1909"),  UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; CPH1909) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("HTC U12 life"),UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; HTC U12 life) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("69.0.3497.100"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("MI MAX 3"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; MI MAX 3 Build/OPM1.171019.019) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/69.0.3497.100 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("MI MAX 3"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; MI MAX 3 Build/OPM1.171019.019; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("66.0.3359.126"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("OPPO R11"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; OPPO R11 Build/OPM1.171019.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/66.0.3359.126 MQQBrowser/6.2 TBS/45016 Mobile Safari/537.36 MMWEBID/6285 MicroMessenger/7.0.9.1560(0x27000935) Process/tools NetType/4G Language/zh_CN ABI/arm64")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("66.0.3359.126"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("OPPO R11"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; OPPO R11 Build/OPM1.171019.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/66.0.3359.126 MQQBrowser/6.2 TBS/045033 Mobile Safari/537.36 MMWEBID/6285 MicroMessenger/7.0.10.1580(0x27000A54) Process/tools NetType/4G Language/zh_CN ABI/arm64")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("66.0.3359.126"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("OPPO R11"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; OPPO R11 Build/OPM1.171019.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/66.0.3359.126 MQQBrowser/6.2 TBS/045033 Mobile Safari/537.36 MMWEBID/6285 MicroMessenger/7.0.10.1580(0x27000A54) Process/tools NetType/WIFI Language/zh_CN ABI/arm64")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("71.0.3578.99"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("Redmi 5"),  UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; Redmi 5 Build/OPM1.171019.026; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/71.0.3578.99 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("Redmi 5 Plus"),UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; Redmi 5 Plus Build/OPM1.171019.019; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("Redmi 5 Plus"),UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; Redmi 5 Plus Build/OPM1.171019.019; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("Redmi 5 Plus"),UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; Redmi 5 Plus Build/OPM1.171019.019; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("80.0.3987.87"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("Redmi 5 Plus"),UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; Redmi 5 Plus Build/OPM1.171019.019; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/80.0.3987.87 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("78.0.3904.108"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("Redmi 5"),  UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; Redmi 5) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.108 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("7.2"),           Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("SM-C7100"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; SAMSUNG SM-C7100 Build/M1AJQ) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/7.2 Chrome/59.0.3071.125 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("10.2"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("SM-J415GN"),UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; SAMSUNG SM-J415GN) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/10.2 Chrome/71.0.3578.99 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("10.2"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("SM-J710GN"),UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; SAMSUNG SM-J710GN) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/10.2 Chrome/71.0.3578.99 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("9.2"),           Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("SM-J730GM"),UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; SAMSUNG SM-J730GM Build/M1AJQ) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/9.2 Chrome/67.0.3396.87 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("SM-C7100"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; SM-C7100 Build/M1AJQ; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("SM-C7100"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; SM-C7100 Build/M1AJQ; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("SM-C7100"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; SM-C7100 Build/M1AJQ; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("78.0.3904.108"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("SM-G610F"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; SM-G610F) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.108 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("68.0.3440.91"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("SM-J710GN"),UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; SM-J710GN Build/M1AJQ) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/68.0.3440.91 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("SM-N9600"), UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; SM-N9600 Build/M1AJQ; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("SM-T580"),  UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; SM-T580) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("SUGAR Y12s"),UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; SUGAR Y12s Build/O11019; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("SUGAR Y12s"),UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; SUGAR Y12s Build/O11019; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("SUGAR Y12s"),UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; SUGAR Y12s Build/O11019; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("70.0.3538.110"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("vivo 1801"),UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; vivo 1801 Build/OPM1.171019.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/70.0.3538.110 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("70.0.3538.110"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("vivo 1807"),UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; vivo 1807 Build/OPM1.171019.026; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/70.0.3538.110 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("76.0.3809.132"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("vivo 1811"),UTF8STRC("Mozilla/5.0 (Linux; Android 8.1.0; vivo 1811 Build/OPM1.171019.026; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/76.0.3809.132 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("ALP-AL00"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; ALP-AL00) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("ALP-AL00"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; ALP-AL00) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("ALP-AL00"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; ALP-AL00) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("75.0.3770.101"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("ALP-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; ALP-L29) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/75.0.3770.101 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("ALP-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; ALP-L29) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("78.0.3904.108"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("ANE-LX2"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; ANE-LX2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.108 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("ANE-LX2"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; ANE-LX2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("ANE-LX2"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; ANE-LX2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("ANE-LX2"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; ANE-LX2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("76.0.3809.89"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("ASK-AL00x"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; ASK-AL00x Build/HONORASK-AL00x; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/76.0.3809.89 Mobile Safari/537.36 T7/11.18 SP-engine/2.14.0 baiduboxapp/11.18.0.12 (Baidu; P1 9)")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("ASUS_I001DC"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; ASUS_I001DC Build/PKQ1.190414.001; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("ASUS_I001DC"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; ASUS_I001DC Build/PKQ1.190414.001; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("ASUS_I001DC"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; ASUS_I001DC Build/PKQ1.190414.001; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("ASUS_X00QD"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; ASUS_X00QD) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("CLT-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; CLT-L29 Build/HUAWEICLT-L29; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("CLT-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; CLT-L29 Build/HUAWEICLT-L29; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("CLT-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; CLT-L29 Build/HUAWEICLT-L29; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("CLT-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; CLT-L29) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("CPH1941"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; CPH1941) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("CPH1969"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; CPH1969) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("ELE-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; ELE-L29) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("EML-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; EML-L29 Build/HUAWEIEML-L29; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("EML-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; EML-L29 Build/HUAWEIEML-L29; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("EML-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; EML-L29 Build/HUAWEIEML-L29; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("74.0.3729.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("EVR-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; EVR-L29) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("FIG-LX2"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; FIG-LX2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("75.0.3770.143"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("FLA-LX2"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; FLA-LX2 Build/HUAWEIFLA-LX2; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/75.0.3770.143 Mobile Safari/537.36 SP-engine/2.14.0 baiduboxapp/11.18.0.8 (Baidu; P1 9)")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("76.0.3809.89"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("FLA-LX2"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; FLA-LX2 Build/HUAWEIFLA-LX2; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/76.0.3809.89 Mobile Safari/537.36 T7/11.18 SP-engine/2.14.0 baiduboxapp/11.18.0.8 (Baidu; P1 9)")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("G8142"),    UTF8STRC("Mozilla/5.0 (Linux; Android 9; G8142 Build/47.2.A.10.107; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("G8142"),    UTF8STRC("Mozilla/5.0 (Linux; Android 9; G8142 Build/47.2.A.10.107; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("G8142"),    UTF8STRC("Mozilla/5.0 (Linux; Android 9; G8142) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("G8142"),    UTF8STRC("Mozilla/5.0 (Linux; Android 9; G8142) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("G8142"),    UTF8STRC("Mozilla/5.0 (Linux; Android 9; G8142) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("66.0.3359.158"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("GCE x86 phone"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; GCE x86 phone Build/PGR1.180815.001; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/66.0.3359.158 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("H4133"),    UTF8STRC("Mozilla/5.0 (Linux; Android 9; H4133) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("H4233"),    UTF8STRC("Mozilla/5.0 (Linux; Android 9; H4233 Build/50.2.A.0.400; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("H4233"),    UTF8STRC("Mozilla/5.0 (Linux; Android 9; H4233 Build/50.2.A.0.400; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("H4233"),    UTF8STRC("Mozilla/5.0 (Linux; Android 9; H4233 Build/50.2.A.3.22; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("H4233"),    UTF8STRC("Mozilla/5.0 (Linux; Android 9; H4233 Build/50.2.A.3.22; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("78.0.3904.108"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("HMA-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; HMA-L29 Build/HUAWEIHMA-L29; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/78.0.3904.108 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("HMA-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; HMA-L29 Build/HUAWEIHMA-L29; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("HMA-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; HMA-L29 Build/HUAWEIHMA-L29; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("78.0.3904.108"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("HMA-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; HMA-L29) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.108 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("HMA-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; HMA-L29) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("HMA-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; HMA-L29) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("HTC 2Q7A100"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; HTC 2Q7A100) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("HTC_U-3u"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; HTC_U-3u) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("77.0.3865.92"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("INE-LX2"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; INE-LX2 Build/HUAWEIINE-LX2; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/77.0.3865.92 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("INE-LX2"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; INE-LX2 Build/HUAWEIINE-LX2; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("INE-LX2"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; INE-LX2 Build/HUAWEIINE-LX2; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("INE-LX2"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; INE-LX2 Build/HUAWEIINE-LX2; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("INE-LX2"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; INE-LX2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("INE-LX2"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; INE-LX2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("LG-H870DS"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; LG-H870DS) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("LG-H870DS"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; LG-H870DS) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("LG-H870DS"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; LG-H870DS) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("LYA-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; LYA-L29 Build/HUAWEILYA-L29; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("LYA-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; LYA-L29 Build/HUAWEILYA-L29; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("78.0.3904.108"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("MAR-LX2"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; MAR-LX2 Build/HUAWEIMAR-L22B; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/78.0.3904.108 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("MAR-LX2"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; MAR-LX2 Build/HUAWEIMAR-L22B; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("MAR-LX2"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; MAR-LX2 Build/HUAWEIMAR-L22B; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("MAR-LX2"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; MAR-LX2 Build/HUAWEIMAR-L22B; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("MHA-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; MHA-L29) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("MI 8"),     UTF8STRC("Mozilla/5.0 (Linux; Android 9; MI 8 Build/PKQ1.180729.001; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("MI 8"),     UTF8STRC("Mozilla/5.0 (Linux; Android 9; MI 8 Build/PKQ1.180729.001; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("MI 8"),     UTF8STRC("Mozilla/5.0 (Linux; Android 9; MI 8 Build/PKQ1.180729.001; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("10.2"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("MI 8"),     UTF8STRC("Mozilla/5.0 (Linux; Android 9; MI 8) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/10.2 Chrome/71.0.3578.99 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("MI MAX 3"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; MI MAX 3 Build/PKQ1.181007.001; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("MI MAX 3"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; MI MAX 3 Build/PKQ1.181007.001; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("MI MAX 3"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; MI MAX 3 Build/PKQ1.181007.001; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("Mi 9T"),    UTF8STRC("Mozilla/5.0 (Linux; Android 9; Mi 9T) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("Mi A1"),    UTF8STRC("Mozilla/5.0 (Linux; Android 9; Mi A1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("74.0.3729.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("Mi A3"),    UTF8STRC("Mozilla/5.0 (Linux; Android 9; Mi A3) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("Mi MIX 2"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; Mi MIX 2 Build/PKQ1.190118.001; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("Mi MIX 2"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; Mi MIX 2 Build/PKQ1.190118.001; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("Nokia 7 plus"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; Nokia 7 plus) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("78.0.3904.108"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("ONEPLUS A5000"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; ONEPLUS A5000 Build/PKQ1.180716.001; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/78.0.3904.108 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("78.0.3904.108"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("ONEPLUS A5010"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; ONEPLUS A5010 Build/PKQ1.180716.001; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/78.0.3904.108 Mobile Safari/537.36 [FB_IAB/FB4A;FBAV/251.0.0.31.111;]")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("78.0.3904.108"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("ONEPLUS A5010"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; ONEPLUS A5010) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.108 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("ONEPLUS A6010"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; ONEPLUS A6010) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("PAR-TL20"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; PAR-TL20 Build/HUAWEIPAR-TL20; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("PAR-TL20"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; PAR-TL20 Build/HUAWEIPAR-TL20; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("PAR-TL20"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; PAR-TL20 Build/HUAWEIPAR-TL20; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("PAR-TL20"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; PAR-TL20) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("PAR-TL20"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; PAR-TL20) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("POCOPHONE F1"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; POCOPHONE F1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("RVL-AL09"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; RVL-AL09) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("RVL-AL09"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; RVL-AL09) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("Redmi Note 5"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; Redmi Note 5) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("Redmi Note 7"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; Redmi Note 7 Build/PKQ1.180904.001; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("Redmi Note 7"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; Redmi Note 7 Build/PKQ1.180904.001; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("Redmi Note 7"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; Redmi Note 7 Build/PKQ1.180904.001; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("80.0.3987.87"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("Redmi Note 7"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; Redmi Note 7 Build/PKQ1.180904.001; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/80.0.3987.87 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("Redmi Note 8 Pro"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; Redmi Note 8 Pro Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("Redmi Note 8 Pro"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; Redmi Note 8 Pro) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("78.0.3904.108"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("Redmi Note 8T"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; Redmi Note 8T) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.108 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("10.2"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A202F"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SAMSUNG SM-A202F) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/10.2 Chrome/71.0.3578.99 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("10.2"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A5070"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SAMSUNG SM-A5070) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/10.2 Chrome/71.0.3578.99 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("10.2"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A6060"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SAMSUNG SM-A6060) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/10.2 Chrome/71.0.3578.99 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("9.4"),           Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A7050"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SAMSUNG SM-A7050 Build/PPR1.180610.011) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/9.4 Chrome/67.0.3396.87 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("10.2"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A7050"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SAMSUNG SM-A7050) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/10.2 Chrome/71.0.3578.99 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("10.2"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A8050"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SAMSUNG SM-A8050) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/10.2 Chrome/71.0.3578.99 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("9.0"),           Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-G975U"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SAMSUNG SM-G975U Build/PPR1.180610.011) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/9.0 Chrome/67.0.3396.87 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("10.2"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-G975U"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SAMSUNG SM-G975U) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/10.2 Chrome/71.0.3578.99 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("10.2"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-G9550"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SAMSUNG SM-G9550) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/10.2 Chrome/71.0.3578.99 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("10.2"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-G9600"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SAMSUNG SM-G9600) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/10.2 Chrome/71.0.3578.99 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("10.2"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-G9730"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SAMSUNG SM-G9730) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/10.2 Chrome/71.0.3578.99 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("9.4"),           Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-G9750"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SAMSUNG SM-G9750 Build/PPR1.180610.011) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/9.4 Chrome/67.0.3396.87 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("10.2"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-G9750"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SAMSUNG SM-G9750) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/10.2 Chrome/71.0.3578.99 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("10.2"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-N960F"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SAMSUNG SM-N960F) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/10.2 Chrome/71.0.3578.99 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("10.2"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-N9500"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SAMSUNG SM-N9500) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/10.2 Chrome/71.0.3578.99 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("10.2"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-N9600"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SAMSUNG SM-N9600) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/10.2 Chrome/71.0.3578.99 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("10.2"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-P205"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; SAMSUNG SM-P205) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/10.2 Chrome/71.0.3578.99 Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A205GN"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A205GN Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A205GN"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A205GN Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A405FN"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A405FN) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A530F"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A530F) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A600FN"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A600FN) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A705FN"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A705FN) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A705MN"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A705MN) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("71.0.3578.99"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A730F"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A730F Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/71.0.3578.99 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("78.0.3904.108"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A730F"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A730F Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/78.0.3904.108 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A730F"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A730F Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A730F"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A730F Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A730F"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A730F) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("78.0.3904.108"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A920F"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A920F) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.108 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A3050"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A3050 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A3050"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A3050 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A5070"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A5070 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A5070"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A5070 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A5070"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A5070 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A6060"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A6060) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("74.0.3729.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A7050"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A7050 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/74.0.3729.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A7050"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A7050 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A7050"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A7050 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A7050"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A7050) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("80.0.3987.18"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A7050"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A7050) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.18 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A8050"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A8050 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A8050"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A8050 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A8050"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A8050 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("78.0.3904.108"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A9200"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A9200 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/78.0.3904.108 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A9200"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A9200 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A9200"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A9200 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-A9200"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-A9200 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-G955F"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-G955F) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-G9550"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-G9550) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-G9600"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-G9600 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-G9650"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-G9650 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-G9650"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-G9650 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-G9650"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-G9650 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("78.0.3904.108"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-G9650"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-G9650) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.108 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-G9730"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-G9730 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-G9730"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-G9730 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-G9730"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-G9730) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-G9730"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-G9730) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-G9750"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-G9750 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-G9750"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-G9750) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-J330FN"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-J330FN) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("74.0.3729.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-J600G"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-J600G Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/74.0.3729.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("74.0.3729.157"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-J600G"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-J600G Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/74.0.3729.157 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("75.0.3770.143"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-J600G"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-J600G Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/75.0.3770.143 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-N960F"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-N960F) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-N960F"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-N960F) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("78.0.3904.108"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-N9500"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-N9500 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/78.0.3904.108 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-N9500"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-N9500 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-N9500"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-N9500 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-N9500"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-N9500 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-N9500"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-N9500) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-N9500"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-N9500) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-N9500"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-N9500) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.79"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-N9600"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-N9600 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.79 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-N9600"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-N9600 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-N9600"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-N9600 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-N9600"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-N9600 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-N9600"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-N9600) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-N9600"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-N9600) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-N9700"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-N9700 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-N9700"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-N9700) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-N9750"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-N9750 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-N9750"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-N9750 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-N9750"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-N9750) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("SM-N9750"), UTF8STRC("Mozilla/5.0 (Linux; Android 9; SM-N9750) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("STK-L22"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; STK-L22) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("STK-L22"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; STK-L22) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("STK-L22"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; STK-L22) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("STK-LX3"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; STK-LX3) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("TA-1003"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; TA-1003) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("78.0.3904.108"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("VCE-L22"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; VCE-L22 Build/HUAWEIVCE-L22; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/78.0.3904.108 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("VCE-L22"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; VCE-L22 Build/HUAWEIVCE-L22; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("VCE-L22"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; VCE-L22 Build/HUAWEIVCE-L22; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("VCE-L22"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; VCE-L22 Build/HUAWEIVCE-L22; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("77.0.3865.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("VCE-L22"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; VCE-L22) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/77.0.3865.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("VOG-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; VOG-L29 Build/HUAWEIVOG-L29; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("VOG-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; VOG-L29 Build/HUAWEIVOG-L29; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("78.0.3904.108"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("VOG-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; VOG-L29) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.108 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("VOG-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; VOG-L29) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("VOG-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; VOG-L29) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("YAL-L21"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; YAL-L21 Build/HUAWEIYAL-L61; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("YAL-L21"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; YAL-L21 Build/HUAWEIYAL-L61; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("YAL-L21"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; YAL-L21 Build/HUAWEIYAL-L61; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("74.0.3729.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("YAL-L21"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; YAL-L21) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("YAL-L21"),  UTF8STRC("Mozilla/5.0 (Linux; Android 9; YAL-L21) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("74.0.3729.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("vivo 1901"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; vivo 1901 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/74.0.3729.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("74.0.3729.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("vivo 1904"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; vivo 1904 Build/PPR1.180610.011; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/74.0.3729.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("78.0.3904.62"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("vivo 1909"),UTF8STRC("Mozilla/5.0 (Linux; Android 9; vivo 1909 Build/PKQ1.190302.001; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/78.0.3904.62 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("74.0.3729.185"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("SDK built for x86 Build/QSR1.190920.001"),0, 0,UTF8STRC("Mozilla/5.0 (Linux; Android 10; Android SDK built for x86 Build/QSR1.190920.001; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/74.0.3729.185 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("74.0.3729.185"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("SDK built for x86"),0, 0,          UTF8STRC("Mozilla/5.0 (Linux; Android 10; Android SDK built for x86) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.185 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("74.0.3729.186"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("HMA-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 10; HMA-L29 Build/HUAWEIHMA-L29; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/74.0.3729.186 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("HMA-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 10; HMA-L29 Build/HUAWEIHMA-L29; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("HMA-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 10; HMA-L29 Build/HUAWEIHMA-L29; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("HMA-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 10; HMA-L29) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("HMA-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 10; HMA-L29) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("J9110"),    UTF8STRC("Mozilla/5.0 (Linux; Android 10; J9110) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("70.0.3538.64"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("LIO-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 10; LIO-L29 Build/HUAWEILIO-L29; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/70.0.3538.64 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("74.0.3729.186"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("LYA-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 10; LYA-L29 Build/HUAWEILYA-L29; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/74.0.3729.186 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("LYA-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 10; LYA-L29 Build/HUAWEILYA-L29; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("Nokia 8.1"),UTF8STRC("Mozilla/5.0 (Linux; Android 10; Nokia 8.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("Nokia 8.1"),UTF8STRC("Mozilla/5.0 (Linux; Android 10; Nokia 8.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("Nokia 9"),  UTF8STRC("Mozilla/5.0 (Linux; Android 10; Nokia 9) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("ONEPLUS A6010"),UTF8STRC("Mozilla/5.0 (Linux; Android 10; ONEPLUS A6010) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("11.0"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("SM-G9730"), UTF8STRC("Mozilla/5.0 (Linux; Android 10; SAMSUNG SM-G9730) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/11.0 Chrome/75.0.3770.143 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("10.2"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("SM-G9750"), UTF8STRC("Mozilla/5.0 (Linux; Android 10; SAMSUNG SM-G9750) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/10.2 Chrome/71.0.3578.99 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("11.0"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("SM-G9750"), UTF8STRC("Mozilla/5.0 (Linux; Android 10; SAMSUNG SM-G9750) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/11.0 Chrome/75.0.3770.143 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("10.2"),          Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("SM-N9750"), UTF8STRC("Mozilla/5.0 (Linux; Android 10; SAMSUNG SM-N9750) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/10.2 Chrome/71.0.3578.99 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("78.0.3904.96"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("SM-A3050"), UTF8STRC("Mozilla/5.0 (Linux; Android 10; SM-A3050 Build/QP1A.190711.020; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/78.0.3904.96 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("SM-A3050"), UTF8STRC("Mozilla/5.0 (Linux; Android 10; SM-A3050 Build/QP1A.190711.020; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("SM-A3050"), UTF8STRC("Mozilla/5.0 (Linux; Android 10; SM-A3050 Build/QP1A.190711.020; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("SM-G9700"), UTF8STRC("Mozilla/5.0 (Linux; Android 10; SM-G9700) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("SM-G9700"), UTF8STRC("Mozilla/5.0 (Linux; Android 10; SM-G9700) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("SM-G9730"), UTF8STRC("Mozilla/5.0 (Linux; Android 10; SM-G9730 Build/QP1A.190711.020; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("77.0.3865.92"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("SM-G9750"), UTF8STRC("Mozilla/5.0 (Linux; Android 10; SM-G9750 Build/QP1A.190711.020; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/77.0.3865.92 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("SM-G9750"), UTF8STRC("Mozilla/5.0 (Linux; Android 10; SM-G9750 Build/QP1A.190711.020; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("SM-G9750"), UTF8STRC("Mozilla/5.0 (Linux; Android 10; SM-G9750 Build/QP1A.190711.020; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("SM-G9750"), UTF8STRC("Mozilla/5.0 (Linux; Android 10; SM-G9750 Build/QP1A.190711.020; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("SM-G9750"), UTF8STRC("Mozilla/5.0 (Linux; Android 10; SM-G9750) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("SM-G9750"), UTF8STRC("Mozilla/5.0 (Linux; Android 10; SM-G9750) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("SM-G9750"), UTF8STRC("Mozilla/5.0 (Linux; Android 10; SM-G9750) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.93"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("SM-N960F"), UTF8STRC("Mozilla/5.0 (Linux; Android 10; SM-N960F) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.93 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("SM-N960F"), UTF8STRC("Mozilla/5.0 (Linux; Android 10; SM-N960F) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("SM-N960F"), UTF8STRC("Mozilla/5.0 (Linux; Android 10; SM-N960F) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("77.0.3865.92"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("SM-N9750"), UTF8STRC("Mozilla/5.0 (Linux; Android 10; SM-N9750 Build/QP1A.190711.020; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/77.0.3865.92 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("SM-N9750"), UTF8STRC("Mozilla/5.0 (Linux; Android 10; SM-N9750 Build/QP1A.190711.020; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("SM-N9750"), UTF8STRC("Mozilla/5.0 (Linux; Android 10; SM-N9750) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("SM-N9750"), UTF8STRC("Mozilla/5.0 (Linux; Android 10; SM-N9750) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("74.0.3729.186"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("VOG-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 10; VOG-L29 Build/HUAWEIVOG-L29; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/74.0.3729.186 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("VOG-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 10; VOG-L29 Build/HUAWEIVOG-L29; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("VOG-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 10; VOG-L29 Build/HUAWEIVOG-L29; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("80.0.3987.87"),  Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("VOG-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 10; VOG-L29 Build/HUAWEIVOG-L29; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/80.0.3987.87 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.116"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("VOG-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 10; VOG-L29) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.116 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("VOG-L29"),  UTF8STRC("Mozilla/5.0 (Linux; Android 10; VOG-L29) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("74.0.3729.186"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("YAL-L21"),  UTF8STRC("Mozilla/5.0 (Linux; Android 10; YAL-L21 Build/HUAWEIYAL-L61; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/74.0.3729.186 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_ANDROIDWV,  UTF8STRC("79.0.3945.136"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("10"),        UTF8STRC("YAL-L21"),  UTF8STRC("Mozilla/5.0 (Linux; Android 10; YAL-L21 Build/HUAWEIYAL-L61; wv) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/79.0.3945.136 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("53.0.2785.34"),  Manage::OSInfo::OT_NETCAST,      UTF8STRC("8.5"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Linux; NetCast; U) AppleWebKit/537.31 (KHTML, like Gecko) Chrome/53.0.2785.34 Safari/537.31 SmartTV/8.5")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("4.0"),           Manage::OSInfo::OT_ANDROID,      UTF8STRC("4.1.2"),     UTF8STRC("LG-D500"),  UTF8STRC("Mozilla/5.0 (Linux; U; Android 4.1.2; en-us; LG-D500 Build/JZO54K) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 Mobile Safari/534.30")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("57.0.2987.132"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("4.4.2"),     UTF8STRC("Che2-TL00"),UTF8STRC("Mozilla/5.0 (Linux; U; Android 4.4.2; zh-cn; Che2-TL00 Build/HonorChe2-TL00) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/57.0.2987.132 MQQBrowser/8.3 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("30.0.1599.103"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("4.4.2"),     UTF8STRC("LG-D838"),  UTF8STRC("Mozilla/5.0 (Linux; U; Android 4.4.2; zh-hk; LG-D838 Build/KOT49I.D83810b) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/30.0.1599.103 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("4.0"),           Manage::OSInfo::OT_ANDROID,      UTF8STRC("4.4.4"),     UTF8STRC("Che1-CL10"),UTF8STRC("Mozilla/5.0 (Linux; U; Android 4.4.4; zh-cn; Che1-CL10 Build/Che1-CL10) AppleWebKit/534.30 (KHTML, like Gecko) Version/4.0 UCBrowser/1.0.0.100 U3/0.8.0 Mobile Safari/534.30 Tx(Tx/6.8.3) WindVane/8.0.0 720X1280 GCanvas/1.4.2.21")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("57.0.2987.108"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("6.0"),       UTF8STRC("GN5005"),   UTF8STRC("Mozilla/5.0 (Linux; U; Android 6.0; zh-CN; GN5005 Build/MRA58K) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/57.0.2987.108 UCBrowser/12.6.5.1045 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("71.0.3578.141"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.1.1"),     UTF8STRC("MI MAX 2"), UTF8STRC("Mozilla/5.0 (Linux; U; Android 7.1.1; zh-tw; MI MAX 2 Build/NMF26F) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/71.0.3578.141 Mobile Safari/537.36 XiaoMi/MiuiBrowser/11.3.4-g")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("71.0.3578.141"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.1.1"),     UTF8STRC("MI MAX 2"), UTF8STRC("Mozilla/5.0 (Linux; U; Android 7.1.1; zh-tw; MI MAX 2 Build/NMF26F) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/71.0.3578.141 Mobile Safari/537.36 XiaoMi/MiuiBrowser/11.4.3-g")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("61.0.3163.128"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("7.1.2"),     UTF8STRC("Redmi 4A"), UTF8STRC("Mozilla/5.0 (Linux; U; Android 7.1.2; id-id; Redmi 4A Build/N2G47H) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/61.0.3163.128 Mobile Safari/537.36 XiaoMi/MiuiBrowser/10.1.2")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("57.0.2987.132"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.0zh-cn"),  UTF8STRC("ONEPLUS A5010"),UTF8STRC("Mozilla/5.0 (Linux; U; Android 8.0zh-cn; ONEPLUS A5010 Build/PKQ1.180716.001) AppleWebKit/537.36 (KHTML, like Gecko)Version/4.0 Chrome/57.0.2987.132 MQQBrowser/8.1 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_UCBROWSER,  UTF8STRC("11.9.4.974"),    Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("EML-AL00"), UTF8STRC("Mozilla/5.0 (Linux; U; Android 8.1.0; zh-CN; EML-AL00 Build/HUAWEIEML-AL00) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/57.0.2987.108 baidu.sogo.uc.UCBrowser/11.9.4.974 UWS/2.13.1.48 Mobile Safari/537.36 AliApp(DingTalk/4.5.11) com.alibaba.android.rimet/10487439 Channel/227200 language/zh-CN")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("57.0.2987.108"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("OPPO R11"), UTF8STRC("Mozilla/5.0 (Linux; U; Android 8.1.0; zh-CN; OPPO R11 Build/OPM1.171019.011) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/57.0.2987.108 UCBrowser/12.7.7.1057 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("57.0.2987.108"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("OPPO R11"), UTF8STRC("Mozilla/5.0 (Linux; U; Android 8.1.0; zh-CN; OPPO R11 Build/OPM1.171019.011) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/57.0.2987.108 UCBrowser/12.7.9.1059 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("57.0.2987.108"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("8.1.0"),     UTF8STRC("OPPO R11"), UTF8STRC("Mozilla/5.0 (Linux; U; Android 8.1.0; zh-CN; OPPO R11 Build/OPM1.171019.011) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/57.0.2987.108 UCBrowser/12.8.0.1060 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("57.0.2987.108"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("STF-AL00"), UTF8STRC("Mozilla/5.0 (Linux; U; Android 9; zh-CN; STF-AL00 Build/HUAWEISTF-AL00) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/57.0.2987.108 UCBrowser/12.7.9.1059 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("71.0.3578.141"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("Redmi Note 8 Pro"),UTF8STRC("Mozilla/5.0 (Linux; U; Android 9; zh-cn; Redmi Note 8 Pro Build/PPR1.180610.011) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/71.0.3578.141 Mobile Safari/537.36 XiaoMi/MiuiBrowser/11.5.12")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("71.0.3578.141"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("Redmi Note 7"),UTF8STRC("Mozilla/5.0 (Linux; U; Android 9; zh-hk; Redmi Note 7 Build/PKQ1.180904.001) AppleWebKit/537.36 (KHTML, like Gecko) Version/4.0 Chrome/71.0.3578.141 Mobile Safari/537.36 XiaoMi/MiuiBrowser/11.3.4-g")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("57.0.2987.132"), Manage::OSInfo::OT_ANDROID,      UTF8STRC("9"),         UTF8STRC("BKL-AL20"), UTF8STRC("Mozilla/5.0 (Linux; U; Android 9;zh-cn; BKL-AL20 Build/HUAWEIBKL-AL20) AppleWebKit/537.36 (KHTML, like Gecko)Version/4.0 Chrome/57.0.2987.132 MQQBrowser/8.1 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("47.0"),          Manage::OSInfo::OT_MACOS,        UTF8STRC("10.11"),     0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10.11; rv:47.0) Gecko/20100101 Firefox/47.0")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("8.0"),           Manage::OSInfo::OT_MACOS,        UTF8STRC("10.10"),     0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10) AppleWebKit/537.16 (KHTML, like Gecko) Version/8.0 Safari/537.16")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("10.1.2"),        Manage::OSInfo::OT_MACOS,        UTF8STRC("10.10.5"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_5) AppleWebKit/603.3.8 (KHTML, like Gecko) Version/10.1.2 Safari/603.3.8")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("9.0.1"),         Manage::OSInfo::OT_MACOS,        UTF8STRC("10.11.1"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_1) AppleWebKit/601.2.4 (KHTML, like Gecko) Version/9.0.1 Safari/601.2.4 facebookexternalhit/1.1 Facebot Twitterbot/1.0")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("50.0.2661.102"), Manage::OSInfo::OT_MACOS,        UTF8STRC("10.11.5"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_5) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/50.0.2661.102 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.88"),  Manage::OSInfo::OT_MACOS,        UTF8STRC("10.11.6"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.88 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.130"), Manage::OSInfo::OT_MACOS,        UTF8STRC("10.11.6"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.130 Safari/537.36")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("9.1.2"),         Manage::OSInfo::OT_MACOS,        UTF8STRC("10.11.6"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_6) AppleWebKit/601.7.7 (KHTML, like Gecko) Version/9.1.2 Safari/601.7.7")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("55.0.2883.95"),  Manage::OSInfo::OT_MACOS,        UTF8STRC("10.12.2"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/55.0.2883.95 Safari/537.36")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("537.36"),        Manage::OSInfo::OT_MACOS,        UTF8STRC("10.12.4"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_4) AppleWebKit/537.36 (KHTML, like Gecko) 			Chrome/55.0.2883.95 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("78.0.3904.70"),  Manage::OSInfo::OT_MACOS,        UTF8STRC("10.12.6"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.70 Safari/537.36")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("12.1.2"),        Manage::OSInfo::OT_MACOS,        UTF8STRC("10.12.6"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_6) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/12.1.2 Safari/605.1.15")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.88"),  Manage::OSInfo::OT_MACOS,        UTF8STRC("10.13.6"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.88 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("81.0.4000.3"),   Manage::OSInfo::OT_MACOS,        UTF8STRC("10.13.6"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/81.0.4000.3 Safari/537.36")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("12.1.1"),        Manage::OSInfo::OT_MACOS,        UTF8STRC("10.13.6"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_6) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/12.1.1 Safari/605.1.15")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("13.0.3"),        Manage::OSInfo::OT_MACOS,        UTF8STRC("10.13.6"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_6) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.0.3 Safari/605.1.15")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("13.0.4"),        Manage::OSInfo::OT_MACOS,        UTF8STRC("10.13.6"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_6) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.0.4 Safari/605.1.15")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("78.0.3904.108"), Manage::OSInfo::OT_MACOS,        UTF8STRC("10.14.0"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.108 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("71.0.3578.80"),  Manage::OSInfo::OT_MACOS,        UTF8STRC("10.14.1"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/71.0.3578.80 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("74.0.3729.131"), Manage::OSInfo::OT_MACOS,        UTF8STRC("10.14.2"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.131 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.88"),  Manage::OSInfo::OT_MACOS,        UTF8STRC("10.14.6"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.88 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.130"), Manage::OSInfo::OT_MACOS,        UTF8STRC("10.14.6"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.130 Safari/537.36")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_MACOS,        UTF8STRC("10.14.6"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6) AppleWebKit/605.1.15 (KHTML, like Gecko)")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("13.0.4"),        Manage::OSInfo::OT_MACOS,        UTF8STRC("10.14.6"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.0.4 Safari/605.1.15")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("13.0.5"),        Manage::OSInfo::OT_MACOS,        UTF8STRC("10.14.6"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.0.5 Safari/605.1.15")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("13.0.3"),        Manage::OSInfo::OT_MACOS,        UTF8STRC("10.15"),     0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.0.3 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("13.0.3"),        Manage::OSInfo::OT_MACOS,        UTF8STRC("10.15"),     0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.0.3 Safari/605.1.15")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("13.0.4"),        Manage::OSInfo::OT_MACOS,        UTF8STRC("10.15"),     0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.0.4 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("13.0.4"),        Manage::OSInfo::OT_MACOS,        UTF8STRC("10.15"),     0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.0.4 Safari/605.1.15")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.79"),  Manage::OSInfo::OT_MACOS,        UTF8STRC("10.15.0"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.79 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("78.0.3904.97"),  Manage::OSInfo::OT_MACOS,        UTF8STRC("10.15.1"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.97 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("78.0.3904.108"), Manage::OSInfo::OT_MACOS,        UTF8STRC("10.15.1"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.108 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.88"),  Manage::OSInfo::OT_MACOS,        UTF8STRC("10.15.1"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.88 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.88"),  Manage::OSInfo::OT_MACOS,        UTF8STRC("10.15.2"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.88 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("81.0.4000.3"),   Manage::OSInfo::OT_MACOS,        UTF8STRC("10.15.2"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/81.0.4000.3 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("81.0.4033.2"),   Manage::OSInfo::OT_MACOS,        UTF8STRC("10.15.2"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/81.0.4033.2 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("81.0.4040.5"),   Manage::OSInfo::OT_MACOS,        UTF8STRC("10.15.2"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/81.0.4040.5 Safari/537.36")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("13.0.4"),        Manage::OSInfo::OT_MACOS,        UTF8STRC("10.15.2"),   0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_2) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.0.4 Safari/605.1.15")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("53.0"),          Manage::OSInfo::OT_MACOS,        0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (Macintosh; PPC Mac OS X 10.12; rv:53.0) Gecko/20100101 Firefox/53.0")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("49.0.2623.112"), Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("5.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/49.0.2623.112 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("63.0.3239.132"), Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("5.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("69.0.3497.100"), Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("5.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/69.0.3497.100 Safari/537.36")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("8.0.1"),         Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("5.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 5.1; rv:8.0.1) Gecko/20100101 Firefox/8.0.1")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("9.0.1"),         Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("5.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 5.1; rv:9.0.1) Gecko/20100101 Firefox/9.0.1")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("11.0"),          Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("5.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 5.1; rv:11.0) Gecko Firefox/11.0 (via ggpht.com GoogleImageProxy)")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("52.0"),          Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("5.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 5.1; rv:52.0) Gecko/20100101 Firefox/52.0")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("49.0.2623.112"), Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.0"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/49.0.2623.112 Safari/537.36")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("52.0"),          Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.0"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.0; rv:52.0) Gecko/20100101 Firefox/52.0")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("36.0.1985.125"), Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/36.0.1985.125 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("38.0.2125.122"), Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/38.0.2125.122 Safari/537.36 SE 2.X MetaSr 1.0")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("41.0.2228.0"),   Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2228.0 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("45.0.2454.101"), Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2454.101 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("61.0.3163.91"),  Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/61.0.3163.91 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("63.0.3239.132"), Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("64.0.3282.167"), Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/64.0.3282.167 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("65.0.3325.181"), Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/65.0.3325.181 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("66.0.3359.139"), Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.139 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("69.0.3497.100"), Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/69.0.3497.100 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.88"),  Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.88 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.117"), Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.117 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.130"), Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.130 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("80.0.3987.87"),  Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.87 Safari/537.36")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("537.36"),        Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) baidu.sogo.uc.Chrome/36.0.1985.125 Safari/537.36")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; Trident/7.0; rv:11.0) like Gecko")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("23.0.1271.64"),  Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.11 (KHTML, like Gecko) Chrome/23.0.1271.64 Safari/537.11")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("31.0.1650.63"),  Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/31.0.1650.63 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("39.0.2171.95"),  Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.95 Safari/537.36 MicroMessenger/6.5.2.501 NetType/WIFI WindowsWechat QBCore/3.43.27.400 QQBrowser/9.0.2524.400")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("42.0.2311.135"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/42.0.2311.135 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("43.0.2357.124"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/43.0.2357.124 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("45.0.2454.101"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2454.101 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("49.0.2623.105"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/49.0.2623.105 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("51.0.2704.103"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("52.0.2743.116"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/52.0.2743.116 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("53.0.2785.104"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.104 Safari/537.36 Core/1.53.2141.400 QQBrowser/9.5.10219.400")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("55.0.2883.87"),  Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/55.0.2883.87 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("56.0.2924.87"),  Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/56.0.2924.87 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("63.0.3239.132"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("65.0.3314.0"),   Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/65.0.3314.0 Safari/537.36 SE 2.X MetaSr 1.0")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("70.0.3538.25"),  Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/70.0.3538.25 Safari/537.36 Core/1.70.3704.400 QQBrowser/10.4.3587.400")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("76.0"),          Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/76.0 Safari/537.36 Firefox/66.0")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("76.0.3809.132"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/76.0.3809.132 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.88"),  Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.88 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.117"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.117 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.130"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.130 Safari/537.36")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; WOW64) Chrome")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("23.0.1271.64"),  Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; WOW64; rv:31.0) Gecko/20100101 Chrome/23.0.1271.64 Safari/537.11")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("31.0"),          Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; WOW64; rv:31.0) Gecko/20100101 Firefox/31.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("33.0"),          Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; WOW64; rv:33.0) Gecko/20100101 Firefox/33.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("52.0"),          Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; WOW64; rv:52.0) Gecko/20100101 Firefox/52.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("54.0"),          Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; WOW64; rv:54.0) Gecko/20100101 Firefox/54.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("57.0"),          Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; WOW64; rv:57.0) Gecko/20100101 Firefox/57.0")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("40.0.2214.85"),  Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/40.0.2214.85 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("49.0.2623.105"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/49.0.2623.105 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("64.0.3282.140"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/64.0.3282.140 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("66.0.3359.139"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.139 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("67.0.3396.99"),  Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("72.0.3626.109"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/72.0.3626.109 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("75.0.3770.142"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/75.0.3770.142 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("76.0.3809.100"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/76.0.3809.100 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("78.0.3904.108"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.108 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.79"),  Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.79 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.88"),  Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.88 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.117"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.117 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.130"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.130 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("80.0.3987.87"),  Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.87 Safari/537.36")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; Win64; x64; Trident/7.0; rv:11.0) like Gecko")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("28.0"),          Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:28.0) Gecko/20100101 Firefox/28.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("31.0"),          Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:31.0) Gecko/20100101 Firefox/31.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("52.0"),          Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:52.0) Gecko/20100101 Firefox/52.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("57.0"),          Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:57.0) Gecko/20100101 Firefox/57.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("58.0"),          Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:58.0) Gecko/20100101 Firefox/58.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("59.0"),          Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:59.0) Gecko/20100101 Firefox/59.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("60.0"),          Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:60.0) Gecko/20100101 Firefox/60.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("69.0"),          Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:69.0) Gecko/20100101 Firefox/69.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("70.0"),          Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:70.0) Gecko/20100101 Firefox/70.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("71.0"),          Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:71.0) Gecko/20100101 Firefox/71.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("72.0"),          Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:72.0) Gecko/20100101 Firefox/72.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("73.0"),          Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:73.0) Gecko/20100101 Firefox/73.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("71.0"),          Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; rv:71.0) Gecko/20100101 Firefox/71.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("72.0"),          Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.1; rv:72.0) Gecko/20100101 Firefox/72.0")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.88"),  Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.2"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.88 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.130"), Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.2"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.130 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.88"),  Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.3"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.3) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.88 Safari/537.36")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.3"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.3; WOW64; Trident/7.0; rv:11.0) like Gecko")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.88"),  Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.3"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.3; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.88 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.117"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.3"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.3; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.117 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.130"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.3"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.3; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.130 Safari/537.36")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("72.0"),          Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.3"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 6.3; Win64; x64; rv:72.0) Gecko/20100101 Firefox/72.0")},
	{Net::BrowserInfo::BT_EDGE,       UTF8STRC("18.17763"),      Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/64.0.3282.140 Safari/537.36 Edge/18.17763")},
	{Net::BrowserInfo::BT_EDGE,       UTF8STRC("18.18362"),      Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/70.0.3538.102 Safari/537.36 Edge/18.18362")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.88"),  Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.88 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.117"), Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.117 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.130"), Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.130 Safari/537.36")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Trident/7.0; rv:11.0) like Gecko")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("48.0.2564.116"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/48.0.2564.116 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("49.0.2623.112"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/49.0.2623.112 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("51.0.2704.103"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("52.0.2743.116"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/52.0.2743.116 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("53.0.2785.116"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.116 Safari/537.36 QBCore/4.0.1278.400 QQBrowser/9.0.2524.400 Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2875.116 Safari/537.36 NetType/WIFI MicroMessenger/7.0.5 WindowsWechat")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("55.0.2883.87"),  Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/55.0.2883.87 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("58.0.3029.110"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36 SE 2.X MetaSr 1.0")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("63.0.3239.132"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("65.0.3314.0"),   Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/65.0.3314.0 Safari/537.36 SE 2.X MetaSr 1.0")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("65.0.3325.181"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/65.0.3325.181 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("66.0.3359.117"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.117 Safari/537.36 ")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("69.0.3497.100"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/69.0.3497.100 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("69.0.3947.100"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/69.0.3947.100 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("73.0.3683.75"),  Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/73.0.3683.75 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("73.0.3683.86"),  Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/73.0.3683.86 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("77.0.3865.120"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/77.0.3865.120 Safari/537.36 Avast/77.2.2153.120")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("78.0.3904.108"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.108 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.88"),  Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.88 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.130"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.130 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.130"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.130 Safari/537.36 OPR/66.0.3515.44 (Edition Campaign 67)")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; ASJB; rv:11.0) like Gecko")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; Touch; rv:11.0) like Gecko")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; WOW64; Trident/7.0; rv:11.0) like Gecko")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("35.0"),          Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; WOW64; rv:35.0) Gecko/20100101 Firefox/35.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("60.0"),          Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; WOW64; rv:60.0) Gecko/20100101 Firefox/60.0")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("57.0.2987.133"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/57.0.2987.133 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("59.0.3071.104"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/59.0.3071.104 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("60.0.3112.113"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/60.0.3112.113 Safari/537.36 ")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("61.0.3163.100"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/61.0.3163.100 Safari/537.36 ")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("63.0.3239.132"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36")},
	{Net::BrowserInfo::BT_EDGE,       UTF8STRC("17.17134"),      Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/64.0.3282.140 Safari/537.36 Edge/17.17134")},
	{Net::BrowserInfo::BT_EDGE,       UTF8STRC("18.17763"),      Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/64.0.3282.140 Safari/537.36 Edge/18.17763")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("65.0.3325.181"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/65.0.3325.181 Safari/537.36 OPR/52.0.2871.64")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("66.0.3359.139"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.139 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("68.0.3440.106"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/68.0.3440.106 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("70.0.3538.102"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/70.0.3538.102 Safari/537.36")},
	{Net::BrowserInfo::BT_EDGE,       UTF8STRC("18.18362"),      Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/70.0.3538.102 Safari/537.36 Edge/18.18362")},
	{Net::BrowserInfo::BT_EDGE,       UTF8STRC("18.18363"),      Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/70.0.3538.102 Safari/537.36 Edge/18.18363")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("71.0.3578.98"),  Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/71.0.3578.98 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("72.0.3626.119"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/72.0.3626.119 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("72.0.3626.121"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/72.0.3626.121 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("73.0.3683.86"),  Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/73.0.3683.86 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("74.0.3729.169"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("75.0.3770.100"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/75.0.3770.100 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("75.0.3770.142"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/75.0.3770.142 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("76.0.3809.132"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/76.0.3809.132 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("77.0.3865.93"),  Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/77.0.3865.93 Safari/537.36 Vivaldi/2.8.1664.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("77.0.3865.120"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/77.0.3865.120 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("78.0.3904.70"),  Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.70 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("78.0.3904.97"),  Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.97 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("78.0.3904.108"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.108 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("78.0.3904.108"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.108 Safari/537.36 OPR/65.0.3467.78")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.79"),  Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.79 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.88"),  Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.88 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.117"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.117 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.130"), Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.130 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("80.0.3987.87"),  Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.87 Safari/537.36")},
	{Net::BrowserInfo::BT_EDGE,       UTF8STRC("18.18362"),      Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64; Cortana 1.13.0.18362; 10.0.0.0.18362.535) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/70.0.3538.102 Safari/537.36 Edge/18.18362")},
	{Net::BrowserInfo::BT_EDGE,       UTF8STRC("18.18362"),      Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64; Cortana 1.13.0.18362; 10.0.0.0.18362.592) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/70.0.3538.102 Safari/537.36 Edge/18.18362")},
	{Net::BrowserInfo::BT_EDGE,       UTF8STRC("18.18363"),      Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64; Cortana 1.13.0.18362; 10.0.0.0.18363.535) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/70.0.3538.102 Safari/537.36 Edge/18.18363")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64; Trident/7.0; Touch; rv:11.0) like Gecko")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64; Trident/7.0; rv:11.0) like Gecko")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("61.0"),          Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:61.0) Gecko/20100101 Firefox/61.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("65.0"),          Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:65.0) Gecko/20100101 Firefox/65.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("69.0"),          Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:69.0) Gecko/20100101 Firefox/69.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("71.0"),          Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:71.0) Gecko/20100101 Firefox/71.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("72.0"),          Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("10.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:72.0) Gecko/20100101 Firefox/72.0")},
	{Net::BrowserInfo::BT_IE,         UTF8STRC("9.0"),           Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("9.0"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows; U; MSIE 9.0; Windows NT 9.0; en-US)")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("3.0"),           Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("5.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows; U; Windows NT 5.1; de; rv:1.9) Gecko/2008052906 Firefox/3.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("3.6)"),          Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.0"),       0, 0,                 UTF8STRC("Mozilla/5.0 (Windows; U; Windows NT 6.0;en-US; rv:1.9.2) Gecko/20100115 Firefox/3.6)")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.86"),  Manage::OSInfo::OT_CHROMEOS,     UTF8STRC("12607.58.0"),0, 0,                 UTF8STRC("Mozilla/5.0 (X11; CrOS x86_64 12607.58.0) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/79.0.3945.86 Safari/537.36")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("49.0"),          Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (X11; Linux i686 on x86_64; rv:49.0) Gecko/20100101 Firefox/49.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("10.0"),          Manage::OSInfo::OT_LINUX_I686,   0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (X11; Linux i686; rv:10.0) Gecko/20100101 Firefox/10.0")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("11.0.696.34"),   Manage::OSInfo::OT_LINUX_X86_64, 0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/534.24 (KHTML, like Gecko) Chrome/11.0.696.34 Safari/534.24")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("71.0.3578.141"), Manage::OSInfo::OT_LINUX_X86_64, 0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/534.24 (KHTML, like Gecko) Chrome/71.0.3578.141 Safari/534.24 XiaoMi/MiuiBrowser/11.3.4-g")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("49.0.2623.75"),  Manage::OSInfo::OT_LINUX_X86_64, 0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/49.0.2623.75 Safari/537.36 Google Favicon")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("63.0.3239.108"), Manage::OSInfo::OT_LINUX_X86_64, 0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.108 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("72.0.3602.2"),   Manage::OSInfo::OT_LINUX_X86_64, 0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/72.0.3602.2 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("72.0.3626.121"), Manage::OSInfo::OT_LINUX_X86_64, 0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/72.0.3626.121 Safari/537.36")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("537.36"),        Manage::OSInfo::OT_LINUX_X86_64, 0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) HeadlessChrome/68.0.3427.1 Safari/537.36")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("537.36"),        Manage::OSInfo::OT_LINUX_X86_64, 0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) HeadlessChrome/78.0.3904.70 Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("9.0"),           Manage::OSInfo::OT_LINUX_X86_64, 0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/9.0 Chrome/67.0.3396.87 Safari/537.36")},
	{Net::BrowserInfo::BT_SAMSUNG,    UTF8STRC("10.2"),          Manage::OSInfo::OT_LINUX_X86_64, 0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) SamsungBrowser/10.2 Chrome/71.0.3578.99 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("70.0.3538.77"),  Manage::OSInfo::OT_LINUX_X86_64, 0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Ubuntu Chromium/70.0.3538.77 Chrome/70.0.3538.77 Safari/537.36")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.79"),  Manage::OSInfo::OT_LINUX_X86_64, 0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Ubuntu Chromium/79.0.3945.79 Chrome/79.0.3945.79 Safari/537.36")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("537.36"),        Manage::OSInfo::OT_LINUX_X86_64, 0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko)Chrome/75.0.3770.100 Safari/537.36")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("28.0"),          Manage::OSInfo::OT_LINUX_X86_64, 0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (X11; Linux x86_64; rv:28.0) Gecko/20100101 Firefox/28.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("45.0"),          Manage::OSInfo::OT_LINUX_X86_64, 0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (X11; Linux x86_64; rv:45.0) Gecko/20100101 Firefox/45.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("52.0"),          Manage::OSInfo::OT_LINUX_X86_64, 0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (X11; Linux x86_64; rv:52.0) Gecko/20100101 Firefox/52.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("68.0"),          Manage::OSInfo::OT_LINUX_X86_64, 0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (X11; Linux x86_64; rv:68.0) Gecko/20100101 Firefox/68.0")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_LINUX_X86_64, 0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (X11; Linux x86_64; rv:109)")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("47.0"),          Manage::OSInfo::OT_LINUX_X86_64, 0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:47.0) Gecko/20100101 Firefox/47.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("48.0"),          Manage::OSInfo::OT_LINUX_X86_64, 0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:48.0) Gecko/20100101 Firefox/48.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("52.0"),          Manage::OSInfo::OT_LINUX_X86_64, 0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:52.0) Gecko/20100101 Firefox/52.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("58.0"),          Manage::OSInfo::OT_LINUX_X86_64, 0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:58.0) Gecko/20100101 Firefox/58.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("62.0"),          Manage::OSInfo::OT_LINUX_X86_64, 0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:62.0) Gecko/20100101 Firefox/62.0")},
	{Net::BrowserInfo::BT_FIREFOX,    UTF8STRC("71.0"),          Manage::OSInfo::OT_LINUX_X86_64, 0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:71.0) Gecko/20100101 Firefox/71.0")},
	{Net::BrowserInfo::BT_AHREFSBOT,  UTF8STRC("6.1"),           Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (compatible; AhrefsBot/6.1; +http://ahrefs.com/robot/)")},
	{Net::BrowserInfo::BT_BLEXBOT,    UTF8STRC("0"),             Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (compatible; BLEXBot/1.0; +http://webmeup-crawler.com/)")},
	{Net::BrowserInfo::BT_BAIDU,      UTF8STRC("2.0"),           Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (compatible; Baiduspider-render/2.0; +http://www.baidu.com/search/spider.html)")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (compatible; Baiduspider/2.0; +http://www.baidu.com/search/spider.html)")},
	{Net::BrowserInfo::BT_GOOGLEBOTD, UTF8STRC("2.1"),           Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (compatible; Googlebot/2.1; +http://www.google.com/bot.html)")},
	{Net::BrowserInfo::BT_MJ12BOT,    UTF8STRC("1.4.8"),         Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (compatible; MJ12bot/v1.4.8; http://mj12bot.com/)")},
	{Net::BrowserInfo::BT_IE,         UTF8STRC("9.0"),           Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)")},
	{Net::BrowserInfo::BT_IE,         UTF8STRC("9.0"),           Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0;  Trident/5.0)")},
	{Net::BrowserInfo::BT_IE,         UTF8STRC("9.0"),           Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Win64; x64; Trident/5.0)")},
	{Net::BrowserInfo::BT_IE,         UTF8STRC("10.0"),          Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.1; Trident/6.0)")},
	{Net::BrowserInfo::BT_IE,         UTF8STRC("10.0"),          Manage::OSInfo::OT_WINDOWS_NT64, UTF8STRC("6.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.1; WOW64; Trident/6.0)")},
	{Net::BrowserInfo::BT_IE,         UTF8STRC("10.0"),          Manage::OSInfo::OT_WINDOWS_NT,   UTF8STRC("6.2"),       0, 0,                 UTF8STRC("Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.2)")},
	{Net::BrowserInfo::BT_NETCRAFTAG, UTF8STRC("1.0"),           Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (compatible; NetcraftSurveyAgent/1.0; +info@netcraft.com)")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (compatible; Nimbostratus-Bot/v1.3.2; http://cloudsystemnetworks.com)")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (compatible; Nmap Scripting Engine; https://nmap.org/book/nse.html)")},
	{Net::BrowserInfo::BT_SEMRUSH,    UTF8STRC("6~bl"),          Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (compatible; SemrushBot/6~bl; +http://www.semrush.com/bot.html)")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (compatible; SeznamBot/3.2; +http://napoveda.seznam.cz/en/seznambot-intro/)")},
	{Net::BrowserInfo::BT_YANDEX,     UTF8STRC("3.0"),           Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (compatible; YandexBot/3.0; +http://yandex.com/bots)")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (compatible; YandexImages/3.0; +http://yandex.com/bots)")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (compatible; Yeti/1.1; +http://naver.me/spd)")},
	{Net::BrowserInfo::BT_BING,       UTF8STRC("2.0"),           Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (compatible; bingbot/2.0; +http://www.bing.com/bingbot.htm)")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 (compatible; startmebot/1.0; +https://start.me/bot)")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("63.0.3239.73"),  Manage::OSInfo::OT_IPAD,         UTF8STRC("9.3.5"),     0, 0,                 UTF8STRC("Mozilla/5.0 (iPad; CPU OS 9_3_5 like Mac OS X) AppleWebKit/601.1 (KHTML, like Gecko) CriOS/63.0.3239.73 Mobile/13G36 Safari/601.1.46")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("10.0"),          Manage::OSInfo::OT_IPAD,         UTF8STRC("10.3.3"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPad; CPU OS 10_3_3 like Mac OS X) AppleWebKit/603.3.8 (KHTML, like Gecko) Version/10.0 Mobile/14G60 Safari/602.1")},
	{Net::BrowserInfo::BT_GSA,        UTF8STRC("89.2.287201133"),Manage::OSInfo::OT_IPAD,         UTF8STRC("12.4"),      0, 0,                 UTF8STRC("Mozilla/5.0 (iPad; CPU OS 12_4 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) GSA/89.2.287201133 Mobile/15E148 Safari/605.1")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("12.1.2"),        Manage::OSInfo::OT_IPAD,         UTF8STRC("12.4"),      0, 0,                 UTF8STRC("Mozilla/5.0 (iPad; CPU OS 12_4 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/12.1.2 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("12.1.2"),        Manage::OSInfo::OT_IPAD,         UTF8STRC("12.4.1"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPad; CPU OS 12_4_1 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/12.1.2 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_IPAD,         UTF8STRC("13.1.2"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPad; CPU OS 13_1_2 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Mobile/15E148")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("13.0.1"),        Manage::OSInfo::OT_IPAD,         UTF8STRC("13.1.3"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPad; CPU OS 13_1_3 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.0.1 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("13.0.3"),        Manage::OSInfo::OT_IPAD,         UTF8STRC("13.2.3"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPad; CPU OS 13_2_3 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.0.3 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.73"),  Manage::OSInfo::OT_IPAD,         UTF8STRC("13.3"),      0, 0,                 UTF8STRC("Mozilla/5.0 (iPad; CPU OS 13_3 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) CriOS/79.0.3945.73 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_IPAD,         UTF8STRC("13.3"),      0, 0,                 UTF8STRC("Mozilla/5.0 (iPad; CPU OS 13_3 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Mobile/15E148")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("13.0.4"),        Manage::OSInfo::OT_IPAD,         UTF8STRC("13.3"),      0, 0,                 UTF8STRC("Mozilla/5.0 (iPad; CPU OS 13_3 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.0.4 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_IPAD,         UTF8STRC("13.3.1"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPad; CPU OS 13_3_1 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Mobile/15E148")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("8.0"),           Manage::OSInfo::OT_IPHONE,       UTF8STRC("8.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 8_1 like Mac OS X) AppleWebKit/600.1.4 (KHTML, like Gecko) Version/8.0 Mobile/12B411 Safari/600.1.4 (compatible; YandexMobileBot/3.0; +http://yandex.com/bots)")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("9.0"),           Manage::OSInfo::OT_IPHONE,       UTF8STRC("9.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 9_1 like Mac OS X) AppleWebKit/601.1.46 (KHTML, like Gecko) Version/9.0 Mobile/13B143 Safari/601.1")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("9.0"),           Manage::OSInfo::OT_IPHONE,       UTF8STRC("9.1"),       0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 9_1 like Mac OS X) AppleWebKit/601.1.46 (KHTML, like Gecko) Version/9.0 Mobile/13B143 Safari/601.1 (compatible; Baiduspider-render/2.0; +http://www.baidu.com/search/spider.html)")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_IPHONE,       UTF8STRC("10.2.1"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 10_2_1 like Mac OS X) AppleWebKit/602.4.6 (KHTML, like Gecko) Mobile/14D27 MicroMessenger/6.5.5 NetType/WIFI Language/en")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_IPHONE,       UTF8STRC("10.2.1"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 10_2_1 like Mac OS X) AppleWebKit/602.4.6 (KHTML, like Gecko) Mobile/14D27 MicroMessenger/6.5.5 NetType/WIFI Language/zh_CN")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("603.2.4"),       Manage::OSInfo::OT_IPHONE,       UTF8STRC("10.3.2"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 10_3_2 like Mac OS X) AppleWebKit/603.2.4 (KHTML, like Gecko) FxiOS/7.5b3349 Mobile/14F89 Safari/603.2.4")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("11.0"),          Manage::OSInfo::OT_IPHONE,       UTF8STRC("11.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 11_0 like Mac OS X) AppleWebKit/604.1.38 (KHTML, like Gecko) Version/11.0 Mobile/15A372 Safari/604.1")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("11.0"),          Manage::OSInfo::OT_IPHONE,       UTF8STRC("11.2.6"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 11_2_6 like Mac OS X) AppleWebKit/604.5.6 (KHTML, like Gecko) Version/11.0 Mobile/15D100 Safari/604.1")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_IPHONE,       UTF8STRC("11.3"),      0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 11_3 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Mobile/15E302")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("11.0"),          Manage::OSInfo::OT_IPHONE,       UTF8STRC("11.4.1"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 11_4_1 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/11.0 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("12.0"),          Manage::OSInfo::OT_IPHONE,       UTF8STRC("12.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 12_0 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/12.0 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_GSA,        UTF8STRC("89.2.287201133"),Manage::OSInfo::OT_IPHONE,       UTF8STRC("12.1"),      0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 12_1 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) GSA/89.2.287201133 Mobile/15E148 Safari/605.1")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("12.0"),          Manage::OSInfo::OT_IPHONE,       UTF8STRC("12.1.2"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 12_1_2 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/12.0 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("12.0"),          Manage::OSInfo::OT_IPHONE,       UTF8STRC("12.1.3"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 12_1_3 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/12.0 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_IPHONE,       UTF8STRC("12.1.4"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 12_1_4 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Mobile/16D57")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("12.0"),          Manage::OSInfo::OT_IPHONE,       UTF8STRC("12.1.4"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 12_1_4 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/12.0 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("12.1"),          Manage::OSInfo::OT_IPHONE,       UTF8STRC("12.2"),      0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 12_2 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/12.1 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_GSA,        UTF8STRC("89.2.287201133"),Manage::OSInfo::OT_IPHONE,       UTF8STRC("12.3"),      0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 12_3 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) GSA/89.2.287201133 Mobile/15E148 Safari/605.1")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_IPHONE,       UTF8STRC("12.3.1"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 12_3_1 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Mobile/15E148")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("12.1.1"),        Manage::OSInfo::OT_IPHONE,       UTF8STRC("12.3.1"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 12_3_1 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/12.1.1 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_IPHONE,       UTF8STRC("12.3.2"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 12_3_2 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Mobile/15E148")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("12.1.1"),        Manage::OSInfo::OT_IPHONE,       UTF8STRC("12.3.2"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 12_3_2 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/12.1.1 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_IPHONE,       UTF8STRC("12.4.1"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 12_4_1 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Mobile/15E148")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("12.1.2"),        Manage::OSInfo::OT_IPHONE,       UTF8STRC("12.4.1"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 12_4_1 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/12.1.2 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_IPHONE,       UTF8STRC("12.4.3"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 12_4_3 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Mobile/15E148")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("12.1.2"),        Manage::OSInfo::OT_IPHONE,       UTF8STRC("12.4.3"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 12_4_3 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/12.1.2 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_IPHONE,       UTF8STRC("12.4.4"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 12_4_4 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Mobile/15E148")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("13.0"),          Manage::OSInfo::OT_IPHONE,       UTF8STRC("13.0"),      0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 13_0 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.0 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.73"),  Manage::OSInfo::OT_IPHONE,       UTF8STRC("13.1"),      0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 13_1 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) CriOS/79.0.3945.73 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_GSA,        UTF8STRC("89.2.287201133"),Manage::OSInfo::OT_IPHONE,       UTF8STRC("13.1"),      0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 13_1 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) GSA/89.2.287201133 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_IPHONE,       UTF8STRC("13.1"),      0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 13_1 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Mobile/15E148")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("13.0.1"),        Manage::OSInfo::OT_IPHONE,       UTF8STRC("13.1"),      0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 13_1 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.0.1 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_IPHONE,       UTF8STRC("13.1.1"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 13_1_1 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Mobile/15E148")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("13.0.1"),        Manage::OSInfo::OT_IPHONE,       UTF8STRC("13.1.1"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 13_1_1 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.0.1 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_IPHONE,       UTF8STRC("13.1.2"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 13_1_2 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Mobile/15E148")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("13.0.1"),        Manage::OSInfo::OT_IPHONE,       UTF8STRC("13.1.2"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 13_1_2 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.0.1 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_IPHONE,       UTF8STRC("13.1.3"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 13_1_3 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Mobile/15E148")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("13.0.1"),        Manage::OSInfo::OT_IPHONE,       UTF8STRC("13.1.3"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 13_1_3 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.0.1 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.73"),  Manage::OSInfo::OT_IPHONE,       UTF8STRC("13.2"),      0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 13_2 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) CriOS/79.0.3945.73 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_IPHONE,       UTF8STRC("13.2.2"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 13_2_2 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Mobile/15E148")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("13.0.3"),        Manage::OSInfo::OT_IPHONE,       UTF8STRC("13.2.2"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 13_2_2 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.0.3 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_IPHONE,       UTF8STRC("13.2.3"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 13_2_3 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Mobile/15E148")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("75.0.3770.103"), Manage::OSInfo::OT_IPHONE,       UTF8STRC("13.3"),      0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 13_3 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) CriOS/75.0.3770.103 Mobile/15E148 Safari/605.1")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("79.0.3945.73"),  Manage::OSInfo::OT_IPHONE,       UTF8STRC("13.3"),      0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 13_3 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) CriOS/79.0.3945.73 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_GSA,        UTF8STRC("88.1.284108841"),Manage::OSInfo::OT_IPHONE,       UTF8STRC("13.3"),      0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 13_3 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) GSA/88.1.284108841 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_GSA,        UTF8STRC("89.2.287201133"),Manage::OSInfo::OT_IPHONE,       UTF8STRC("13.3"),      0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 13_3 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) GSA/89.2.287201133 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_IPHONE,       UTF8STRC("13.3"),      0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 13_3 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Mobile/15E148")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("13.0.4"),        Manage::OSInfo::OT_IPHONE,       UTF8STRC("13.3"),      0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 13_3 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.0.4 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_UCBROWSER,  UTF8STRC("12.7.6.1251"),   Manage::OSInfo::OT_IPHONE,       UTF8STRC("13.3"),      0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 13_3 like Mac OS X; zh-CN) AppleWebKit/537.51.1 (KHTML, like Gecko) Mobile/17C54 UCBrowser/12.7.6.1251 Mobile  AliApp(TUnionSDK/0.1.20.3)")},
	{Net::BrowserInfo::BT_UCBROWSER,  UTF8STRC("12.7.8.1259"),   Manage::OSInfo::OT_IPHONE,       UTF8STRC("13.3"),      0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 13_3 like Mac OS X; zh-CN) AppleWebKit/537.51.1 (KHTML, like Gecko) Mobile/17C54 UCBrowser/12.7.8.1259 Mobile  AliApp(TUnionSDK/0.1.20.3)")},
	{Net::BrowserInfo::BT_UCBROWSER,  UTF8STRC("12.7.9.1260"),   Manage::OSInfo::OT_IPHONE,       UTF8STRC("13.3"),      0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 13_3 like Mac OS X; zh-CN) AppleWebKit/537.51.1 (KHTML, like Gecko) Mobile/17C54 UCBrowser/12.7.9.1260 Mobile  AliApp(TUnionSDK/0.1.20.3)")},
	{Net::BrowserInfo::BT_UCBROWSER,  UTF8STRC("12.8.0.1256"),   Manage::OSInfo::OT_IPHONE,       UTF8STRC("13.3"),      0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 13_3 like Mac OS X; zh-CN) AppleWebKit/537.51.1 (KHTML, like Gecko) Mobile/17C54 UCBrowser/12.8.0.1256 Mobile  AliApp(TUnionSDK/0.1.20.3)")},
	{Net::BrowserInfo::BT_UNKNOWN,    0, 0,                      Manage::OSInfo::OT_IPHONE,       UTF8STRC("13.3.1"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 13_3_1 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Mobile/15E148")},
	{Net::BrowserInfo::BT_SAFARI,     UTF8STRC("13.0.5"),        Manage::OSInfo::OT_IPHONE,       UTF8STRC("13.3.1"),    0, 0,                 UTF8STRC("Mozilla/5.0 (iPhone; CPU iPhone OS 13_3_1 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/13.0.5 Mobile/15E148 Safari/604.1")},
	{Net::BrowserInfo::BT_NETSEEN,    UTF8STRC("1.0"),           Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 NetSeen/1.0")},
	{Net::BrowserInfo::BT_ZGRAB,      UTF8STRC("0.x"),           Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0 zgrab/0.x")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("42.0.2311.138"), Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0(Linux;Android 5.1.1;OPPO A33 Build/LMY47V;wv) AppleWebKit/537.36(KHTML,link Gecko) Version/4.0 Chrome/42.0.2311.138 Mobile Safari/537.36 Mb2345Browser/9.0")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("43.0.2357.121"), Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0(Linux;Android 5.1.1;OPPO A33 Build/LMY47V;wv) AppleWebKit/537.36(KHTML,link Gecko) Version/4.0 Chrome/43.0.2357.121 Mobile Safari/537.36 LieBaoFast/4.51.3")},
	{Net::BrowserInfo::BT_CHROME,     UTF8STRC("40.0.2214.89"),  Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Mozilla/5.0(Linux;U;Android 5.1.1;zh-CN;OPPO A33 Build/LMY47V) AppleWebKit/537.36(KHTML,like Gecko) Version/4.0 Chrome/40.0.2214.89 UCBrowser/11.7.0.953 Mobile Safari/537.36")},
	{Net::BrowserInfo::BT_NETSYSRES,  0, 0,                      Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("NetSystemsResearch studies the availability of various services across the internet. Our website is netsystemsresearch.com")},
	{Net::BrowserInfo::BT_NUTCH,      UTF8STRC("0.1"),           Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Nutch Master Test/Dolphin-0.1-Beta")},
	{Net::BrowserInfo::BT_PYURLLIB,   UTF8STRC("1.17"),          Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Python-urllib/1.17")},
	{Net::BrowserInfo::BT_PYURLLIB,   UTF8STRC("2.7"),           Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Python-urllib/2.7")},
	{Net::BrowserInfo::BT_PYURLLIB,   UTF8STRC("3.5"),           Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Python-urllib/3.5")},
	{Net::BrowserInfo::BT_PYURLLIB,   UTF8STRC("3.6"),           Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Python-urllib/3.6")},
	{Net::BrowserInfo::BT_PYURLLIB,   UTF8STRC("3.7"),           Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Python-urllib/3.7")},
	{Net::BrowserInfo::BT_SCRAPY,     UTF8STRC("1.5.1"),         Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Scrapy/1.5.1 (+https://scrapy.org)")},
	{Net::BrowserInfo::BT_SOGOUPIC,   UTF8STRC("3.0"),           Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Sogou Pic Spider/3.0(+http://www.sogou.com/docs/help/webmasters.htm#07)")},
	{Net::BrowserInfo::BT_SOGOUWEB,   UTF8STRC("4.0"),           Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Sogou web spider/4.0(+http://www.sogou.com/docs/help/webmasters.htm#07)")},
	{Net::BrowserInfo::BT_WESTWIND,   UTF8STRC("4.35"),          Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("West Wind Internet Protocols 4.35")},
	{Net::BrowserInfo::BT_WESTWIND,   UTF8STRC("4.68"),          Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("West Wind Internet Protocols 4.68")},
	{Net::BrowserInfo::BT_WGET,       UTF8STRC("1.17.1"),        Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Wget/1.17.1 (linux-gnu)")},
	{Net::BrowserInfo::BT_WGET,       UTF8STRC("1.18"),          Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Wget/1.18 (linux-gnueabihf)")},
	{Net::BrowserInfo::BT_WGET,       UTF8STRC("1.19.4"),        Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Wget/1.19.4 (linux-gnu)")},
	{Net::BrowserInfo::BT_WGET,       UTF8STRC("1.19.5"),        Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("Wget/1.19.5 (linux-gnu)")},
	{Net::BrowserInfo::BT_WHATSAPP,   UTF8STRC("0.4.315"),       Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("WhatsApp/0.4.315 N")},
	{Net::BrowserInfo::BT_WHATSAPP,   UTF8STRC("2.19.360"),      Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("WhatsApp/2.19.360 A")},
	{Net::BrowserInfo::BT_YISOU,      0, 0,                      Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("YisouSpider")},
	{Net::BrowserInfo::BT_CURL,       UTF8STRC("7.47.0"),        Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("curl/7.47.0")},
	{Net::BrowserInfo::BT_CURL,       UTF8STRC("7.58.0"),        Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("curl/7.58.0")},
	{Net::BrowserInfo::BT_FACEBOOK,   UTF8STRC("1.1"),           Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("facebookexternalhit/1.1 (+http://www.facebook.com/externalhit_uatext.php)")},
	{Net::BrowserInfo::BT_LIBWWW_PERL,UTF8STRC("6.13"),          Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("libwww-perl/6.13")},
	{Net::BrowserInfo::BT_LIBWWW_PERL,UTF8STRC("6.43"),          Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("libwww-perl/6.43")},
	{Net::BrowserInfo::BT_MASSCAN,    UTF8STRC("1.0"),           Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("masscan/1.0 (https://github.com/robertdavidgraham/masscan)")},
	{Net::BrowserInfo::BT_NETSEEN,    UTF8STRC("2.0b"),          Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("msnbot/2.0b (+http://search.msn.com/msnbot.htm)")},
	{Net::BrowserInfo::BT_PYREQUESTS, UTF8STRC("2.6.0"),         Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("python-requests/2.6.0 CPython/2.7.5 Linux/3.10.0-862.11.6.el7.x86_64")},
	{Net::BrowserInfo::BT_PYREQUESTS, UTF8STRC("2.12.4"),        Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("python-requests/2.12.4")},
	{Net::BrowserInfo::BT_PYREQUESTS, UTF8STRC("2.21.0"),        Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("python-requests/2.21.0")},
	{Net::BrowserInfo::BT_PYREQUESTS, UTF8STRC("2.22.0"),        Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("python-requests/2.22.0")},
	{Net::BrowserInfo::BT_SSWR,       UTF8STRC("1.0"),           Manage::OSInfo::OT_UNKNOWN,      0, 0,                  0, 0,                 UTF8STRC("sswr/1.0")},
};

const Net::UserAgentDB::UAEntry *Net::UserAgentDB::GetUserAgentInfo(const UTF8Char *userAgent)
{
	OSInt i = 0;
	OSInt j = (sizeof(uaList) / sizeof(uaList[0])) - 1;
	OSInt k;
	OSInt l;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = Text::StrCompare(userAgent, (const UTF8Char*)uaList[k].userAgent);
		if (l > 0)
		{
			i = k + 1;
		}
		else if (l < 0)
		{
			j = k - 1;
		}
		else
		{
			return &uaList[k];
		}
	}
	return 0;
}

Net::UserAgentDB::UAEntry *Net::UserAgentDB::GetUAEntryList(UOSInt *cnt)
{
	*cnt = sizeof(uaList) / sizeof(uaList[0]);
	return uaList;
}

void Net::UserAgentDB::ParseUserAgent(Net::UserAgentDB::UAEntry *ent, Text::CString userAgent)
{
	ent->browser = Net::BrowserInfo::BT_UNKNOWN;
	ent->browserVer = 0;
	ent->browserVerLen = 0;
	ent->devName = 0;
	ent->devNameLen = 0;
	ent->osVer = 0;
	ent->osVerLen = 0;
	ent->os = Manage::OSInfo::OT_UNKNOWN;
	ent->userAgent = userAgent.v;
	ent->userAgentLen = userAgent.leng;

	UOSInt i;
	if (userAgent.Equals(UTF8STRC("Microsoft Windows Network Diagnostics")))
	{
		ent->browser = Net::BrowserInfo::BT_WINDIAG;
		return;
	}
	else if (userAgent.StartsWith(UTF8STRC("West Wind Internet Protocols ")))
	{
		ent->browser = Net::BrowserInfo::BT_WESTWIND;
		ent->browserVer = Text::StrCopyNewC(&ent->userAgent[29], userAgent.leng - 29);
		ent->browserVerLen = userAgent.leng - 29;
		return;
	}
	else if (userAgent.StartsWith(UTF8STRC("Sogou web spider/")))
	{
		ent->browser = Net::BrowserInfo::BT_SOGOUWEB;
		i = userAgent.IndexOf('(');
		if (i != INVALID_INDEX)
		{
			ent->browserVer = Text::StrCopyNewC(&ent->userAgent[17], i - 17);
			ent->browserVerLen = i - 17;
		}
		else
		{
			ent->browserVer = Text::StrCopyNewC(&ent->userAgent[17], userAgent.leng - 17);
			ent->browserVerLen = userAgent.leng - 17;
		}
		return;
	}
	else if (userAgent.StartsWith(UTF8STRC("Sogou Pic Spider/")))
	{
		ent->browser = Net::BrowserInfo::BT_SOGOUPIC;
		i = userAgent.IndexOf('(');
		if (i != INVALID_INDEX)
		{
			ent->browserVer = Text::StrCopyNewC(&ent->userAgent[17], i - 17);
			ent->browserVerLen = i - 17;
		}
		else
		{
			ent->browserVer = Text::StrCopyNewC(&ent->userAgent[17], userAgent.leng - 17);
			ent->browserVerLen = userAgent.leng - 17;
		}
		return;
	}
	else if (userAgent.Equals(UTF8STRC("Nutch Master Test/Dolphin-0.1-Beta")))
	{
		ent->browser = Net::BrowserInfo::BT_NUTCH;
		ent->browserVer = Text::StrCopyNewC(UTF8STRC("0.1"));
		ent->browserVerLen = 3;
		return;
	}
	else if (userAgent.Equals(UTF8STRC("YisouSpider")))
	{
		ent->browser = Net::BrowserInfo::BT_YISOU;
		return;
	}
	else if (userAgent.StartsWith(UTF8STRC("HTTP Banner Detection")))
	{
		ent->browser = Net::BrowserInfo::BT_BANNERDET;
		return;
	}
	else if (userAgent.StartsWith(UTF8STRC("NetSystemsResearch ")))
	{
		ent->browser = Net::BrowserInfo::BT_NETSYSRES;
		return;
	}
	Text::PString strs[30];
	Text::PString strs2[10];
	UTF8Char *sptr;
	UTF8Char c;
	UOSInt nstrs;
	UOSInt nstrs2;
	UOSInt j;
	UOSInt k;
	Bool bst;
	Bool lastIsAndroid;
	UTF8Char *sbuff = MemAlloc(UTF8Char, userAgent.leng + 1);
	UTF8Char *sbuffEnd;
	sbuffEnd = userAgent.ConcatTo(sbuff);
	if (sbuff[0] == '"' && sbuff[userAgent.leng - 1] == '"')
	{
		sbuff[userAgent.leng - 1] = 0;
		sbuff[0] = ' ';
		sbuffEnd = Text::StrTrimC(sbuff, userAgent.leng - 1);
	}
	if (Text::StrEqualsC(sbuff, (UOSInt)(sbuffEnd - sbuff), UTF8STRC("nlpproject.info research")))
	{
		ent->browser = Net::BrowserInfo::BT_NLPPROJECT;
		MemFree(sbuff);
		return;
	}

	bst = false;
	nstrs = 1;
	strs[0].v = sbuff;
	sptr = sbuff;
	while ((c = *sptr) != 0)
	{
		if (c == ' ' && !bst)
		{
			sptr[0] = 0;
			strs[nstrs - 1].leng = (UOSInt)(sptr - strs[nstrs - 1].v);
			strs[nstrs++].v = sptr + 1;
		}
		else if (c == ')' && bst)
		{
			bst = false;
		}
		else if (c == '(')
		{
			bst = true;
		}
		sptr++;
	}
	strs[nstrs - 1].leng = (UOSInt)(sptr - strs[nstrs - 1].v);
	i = 0;
	while (i < nstrs)
	{
		if (strs[i].v[0] == '(')
		{
			UOSInt charCnt = strs[i].leng;
			if (strs[i].v[charCnt - 1] == ')')
			{
				strs[i].v[charCnt - 1] = 0;
			}
			nstrs2 = Text::StrSplitTrimP(strs2, 10, {&strs[i].v[1], charCnt - 2}, ';');
			j = 0;
			lastIsAndroid = false;
			while (j < nstrs2)
			{
				if (Text::StrStartsWithC(strs2[j].v, strs2[j].leng, UTF8STRC("MSIE ")))
				{
					ent->browser = Net::BrowserInfo::BT_IE;
					SDEL_TEXT(ent->browserVer);
					ent->browserVer = Text::StrCopyNewC(&strs2[j].v[5], strs2[j].leng - 5);
					ent->browserVerLen = strs2[j].leng - 5;
				}
				else if (Text::StrStartsWithC(strs2[j].v, strs2[j].leng, UTF8STRC("Windows NT ")))
				{
					ent->os = Manage::OSInfo::OT_WINDOWS_NT;
					SDEL_TEXT(ent->osVer);
					ent->osVer = Text::StrCopyNewC(&strs2[j].v[11], strs2[j].leng - 11);
					ent->osVerLen = strs2[j].leng - 11;
				}
				else if (Text::StrEqualsC(strs2[j].v, strs2[j].leng, UTF8STRC("WOW64")))
				{
					if (ent->os == Manage::OSInfo::OT_WINDOWS_NT)
					{
						ent->os = Manage::OSInfo::OT_WINDOWS_NT64;
					}
				}
				else if (Text::StrEqualsC(strs2[j].v, strs2[j].leng, UTF8STRC("Win64")))
				{
					if (ent->os == Manage::OSInfo::OT_WINDOWS_NT)
					{
						ent->os = Manage::OSInfo::OT_WINDOWS_NT64;
					}
				}
				else if (Text::StrEqualsC(strs2[j].v, strs2[j].leng, UTF8STRC("iPad")))
				{
					ent->os = Manage::OSInfo::OT_IPAD;
				}
				else if (ent->os == Manage::OSInfo::OT_UNKNOWN && Text::StrEqualsC(strs2[j].v, strs2[j].leng, UTF8STRC("Linux i686")))
				{
					ent->os = Manage::OSInfo::OT_LINUX_I686;
				}
				else if (ent->os == Manage::OSInfo::OT_UNKNOWN && Text::StrEqualsC(strs2[j].v, strs2[j].leng, UTF8STRC("Linux x86_64")))
				{
					ent->os = Manage::OSInfo::OT_LINUX_X86_64;
				}
				else if (Text::StrEqualsC(strs2[j].v, strs2[j].leng, UTF8STRC("Android")))
				{
					ent->os = Manage::OSInfo::OT_ANDROID;
				}
				else if (Text::StrEqualsC(strs2[j].v, strs2[j].leng, UTF8STRC("wv")))
				{
					if (ent->os == Manage::OSInfo::OT_ANDROID)
					{
						ent->browser = Net::BrowserInfo::BT_ANDROIDWV;
					}
				}
				else if (Text::StrStartsWithC(strs2[j].v, strs2[j].leng, UTF8STRC("Android ")))
				{
					ent->os = Manage::OSInfo::OT_ANDROID;
					SDEL_TEXT(ent->osVer);
					ent->osVer = Text::StrCopyNewC(&strs2[j].v[8], strs2[j].leng - 8);
					ent->osVerLen = strs2[j].leng - 81;
					lastIsAndroid = true;
				}
				else if (Text::StrStartsWithC(strs2[j].v, strs2[j].leng, UTF8STRC("CrOS ")))
				{
					ent->os = Manage::OSInfo::OT_CHROMEOS;
					SDEL_TEXT(ent->osVer);
					k = Text::StrIndexOfCharC(&strs2[j].v[5], strs2[j].leng - 5, ' ');
					ent->osVer = Text::StrCopyNewC(&strs2[j].v[6 + k], strs2[j].leng - k - 6);
					ent->osVerLen = strs2[j].leng - k - 6;
				}
				else if (ent->os == Manage::OSInfo::OT_IPAD && Text::StrEqualsC(strs2[j].v, strs2[j].leng, UTF8STRC("U")))
				{
					ent->os = Manage::OSInfo::OT_ANDROID;
				}
				else if (Text::StrEqualsC(strs2[j].v, strs2[j].leng, UTF8STRC("iPhone")))
				{
					ent->os = Manage::OSInfo::OT_IPHONE;
				}
				else if (Text::StrEqualsC(strs2[j].v, strs2[j].leng, UTF8STRC("Macintosh")))
				{
					ent->os = Manage::OSInfo::OT_MACOS;
				}
				else if ((ent->os == Manage::OSInfo::OT_IPAD || ent->os == Manage::OSInfo::OT_IPHONE) && Text::StrStartsWithC(strs2[j].v, strs2[j].leng, UTF8STRC("CPU OS ")))
				{
					SDEL_TEXT(ent->osVer);
					k = Text::StrIndexOfChar(&strs2[j].v[7], ' ');
					if (k != INVALID_INDEX)
					{
						strs2[j].v[k + 7] = 0;
						strs2[j].leng = k + 7;
					}
					Text::StrReplace(&strs2[j].v[7], '_', '.');
					ent->osVer = Text::StrCopyNewC(&strs2[j].v[7], strs2[j].leng - 7);
					ent->osVerLen = strs2[j].leng - 7;
				}
				else if (ent->os == Manage::OSInfo::OT_IPHONE && Text::StrStartsWithC(strs2[j].v, strs2[j].leng, UTF8STRC("CPU iPhone OS ")))
				{
					SDEL_TEXT(ent->osVer);
					k = Text::StrIndexOfChar(&strs2[j].v[14], ' ');
					if (k != INVALID_INDEX)
					{
						strs2[j].v[k + 14] = 0;
						strs2[j].leng = k + 14;
					}
					Text::StrReplace(&strs2[j].v[14], '_', '.');
					ent->osVer = Text::StrCopyNewC(&strs2[j].v[14], strs2[j].leng - 14);
					ent->osVerLen = strs2[j].leng - 14;
				}
				else if (ent->os == Manage::OSInfo::OT_MACOS && Text::StrStartsWithC(strs2[j].v, strs2[j].leng, UTF8STRC("Intel Mac OS X ")))
				{
					SDEL_TEXT(ent->osVer);
					k = Text::StrIndexOfCharC(&strs2[j].v[15], strs2[j].leng - 15, ' ');
					if (k != INVALID_INDEX)
					{
						strs2[j].v[k + 15] = 0;
						strs2[j].leng = k + 15;
					}
					Text::StrReplace(&strs2[j].v[15], '_', '.');
					ent->osVer = Text::StrCopyNewC(&strs2[j].v[15], strs2[j].leng - 15);
					ent->osVerLen = strs2[j].leng - 15;
				}
				else if (Text::StrEqualsC(strs2[j].v ,strs[j].leng, UTF8STRC("Trident/7.0")))
				{
					ent->browser = Net::BrowserInfo::BT_IE;
					SDEL_TEXT(ent->browserVer);
					ent->browserVer = Text::StrCopyNewC(UTF8STRC("11.0"));
					ent->browserVerLen = 4;
				}
				else if (Text::StrEqualsC(strs2[j].v, strs2[j].leng, UTF8STRC("JuziBrowser"))) //JuziBrowser
				{
				}
				else if (Text::StrEqualsC(strs2[j].v, strs2[j].leng, UTF8STRC("SE 2.X MetaSr 1.0"))) //Sugou Browser
				{
				}
				else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrEqualsC(strs2[j].v, strs[j].leng, UTF8STRC("Indy Library")))
				{
					ent->browser = Net::BrowserInfo::BT_INDY;
				}
				else if (Text::StrStartsWithC(strs2[j].v, strs2[j].leng, UTF8STRC("Googlebot/")))
				{
					if (ent->os == Manage::OSInfo::OT_ANDROID)
					{
						ent->browser = Net::BrowserInfo::BT_GOOGLEBOTS;
						SDEL_TEXT(ent->browserVer);
						ent->browserVer = Text::StrCopyNewC(&strs2[j].v[10], strs2[j].leng - 10);
						ent->browserVerLen = strs2[j].leng - 10;
					}
					else if (ent->os == Manage::OSInfo::OT_UNKNOWN)
					{
						ent->browser = Net::BrowserInfo::BT_GOOGLEBOTD;
						SDEL_TEXT(ent->browserVer);
						ent->browserVer = Text::StrCopyNewC(&strs2[j].v[10], strs2[j].leng - 10);
						ent->browserVerLen = strs2[j].leng - 10;
					}
				}
				else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs2[j].v, strs2[j].leng, UTF8STRC("Baiduspider-render/")))
				{
					ent->browser = Net::BrowserInfo::BT_BAIDU;
					ent->browserVer = Text::StrCopyNewC(&strs2[j].v[19], strs2[j].leng - 19);
						ent->browserVerLen = strs2[j].leng - 19;
				}
				else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs2[j].v, strs2[j].leng, UTF8STRC("SemrushBot/")))
				{
					ent->browser = Net::BrowserInfo::BT_SEMRUSH;
					ent->browserVer = Text::StrCopyNewC(&strs2[j].v[11], strs2[j].leng - 11);
					ent->browserVerLen = strs2[j].leng - 11;
				}
				else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs2[j].v, strs2[j].leng, UTF8STRC("YandexBot/")))
				{
					ent->browser = Net::BrowserInfo::BT_YANDEX;
					ent->browserVer = Text::StrCopyNewC(&strs2[j].v[10], strs2[j].leng - 10);
					ent->browserVerLen = strs2[j].leng - 10;
				}
				else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs2[j].v, strs2[j].leng, UTF8STRC("BLEXBot/")))
				{
					ent->browser = Net::BrowserInfo::BT_BLEXBOT;
					ent->browserVer = Text::StrCopyNewC(&strs2[j].v[10], strs2[j].leng - 10);
					ent->browserVerLen = strs2[j].leng - 10;
				}
				else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs2[j].v, strs2[j].leng, UTF8STRC("bingbot/")))
				{
					ent->browser = Net::BrowserInfo::BT_BING;
					ent->browserVer = Text::StrCopyNewC(&strs2[j].v[8], strs2[j].leng - 8);
					ent->browserVerLen = strs2[j].leng - 8;
				}
				else if ((ent->browser == Net::BrowserInfo::BT_UNKNOWN || ent->browser == Net::BrowserInfo::BT_IE) && Text::StrStartsWithC(strs2[j].v, strs2[j].leng, UTF8STRC("MS Web Services Client Protocol ")))
				{
					ent->browser = Net::BrowserInfo::BT_DOTNET;
					SDEL_TEXT(ent->browserVer);
					ent->browserVer = Text::StrCopyNewC(&strs2[j].v[32], strs2[j].leng - 32);
					ent->browserVerLen = strs2[j].leng - 32;
				}
				else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs2[j].v, strs2[j].leng, UTF8STRC("WinHttp.WinHttpRequest.")))
				{
					ent->browser = Net::BrowserInfo::BT_WINHTTP;
					ent->browserVer = Text::StrCopyNewC(&strs2[j].v[23], strs2[j].leng - 23);
					ent->browserVerLen = strs2[j].leng - 23;
				}
				else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs2[j].v, strs2[j].leng, UTF8STRC("NetcraftSurveyAgent/")))
				{
					ent->browser = Net::BrowserInfo::BT_NETCRAFTAG;
					ent->browserVer = Text::StrCopyNewC(&strs2[j].v[20], strs2[j].leng - 20);
					ent->browserVerLen = strs2[j].leng - 20;
				}
				else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs2[j].v, strs2[j].leng, UTF8STRC("AhrefsBot/")))
				{
					ent->browser = Net::BrowserInfo::BT_AHREFSBOT;
					ent->browserVer = Text::StrCopyNewC(&strs2[j].v[10], strs2[j].leng - 10);
					ent->browserVerLen = strs2[j].leng - 10;
				}
				else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs2[j].v, strs2[j].leng, UTF8STRC("MJ12bot/")))
				{
					ent->browser = Net::BrowserInfo::BT_MJ12BOT;
					if (strs2[j].v[8] == 'v')
					{
						ent->browserVer = Text::StrCopyNewC(&strs2[j].v[9], strs2[j].leng - 9);
						ent->browserVerLen = strs2[j].leng - 9;
					}
					else
					{
						ent->browserVer = Text::StrCopyNewC(&strs2[j].v[8], strs2[j].leng - 8);
						ent->browserVerLen = strs2[j].leng - 8;
					}
				}
				else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrEqualsC(strs2[j].v, strs2[j].leng, UTF8STRC("Netcraft Web Server Survey")))
				{
					ent->browser = Net::BrowserInfo::BT_NETCRAFTWEB;
				}
				else if (Text::StrEqualsC(strs2[j].v, strs2[j].leng, UTF8STRC("Mobile")))
				{

				}
				else if (Text::StrEqualsC(strs2[j].v, strs2[j].leng, UTF8STRC("rv:")))
				{

				}
				else if (lastIsAndroid)
				{
					k = Text::StrIndexOfC(strs2[j].v, strs2[j].leng, UTF8STRC(" Build/"));
					if (k != INVALID_INDEX)
					{
						strs2[j].v[k] = 0;
						strs2[j].leng = k;
					}

					k = Text::StrIndexOfC(strs2[j].v, strs2[j].leng, UTF8STRC(" MIUI/"));
					if (k != INVALID_INDEX)
					{
						strs2[j].v[k] = 0;
						strs2[j].leng = k;
					}
					SDEL_TEXT(ent->devName);
					if (Text::StrStartsWithC(strs2[j].v, strs2[j].leng, UTF8STRC("SAMSUNG SM-")))
					{
						ent->devName = Text::StrCopyNewC(&strs2[j].v[8], strs2[j].leng - 8);
						ent->devNameLen = strs2[j].leng - 8;
					}
					else if (Text::StrStartsWithC(strs2[j].v, strs2[j].leng, UTF8STRC("HUAWEI ")))
					{
						ent->devName = Text::StrCopyNewC(&strs2[j].v[7], strs2[j].leng - 7);
						ent->devNameLen = strs2[j].leng - 7;
					}
					else
					{
						ent->devName = Text::StrCopyNewC(strs2[j].v, strs2[j].leng);
						ent->devNameLen = strs2[j].leng;
					}
				}
				j++;
			}
		}
		else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("Firefox/")))
		{
			ent->browser = Net::BrowserInfo::BT_FIREFOX;
			ent->browserVer = Text::StrCopyNewC(&strs[i].v[8], strs[i].leng - 8);
			ent->browserVerLen = strs[i].leng - 8;
		}
		else if ((ent->browser == Net::BrowserInfo::BT_UNKNOWN || ent->browser == Net::BrowserInfo::BT_SAFARI) && Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("SamsungBrowser/")))
		{
			ent->browser = Net::BrowserInfo::BT_SAMSUNG;
			SDEL_TEXT(ent->browserVer);
			ent->browserVer = Text::StrCopyNewC(&strs[i].v[15], strs[i].leng - 15);
			ent->browserVerLen = strs[i].leng - 15;
		}
		else if ((ent->browser == Net::BrowserInfo::BT_UNKNOWN || ent->browser == Net::BrowserInfo::BT_SAFARI) && Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("GSA/")))
		{
			ent->browser = Net::BrowserInfo::BT_GSA;
			SDEL_TEXT(ent->browserVer);
			ent->browserVer = Text::StrCopyNewC(&strs[i].v[4], strs[i].leng - 4);
			ent->browserVerLen = strs[i].leng - 4;
		}
		else if ((ent->browser == Net::BrowserInfo::BT_UNKNOWN || ent->browser == Net::BrowserInfo::BT_SAFARI) && Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("CriOS/")))
		{
			ent->browser = Net::BrowserInfo::BT_CHROME;
			SDEL_TEXT(ent->browserVer);
			ent->browserVer = Text::StrCopyNewC(&strs[i].v[6], strs[i].leng - 6);
			ent->browserVerLen = strs[i].leng - 6;
		}
		else if (Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("Chrome/")))
		{
			if (ent->browser == Net::BrowserInfo::BT_UNKNOWN || ent->browser == Net::BrowserInfo::BT_SAFARI)
			{
				ent->browser = Net::BrowserInfo::BT_CHROME;
				SDEL_TEXT(ent->browserVer);
				ent->browserVer = Text::StrCopyNewC(&strs[i].v[7], strs[i].leng - 7);
				ent->browserVerLen = strs[i].leng - 7;
			}
			else if (ent->browser == Net::BrowserInfo::BT_ANDROIDWV)
			{
				SDEL_TEXT(ent->browserVer);
				ent->browserVer = Text::StrCopyNewC(&strs[i].v[7], strs[i].leng - 7);
				ent->browserVerLen = strs[i].leng - 7;
			}
		}
		else if ((ent->browser == Net::BrowserInfo::BT_UNKNOWN || ent->browser == Net::BrowserInfo::BT_CHROME) && Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("Edge/")))
		{
			ent->browser = Net::BrowserInfo::BT_EDGE;
			SDEL_TEXT(ent->browserVer);
			ent->browserVer = Text::StrCopyNewC(&strs[i].v[5], strs[i].leng - 5);
			ent->browserVerLen = strs[i].leng - 5;
		}
		else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("Safari/")))
		{
			ent->browser = Net::BrowserInfo::BT_SAFARI;
			ent->browserVer = Text::StrCopyNewC(&strs[i].v[7], strs[i].leng - 7);
			ent->browserVerLen = strs[i].leng - 7;
		}
		else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("UCBrowser/")))
		{
			ent->browser = Net::BrowserInfo::BT_UCBROWSER;
			ent->browserVer = Text::StrCopyNewC(&strs[i].v[10], strs[i].leng - 10);
			ent->browserVerLen = strs[i].leng - 10;
		}
		else if ((ent->browser == Net::BrowserInfo::BT_UNKNOWN || ent->browser == Net::BrowserInfo::BT_CHROME) && Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("baidu.sogo.uc.UCBrowser/")))
		{
			ent->browser = Net::BrowserInfo::BT_UCBROWSER;
			SDEL_TEXT(ent->browserVer);
			ent->browserVer = Text::StrCopyNewC(&strs[i].v[24], strs[i].leng - 24);
			ent->browserVerLen = strs[i].leng - 24;
		}
		else if (Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("UBrowser/")))
		{
		}
		else if (Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("baiduboxapp/")))
		{
		}
		else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("Dalvik/")))
		{
			ent->browser = Net::BrowserInfo::BT_DALVIK;
			ent->browserVer = Text::StrCopyNewC(&strs[i].v[7], strs[i].leng - 7);
			ent->browserVerLen = strs[i].leng - 7;
		}
		else if (ent->os == Manage::OSInfo::OT_UNKNOWN && Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("Darwin/")))
		{
			ent->os = Manage::OSInfo::OT_DARWIN;
			ent->osVer = Text::StrCopyNewC(&strs[i].v[7], strs[i].leng - 7);
			ent->osVerLen = strs[i].leng - 7;
		}
		else if (ent->os == Manage::OSInfo::OT_UNKNOWN && Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("SmartTV/")))
		{
			ent->os = Manage::OSInfo::OT_NETCAST;
			ent->osVer = Text::StrCopyNewC(&strs[i].v[8], strs[i].leng - 8);
			ent->osVerLen = strs[i].leng - 8;
		}
		else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("CFNetwork/")))
		{
			ent->browser = Net::BrowserInfo::BT_CFNETWORK;
			ent->browserVer = Text::StrCopyNewC(&strs[i].v[10], strs[i].leng - 10);
			ent->browserVerLen = strs[i].leng - 10;
		}
		else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("Version/")))
		{
			ent->browser = Net::BrowserInfo::BT_SAFARI;
			ent->browserVer = Text::StrCopyNewC(&strs[i].v[8], strs[i].leng - 8);
			ent->browserVerLen = strs[i].leng - 8;
		}
		else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("masscan/")))
		{
			ent->browser = Net::BrowserInfo::BT_MASSCAN;
			ent->browserVer = Text::StrCopyNewC(&strs[i].v[8], strs[i].leng - 8);
			ent->browserVerLen = strs[i].leng - 8;
		}
		else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("zgrab/")))
		{
			ent->browser = Net::BrowserInfo::BT_ZGRAB;
			ent->browserVer = Text::StrCopyNewC(&strs[i].v[6], strs[i].leng - 6);
			ent->browserVerLen = strs[i].leng - 6;
		}
		else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("python-requests/")))
		{
			ent->browser = Net::BrowserInfo::BT_PYREQUESTS;
			ent->browserVer = Text::StrCopyNewC(&strs[i].v[16], strs[i].leng - 16);
			ent->browserVerLen = strs[i].leng - 16;
		}
		else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("Python-urllib/")))
		{
			ent->browser = Net::BrowserInfo::BT_PYURLLIB;
			ent->browserVer = Text::StrCopyNewC(&strs[i].v[14], strs[i].leng - 14);
			ent->browserVerLen = strs[i].leng - 14;
		}
		else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("Wget/")))
		{
			ent->browser = Net::BrowserInfo::BT_WGET;
			ent->browserVer = Text::StrCopyNewC(&strs[i].v[5], strs[i].leng - 5);
			ent->browserVerLen = strs[i].leng - 5;
		}
		else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("Scrapy/")))
		{
			ent->browser = Net::BrowserInfo::BT_SCRAPY;
			ent->browserVer = Text::StrCopyNewC(&strs[i].v[7], strs[i].leng - 7);
			ent->browserVerLen = strs[i].leng - 7;
		}
		else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("Go-http-client/")))
		{
			ent->browser = Net::BrowserInfo::BT_GOHTTP;
			ent->browserVer = Text::StrCopyNewC(&strs[i].v[15], strs[i].leng - 15);
			ent->browserVerLen = strs[i].leng - 15;
		}
		else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("Apache-HttpClient/")))
		{
			ent->browser = Net::BrowserInfo::BT_APACHEHTTP;
			ent->browserVer = Text::StrCopyNewC(&strs[i].v[18], strs[i].leng - 18);
			ent->browserVerLen = strs[i].leng - 18;
		}
		else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("WhatsApp/")))
		{
			ent->browser = Net::BrowserInfo::BT_WHATSAPP;
			ent->browserVer = Text::StrCopyNewC(&strs[i].v[9], strs[i].leng - 9);
			ent->browserVerLen = strs[i].leng - 9;
		}
		else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("curl/")))
		{
			ent->browser = Net::BrowserInfo::BT_CURL;
			ent->browserVer = Text::StrCopyNewC(&strs[i].v[5], strs[i].leng - 5);
			ent->browserVerLen = strs[i].leng - 5;
		}
		else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("sswr/")))
		{
			ent->browser = Net::BrowserInfo::BT_SSWR;
			ent->browserVer = Text::StrCopyNewC(&strs[i].v[5], strs[i].leng - 5);
			ent->browserVerLen = strs[i].leng - 5;
		}
		else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("facebookexternalhit/")))
		{
			ent->browser = Net::BrowserInfo::BT_FACEBOOK;
			ent->browserVer = Text::StrCopyNewC(&strs[i].v[20], strs[i].leng - 20);
			ent->browserVerLen = strs[i].leng - 20;
		}
		else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("NetSeen/")))
		{
			ent->browser = Net::BrowserInfo::BT_NETSEEN;
			ent->browserVer = Text::StrCopyNewC(&strs[i].v[8], strs[i].leng - 8);
			ent->browserVerLen = strs[i].leng - 8;
		}
		else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("msnbot/")))
		{
			ent->browser = Net::BrowserInfo::BT_MSNBOT;
			ent->browserVer = Text::StrCopyNewC(&strs[i].v[7], strs[i].leng - 7);
			ent->browserVerLen = strs[i].leng - 7;
		}
		else if (ent->browser == Net::BrowserInfo::BT_UNKNOWN && Text::StrStartsWithC(strs[i].v, strs[i].leng, UTF8STRC("libwww-perl/")))
		{
			ent->browser = Net::BrowserInfo::BT_LIBWWW_PERL;
			ent->browserVer = Text::StrCopyNewC(&strs[i].v[12], strs[i].leng - 12);
			ent->browserVerLen = strs[i].leng - 12;
		}
		else if (Text::StrEqualsC(strs[i].v, strs[i].leng, UTF8STRC("LBBROWSER")))
		{
		}
		i++;
	}
	MemFree(sbuff);
}

Text::CString Net::UserAgentDB::FindUserAgent(Manage::OSInfo::OSType os, Net::BrowserInfo::BrowserType browser)
{
	const UTF8Char *minOS = 0;
	const UTF8Char *minBrowser = 0;
	Text::CString ua = CSTR("sswr/1.0");
	OSInt comp;
	OSInt i = 0;
	OSInt j = (sizeof(uaList) / sizeof(uaList[0])) - 1;
	while (i < j)
	{
		if (uaList[i].browser == browser && uaList[i].os == os)
		{
			if (minOS == 0)
			{
				if (uaList[i].osVer != 0)
				{
					comp = 1;
				}
				else
				{
					comp = 0;
				}
			}
			else
			{
				if (uaList[i].osVer == 0)
				{
					comp = -1;
				}
				else
				{
					comp = Text::StrCompare(uaList[i].osVer, minOS);
				}
			}
			if (comp == 0)
			{
				if (minBrowser == 0)
				{
					if (uaList[i].browserVer != 0)
					{
						comp = 1;
					}
					else
					{
						comp = 0;
					}
				}
				else
				{
					if (uaList[i].browserVer == 0)
					{
						comp = -1;
					}
					else
					{
						comp = Text::StrCompare(uaList[i].browserVer, minBrowser);
					}
				}
			}
			if (comp > 0)
			{
				ua = {uaList[i].userAgent, uaList[i].userAgentLen};
				minBrowser = uaList[i].browserVer;
				minOS = uaList[i].osVer;
			}
		}
		i++;
	}
	return ua;
}
