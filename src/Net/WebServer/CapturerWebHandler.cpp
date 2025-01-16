#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Sort/ArtificialQuickSortFunc.h"
#include "Net/MACInfo.h"
#include "Net/PacketAnalyzerBluetooth.h"
#include "Net/WebServer/CapturerWebHandler.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XML.h"

#define BTTIMEOUT 30000

Bool __stdcall Net::WebServer::CapturerWebHandler::IndexFunc(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svc)
{
	NN<Net::WebServer::CapturerWebHandler> me = NN<Net::WebServer::CapturerWebHandler>::ConvertFrom(svc);
	Text::StringBuilderUTF8 sb;
	NN<Net::WiFiCapturer> wifiCapture;
	sb.AppendC(UTF8STRC("<html><head><title>Capture Handler</title>\r\n"));
	sb.AppendC(UTF8STRC("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>\r\n"));
	sb.AppendC(UTF8STRC("</head><body>\r\n"));
	if (me->wifiCapture.SetTo(wifiCapture))
	{
		Data::Timestamp lastScanTime = wifiCapture->GetLastScanTime().ToLocalTime();
		sb.AppendC(UTF8STRC("Wifi Last Scan Time = "));
		sb.AppendTSNoZone(lastScanTime);
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		Sync::MutexUsage mutUsage;
		NN<Data::ArrayListNN<Net::WiFiLogFile::LogFileEntry>> logList = wifiCapture->GetLogList(mutUsage);
		sb.AppendC(UTF8STRC("<a href=\"wifidet.html\">"));
		sb.AppendC(UTF8STRC("Wifi Record count = "));
		sb.AppendUOSInt(logList->GetCount());
		sb.AppendC(UTF8STRC("</a><br/>\r\n"));
		UOSInt i = logList->GetCount();
		UOSInt j = 0;
		while (i-- > 0)
		{
			if (logList->GetItemNoCheck(i)->lastScanTime == lastScanTime)
			{
				j++;
			}
		}
		sb.AppendC(UTF8STRC("<a href=\"wificurr.html\">"));
		sb.AppendC(UTF8STRC("Wifi current count = "));
		sb.AppendUOSInt(j);
		sb.AppendC(UTF8STRC("</a><br/>\r\n"));
	}
	NN<IO::BTCapturer> btCapture;
	if (me->btCapture.SetTo(btCapture))
	{
		Int64 currTime = Data::DateTimeUtil::GetCurrTimeMillis();
		Sync::MutexUsage mutUsage;
		NN<IO::BTScanLog::ScanRecord3> entry;
		Data::ArrayListNN<IO::BTScanLog::ScanRecord3> logList;
		logList.AddAll(btCapture->GetPublicList(mutUsage));
		sb.AppendC(UTF8STRC("<a href=\"btdetpub.html\">"));
		sb.AppendC(UTF8STRC("BT Public count = "));
		sb.AppendUOSInt(logList.GetCount());
		sb.AppendC(UTF8STRC("</a><br/>\r\n"));
		logList.AddAll(btCapture->GetRandomList(mutUsage));
		sb.AppendC(UTF8STRC("<a href=\"btdet.html\">"));
		sb.AppendC(UTF8STRC("BT Total count = "));
		sb.AppendUOSInt(logList.GetCount());
		sb.AppendC(UTF8STRC("</a><br/>\r\n"));
		UOSInt i = logList.GetCount();
		UOSInt j = 0;
		while (i-- > 0)
		{
			entry = logList.GetItemNoCheck(i);
			if (entry->inRange && (currTime - entry->lastSeenTime) <= BTTIMEOUT)
			{
				j++;
			}
		}
		sb.AppendC(UTF8STRC("<a href=\"btcurr.html\">"));
		sb.AppendC(UTF8STRC("BT current count = "));
		sb.AppendUOSInt(j);
		sb.AppendC(UTF8STRC("</a><br/>\r\n"));
	}
	if (me->radioLogger)
	{
		sb.AppendC(UTF8STRC("Log Wifi count = "));
		sb.AppendU64(me->radioLogger->GetWiFiCount());
		sb.AppendC(UTF8STRC("<br/>\r\n"));
		sb.AppendC(UTF8STRC("Log BT count = "));
		sb.AppendU64(me->radioLogger->GetBTCount());
		sb.AppendC(UTF8STRC("<br/>\r\n"));

	}
	sb.AppendC(UTF8STRC("</table></body><html>"));

	me->AddResponseHeaders(req, resp);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(sb.GetLength());
	resp->Write(sb.ToByteArray());
	return true;
}

