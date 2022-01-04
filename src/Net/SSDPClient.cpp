#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/SSDPClient.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XMLReader.h"

void __stdcall Net::SSDPClient::OnPacketRecv(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData)
{
	Net::SSDPClient *me = (Net::SSDPClient*)userData;
	if (Text::StrStartsWith(buff, (const UTF8Char*)"HTTP/1.1 200 OK"))
	{
		if (addr->addrType == Net::AddrType::IPv4)
		{
			const UTF8Char *time = 0;
			const UTF8Char *location = 0;
			const UTF8Char *opt = 0;
			const UTF8Char *server = 0;
			const UTF8Char *st = 0;
			const UTF8Char *usn = 0;
			const UTF8Char *userAgent = 0;
			Text::StringBuilderUTF8 sb;
			sb.AppendC(buff, dataSize);
			UTF8Char *sarr[2];
			UOSInt lineCnt;
			sarr[1] = sb.ToString();
			while (true)
			{
				lineCnt = Text::StrSplitLine(sarr, 2, sarr[1]);
				if (Text::StrStartsWithICase(sarr[0], (const UTF8Char*)"DATE: "))
				{
					time = &sarr[0][6];
				}
				else if (Text::StrStartsWithICase(sarr[0], (const UTF8Char*)"LOCATION: "))
				{
					location = &sarr[0][10];
				}
				else if (Text::StrStartsWithICase(sarr[0], (const UTF8Char*)"OPT: "))
				{
					opt = &sarr[0][5];
				}
				else if (Text::StrStartsWithICase(sarr[0], (const UTF8Char*)"ST: "))
				{
					st = &sarr[0][4];
				}
				else if (Text::StrStartsWithICase(sarr[0], (const UTF8Char*)"USN: "))
				{
					usn = &sarr[0][5];
				}
				else if (Text::StrStartsWithICase(sarr[0], (const UTF8Char*)"X-USER-AGENT: "))
				{
					userAgent = &sarr[0][14];
				}

				if (lineCnt != 2)
				{
					break;
				}
			}
			
			if (usn)
			{
				Sync::MutexUsage mutUsage(me->mut);
				UInt32 ip = ReadNUInt32(addr->addr);
				SSDPDevice *dev = me->devMap->Get(ip);
				SSDPService *svc;
				if (dev == 0)
				{
					dev = MemAlloc(SSDPDevice, 1);
					dev->addr = *addr;
					NEW_CLASS(dev->services, Data::ArrayList<SSDPService*>());
					me->devMap->Put(ip, dev);
				}
				Bool found = false;
				UOSInt i = dev->services->GetCount();
				while (i-- > 0)
				{
					svc = dev->services->GetItem(i);
					if (Text::StrEquals(usn, svc->usn))
					{
						found = true;
					}
				}
				if (!found)
				{
					svc = MemAlloc(SSDPService, 1);
					if (time)
					{
						Data::DateTime dt;
						dt.SetValue(time);
						svc->time = dt.ToTicks();
					}
					else
					{
						svc->time = 0;
					}
					svc->location = SCOPY_TEXT(location);
					svc->opt = SCOPY_TEXT(opt);
					svc->server = SCOPY_TEXT(server);
					svc->st = SCOPY_TEXT(st);
					svc->usn = SCOPY_TEXT(usn);
					svc->userAgent = SCOPY_TEXT(userAgent);
					dev->services->Add(svc);
				}
			}
		}
	}
}

void Net::SSDPClient::SSDPServiceFree(SSDPService *svc)
{
	SDEL_TEXT(svc->location);
	SDEL_TEXT(svc->opt);
	SDEL_TEXT(svc->server);
	SDEL_TEXT(svc->st);
	SDEL_TEXT(svc->usn);
	SDEL_TEXT(svc->userAgent);
	MemFree(svc);
}

void Net::SSDPClient::SSDPDeviceFree(SSDPDevice *dev)
{
	LIST_FREE_FUNC(dev->services, SSDPServiceFree);
	DEL_CLASS(dev->services);
	MemFree(dev);
}

Net::SSDPClient::SSDPClient(Net::SocketFactory *sockf, const UTF8Char *userAgent)
{
	NEW_CLASS(this->mut, Sync::Mutex());
	NEW_CLASS(this->devMap, Data::UInt32Map<SSDPDevice*>());
	this->userAgent = SCOPY_TEXT(userAgent);
	NEW_CLASS(this->udp, Net::UDPServer(sockf, 0, 0, 0, OnPacketRecv, this, 0, 0, 2, false));
	this->udp->SetBroadcast(true);
}

