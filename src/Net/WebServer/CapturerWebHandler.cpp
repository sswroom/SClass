#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/MACInfo.h"
#include "Net/WebServer/CapturerWebHandler.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

#define BTTIMEOUT 30000

Bool __stdcall Net::WebServer::CaptuererWebHandler::IndexFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, WebServiceHandler *svc)
{
	Net::WebServer::CaptuererWebHandler *me = (Net::WebServer::CaptuererWebHandler*)svc;
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
		Data::ArrayList<IO::ProgCtrl::BluetoothCtlProgCtrl::DeviceInfo*> *logList = me->btCapture->GetLogList(&mutUsage);
		IO::ProgCtrl::BluetoothCtlProgCtrl::DeviceInfo *entry;
		sb.Append((const UTF8Char*)"<a href=\"btdet.html\">");
		sb.Append((const UTF8Char*)"BT Record count = ");
		sb.AppendUOSInt(logList->GetCount());
		sb.Append((const UTF8Char*)"</a><br/>\r\n");
		UOSInt i = logList->GetCount();
		UOSInt j = 0;
		while (i-- > 0)
		{
			entry = logList->GetItem(i);
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

Bool __stdcall Net::WebServer::CaptuererWebHandler::BTCurrentFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, WebServiceHandler *svc)
{
	Net::WebServer::CaptuererWebHandler *me = (Net::WebServer::CaptuererWebHandler*)svc;
	if (me->btCapture == 0)
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_IMPLEMENTED);
		return true;
	}
	Text::StringBuilderUTF8 sb;
	Data::ArrayList<IO::ProgCtrl::BluetoothCtlProgCtrl::DeviceInfo*> *entryList;

	Sync::MutexUsage mutUsage;
	entryList = me->btCapture->GetLogList(&mutUsage);
	sb.Append((const UTF8Char*)"<html><head><title>Capture Handler</title>\r\n");
	sb.Append((const UTF8Char*)"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>\r\n");
	sb.Append((const UTF8Char*)"</head><body>\r\n");
	sb.Append((const UTF8Char*)"Current Bluetooth:<br/>\r\n");
	AppendBTTable(&sb, entryList, true);
	mutUsage.EndUse();
	sb.Append((const UTF8Char*)"</body><html>");

	resp->AddDefHeaders(req);
	resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
	resp->AddContentType((const UTF8Char*)"text/html");
	resp->AddContentLength(sb.GetLength());
	resp->Write(sb.ToString(), sb.GetLength());
	return true;

}

Bool __stdcall Net::WebServer::CaptuererWebHandler::BTDetailFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, WebServiceHandler *svc)
{
	Net::WebServer::CaptuererWebHandler *me = (Net::WebServer::CaptuererWebHandler*)svc;
	if (me->btCapture == 0)
	{
		resp->ResponseError(req, Net::WebStatus::SC_NOT_IMPLEMENTED);
		return true;
	}
	Text::StringBuilderUTF8 sb;
	Data::ArrayList<IO::ProgCtrl::BluetoothCtlProgCtrl::DeviceInfo*> *entryList;

	Sync::MutexUsage mutUsage;
	entryList = me->btCapture->GetLogList(&mutUsage);
	sb.Append((const UTF8Char*)"<html><head><title>Capture Handler</title>\r\n");
	sb.Append((const UTF8Char*)"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>\r\n");
	sb.Append((const UTF8Char*)"</head><body>\r\n");
	sb.Append((const UTF8Char*)"All Bluetooth Count = ");
	sb.AppendUOSInt(entryList->GetCount());
	sb.Append((const UTF8Char*)"<br/>\r\n");
	AppendBTTable(&sb, entryList, false);
	mutUsage.EndUse();
	sb.Append((const UTF8Char*)"</body><html>");

	resp->AddDefHeaders(req);
	resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
	resp->AddContentType((const UTF8Char*)"text/html");
	resp->AddContentLength(sb.GetLength());
	resp->Write(sb.ToString(), sb.GetLength());
	return true;
}