Bool __stdcall Net::WebServer::CapturerWebHandler::BTCurrentFunc(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svc)
{
	NN<Net::WebServer::CapturerWebHandler> me = NN<Net::WebServer::CapturerWebHandler>::ConvertFrom(svc);
	NN<IO::BTCapturer> btCapture;
	if (!me->btCapture.SetTo(btCapture))
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_IMPLEMENTED);
		return true;
	}
	Text::StringBuilderUTF8 sb;
	Data::ArrayListNN<IO::BTScanLog::ScanRecord3> entryList;

	Sync::MutexUsage mutUsage;
	entryList.AddAll(btCapture->GetPublicList(mutUsage));
	entryList.AddAll(btCapture->GetRandomList(mutUsage));
	sb.AppendC(UTF8STRC("<html><head><title>Capture Handler</title>\r\n"));
	sb.AppendC(UTF8STRC("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>\r\n"));
	sb.AppendC(UTF8STRC("<meta http-equiv=\"refresh\" content=\"10\">\r\n"));
	sb.AppendC(UTF8STRC("</head><body>\r\n"));
	sb.AppendC(UTF8STRC("Current Bluetooth:<br/>\r\n"));
	AppendBTTable(sb, req, entryList, true);
	mutUsage.EndUse();
	sb.AppendC(UTF8STRC("</body><html>"));

	me->AddResponseHeaders(req, resp);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(sb.GetLength());
	resp->Write(sb.ToByteArray());
	return true;

}

Bool __stdcall Net::WebServer::CapturerWebHandler::BTDetailFunc(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svc)
{
	NN<Net::WebServer::CapturerWebHandler> me = NN<Net::WebServer::CapturerWebHandler>::ConvertFrom(svc);
	NN<IO::BTCapturer> btCapture;
	if (!me->btCapture.SetTo(btCapture))
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_IMPLEMENTED);
		return true;
	}
	Text::StringBuilderUTF8 sb;
	Data::ArrayListNN<IO::BTScanLog::ScanRecord3> entryList;

	Sync::MutexUsage mutUsage;
	entryList.AddAll(btCapture->GetPublicList(mutUsage));
	entryList.AddAll(btCapture->GetRandomList(mutUsage));
	sb.AppendC(UTF8STRC("<html><head><title>Capture Handler</title>\r\n"));
	sb.AppendC(UTF8STRC("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>\r\n"));
	sb.AppendC(UTF8STRC("</head><body>\r\n"));
	sb.AppendC(UTF8STRC("All Bluetooth Count = "));
	sb.AppendUOSInt(entryList.GetCount());
	sb.AppendC(UTF8STRC("<br/>\r\n"));
	AppendBTTable(sb, req, entryList, false);
	mutUsage.EndUse();
	sb.AppendC(UTF8STRC("</body><html>"));

	me->AddResponseHeaders(req, resp);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(sb.GetLength());
	resp->Write(sb.ToByteArray());
	return true;
}


