#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Sort/ArtificialQuickSortC.h"
#include "Net/MACInfo.h"
#include "Net/PacketAnalyzerBluetooth.h"
#include "Net/WebServer/CapturerWebHandler.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XML.h"

#define BTTIMEOUT 30000

Bool __stdcall Net::WebServer::CapturerWebHandler::IndexFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, WebServiceHandler *svc)
{
	Net::WebServer::CapturerWebHandler *me = (Net::WebServer::CapturerWebHandler*)svc;
	Text::StringBuilderUTF8 sb;
	Data::DateTime dt;
	sb.Append((const UTF8Char*)"<html><head><title>Capture Handler</title>\r\n");
	sb.Append((const UTF8Char*)"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>\r\n");
	sb.Append((const UTF8Char*)"</head><body>\r\n");
	if (me->wifiCapture)
	{
		Int64 lastScanTimeTicks = me->wifiCapture->GetLastScanTimeTicks();
		dt.SetTicks(lastScanTimeTicks);
		dt.ToLocalTime();
		sb.Append((const UTF8Char*)"Wifi Last Scan Time = ");
		sb.AppendDate(&dt);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		Sync::MutexUsage mutUsage;
		Data::ArrayList<Net::WiFiLogFile::LogFileEntry*> *logList = me->wifiCapture->GetLogList(&mutUsage);
		sb.Append((const UTF8Char*)"<a href=\"wifidet.html\">");
		sb.Append((const UTF8Char*)"Wifi Record count = ");
		sb.AppendUOSInt(logList->GetCount());
		sb.Append((const UTF8Char*)"</a><br/>\r\n");
		UOSInt i = logList->GetCount();
		UOSInt j = 0;
		while (i-- > 0)
		{
			if (logList->GetItem(i)->lastScanTimeTicks == lastScanTimeTicks)
			{
				j++;
			}
		}
		sb.Append((const UTF8Char*)"<a href=\"wificurr.html\">");
		sb.Append((const UTF8Char*)"Wifi current count = ");
		sb.AppendUOSInt(j);
		sb.Append((const UTF8Char*)"</a><br/>\r\n");
	}
	if (me->btCapture)
	{
		Int64 currTime;
		dt.SetCurrTimeUTC();
		currTime = dt.ToTicks();
		Sync::MutexUsage mutUsage;
		IO::BTScanLog::ScanRecord3 *entry;
		Data::ArrayList<IO::BTScanLog::ScanRecord3*> logList;
		logList.AddAll(me->btCapture->GetPublicList(&mutUsage));
		sb.Append((const UTF8Char*)"<a href=\"btdetpub.html\">");
		sb.Append((const UTF8Char*)"BT Public count = ");
		sb.AppendUOSInt(logList.GetCount());
		sb.Append((const UTF8Char*)"</a><br/>\r\n");
		logList.AddAll(me->btCapture->GetRandomList(&mutUsage));
		sb.Append((const UTF8Char*)"<a href=\"btdet.html\">");
		sb.Append((const UTF8Char*)"BT Total count = ");
		sb.AppendUOSInt(logList.GetCount());
		sb.Append((const UTF8Char*)"</a><br/>\r\n");
		UOSInt i = logList.GetCount();
		UOSInt j = 0;
		while (i-- > 0)
		{
			entry = logList.GetItem(i);
			if (entry->inRange && (currTime - entry->lastSeenTime) <= BTTIMEOUT)
			{
				j++;
			}
		}
		sb.Append((const UTF8Char*)"<a href=\"btcurr.html\">");
		sb.Append((const UTF8Char*)"BT current count = ");
		sb.AppendUOSInt(j);
		sb.Append((const UTF8Char*)"</a><br/>\r\n");
	}
	if (me->radioLogger)
	{
		sb.Append((const UTF8Char*)"Log Wifi count = ");
		sb.AppendU64(me->radioLogger->GetWiFiCount());
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"Log BT count = ");
		sb.AppendU64(me->radioLogger->GetBTCount());
		sb.Append((const UTF8Char*)"<br/>\r\n");

	}
	sb.Append((const UTF8Char*)"</table></body><html>");

	resp->AddDefHeaders(req);
	resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
	resp->AddContentType((const UTF8Char*)"text/html");
	resp->AddContentLength(sb.GetLength());
	resp->Write(sb.ToString(), sb.GetLength());
	return true;
}