Bool __stdcall Net::WebServer::CaptuererWebHandler::WiFiCurrentFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, WebServiceHandler *svc)
{
	Net::WebServer::CaptuererWebHandler *me = (Net::WebServer::CaptuererWebHandler*)svc;
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
	sb.Append((const UTF8Char*)"<h2>Current WiFi List</h2>\r\n");
	Int64 lastScanTimeTicks = me->wifiCapture->GetLastScanTimeTicks();
	Data::DateTime dt;
	dt.SetTicks(lastScanTimeTicks);
	dt.ToLocalTime();
	sb.Append((const UTF8Char*)"Wifi Last Scan Time = ");
	sb.AppendDate(&dt);
	sb.Append((const UTF8Char*)"<br/>\r\n");
	AppendWiFiTable(&sb, entryList, lastScanTimeTicks);
	mutUsage.EndUse();
	sb.Append((const UTF8Char*)"</body><html>");

	resp->AddDefHeaders(req);
	resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
	resp->AddContentType((const UTF8Char*)"text/html");
	resp->AddContentLength(sb.GetLength());
	resp->Write(sb.ToString(), sb.GetLength());
	return true;
}

Bool __stdcall Net::WebServer::CaptuererWebHandler::WiFiDetailFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, WebServiceHandler *svc)
{
	Net::WebServer::CaptuererWebHandler *me = (Net::WebServer::CaptuererWebHandler*)svc;
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
	AppendWiFiTable(&sb, entryList, 0);
	mutUsage.EndUse();
	sb.Append((const UTF8Char*)"</body><html>");

	resp->AddDefHeaders(req);
	resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
	resp->AddContentType((const UTF8Char*)"text/html");
	resp->AddContentLength(sb.GetLength());
	resp->Write(sb.ToString(), sb.GetLength());
	return true;
}