Bool __stdcall Net::WebServer::CapturerWebHandler::BTDetailPubFunc(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svc)
{
	NN<Net::WebServer::CapturerWebHandler> me = NN<Net::WebServer::CapturerWebHandler>::ConvertFrom(svc);
	NN<IO::BTCapturer> btCapture;
	if (!me->btCapture.SetTo(btCapture))
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_IMPLEMENTED);
		return true;
	}
	Text::StringBuilderUTF8 sb;
	NN<const Data::ReadingListNN<IO::BTScanLog::ScanRecord3>> entryList;

	Sync::MutexUsage mutUsage;
	entryList = btCapture->GetPublicList(mutUsage);
	sb.AppendC(UTF8STRC("<html><head><title>Capture Handler</title>\r\n"));
	sb.AppendC(UTF8STRC("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>\r\n"));
	sb.AppendC(UTF8STRC("</head><body>\r\n"));
	sb.AppendC(UTF8STRC("Public Bluetooth Count = "));
	sb.AppendUOSInt(entryList->GetCount());
	sb.AppendC(UTF8STRC("<br/>\r\n"));
	AppendBTTable(sb, req, entryList, false);
	mutUsage.EndUse();
	sb.AppendC(UTF8STRC("</body><html>"));

	me->AddResponseHeaders(req, resp);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(sb.GetLength());
	resp->Write(sb.ToByteArray());
	return true;
}

Bool __stdcall Net::WebServer::CapturerWebHandler::WiFiCurrentFunc(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svc)
{
	NN<Net::WebServer::CapturerWebHandler> me = NN<Net::WebServer::CapturerWebHandler>::ConvertFrom(svc);
	NN<Net::WiFiCapturer> wifiCapture;
	if (!me->wifiCapture.SetTo(wifiCapture))
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_IMPLEMENTED);
		return true;
	}
	Text::StringBuilderUTF8 sb;
	NN<Data::ArrayListNN<Net::WiFiLogFile::LogFileEntry>> entryList;

	Sync::MutexUsage mutUsage;
	entryList = wifiCapture->GetLogList(mutUsage);
	sb.AppendC(UTF8STRC("<html><head><title>Capture Handler</title>\r\n"));
	sb.AppendC(UTF8STRC("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>\r\n"));
	sb.AppendC(UTF8STRC("<meta http-equiv=\"refresh\" content=\"10\">\r\n"));
	sb.AppendC(UTF8STRC("</head><body>\r\n"));
	sb.AppendC(UTF8STRC("<h2>Current WiFi List</h2>\r\n"));
	Data::Timestamp lastScanTime = wifiCapture->GetLastScanTime().ToLocalTime();
	sb.AppendC(UTF8STRC("Wifi Last Scan Time = "));
	sb.AppendTSNoZone(lastScanTime);
	sb.AppendC(UTF8STRC("<br/>\r\n"));
	AppendWiFiTable(sb, req, entryList, lastScanTime);
	mutUsage.EndUse();
	sb.AppendC(UTF8STRC("</body><html>"));

	me->AddResponseHeaders(req, resp);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(sb.GetLength());
	resp->Write(sb.ToByteArray());
	return true;
}

Bool __stdcall Net::WebServer::CapturerWebHandler::WiFiDetailFunc(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svc)
{
	NN<Net::WebServer::CapturerWebHandler> me = NN<Net::WebServer::CapturerWebHandler>::ConvertFrom(svc);
	NN<Net::WiFiCapturer> wifiCapture;
	if (!me->wifiCapture.SetTo(wifiCapture))
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_IMPLEMENTED);
		return true;
	}
	Text::StringBuilderUTF8 sb;
	NN<Data::ArrayListNN<Net::WiFiLogFile::LogFileEntry>> entryList;

	Sync::MutexUsage mutUsage;
	entryList = wifiCapture->GetLogList(mutUsage);
	sb.AppendC(UTF8STRC("<html><head><title>Capture Handler</title>\r\n"));
	sb.AppendC(UTF8STRC("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>\r\n"));
	sb.AppendC(UTF8STRC("</head><body>\r\n"));
	sb.AppendC(UTF8STRC("<a href=\"wifidown.html\">Download</a><br/>\r\n"));
	sb.AppendC(UTF8STRC("All WiFi Count = "));
	sb.AppendUOSInt(entryList->GetCount());
	sb.AppendC(UTF8STRC("<br/>\r\n"));
	AppendWiFiTable(sb, req, entryList, 0);
	mutUsage.EndUse();
	sb.AppendC(UTF8STRC("</body><html>"));

	me->AddResponseHeaders(req, resp);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->AddContentType(CSTR("text/html"));
	resp->AddContentLength(sb.GetLength());
	resp->Write(sb.ToByteArray());
	return true;
}