Bool __stdcall Net::WebServer::CapturerWebHandler::BTCurrentFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, WebServiceHandler *svc)
{
	Net::WebServer::CapturerWebHandler *me = (Net::WebServer::CapturerWebHandler*)svc;
	if (me->btCapture == 0)
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_IMPLEMENTED);
		return true;
	}
	Text::StringBuilderUTF8 sb;
	Data::ArrayList<IO::BTScanLog::ScanRecord3*> entryList;

	Sync::MutexUsage mutUsage;
	entryList.AddAll(me->btCapture->GetPublicList(&mutUsage));
	entryList.AddAll(me->btCapture->GetRandomList(&mutUsage));
	sb.Append((const UTF8Char*)"<html><head><title>Capture Handler</title>\r\n");
	sb.Append((const UTF8Char*)"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>\r\n");
	sb.Append((const UTF8Char*)"<meta http-equiv=\"refresh\" content=\"10\">\r\n");
	sb.Append((const UTF8Char*)"</head><body>\r\n");
	sb.Append((const UTF8Char*)"Current Bluetooth:<br/>\r\n");
	AppendBTTable(&sb, req, &entryList, true);
	mutUsage.EndUse();
	sb.Append((const UTF8Char*)"</body><html>");

	resp->AddDefHeaders(req);
	resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
	resp->AddContentType((const UTF8Char*)"text/html");
	resp->AddContentLength(sb.GetLength());
	resp->Write(sb.ToString(), sb.GetLength());
	return true;

}

Bool __stdcall Net::WebServer::CapturerWebHandler::BTDetailFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, WebServiceHandler *svc)
{
	Net::WebServer::CapturerWebHandler *me = (Net::WebServer::CapturerWebHandler*)svc;
	if (me->btCapture == 0)
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_IMPLEMENTED);
		return true;
	}
	Text::StringBuilderUTF8 sb;
	Data::ArrayList<IO::BTScanLog::ScanRecord3*> entryList;

	Sync::MutexUsage mutUsage;
	entryList.AddAll(me->btCapture->GetPublicList(&mutUsage));
	entryList.AddAll(me->btCapture->GetRandomList(&mutUsage));
	sb.Append((const UTF8Char*)"<html><head><title>Capture Handler</title>\r\n");
	sb.Append((const UTF8Char*)"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>\r\n");
	sb.Append((const UTF8Char*)"</head><body>\r\n");
	sb.Append((const UTF8Char*)"All Bluetooth Count = ");
	sb.AppendUOSInt(entryList.GetCount());
	sb.Append((const UTF8Char*)"<br/>\r\n");
	AppendBTTable(&sb, req, &entryList, false);
	mutUsage.EndUse();
	sb.Append((const UTF8Char*)"</body><html>");

	resp->AddDefHeaders(req);
	resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
	resp->AddContentType((const UTF8Char*)"text/html");
	resp->AddContentLength(sb.GetLength());
	resp->Write(sb.ToString(), sb.GetLength());
	return true;
}


Bool __stdcall Net::WebServer::CapturerWebHandler::BTDetailPubFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, WebServiceHandler *svc)
{
	Net::WebServer::CapturerWebHandler *me = (Net::WebServer::CapturerWebHandler*)svc;
	if (me->btCapture == 0)
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_IMPLEMENTED);
		return true;
	}
	Text::StringBuilderUTF8 sb;
	Data::ArrayList<IO::BTScanLog::ScanRecord3*> *entryList;

	Sync::MutexUsage mutUsage;
	entryList = me->btCapture->GetPublicList(&mutUsage);
	sb.Append((const UTF8Char*)"<html><head><title>Capture Handler</title>\r\n");
	sb.Append((const UTF8Char*)"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>\r\n");
	sb.Append((const UTF8Char*)"</head><body>\r\n");
	sb.Append((const UTF8Char*)"Public Bluetooth Count = ");
	sb.AppendUOSInt(entryList->GetCount());
	sb.Append((const UTF8Char*)"<br/>\r\n");
	AppendBTTable(&sb, req, entryList, false);
	mutUsage.EndUse();
	sb.Append((const UTF8Char*)"</body><html>");

	resp->AddDefHeaders(req);
	resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
	resp->AddContentType((const UTF8Char*)"text/html");
	resp->AddContentLength(sb.GetLength());
	resp->Write(sb.ToString(), sb.GetLength());
	return true;
}

