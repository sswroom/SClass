#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/SSDPClient.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XMLReader.h"

void __stdcall Net::SSDPClient::OnPacketRecv(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData)
{
	NN<Net::SSDPClient> me = userData.GetNN<Net::SSDPClient>();
	if (Text::StrStartsWith(&data[0], (const UTF8Char*)"HTTP/1.1 200 OK"))
	{
		if (addr->addrType == Net::AddrType::IPv4)
		{
			Text::CString time = CSTR_NULL;
			Text::CString location = CSTR_NULL;
			Text::CString opt = CSTR_NULL;
			Text::CString server = CSTR_NULL;
			Text::CString st = CSTR_NULL;
			Text::CString usn = CSTR_NULL;
			Text::CStringNN nnusn;
			Text::CString userAgent = CSTR_NULL;
			Text::StringBuilderUTF8 sb;
			sb.AppendC(&data[0], data.GetSize());
			Text::PString sarr[2];
			UOSInt lineCnt;
			sarr[1] = sb;
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
			
			if (usn.SetTo(nnusn) && nnusn.leng > 0)
			{
				Sync::MutexUsage mutUsage(me->mut);
				UInt32 ip = ReadNUInt32(addr->addr);
				NN<SSDPDevice> dev;
				NN<SSDPService> svc;
				if (!me->devMap.Get(ip).SetTo(dev))
				{
					NEW_CLASSNN(dev, SSDPDevice());
					dev->addr = addr.Ptr()[0];
					me->devMap.Put(ip, dev);
				}
				Bool found = false;
				UOSInt i = dev->services.GetCount();
				while (i-- > 0)
				{
					svc = dev->services.GetItemNoCheck(i);
					if (nnusn.Equals(svc->usn))
					{
						found = true;
					}
				}
				if (!found)
				{
					svc = MemAllocNN(SSDPService);
					if (time.leng > 0)
					{
						Data::DateTime dt;
						dt.SetValue(time.OrEmpty());
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
					svc->usn = Text::String::New(nnusn);
					svc->userAgent = Text::String::NewOrNull(userAgent);
					dev->services.Add(svc);
				}
			}
		}
	}
}

void __stdcall Net::SSDPClient::SSDPServiceFree(NN<SSDPService> svc)
{
	OPTSTR_DEL(svc->location);
	OPTSTR_DEL(svc->opt);
	OPTSTR_DEL(svc->server);
	OPTSTR_DEL(svc->st);
	svc->usn->Release();
	OPTSTR_DEL(svc->userAgent);
	MemFreeNN(svc);
}

void __stdcall Net::SSDPClient::SSDPDeviceFree(NN<SSDPDevice> dev)
{
	dev->services.FreeAll(SSDPServiceFree);
	dev.Delete();
}

Net::SSDPClient::SSDPClient(NN<Net::SocketFactory> sockf, Text::CString userAgent, NN<IO::LogTool> log)
{
	this->userAgent = Text::String::NewOrNull(userAgent);
	NEW_CLASS(this->udp, Net::UDPServer(sockf, 0, 0, CSTR_NULL, OnPacketRecv, this, log, CSTR_NULL, 2, false));
	this->udp->SetBroadcast(true);
}

Net::SSDPClient::~SSDPClient()
{
	DEL_CLASS(this->udp);
	OPTSTR_DEL(this->userAgent);
	this->devMap.FreeAll(SSDPDeviceFree);
}

Bool Net::SSDPClient::IsError() const
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
	NN<Text::String> s;
	if (this->userAgent.SetTo(s))
	{
		sb.AppendC(UTF8STRC("User-Agent: "));
		sb.Append(s);
		sb.AppendC(UTF8STRC("\r\n"));
	}
	sb.AppendC(UTF8STRC("\r\n\r\n"));
	Net::SocketUtil::AddressInfo addr;
	Net::SocketUtil::SetAddrInfo(addr, CSTR("239.255.255.250"));
	return this->udp->SendTo(addr, 1900, sb.ToString(), sb.GetLength());
}

NN<const Data::ReadingListNN<Net::SSDPClient::SSDPDevice>> Net::SSDPClient::GetDevices(NN<Sync::MutexUsage> mutUsage) const
{
	mutUsage->ReplaceMutex(this->mut);
	return this->devMap;
}

#define SET_VALUE(v) SDEL_STRING(v); sb.ClearStr(); reader.ReadNodeText(sb); v = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
NN<Net::SSDPClient::SSDPRoot> Net::SSDPClient::SSDPRootParse(Optional<Text::EncodingFactory> encFact, NN<IO::Stream> stm)
{
	NN<SSDPRoot> root = MemAllocNN(SSDPRoot);
	root.ZeroContent();
	Text::XMLReader reader(encFact, stm, Text::XMLReader::PM_XML);
	Text::StringBuilderUTF8 sb;
	NN<Text::String> nodeText;
	while (reader.NextElementName().SetTo(nodeText))
	{
		if (nodeText->Equals(UTF8STRC("root")))
		{
			while (reader.NextElementName().SetTo(nodeText))
			{
				if (nodeText->Equals(UTF8STRC("device")))
				{
					while (reader.NextElementName().SetTo(nodeText))
					{
						if (nodeText->EqualsICase(UTF8STRC("udn")))
						{
							SET_VALUE(root->udn);
						}
						else if (nodeText->EqualsICase(UTF8STRC("friendlyName")))
						{
							SET_VALUE(root->friendlyName);
						}
						else if (nodeText->EqualsICase(UTF8STRC("manufacturer")))
						{
							SET_VALUE(root->manufacturer);
						}
						else if (nodeText->EqualsICase(UTF8STRC("manufacturerURL")))
						{
							SET_VALUE(root->manufacturerURL);
						}
						else if (nodeText->EqualsICase(UTF8STRC("modelName")))
						{
							SET_VALUE(root->modelName);
						}
						else if (nodeText->EqualsICase(UTF8STRC("modelNumber")))
						{
							SET_VALUE(root->modelNumber);
						}
						else if (nodeText->EqualsICase(UTF8STRC("modelURL")))
						{
							SET_VALUE(root->modelURL);
						}
						else if (nodeText->EqualsICase(UTF8STRC("serialNumber")))
						{
							SET_VALUE(root->serialNumber);
						}
						else if (nodeText->EqualsICase(UTF8STRC("presentationURL")))
						{
							SET_VALUE(root->presentationURL);
						}
						else if (nodeText->EqualsICase(UTF8STRC("deviceType")))
						{
							SET_VALUE(root->deviceType);
						}
						else if (nodeText->EqualsICase(UTF8STRC("deviceURL")))
						{
							SET_VALUE(root->deviceURL);
						}
						else
						{
							reader.SkipElement();
						}
					}
				}
				else
				{
					reader.SkipElement();
				}
			}
		}
		else
		{
			reader.SkipElement();
		}
	}
	return root;
}

void __stdcall Net::SSDPClient::SSDPRootFree(NN<SSDPRoot> root)
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
	MemFreeNN(root);
}