Bool __stdcall Net::WebServer::CapturerWebHandler::WiFiDownloadFunc(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NN<WebServiceHandler> svc)
{
	NN<Net::WebServer::CapturerWebHandler> me = NN<Net::WebServer::CapturerWebHandler>::ConvertFrom(svc);
	NN<Net::WiFiCapturer> wifiCapture;
	if (!me->wifiCapture.SetTo(wifiCapture))
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_IMPLEMENTED);
		return true;
	}
	UTF8Char sbuff[64];
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Text::StringBuilderUTF8 sb;
	UnsafeArray<UInt8> ieBuff;
	NN<Data::ArrayListNN<Net::WiFiLogFile::LogFileEntry>> entryList;
	NN<Net::WiFiLogFile::LogFileEntry> entry;
	Sync::MutexUsage mutUsage;
	entryList = wifiCapture->GetLogList(mutUsage);
	i = 0;
	j = entryList->GetCount();
	while (i < j)
	{
		entry = entryList->GetItemNoCheck(i);
		sb.AppendHexBuff(entry->mac, 6, ':', Text::LineBreakType::None);
		sb.AppendUTF8Char('\t');
		sb.Append(entry->ssid);
		sb.AppendUTF8Char('\t');
		sb.AppendI32(entry->phyType);
		sb.AppendUTF8Char('\t');
		sb.AppendDouble(entry->freq);
		sb.AppendUTF8Char('\t');
		sb.AppendOpt(entry->manuf);
		sb.AppendUTF8Char('\t');
		sb.AppendOpt(entry->model);
		sb.AppendUTF8Char('\t');
		sb.AppendOpt(entry->serialNum);
		sb.AppendUTF8Char('\t');
		sb.AppendHexBuff(entry->ouis[0], 3, 0, Text::LineBreakType::None);
		sb.AppendUTF8Char(',');
		sb.AppendHexBuff(entry->ouis[1], 3, 0, Text::LineBreakType::None);
		sb.AppendUTF8Char(',');
		sb.AppendHexBuff(entry->ouis[2], 3, 0, Text::LineBreakType::None);
		sb.AppendUTF8Char('\t');
		sb.AppendOpt(entry->country);
		sb.AppendUTF8Char('\t');
		k = 0;
		while (k < 20)
		{
			if (entry->neighbour[k] == 0)
				break;
			if (k > 0)
			{
				sb.AppendUTF8Char(',');
			}
			sb.AppendHex64(entry->neighbour[k]);
			k++;
		}
		sb.AppendC(UTF8STRC("\t"));
		if (entry->ieLen > 0 && entry->ieBuff.SetTo(ieBuff))
		{
			sb.AppendHexBuff(ieBuff, entry->ieLen, 0, Text::LineBreakType::None);
		}
		sb.AppendC(UTF8STRC("\r\n"));
		i++;
	}
	mutUsage.EndUse();

	Data::DateTime dt;
	dt.SetCurrTime();
	Text::StrConcatC(dt.ToString(sbuff, "yyyyMMddHHmmss"), UTF8STRC(".txt"));
	me->AddResponseHeaders(req, resp);
	resp->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
	resp->AddContentType(CSTR("text/plain"));
	resp->AddContentLength(sb.GetLength());
	resp->AddContentDisposition(true, sbuff, req->GetBrowser());
	resp->Write(sb.ToByteArray());
	return true;
}

