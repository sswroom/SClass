#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/ByteTool.h"
#include "Data/UInt64Map.h"
#include "IO/ConsoleWriter.h"
#include "IO/Path.h"
#include "Manage/ExceptionRecorder.h"
#include "Math/Math.h"
#include "Net/MACInfo.h"
#include "Net/OSSocketFactory.h"
#include "Net/WirelessLAN.h"
#include "Net/WebServer/WebListener.h"
#include "Net/WebServer/WebStandardHandler.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"

typedef struct
{
	UInt8 mac[6];
	UInt64 imac;
	const UTF8Char *ssid;
	Int32 phyType;
	Double freq;
	const UTF8Char *manuf;
	const UTF8Char *model;
	const UTF8Char *serialNum;
	const UTF8Char *country;
	UInt8 ouis[3][3];
	UInt64 neighbour[20];
	UOSInt ieLen;
	UInt8 *ieBuff;
} WiFiEntry;

Net::WirelessLAN *wlan;
Int32 threadCnt;
Bool threadToStop;
Data::UInt64Map<WiFiEntry*> *entryMap;
Sync::Mutex *entryMut;
const UTF8Char *lastFileName;

class MyWebHandler : public Net::WebServer::WebStandardHandler
{
private:
	typedef Bool (__stdcall *RequestHandler)(MyWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp);
	Data::StringUTF8Map<RequestHandler> *reqMap;