Bool __stdcall Net::WebServer::CapturerWebHandler::WiFiCurrentFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, WebServiceHandler *svc)
{
	Net::WebServer::CapturerWebHandler *me = (Net::WebServer::CapturerWebHandler*)svc;
	if (me->wifiCapture == 0)
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_IMPLEMENTED);
		return true;
	}
	Text::StringBuilderUTF8 sb;
	Data::ArrayList<Net::WiFiLogFile::LogFileEntry*> *entryList;

	Sync::MutexUsage mutUsage;
	entryList = me->wifiCapture->GetLogList(&mutUsage);
	sb.Append((const UTF8Char*)"<html><head><title>Capture Handler</title>\r\n");
	sb.Append((const UTF8Char*)"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>\r\n");
	sb.Append((const UTF8Char*)"<meta http-equiv=\"refresh\" content=\"10\">\r\n");
	sb.Append((const UTF8Char*)"</head><body>\r\n");
	sb.Append((const UTF8Char*)"<h2>Current WiFi List</h2>\r\n");
	Int64 lastScanTimeTicks = me->wifiCapture->GetLastScanTimeTicks();
	Data::DateTime dt;
	dt.SetTicks(lastScanTimeTicks);
	dt.ToLocalTime();
	sb.Append((const UTF8Char*)"Wifi Last Scan Time = ");
	sb.AppendDate(&dt);
	sb.Append((const UTF8Char*)"<br/>\r\n");
	AppendWiFiTable(&sb, req, entryList, lastScanTimeTicks);
	mutUsage.EndUse();
	sb.Append((const UTF8Char*)"</body><html>");

	resp->AddDefHeaders(req);
	resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
	resp->AddContentType((const UTF8Char*)"text/html");
	resp->AddContentLength(sb.GetLength());
	resp->Write(sb.ToString(), sb.GetLength());
	return true;
}

Bool __stdcall Net::WebServer::CapturerWebHandler::WiFiDetailFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, WebServiceHandler *svc)
{
	Net::WebServer::CapturerWebHandler *me = (Net::WebServer::CapturerWebHandler*)svc;
	if (me->wifiCapture == 0)
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_IMPLEMENTED);
		return true;
	}
	Text::StringBuilderUTF8 sb;
	Data::ArrayList<Net::WiFiLogFile::LogFileEntry*> *entryList;

	Sync::MutexUsage mutUsage;
	entryList = me->wifiCapture->GetLogList(&mutUsage);
	sb.Append((const UTF8Char*)"<html><head><title>Capture Handler</title>\r\n");
	sb.Append((const UTF8Char*)"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>\r\n");
	sb.Append((const UTF8Char*)"</head><body>\r\n");
	sb.Append((const UTF8Char*)"<a href=\"wifidown.html\">Download</a><br/>\r\n");
	sb.Append((const UTF8Char*)"All WiFi Count = ");
	sb.AppendUOSInt(entryList->GetCount());
	sb.Append((const UTF8Char*)"<br/>\r\n");
	AppendWiFiTable(&sb, req, entryList, 0);
	mutUsage.EndUse();
	sb.Append((const UTF8Char*)"</body><html>");

	resp->AddDefHeaders(req);
	resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
	resp->AddContentType((const UTF8Char*)"text/html");
	resp->AddContentLength(sb.GetLength());
	resp->Write(sb.ToString(), sb.GetLength());
	return true;
}