void Net::WebServer::CapturerWebHandler::AppendWiFiTable(NN<Text::StringBuilderUTF8> sb, NN<Net::WebServer::IWebRequest> req, NN<Data::ArrayListNN<Net::WiFiLogFile::LogFileEntry>> entryList, const Data::Timestamp &scanTime)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UInt32 sort = 0;
	UOSInt i;
	UOSInt j;
	NN<Text::String> s;
	NN<Net::WiFiLogFile::LogFileEntry> entry;
	sptr = req->GetRequestPath(sbuff, 512);
	sb->AppendC(UTF8STRC("<table border=\"1\">\r\n"));
	sb->AppendC(UTF8STRC("<tr><td><a href="));
	s = Text::XML::ToNewAttrText(UARR(sbuff));
	sb->Append(s);
	s->Release();
	sb->AppendC(UTF8STRC(">MAC</a></td><td>Vendor</td><td>SSID</td><td><a href="));
	Text::StrConcatC(sptr, UTF8STRC("?sort=1"));
	s = Text::XML::ToNewAttrText(UARR(sbuff));
	sb->Append(s);
	s->Release();
	sb->AppendC(UTF8STRC(">RSSI</td><td>PHYType</td><td>Frequency</td><td>Manufacturer</td><td>Model</td><td>S/N</td></tr>\r\n"));

	Data::ArrayListNN<Net::WiFiLogFile::LogFileEntry> sortList;
	req->GetQueryValueU32(CSTR("sort"), sort);
	if (sort == 1)
	{
		sortList.AddAll(entryList);
		entryList = sortList;
		Data::Sort::ArtificialQuickSortFunc<NN<Net::WiFiLogFile::LogFileEntry>>::Sort(sortList, WiFiLogRSSICompare);
	}

	i = 0;
	j = entryList->GetCount();
	while (i < j)
	{
		entry = entryList->GetItemNoCheck(i);
		if (scanTime.IsNull() || scanTime == entry->lastScanTime)
		{
			sb->AppendC(UTF8STRC("<tr><td>"));
			sb->AppendHexBuff(entry->mac, 6, ':', Text::LineBreakType::None);
			sb->AppendC(UTF8STRC("</td><td>"));
			NN<const Net::MACInfo::MACEntry> macEntry = Net::MACInfo::GetMACInfo(entry->macInt);
			sb->AppendC(macEntry->name, macEntry->nameLen);
			sb->AppendC(UTF8STRC("</td><td>"));
			sb->Append(entry->ssid);
			sb->AppendC(UTF8STRC("</td><td>"));
			Text::SBAppendF64(sb, entry->lastRSSI);
			sb->AppendC(UTF8STRC("</td><td>"));
			sb->AppendI32(entry->phyType);
			sb->AppendC(UTF8STRC("</td><td>"));
			Text::SBAppendF64(sb, entry->freq);
			sb->AppendC(UTF8STRC("</td><td>"));
			sb->AppendOpt(entry->manuf);
			sb->AppendC(UTF8STRC("</td><td>"));
			sb->AppendOpt(entry->model);
			sb->AppendC(UTF8STRC("</td><td>"));
			sb->AppendOpt(entry->serialNum);
			sb->AppendC(UTF8STRC("</td></tr>\r\n"));
		}
		i++;
	}
	sb->AppendC(UTF8STRC("</table>"));
}