	static Bool __stdcall DefReq(MyWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
	{
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		Data::ArrayList<WiFiEntry*> *entryList;
		WiFiEntry *entry;

		Sync::MutexUsage mutUsage(entryMut);
		entryList = entryMap->GetValues();
		i = 0;
		j = entryList->GetCount();

		sb.Append((const UTF8Char*)"<html><head><title>WiFiCapture</title>\r\n");
		sb.Append((const UTF8Char*)"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>\r\n");
		sb.Append((const UTF8Char*)"</head><body>\r\n");
		sb.Append((const UTF8Char*)"<a href=\"download\">Download</a><br/>\r\n");
		sb.Append((const UTF8Char*)"Count = ");
		sb.AppendUOSInt(j);
		sb.Append((const UTF8Char*)"<br/>\r\n");
		sb.Append((const UTF8Char*)"<table border=\"1\">\r\n");
		sb.Append((const UTF8Char*)"<tr><td>MAC</td><td>Vendor</td><td>SSID</td><td>PHYType</td><td>Frequency</td><td>Manufacturer</td><td>Model</td><td>S/N</td></tr>\r\n");
		
		while (i < j)
		{
			entry = entryList->GetItem(i);
			sb.Append((const UTF8Char*)"<tr><td>");
			sb.AppendHexBuff(entry->mac, 6, ':', Text::LBT_NONE);
			sb.Append((const UTF8Char*)"</td><td>");
			sb.Append((const UTF8Char*)Net::MACInfo::GetMACInfo(entry->imac)->name);
			sb.Append((const UTF8Char*)"</td><td>");
			if (entry->ssid)
			{
				sb.Append(entry->ssid);
			}
			sb.Append((const UTF8Char*)"</td><td>");
			sb.AppendI32(entry->phyType);
			sb.Append((const UTF8Char*)"</td><td>");
			Text::SBAppendF64(&sb, entry->freq);
			sb.Append((const UTF8Char*)"</td><td>");
			if (entry->manuf) sb.Append(entry->manuf);
			sb.Append((const UTF8Char*)"</td><td>");
			if (entry->model) sb.Append(entry->model);
			sb.Append((const UTF8Char*)"</td><td>");
			if (entry->serialNum) sb.Append(entry->serialNum);
			sb.Append((const UTF8Char*)"</td></tr>\r\n");
			i++;
		}
		mutUsage.EndUse();
		sb.Append((const UTF8Char*)"</table></body><html>");
	
		resp->AddDefHeaders(req);
		resp->AddHeader((const UTF8Char*)"Cache-Control", (const UTF8Char*)"no-cache");
		resp->AddContentType((const UTF8Char*)"text/html");
		resp->AddContentLength(sb.GetLength());
		resp->Write(sb.ToString(), sb.GetLength());
		return true;
	}

	static Bool __stdcall DownloadReq(MyWebHandler *me, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
	{
		UTF8Char sbuff[64];
		UOSInt i;
		UOSInt j;
		UOSInt k;
		Text::StringBuilderUTF8 sb;
		Data::ArrayList<WiFiEntry*> *entryList;
		WiFiEntry *entry;
		Sync::MutexUsage mutUsage(entryMut);
		entryList = entryMap->GetValues();
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

public:
	MyWebHandler()
	{
		NEW_CLASS(this->reqMap, Data::StringUTF8Map<RequestHandler>());
		this->reqMap->Put((const UTF8Char*)"/index", DefReq);
		this->reqMap->Put((const UTF8Char*)"/download", DownloadReq);
	}

	virtual ~MyWebHandler()
	{
		DEL_CLASS(this->reqMap);
	}

	virtual Bool ProcessRequest(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq)
	{
		if (this->DoRequest(req, resp, subReq))
		{
			return true;
		}
		RequestHandler reqHdlr = this->reqMap->Get(subReq);
		if (reqHdlr)
		{
			return reqHdlr(this, req, resp);
		}
		return DefReq(this, req, resp);
	}

	virtual void Release()
	{

	}

};

void StoreStatus()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Data::DateTime dt;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	OSInt si;
	IO::FileStream *fs;
	Text::UTF8Writer *writer;
	IO::Path::GetProcessFileName(sbuff);
	si = Text::StrLastIndexOf(sbuff, IO::Path::PATH_SEPERATOR);
	sptr = &sbuff[si + 1];
	dt.SetCurrTime();
	sptr = dt.ToString(sptr, "yyyyMMddHHmmss");
	sptr = Text::StrConcat(sptr, (const UTF8Char*)".txt");
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (!fs->IsError())
	{
		Text::StringBuilderUTF8 sb;
		Data::ArrayList<WiFiEntry*> *entryList;
		WiFiEntry *entry;

		NEW_CLASS(writer, Text::UTF8Writer(fs));
		writer->WriteSignature();

		Sync::MutexUsage mutUsage(entryMut);
		entryList = entryMap->GetValues();
		i = 0;
		j = entryList->GetCount();
		while (i < j)
		{
			entry = entryList->GetItem(i);
			sb.ClearStr();
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
			writer->WriteLine(sb.ToString());
			i++;
		}
		mutUsage.EndUse();

		DEL_CLASS(writer);

		if (lastFileName)
		{
			IO::Path::DeleteFile(lastFileName);
			Text::StrDelNew(lastFileName);
		}
		lastFileName = Text::StrCopyNew(sbuff);
	}
	DEL_CLASS(fs);
}

UInt32 __stdcall ScanThread(void *userObj)
{
	Net::WirelessLAN::Interface *interf = (Net::WirelessLAN::Interface*)userObj;
	Data::ArrayList<Net::WirelessLAN::BSSInfo *> *bssList;
	Net::WirelessLAN::BSSInfo *bss;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	UOSInt m;
	UInt64 imac;
	UInt8 mac[8];
	const UInt8 *macPtr;
	WiFiEntry *entry;
	const UTF8Char *namePtr;
	Data::DateTime *dt;
	UInt64 maxIMAC;
	Int32 maxRSSI;
	Int64 lastStoreTime;
	Int64 currTime;
	UOSInt ieLen;
	Net::WirelessLANIE *ie;
	const UInt8 *ieBuff;

	Sync::Interlocked::Increment(&threadCnt);
	mac[0] = 0;
	mac[1] = 0;
	NEW_CLASS(bssList, Data::ArrayList<Net::WirelessLAN::BSSInfo*>());
	NEW_CLASS(dt, Data::DateTime());
	dt->SetCurrTimeUTC();
	lastStoreTime = dt->ToTicks();	
	while (!threadToStop)
	{
		if (interf->Scan())
		{
			Sync::Thread::Sleep(10000);
			interf->GetBSSList(bssList);
			maxIMAC = 0;
			maxRSSI = -128;
			i = 0;
			j = bssList->GetCount();
			while (i < j)
			{
				bss = bssList->GetItem(i);
				macPtr = bss->GetMAC();
				mac[2] = macPtr[0];
				mac[3] = macPtr[1];
				mac[4] = macPtr[2];
				mac[5] = macPtr[3];
				mac[6] = macPtr[4];
				mac[7] = macPtr[5];
				imac = ReadMUInt64(mac);
				if (maxRSSI < bss->GetRSSI())
				{
					maxRSSI = Math::Double2Int32(bss->GetRSSI());
					maxIMAC = imac;
				}
				Sync::MutexUsage mutUsage(entryMut);
				entry = entryMap->Get(imac);
				const UInt8 *oui1 = bss->GetChipsetOUI(0);
				const UInt8 *oui2 = bss->GetChipsetOUI(1);
				const UInt8 *oui3 = bss->GetChipsetOUI(2);
				ieLen = 0;
				k = bss->GetIECount();
				while (k-- > 0)
				{
					ie = bss->GetIE(k);
					ieLen += (UOSInt)ie->GetIEBuff()[1] + 2;
				}
				if (entry == 0)
				{
					entry = MemAlloc(WiFiEntry, 1);
					MemClear(entry->neighbour, sizeof(entry->neighbour));
					entry->imac = imac;
					MemCopyNO(entry->mac, macPtr, 6);
					entry->phyType = bss->GetPHYType();
					entry->freq = bss->GetFreq();
					namePtr = bss->GetSSID();
					if (namePtr)
					{
						entry->ssid = Text::StrCopyNew(namePtr);
					}
					else
					{
						entry->ssid = 0;
					}
					namePtr = bss->GetManuf();
					entry->manuf = namePtr?Text::StrCopyNew(namePtr):0;
					namePtr = bss->GetModel();
					entry->model = namePtr?Text::StrCopyNew(namePtr):0;
					namePtr = bss->GetSN();
					entry->serialNum = namePtr?Text::StrCopyNew(namePtr):0;
					namePtr = bss->GetCountry();
					entry->country = namePtr?Text::StrCopyNew(namePtr):0;
					entry->ouis[0][0] = oui1[0];
					entry->ouis[0][1] = oui1[1];
					entry->ouis[0][2] = oui1[2];
					entry->ouis[1][0] = oui2[0];
					entry->ouis[1][1] = oui2[1];
					entry->ouis[1][2] = oui2[2];
					entry->ouis[2][0] = oui3[0];
					entry->ouis[2][1] = oui3[1];
					entry->ouis[2][2] = oui3[2];
					entry->ieLen = ieLen;
					if (ieLen > 0)
					{
						entry->ieBuff = MemAlloc(UInt8, ieLen);
						k = 0;
						l = bss->GetIECount();
						m = 0;
						while (k < l)
						{
							ie = bss->GetIE(k);
							ieBuff = ie->GetIEBuff();
							MemCopyNO(&entry->ieBuff[m], ieBuff, (UOSInt)ieBuff[1] + 2);
							m += (UOSInt)ieBuff[1] + 2;
							k++;
						}
					}
					else
					{
						entry->ieBuff = 0;
					}
					entryMap->Put(imac, entry);
				}
				else
				{
					if (entry->manuf == 0 && bss->GetManuf())
					{
						entry->manuf = Text::StrCopyNew(bss->GetManuf());
					}
					if (entry->model == 0 && bss->GetModel())
					{
						entry->model = Text::StrCopyNew(bss->GetModel());
					}
					if (entry->serialNum == 0 && bss->GetSN())
					{
						entry->serialNum = Text::StrCopyNew(bss->GetSN());
					}
					if (entry->country == 0 && bss->GetCountry())
					{
						entry->country = Text::StrCopyNew(bss->GetCountry());
					}
					UOSInt l;
					const UInt8 *oui;
					oui = oui1;
					if (oui[0] != 0 || oui[1] != 0 || oui[2] != 0)
					{
						l = 0;
						while (l < 3)
						{
							if (entry->ouis[l][0] == oui[0] && entry->ouis[l][1] == oui[1] && entry->ouis[l][2] == oui[2])
							{
								break;
							}
							else if (entry->ouis[l][0] == 0 && entry->ouis[l][1] == 0 && entry->ouis[l][2] == 0)
							{
								entry->ouis[l][0] = oui[0];
								entry->ouis[l][1] = oui[1];
								entry->ouis[l][2] = oui[2];
							}
							l++;
						}
					}

					oui = oui2;
					if (oui[0] != 0 || oui[1] != 0 || oui[2] != 0)
					{
						l = 0;
						while (l < 3)
						{
							if (entry->ouis[l][0] == oui[0] && entry->ouis[l][1] == oui[1] && entry->ouis[l][2] == oui[2])
							{
								break;
							}
							else if (entry->ouis[l][0] == 0 && entry->ouis[l][1] == 0 && entry->ouis[l][2] == 0)
							{
								entry->ouis[l][0] = oui[0];
								entry->ouis[l][1] = oui[1];
								entry->ouis[l][2] = oui[2];
							}
							l++;
						}
					}

					oui = oui3;
					if (oui[0] != 0 || oui[1] != 0 || oui[2] != 0)
					{
						l = 0;
						while (l < 3)
						{
							if (entry->ouis[l][0] == oui[0] && entry->ouis[l][1] == oui[1] && entry->ouis[l][2] == oui[2])
							{
								break;
							}
							else if (entry->ouis[l][0] == 0 && entry->ouis[l][1] == 0 && entry->ouis[l][2] == 0)
							{
								entry->ouis[l][0] = oui[0];
								entry->ouis[l][1] = oui[1];
								entry->ouis[l][2] = oui[2];
							}
							l++;
						}
					}

					if (ieLen > entry->ieLen)
					{
						if (entry->ieBuff)
						{
							MemFree(entry->ieBuff);
						}
						entry->ieBuff = MemAlloc(UInt8, ieLen);
						k = 0;
						l = bss->GetIECount();
						m = 0;
						while (k < l)
						{
							ie = bss->GetIE(k);
							ieBuff = ie->GetIEBuff();
							MemCopyNO(&entry->ieBuff[m], ieBuff, (UOSInt)ieBuff[1] + 2);
							m += (UOSInt)ieBuff[1] + 2;
							k++;
						}
					}
				}
				mutUsage.EndUse();
				i++;
			}

			if (maxRSSI >= -60 && maxRSSI < 0)
			{
				entry = entryMap->Get(maxIMAC);
				i = 0;
				j = bssList->GetCount();
				while (i < j)
				{
					bss = bssList->GetItem(i);
//					ssid = bss->GetSSID();
					MemCopyNO(&mac[2], bss->GetMAC(), 6);
					mac[0] = 0;
					mac[1] = 0;
					imac = ReadMUInt64(mac);
					if (imac != maxIMAC)
					{
						Bool found = false;
						Int32 minRSSI;
						UOSInt minIndex;
						Int32 rssi1 = Math::Double2Int32(bss->GetRSSI());
						minRSSI = 0;
						minIndex = 0;
						k = 0;
						while (k < 20)
						{
							Int8 rssi2 = (Int8)((entry->neighbour[k] >> 48) & 0xff);
							if ((entry->neighbour[k] & 0xffffffffffffLL) == imac)
							{
								found = true;
								if (rssi1 > rssi2)
								{
									entry->neighbour[k] = imac | (((UInt64)rssi1 & 0xff) << 48) | (((UInt64)bss->GetLinkQuality()) << 56);
								}
								break;
							}
							else if (entry->neighbour[k] == 0)
							{
								entry->neighbour[k] = imac | (((UInt64)rssi1 & 0xff) << 48) | (((UInt64)bss->GetLinkQuality()) << 56);
								found = true;
								break;
							}
							else if (rssi2 < minRSSI)
							{
								minRSSI = rssi2;
								minIndex = k;
							}
							
							k++;
						}

						if (!found && minRSSI < rssi1)
						{
							entry->neighbour[minIndex] = imac | (((UInt64)rssi1 & 0xff) << 48) | (((UInt64)bss->GetLinkQuality()) << 56);
						}
					}
					i++;
				}
			}

			i = bssList->GetCount();
			while (i-- > 0)
			{
				bss = bssList->GetItem(i);
				DEL_CLASS(bss);
			}
			bssList->Clear();
		}
		else
		{
			Sync::Thread::Sleep(5000);
		}
		dt->SetCurrTimeUTC();
		currTime = dt->ToTicks();
		if ((currTime - lastStoreTime) >= 600000)
		{
			lastStoreTime = currTime;
			StoreStatus();
		}
	}
	DEL_CLASS(dt);
	DEL_CLASS(bssList);
	Sync::Interlocked::Decrement(&threadCnt);
	return 0;
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	UOSInt i;
	Net::SocketFactory *sockf;
	MyWebHandler *webHdlr;
	Net::WebServer::WebListener *listener;
	UInt16 webPort = 8080;
	Manage::ExceptionRecorder *exHdlr;
	UTF8Char sbuff[512];
	lastFileName = 0;
	
	UOSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc >= 2)
	{
		Text::StrToUInt16(argv[1], &webPort);
	}
	IO::Path::GetProcessFileName(sbuff);
	IO::Path::AppendPath(sbuff, (const UTF8Char*)"Error.txt");
	NEW_CLASS(exHdlr, Manage::ExceptionRecorder(sbuff, Manage::ExceptionRecorder::EA_RESTART));
	NEW_CLASS(entryMap, Data::UInt64Map<WiFiEntry*>());
	NEW_CLASS(entryMut, Sync::Mutex());
	NEW_CLASS(wlan, Net::WirelessLAN());
	threadCnt = 0;
	threadToStop = false;
	if (wlan->IsError())
	{
		console.WriteLine((const UTF8Char*)"Error in initializing WiFi");
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		NEW_CLASS(sockf, Net::OSSocketFactory(true));
		NEW_CLASS(webHdlr, MyWebHandler());
		NEW_CLASS(listener, Net::WebServer::WebListener(sockf, webHdlr, webPort, 120, 4, (const UTF8Char*)"WiFiCapture/1.0", false, true));
		if (listener->IsError())
		{
			sb.Append((const UTF8Char*)"Error in starting web server at port ");
			sb.AppendI32(webPort);
			console.WriteLine(sb.ToString());
		}
		else
		{
			const UTF8Char *namePtr;
			Data::ArrayList<Net::WirelessLAN::Interface*> interfaces;
			Net::WirelessLAN::Interface *interface;
			wlan->GetInterfaces(&interfaces);
			i = interfaces.GetCount();
			Bool found = false;
			while (i-- > 0)
			{
				interface = interfaces.GetItem(i);
				namePtr = interface->GetName();
				if (Text::StrStartsWith(namePtr, (const UTF8Char*)"rai"))
				{

				}
				else
				{
					Sync::Thread::Create(ScanThread, interface);
					found = true;
				}
			}

			if (!found)
			{
				console.WriteLine((const UTF8Char*)"No WiFi interface found");
			}
			else
			{
				console.WriteLine((const UTF8Char*)"WiFiCapture started");
				progCtrl->WaitForExit(progCtrl);
				threadToStop = true;
				StoreStatus();
				while (threadCnt > 0)
				{
					Sync::Thread::Sleep(10);
				}
			}
			
			i = interfaces.GetCount();
			while (i-- > 0)
			{
				interface = interfaces.GetItem(i);
				DEL_CLASS(interface);
			}			
		}
		DEL_CLASS(listener);
		DEL_CLASS(webHdlr);
		DEL_CLASS(sockf);
	}

	Data::ArrayList<WiFiEntry*> *entryList;
	WiFiEntry *entry;
	entryList = entryMap->GetValues();
	i = entryList->GetCount();
	while (i-- > 0)
	{
		entry = entryList->GetItem(i);
		SDEL_TEXT(entry->ssid);
		SDEL_TEXT(entry->manuf);
		SDEL_TEXT(entry->model);
		SDEL_TEXT(entry->serialNum);
		SDEL_TEXT(entry->country);
		if (entry->ieBuff)
		{
			MemFree(entry->ieBuff);
		}
		MemFree(entry);
	}
	DEL_CLASS(entryMut);
	DEL_CLASS(entryMap);
	DEL_CLASS(wlan);
	DEL_CLASS(exHdlr);
	SDEL_TEXT(lastFileName);
	return 0;
}