Bool __stdcall Net::WebServer::CapturerWebHandler::WiFiDownloadFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, WebServiceHandler *svc)
{
	Net::WebServer::CapturerWebHandler *me = (Net::WebServer::CapturerWebHandler*)svc;
	if (me->wifiCapture)
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_IMPLEMENTED);
		return true;
	}
	UTF8Char sbuff[64];
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Text::StringBuilderUTF8 sb;
	Data::ArrayList<Net::WiFiLogFile::LogFileEntry*> *entryList;
	Net::WiFiLogFile::LogFileEntry *entry;
	Sync::MutexUsage mutUsage;
	entryList = me->wifiCapture->GetLogList(&mutUsage);
	i = 0;
	j = entryList->GetCount();
	while (i < j)
	{
		entry = entryList->GetItem(i);
		sb.AppendHexBuff(entry->mac, 6, ':', Text::LineBreakType::None);
		sb.AppendChar('\t', 1);
		if (entry->ssid)
		{
			sb.Append(entry->ssid);
		}
		sb.AppendChar('\t', 1);
		sb.AppendI32(entry->phyType);
		sb.AppendChar('\t', 1);
		Text::SBAppendF64(&sb, entry->freq);
		sb.AppendChar('\t', 1);
		if (entry->manuf)
			sb.Append(entry->manuf);
		sb.AppendChar('\t', 1);
		if (entry->model)
			sb.Append(entry->model);
		sb.AppendChar('\t', 1);
		if (entry->serialNum)
			sb.Append(entry->serialNum);
		sb.AppendChar('\t', 1);
		sb.AppendHexBuff(entry->ouis[0], 3, 0, Text::LineBreakType::None);
		sb.AppendChar(',', 1);
		sb.AppendHexBuff(entry->ouis[1], 3, 0, Text::LineBreakType::None);
		sb.AppendChar(',', 1);
		sb.AppendHexBuff(entry->ouis[2], 3, 0, Text::LineBreakType::None);
		sb.AppendChar('\t', 1);
		if (entry->country)
		{
			sb.Append(entry->country);
		}
		sb.AppendChar('\t', 1);
		k = 0;
		while (k < 20)
		{
			if (entry->neighbour[k] == 0)
				break;
			if (k > 0)
			{
				sb.AppendChar(',', 1);
			}
			sb.AppendHex64(entry->neighbour[k]);
			k++;
		}
		sb.Append((const UTF8Char*)"\t");
		if (entry->ieLen > 0)
		{
			sb.AppendHexBuff(entry->ieBuff, entry->ieLen, 0, Text::LineBreakType::None);
		}
		sb.Append((const UTF8Char*)"\r\n");
		i++;
	}
	mutUsage.EndUse();

	Data::DateTime dt;
	dt.SetCurrTime();
	Text::StrConcat(dt.ToString(sbuff, "yyyyMMddHHmmss"), (const UTF8Char*)".txt");
	resp->AddDefHeaders(req);
	resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
	resp->AddContentType((const UTF8Char*)"text/plain");
	resp->AddContentLength(sb.GetLength());
	resp->AddContentDisposition(true, sbuff, req->GetBrowser());
	resp->Write(sb.ToString(), sb.GetLength());
	return true;
}

void Net::WebServer::CapturerWebHandler::AppendWiFiTable(Text::StringBuilderUTF *sb, Net::WebServer::IWebRequest *req, Data::ArrayList<Net::WiFiLogFile::LogFileEntry*> *entryList, Int64 scanTime)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UInt32 sort = 0;
	UOSInt i;
	UOSInt j;
	Text::String *s;
	const UTF8Char *csptr;
	Net::WiFiLogFile::LogFileEntry *entry;
	sptr = req->GetRequestPath(sbuff, 512);
	sb->Append((const UTF8Char*)"<table border=\"1\">\r\n");
	sb->Append((const UTF8Char*)"<tr><td><a href=");
	s = Text::XML::ToNewAttrText(sbuff);
	sb->Append(s);
	s->Release();
	sb->Append((const UTF8Char*)">MAC</a></td><td>Vendor</td><td>SSID</td><td><a href=");
	Text::StrConcat(sptr, (const UTF8Char*)"?sort=1");
	s = Text::XML::ToNewAttrText(sbuff);
	sb->Append(s);
	s->Release();
	sb->Append((const UTF8Char*)">RSSI</td><td>PHYType</td><td>Frequency</td><td>Manufacturer</td><td>Model</td><td>S/N</td></tr>\r\n");

	Data::ArrayList<Net::WiFiLogFile::LogFileEntry*> sortList;
	req->GetQueryValueU32((const UTF8Char*)"sort", &sort);
	if (sort == 1)
	{
		sortList.AddAll(entryList);
		entryList = &sortList;
		ArtificialQuickSort_SortCmp((void**)sortList.GetArray(&j), WiFiLogRSSICompare, 0, (OSInt)sortList.GetCount() - 1);
	}

	i = 0;
	j = entryList->GetCount();
	while (i < j)
	{
		entry = entryList->GetItem(i);
		if (scanTime == 0 || scanTime == entry->lastScanTimeTicks)
		{
			sb->Append((const UTF8Char*)"<tr><td>");
			sb->AppendHexBuff(entry->mac, 6, ':', Text::LineBreakType::None);
			sb->Append((const UTF8Char*)"</td><td>");
			sb->Append((const UTF8Char*)Net::MACInfo::GetMACInfo(entry->macInt)->name);
			sb->Append((const UTF8Char*)"</td><td>");
			if (entry->ssid)
			{
				sb->Append(entry->ssid);
			}
			sb->Append((const UTF8Char*)"</td><td>");
			Text::SBAppendF64(sb, entry->lastRSSI);
			sb->Append((const UTF8Char*)"</td><td>");
			sb->AppendI32(entry->phyType);
			sb->Append((const UTF8Char*)"</td><td>");
			Text::SBAppendF64(sb, entry->freq);
			sb->Append((const UTF8Char*)"</td><td>");
			if (entry->manuf) sb->Append(entry->manuf);
			sb->Append((const UTF8Char*)"</td><td>");
			if (entry->model) sb->Append(entry->model);
			sb->Append((const UTF8Char*)"</td><td>");
			if (entry->serialNum) sb->Append(entry->serialNum);
			sb->Append((const UTF8Char*)"</td></tr>\r\n");
		}
		i++;
	}
	sb->Append((const UTF8Char*)"</table>");
}