void Net::WebServer::CapturerWebHandler::AppendBTTable(NN<Text::StringBuilderUTF8> sb, NN<Net::WebServer::IWebRequest> req, NN<const Data::ReadingListNN<IO::BTScanLog::ScanRecord3>> entryList, Bool inRangeOnly)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UInt32 sort = 0;
	NN<Text::String> s;
	Int64 currTime = Data::DateTimeUtil::GetCurrTimeMillis();
	UOSInt i;
	UOSInt j;
	NN<IO::BTScanLog::ScanRecord3> entry;
	sptr = req->GetRequestPath(sbuff, 512);
	sb->AppendC(UTF8STRC("<table border=\"1\">\r\n"));
	sb->AppendC(UTF8STRC("<tr><td><a href="));
	s = Text::XML::ToNewAttrText(UARR(sbuff));
	sb->Append(s);
	s->Release();
	sb->AppendC(UTF8STRC(">MAC</a></td><td>Type</td><td>AddrType</td><td>Vendor</td><td>Name</td><td><a href="));
	Text::StrConcatC(sptr, UTF8STRC("?sort=1"));
	s = Text::XML::ToNewAttrText(UARR(sbuff));
	sb->Append(s);
	s->Release();
	sb->AppendC(UTF8STRC(">RSSI</a></td><td>Measure Power</td><td>TX Power</td><td>In Range</td><td>Connected</td><td>last seen</td><td>Company</td><td>AdvType</td></tr>\r\n"));

	Data::ArrayListNN<IO::BTScanLog::ScanRecord3> sortList;
	req->GetQueryValueU32(CSTR("sort"), sort);
	if (sort == 1)
	{
		sortList.AddAll(entryList);
		entryList = sortList;
		Data::Sort::ArtificialQuickSortFunc<NN<IO::BTScanLog::ScanRecord3>>::Sort(sortList, BTLogRSSICompare);
	}

	i = 0;
	j = entryList->GetCount();
	while (i < j)
	{
		entry = entryList->GetItemNoCheck(i);
		if (!inRangeOnly || (entry->inRange && (currTime - entry->lastSeenTime) <= BTTIMEOUT))
		{
			sb->AppendC(UTF8STRC("<tr><td>"));
			sb->AppendHexBuff(entry->mac, 6, ':', Text::LineBreakType::None);
			sb->AppendC(UTF8STRC("</td><td>"));
			sb->Append(IO::BTScanLog::RadioTypeGetName(entry->radioType));
			sb->AppendC(UTF8STRC("</td><td>"));
			sb->Append(IO::BTScanLog::AddressTypeGetName(entry->addrType));
			sb->AppendC(UTF8STRC("</td><td>"));
			if (entry->addrType == IO::BTScanLog::AT_RANDOM)
			{
				switch (entry->mac[0] & 0xC0)
				{
				case 0x00:
					sb->AppendC(UTF8STRC("Non-resolvable Random"));
					break;
				case 0x40:
					sb->AppendC(UTF8STRC("Resolvable Random"));
					break;
				case 0xC0:
					sb->AppendC(UTF8STRC("Static Random"));
					break;
				default:
					sb->AppendC(UTF8STRC("-"));
					break;
				}
			}
			else
			{
				NN<const Net::MACInfo::MACEntry> macEntry = Net::MACInfo::GetMACInfo(entry->macInt);
				sb->AppendC(macEntry->name, macEntry->nameLen);
			}
			sb->AppendC(UTF8STRC("</td><td>"));
			if (entry->name.SetTo(s))
			{
				sb->Append(s);
			}
			sb->AppendC(UTF8STRC("</td><td>"));
			sb->AppendI32(entry->rssi);
			sb->AppendC(UTF8STRC("</td><td>"));
			sb->AppendI32(entry->measurePower);
			sb->AppendC(UTF8STRC("</td><td>"));
			sb->AppendI32(entry->txPower);
			sb->AppendC(UTF8STRC("</td><td>"));
			sb->AppendC((const UTF8Char*)(entry->inRange?"Y":"N"), 1);
			sb->AppendC(UTF8STRC("</td><td>"));
			sb->AppendC((const UTF8Char*)(entry->connected?"Y":"N"), 1);
			sb->AppendC(UTF8STRC("</td><td>"));
			sb->AppendTSNoZone(Data::Timestamp(entry->lastSeenTime, Data::DateTimeUtil::GetLocalTzQhr()));
			sb->AppendC(UTF8STRC("</td><td>"));
			if (entry->company == 0)
			{
				sb->AppendC(UTF8STRC("-"));
			}
			else
			{
				Text::CStringNN cstr;
				if (Net::PacketAnalyzerBluetooth::CompanyGetName(entry->company).SetTo(cstr))
				{
					sb->Append(cstr);
				}
				else
				{
					sb->AppendC(UTF8STRC("0x"));
					sb->AppendHex16(entry->company);
				}
			}
			sb->AppendC(UTF8STRC("</td><td>"));
			sb->Append(IO::BTScanLog::AdvTypeGetName(entry->advType));
			sb->AppendC(UTF8STRC("</td></tr>\r\n"));
		}
		i++;
	}
	sb->AppendC(UTF8STRC("</table>"));
}

