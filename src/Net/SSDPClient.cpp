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
			Text::CString time = CSTR_NULL;
			Text::CString location = CSTR_NULL;
			Text::CString opt = CSTR_NULL;
			Text::CString server = CSTR_NULL;
			Text::CString st = CSTR_NULL;
			Text::CString usn = CSTR_NULL;
			Text::CString userAgent = CSTR_NULL;
			Text::StringBuilderUTF8 sb;
			sb.AppendC(buff, dataSize);
			Text::PString sarr[2];
			UOSInt lineCnt;
			sarr[1].v = sb.ToString();
			sarr[1].leng = sb.GetLength();
			while (true)
			{
				lineCnt = Text::StrSplitLineP(sarr, 2, sarr[1]);
				if (Text::StrStartsWithICaseC(sarr[0].v, sarr[0].leng, UTF8STRC("DATE: ")))
				{
					time = sarr[0].ToCString().Substring(6);
				}
				else if (Text::StrStartsWithICaseC(sarr[0].v, sarr[0].leng, UTF8STRC("LOCATION: ")))
				{
					location = sarr[0].ToCString().Substring(10);
				}
				else if (Text::StrStartsWithICaseC(sarr[0].v, sarr[0].leng, UTF8STRC("OPT: ")))
				{
					opt = sarr[0].ToCString().Substring(5);
				}
				else if (Text::StrStartsWithICaseC(sarr[0].v, sarr[0].leng, UTF8STRC("ST: ")))
				{
					st = sarr[0].ToCString().Substring(4);
				}
				else if (Text::StrStartsWithICaseC(sarr[0].v, sarr[0].leng, UTF8STRC("USN: ")))
				{
					usn = sarr[0].ToCString().Substring(5);
				}
				else if (Text::StrStartsWithICaseC(sarr[0].v, sarr[0].leng, UTF8STRC("X-USER-AGENT: ")))
				{
					userAgent = sarr[0].ToCString().Substring(14);
				}

				if (lineCnt != 2)
				{
					break;
				}
			}
			
			if (usn.leng > 0)
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
					if (usn.Equals(svc->usn))
					{
						found = true;
					}
				}
				if (!found)
				{
					svc = MemAlloc(SSDPService, 1);
					if (time.leng > 0)
					{
						Data::DateTime dt;
						dt.SetValue(time.v, time.leng);
						svc->time = dt.ToTicks();
					}
					else
					{
						svc->time = 0;
					}
					svc->location = Text::String::NewOrNull(location);
					svc->opt = Text::String::NewOrNull(opt);
					svc->server = Text::String::NewOrNull(server);
					svc->st = Text::String::NewOrNull(st);
					svc->usn = Text::String::NewOrNull(usn);
					svc->userAgent = Text::String::NewOrNull(userAgent);
					dev->services->Add(svc);
				}
			}
		}
	}
}

void Net::SSDPClient::SSDPServiceFree(SSDPService *svc)
{
	SDEL_STRING(svc->location);
	SDEL_STRING(svc->opt);
	SDEL_STRING(svc->server);
	SDEL_STRING(svc->st);
	SDEL_STRING(svc->usn);
	SDEL_STRING(svc->userAgent);
	MemFree(svc);
}

void Net::SSDPClient::SSDPDeviceFree(SSDPDevice *dev)
{
	LIST_FREE_FUNC(dev->services, SSDPServiceFree);
	DEL_CLASS(dev->services);
	MemFree(dev);
}

Net::SSDPClient::SSDPClient(Net::SocketFactory *sockf, Text::CString userAgent)
{
	NEW_CLASS(this->mut, Sync::Mutex());
	NEW_CLASS(this->devMap, Data::UInt32Map<SSDPDevice*>());
	this->userAgent = Text::String::NewOrNull(userAgent);
	NEW_CLASS(this->udp, Net::UDPServer(sockf, 0, 0, CSTR_NULL, OnPacketRecv, this, 0, CSTR_NULL, 2, false));
	this->udp->SetBroadcast(true);
}

Net::SSDPClient::~SSDPClient()
{
	DEL_CLASS(this->udp);
	SDEL_STRING(this->userAgent);
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
	Net::SocketUtil::GetIPAddr(UTF8STRC("239.255.255.250"), &addr);
	return this->udp->SendTo(&addr, 1900, sb.ToString(), sb.GetLength());
}

Data::ArrayList<Net::SSDPClient::SSDPDevice*> *Net::SSDPClient::GetDevices(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(this->mut);
	return this->devMap->GetValues();
}

#define SET_VALUE(v) SDEL_STRING(v); sb.ClearStr(); reader.ReadNodeText(&sb); v = Text::String::New(sb.ToString(), sb.GetLength());
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
			if (sb.EqualsICase(UTF8STRC("/root/device")))
			{
				if (reader.GetNodeText()->EqualsICase(UTF8STRC("udn")))
				{
					SET_VALUE(root->udn);
				}
				else if (reader.GetNodeText()->EqualsICase(UTF8STRC("friendlyName")))
				{
					SET_VALUE(root->friendlyName);
				}
				else if (reader.GetNodeText()->EqualsICase(UTF8STRC("manufacturer")))
				{
					SET_VALUE(root->manufacturer);
				}
				else if (reader.GetNodeText()->EqualsICase(UTF8STRC("manufacturerURL")))
				{
					SET_VALUE(root->manufacturerURL);
				}
				else if (reader.GetNodeText()->EqualsICase(UTF8STRC("modelName")))
				{
					SET_VALUE(root->modelName);
				}
				else if (reader.GetNodeText()->EqualsICase(UTF8STRC("modelNumber")))
				{
					SET_VALUE(root->modelNumber);
				}
				else if (reader.GetNodeText()->EqualsICase(UTF8STRC("modelURL")))
				{
					SET_VALUE(root->modelURL);
				}
				else if (reader.GetNodeText()->EqualsICase(UTF8STRC("serialNumber")))
				{
					SET_VALUE(root->serialNumber);
				}
				else if (reader.GetNodeText()->EqualsICase(UTF8STRC("presentationURL")))
				{
					SET_VALUE(root->presentationURL);
				}
				else if (reader.GetNodeText()->EqualsICase(UTF8STRC("deviceType")))
				{
					SET_VALUE(root->deviceType);
				}
				else if (reader.GetNodeText()->EqualsICase(UTF8STRC("deviceURL")))
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
	SDEL_STRING(root->udn);
	SDEL_STRING(root->friendlyName);
	SDEL_STRING(root->manufacturer);
	SDEL_STRING(root->manufacturerURL);
	SDEL_STRING(root->modelName);
	SDEL_STRING(root->modelNumber);
	SDEL_STRING(root->modelURL);
	SDEL_STRING(root->serialNumber);
	SDEL_STRING(root->presentationURL);
	SDEL_STRING(root->deviceType);
	SDEL_STRING(root->deviceURL);
	MemFree(root);
}