void Net::WebServer::CapturerWebHandler::AppendBTTable(Text::StringBuilderUTF *sb, Net::WebServer::IWebRequest *req, Data::ArrayList<IO::BTScanLog::ScanRecord3*> *entryList, Bool inRangeOnly)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UInt32 sort = 0;
	Text::String *s;
	const UTF8Char *csptr;
	Data::DateTime dt;
	Int64 currTime;
	dt.SetCurrTimeUTC();
	currTime = dt.ToTicks();
	UOSInt i;
	UOSInt j;
	IO::BTScanLog::ScanRecord3 *entry;
	sptr = req->GetRequestPath(sbuff, 512);
	sb->Append((const UTF8Char*)"<table border=\"1\">\r\n");
	sb->Append((const UTF8Char*)"<tr><td><a href=");
	s = Text::XML::ToNewAttrText(sbuff);
	sb->Append(s);
	s->Release();
	sb->Append((const UTF8Char*)">MAC</a></td><td>Type</td><td>AddrType</td><td>Vendor</td><td>Name</td><td><a href=");
	Text::StrConcat(sptr, (const UTF8Char*)"?sort=1");
	s = Text::XML::ToNewAttrText(sbuff);
	sb->Append(s);
	s->Release();
	sb->Append((const UTF8Char*)">RSSI</a></td><td>Measure Power</td><td>TX Power</td><td>In Range</td><td>Connected</td><td>last seen</td><td>Company</td><td>AdvType</td></tr>\r\n");

	Data::ArrayList<IO::BTScanLog::ScanRecord3*> sortList;
	req->GetQueryValueU32((const UTF8Char*)"sort", &sort);
	if (sort == 1)
	{
		sortList.AddAll(entryList);
		entryList = &sortList;
		ArtificialQuickSort_SortCmp((void**)sortList.GetArray(&j), BTLogRSSICompare, 0, (OSInt)sortList.GetCount() - 1);
	}

	i = 0;
	j = entryList->GetCount();
	while (i < j)
	{
		entry = entryList->GetItem(i);
		if (!inRangeOnly || (entry->inRange && (currTime - entry->lastSeenTime) <= BTTIMEOUT))
		{
			sb->Append((const UTF8Char*)"<tr><td>");
			sb->AppendHexBuff(entry->mac, 6, ':', Text::LineBreakType::None);
			sb->Append((const UTF8Char*)"</td><td>");
			sb->Append(IO::BTScanLog::RadioTypeGetName(entry->radioType));
			sb->Append((const UTF8Char*)"</td><td>");
			sb->Append(IO::BTScanLog::AddressTypeGetName(entry->addrType));
			sb->Append((const UTF8Char*)"</td><td>");
			if (entry->addrType == IO::BTScanLog::AT_RANDOM)
			{
				switch (entry->mac[0] & 0xC0)
				{
				case 0x00:
					sb->Append((const UTF8Char*)"Non-resolvable Random");
					break;
				case 0x40:
					sb->Append((const UTF8Char*)"Resolvable Random");
					break;
				case 0xC0:
					sb->Append((const UTF8Char*)"Static Random");
					break;
				default:
					sb->Append((const UTF8Char*)"-");
					break;
				}
			}
			else
			{
				sb->Append((const UTF8Char*)Net::MACInfo::GetMACInfo(entry->macInt)->name);
			}
			sb->Append((const UTF8Char*)"</td><td>");
			if (entry->name)
			{
				sb->Append(entry->name);
			}
			sb->Append((const UTF8Char*)"</td><td>");
			sb->AppendI32(entry->rssi);
			sb->Append((const UTF8Char*)"</td><td>");
			sb->AppendI32(entry->measurePower);
			sb->Append((const UTF8Char*)"</td><td>");
			sb->AppendI32(entry->txPower);
			sb->Append((const UTF8Char*)"</td><td>");
			sb->Append((const UTF8Char*)(entry->inRange?"Y":"N"));
			sb->Append((const UTF8Char*)"</td><td>");
			sb->Append((const UTF8Char*)(entry->connected?"Y":"N"));
			sb->Append((const UTF8Char*)"</td><td>");
			dt.SetTicks(entry->lastSeenTime);
			dt.ToLocalTime();
			sb->AppendDate(&dt);
			sb->Append((const UTF8Char*)"</td><td>");
			if (entry->company == 0)
			{
				sb->Append((const UTF8Char*)"-");
			}
			else
			{
				const UTF8Char *csptr = Net::PacketAnalyzerBluetooth::CompanyGetName(entry->company);
				if (csptr)
				{
					sb->Append(csptr);
				}
				else
				{
					sb->Append((const UTF8Char*)"0x");
					sb->AppendHex16(entry->company);
				}
			}
			sb->Append((const UTF8Char*)"</td><td>");
			sb->Append(IO::BTScanLog::AdvTypeGetName(entry->advType));
			sb->Append((const UTF8Char*)"</td></tr>\r\n");
		}
		i++;
	}
	sb->Append((const UTF8Char*)"</table>");
}