OSInt __stdcall Net::WebServer::CapturerWebHandler::WiFiLogRSSICompare(NN<Net::WiFiLogFile::LogFileEntry> obj1, NN<Net::WiFiLogFile::LogFileEntry> obj2)
{
	if (obj1->lastRSSI == obj2->lastRSSI)
	{
		if (obj1->ssid == obj2->ssid)
		{
			return 0;
		}
		else
		{
			return obj1->ssid->CompareTo(obj2->ssid);
		}
	}
	else if (obj1->lastRSSI == 0)
	{
		return 1;
	}
	else if (obj2->lastRSSI == 0)
	{
		return -1;
	}
	else if (obj1->lastRSSI > obj2->lastRSSI)
	{
		return -1;
	}
	else if (obj1->lastRSSI < obj2->lastRSSI)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

OSInt __stdcall Net::WebServer::CapturerWebHandler::BTLogRSSICompare(NN<IO::BTScanLog::ScanRecord3> obj1, NN<IO::BTScanLog::ScanRecord3> obj2)
{
	NN<Text::String> name1;
	NN<Text::String> name2;
	if (obj1->rssi == obj2->rssi)
	{
		if (obj1->name == obj2->name)
		{
			return 0;
		}
		else if (!obj1->name.SetTo(name1))
		{
			return -1;
		}
		else if (!obj2->name.SetTo(name2))
		{
			return 1;
		}
		else
		{
			return Text::StrCompare(name1->v, name2->v);
		}
	}
	else if (obj1->rssi == 0)
	{
		return 1;
	}
	else if (obj2->rssi == 0)
	{
		return -1;
	}
	else if (obj1->rssi > obj2->rssi)
	{
		return -1;
	}
	else if (obj1->rssi < obj2->rssi)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

Net::WebServer::CapturerWebHandler::CapturerWebHandler(Optional<Net::WiFiCapturer> wifiCapture, Optional<IO::BTCapturer> btCapture, IO::RadioSignalLogger *radioLogger)
{
	this->wifiCapture = wifiCapture;
	this->btCapture = btCapture;
	this->radioLogger = radioLogger;

	this->AddService(CSTR("/index.html"), Net::WebUtil::RequestMethod::HTTP_GET, IndexFunc);
	this->AddService(CSTR("/btcurr.html"), Net::WebUtil::RequestMethod::HTTP_GET, BTCurrentFunc);
	this->AddService(CSTR("/btdet.html"), Net::WebUtil::RequestMethod::HTTP_GET, BTDetailFunc);
	this->AddService(CSTR("/btdetpub.html"), Net::WebUtil::RequestMethod::HTTP_GET, BTDetailPubFunc);
	this->AddService(CSTR("/wificurr.html"), Net::WebUtil::RequestMethod::HTTP_GET, WiFiCurrentFunc);
	this->AddService(CSTR("/wifidet.html"), Net::WebUtil::RequestMethod::HTTP_GET, WiFiDetailFunc);
	this->AddService(CSTR("/wifidown.html"), Net::WebUtil::RequestMethod::HTTP_GET, WiFiDownloadFunc);
}

Net::WebServer::CapturerWebHandler::~CapturerWebHandler()
{
}