Net::SSDPClient::~SSDPClient()
{
	DEL_CLASS(this->udp);
	SDEL_TEXT(this->userAgent);
	Data::ArrayList<SSDPDevice*> *devList = this->devMap->GetValues();
	LIST_FREE_FUNC(devList, SSDPDeviceFree);
	DEL_CLASS(this->devMap);
	DEL_CLASS(this->mut);
}

Bool Net::SSDPClient::IsError()
{
	return this->udp->IsError();
}

Bool Net::SSDPClient::Scan()
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("M-SEARCH * HTTP/1.1\r\n"));
	sb.AppendC(UTF8STRC("Host: 239.255.255.250:1900\r\n"));
	sb.AppendC(UTF8STRC("Man: \"ssdp:discover\"\r\n"));
	sb.AppendC(UTF8STRC("ST: ssdp:all\r\n"));
	sb.AppendC(UTF8STRC("MX: 3\r\n"));
	if (this->userAgent)
	{
		sb.AppendC(UTF8STRC("User-Agent: "));
		sb.Append(this->userAgent);
		sb.AppendC(UTF8STRC("\r\n"));
	}
	sb.AppendC(UTF8STRC("\r\n\r\n"));
	Net::SocketUtil::AddressInfo addr;
	Net::SocketUtil::GetIPAddr((const UTF8Char*)"239.255.255.250", &addr);
	return this->udp->SendTo(&addr, 1900, sb.ToString(), sb.GetLength());
}

Data::ArrayList<Net::SSDPClient::SSDPDevice*> *Net::SSDPClient::GetDevices(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(this->mut);
	return this->devMap->GetValues();
}

#define SET_VALUE(v) SDEL_TEXT(v); sb.ClearStr(); reader.ReadNodeText(&sb); v = Text::StrCopyNew(sb.ToString());
Net::SSDPClient::SSDPRoot *Net::SSDPClient::SSDPRootParse(Text::EncodingFactory *encFact, IO::Stream *stm)
{
	SSDPRoot *root = MemAlloc(SSDPRoot, 1);
	MemClear(root, sizeof(SSDPRoot));
	Text::XMLReader reader(encFact, stm, Text::XMLReader::PM_XML);
	Text::StringBuilderUTF8 sb;
	while (reader.ReadNext())
	{
		if (reader.GetNodeType() == Text::XMLNode::NT_ELEMENT)
		{
			sb.ClearStr();
			reader.GetCurrPath(&sb);
			if (sb.EqualsICase((const UTF8Char*)"/root/device"))
			{
				if (reader.GetNodeText()->EqualsICase((const UTF8Char*)"udn"))
				{
					SET_VALUE(root->udn);
				}
				else if (reader.GetNodeText()->EqualsICase((const UTF8Char*)"friendlyName"))
				{
					SET_VALUE(root->friendlyName);
				}
				else if (reader.GetNodeText()->EqualsICase((const UTF8Char*)"manufacturer"))
				{
					SET_VALUE(root->manufacturer);
				}
				else if (reader.GetNodeText()->EqualsICase((const UTF8Char*)"manufacturerURL"))
				{
					SET_VALUE(root->manufacturerURL);
				}
				else if (reader.GetNodeText()->EqualsICase((const UTF8Char*)"modelName"))
				{
					SET_VALUE(root->modelName);
				}
				else if (reader.GetNodeText()->EqualsICase((const UTF8Char*)"modelNumber"))
				{
					SET_VALUE(root->modelNumber);
				}
				else if (reader.GetNodeText()->EqualsICase((const UTF8Char*)"modelURL"))
				{
					SET_VALUE(root->modelURL);
				}
				else if (reader.GetNodeText()->EqualsICase((const UTF8Char*)"serialNumber"))
				{
					SET_VALUE(root->serialNumber);
				}
				else if (reader.GetNodeText()->EqualsICase((const UTF8Char*)"presentationURL"))
				{
					SET_VALUE(root->presentationURL);
				}
				else if (reader.GetNodeText()->EqualsICase((const UTF8Char*)"deviceType"))
				{
					SET_VALUE(root->deviceType);
				}
				else if (reader.GetNodeText()->EqualsICase((const UTF8Char*)"deviceURL"))
				{
					SET_VALUE(root->deviceURL);
				}
			}
		}
	}
	return root;
}

void Net::SSDPClient::SSDPRootFree(SSDPRoot *root)
{
	SDEL_TEXT(root->udn);
	SDEL_TEXT(root->friendlyName);
	SDEL_TEXT(root->manufacturer);
	SDEL_TEXT(root->manufacturerURL);
	SDEL_TEXT(root->modelName);
	SDEL_TEXT(root->modelNumber);
	SDEL_TEXT(root->modelURL);
	SDEL_TEXT(root->serialNumber);
	SDEL_TEXT(root->presentationURL);
	SDEL_TEXT(root->deviceType);
	SDEL_TEXT(root->deviceURL);
	MemFree(root);
}