Bool __stdcall Net::WebServer::CaptuererWebHandler::WiFiDownloadFunc(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, WebServiceHandler *svc)
{
	Net::WebServer::CaptuererWebHandler *me = (Net::WebServer::CaptuererWebHandler*)svc;
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
		sb.AppendHexBuff(entry->mac, 6, ':', Text::LBT_NONE);
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
		sb.AppendHexBuff(entry->ouis[0], 3, 0, Text::LBT_NONE);
		sb.AppendChar(',', 1);
		sb.AppendHexBuff(entry->ouis[1], 3, 0, Text::LBT_NONE);
		sb.AppendChar(',', 1);
		sb.AppendHexBuff(entry->ouis[2], 3, 0, Text::LBT_NONE);
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
			sb.AppendHexBuff(entry->ieBuff, entry->ieLen, 0, Text::LBT_NONE);
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

void Net::WebServer::CaptuererWebHandler::AppendWiFiTable(Text::StringBuilderUTF *sb, Data::ArrayList<Net::WiFiLogFile::LogFileEntry*> *entryList, Int64 scanTime)
{
	UOSInt i;
	UOSInt j;
	Net::WiFiLogFile::LogFileEntry *entry;
	sb->Append((const UTF8Char*)"<table border=\"1\">\r\n");
	sb->Append((const UTF8Char*)"<tr><td>MAC</td><td>Vendor</td><td>SSID</td><td>PHYType</td><td>Frequency</td><td>Manufacturer</td><td>Model</td><td>S/N</td></tr>\r\n");
	i = 0;
	j = entryList->GetCount();
	while (i < j)
	{
		entry = entryList->GetItem(i);
		if (scanTime == 0 || scanTime == entry->lastScanTimeTicks)
		{
			sb->Append((const UTF8Char*)"<tr><td>");
			sb->AppendHexBuff(entry->mac, 6, ':', Text::LBT_NONE);
			sb->Append((const UTF8Char*)"</td><td>");
			sb->Append((const UTF8Char*)Net::MACInfo::GetMACInfo(entry->macInt)->name);
			sb->Append((const UTF8Char*)"</td><td>");
			if (entry->ssid)
			{
				sb->Append(entry->ssid);
			}
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

void Net::WebServer::CaptuererWebHandler::AppendBTTable(Text::StringBuilderUTF *sb, Data::ArrayList<IO::ProgCtrl::BluetoothCtlProgCtrl::DeviceInfo*> *entryList, Bool inRangeOnly)
{
	Data::DateTime dt;
	Int64 currTime;
	dt.SetCurrTimeUTC();
	currTime = dt.ToTicks();
	UOSInt k;
	UOSInt l;
	UOSInt i;
	UOSInt j;
	IO::ProgCtrl::BluetoothCtlProgCtrl::DeviceInfo *entry;
	sb->Append((const UTF8Char*)"<table border=\"1\">\r\n");
	sb->Append((const UTF8Char*)"<tr><td>MAC</td><td>Vendor</td><td>Name</td><td>RSSI</td><td>TX Power</td><td>In Range</td><td>Connected</td><td>last seen</td><td>Keys</td></tr>\r\n");
	i = 0;
	j = entryList->GetCount();
	while (i < j)
	{
		entry = entryList->GetItem(i);
		if (!inRangeOnly || (entry->inRange && (currTime - entry->lastSeenTime) <= BTTIMEOUT))
		{
			sb->Append((const UTF8Char*)"<tr><td>");
			sb->AppendHexBuff(entry->mac, 6, ':', Text::LBT_NONE);
			sb->Append((const UTF8Char*)"</td><td>");
			sb->Append((const UTF8Char*)Net::MACInfo::GetMACInfo(entry->macInt)->name);
			sb->Append((const UTF8Char*)"</td><td>");
			if (entry->name)
			{
				sb->Append(entry->name);
			}
			sb->Append((const UTF8Char*)"</td><td>");
			sb->AppendI32(entry->rssi);
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
			k = 0;
			l = entry->keys->GetCount();
			while (k < l)
			{
				if (k > 0)
				{
					sb->AppendChar(',', 1);
				}
				sb->AppendHex16((UInt16)entry->keys->GetItem(k));
				k++;
			}
			sb->Append((const UTF8Char*)"</td></tr>\r\n");
		}
		i++;
	}
	sb->Append((const UTF8Char*)"</table>");
}

Net::WebServer::CaptuererWebHandler::CaptuererWebHandler(Net::WiFiCapturer *wifiCapture, IO::BTCapturer *btCapture, IO::RadioSignalLogger *radioLogger)
{
	this->wifiCapture = wifiCapture;
	this->btCapture = btCapture;
	this->radioLogger = radioLogger;

	this->AddService((const UTF8Char*)"/index.html", Net::WebServer::IWebRequest::REQMETH_HTTP_GET, IndexFunc);
	this->AddService((const UTF8Char*)"/btcurr.html", Net::WebServer::IWebRequest::REQMETH_HTTP_GET, BTCurrentFunc);
	this->AddService((const UTF8Char*)"/btdet.html", Net::WebServer::IWebRequest::REQMETH_HTTP_GET, BTDetailFunc);
	this->AddService((const UTF8Char*)"/wificurr.html", Net::WebServer::IWebRequest::REQMETH_HTTP_GET, WiFiCurrentFunc);
	this->AddService((const UTF8Char*)"/wifidet.html", Net::WebServer::IWebRequest::REQMETH_HTTP_GET, WiFiDetailFunc);
	this->AddService((const UTF8Char*)"/wifidown.html", Net::WebServer::IWebRequest::REQMETH_HTTP_GET, WiFiDownloadFunc);
}

Net::WebServer::CaptuererWebHandler::~CaptuererWebHandler()
{
}