OSInt __stdcall Net::WebServer::CapturerWebHandler::WiFiLogRSSICompare(void *obj1, void *obj2)
{
	Net::WiFiLogFile::LogFileEntry *log1 = (Net::WiFiLogFile::LogFileEntry*)obj1;
	Net::WiFiLogFile::LogFileEntry *log2 = (Net::WiFiLogFile::LogFileEntry*)obj2;
	if (log1->lastRSSI == log2->lastRSSI)
	{
		if (log1->ssid == log2->ssid)
		{
			return 0;
		}
		else if (log1->ssid == 0)
		{
			return -1;
		}
		else if (log2->ssid == 0)
		{
			return 1;
		}
		else
		{
			return Text::StrCompare(log1->ssid, log2->ssid);
		}
	}
	else if (log1->lastRSSI == 0)
	{
		return 1;
	}
	else if (log2->lastRSSI == 0)
	{
		return -1;
	}
	else if (log1->lastRSSI > log2->lastRSSI)
	{
		return -1;
	}
	else if (log1->lastRSSI < log2->lastRSSI)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

OSInt __stdcall Net::WebServer::CapturerWebHandler::BTLogRSSICompare(void *obj1, void *obj2)
{
	IO::BTScanLog::ScanRecord3 *log1 = (IO::BTScanLog::ScanRecord3*)obj1;
	IO::BTScanLog::ScanRecord3 *log2 = (IO::BTScanLog::ScanRecord3*)obj2;
	if (log1->rssi == log2->rssi)
	{
		if (log1->name == log2->name)
		{
			return 0;
		}
		else if (log1->name == 0)
		{
			return -1;
		}
		else if (log2->name == 0)
		{
			return 1;
		}
		else
		{
			return Text::StrCompare(log1->name, log2->name);
		}
	}
	else if (log1->rssi == 0)
	{
		return 1;
	}
	else if (log2->rssi == 0)
	{
		return -1;
	}
	else if (log1->rssi > log2->rssi)
	{
		return -1;
	}
	else if (log1->rssi < log2->rssi)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

Net::WebServer::CapturerWebHandler::CapturerWebHandler(Net::WiFiCapturer *wifiCapture, IO::BTCapturer *btCapture, IO::RadioSignalLogger *radioLogger)
{
	this->wifiCapture = wifiCapture;
	this->btCapture = btCapture;
	this->radioLogger = radioLogger;

	this->AddService((const UTF8Char*)"/index.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, IndexFunc);
	this->AddService((const UTF8Char*)"/btcurr.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, BTCurrentFunc);
	this->AddService((const UTF8Char*)"/btdet.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, BTDetailFunc);
	this->AddService((const UTF8Char*)"/btdetpub.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, BTDetailPubFunc);
	this->AddService((const UTF8Char*)"/wificurr.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, WiFiCurrentFunc);
	this->AddService((const UTF8Char*)"/wifidet.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, WiFiDetailFunc);
	this->AddService((const UTF8Char*)"/wifidown.html", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, WiFiDownloadFunc);
}

Net::WebServer::CapturerWebHandler::~CapturerWebHandler()
{
}
