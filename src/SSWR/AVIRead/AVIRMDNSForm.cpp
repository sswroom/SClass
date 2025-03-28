#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRMDNSForm.h"

void __stdcall SSWR::AVIRead::AVIRMDNSForm::DNSRecordRecv(AnyType userData, NN<const Net::SocketUtil::AddressInfo> addr, NN<Data::ArrayListNN<Net::DNSClient::RequestAnswer>> answers)
{
	NN<SSWR::AVIRead::AVIRMDNSForm> me = userData.GetNN<SSWR::AVIRead::AVIRMDNSForm>();
	NN<DeviceInfo> dev;
	NN<ServiceInfo> svc;
	NN<Text::String> s;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	sbuff[0] = 0;
	sptr = Net::SocketUtil::GetAddrName(sbuff, addr).Or(sbuff);
	Sync::MutexUsage mutUsage(me->devMut);
	if (!me->devMap.GetC(CSTRP(sbuff, sptr)).SetTo(dev))
	{
		NEW_CLASSNN(dev, DeviceInfo());
		dev->ip = Text::String::NewP(sbuff, sptr);
		dev->name = 0;
		me->devMap.PutC(CSTRP(sbuff, sptr), dev);
		me->devUpdated = true;
	}

	Text::StringBuilderUTF8 sb;
	NN<Net::DNSClient::RequestAnswer> ans;
	UOSInt k;
	Text::PString sarr[2];
	UOSInt i = 0;
	UOSInt j = answers->GetCount();
	while (i < j)
	{
		ans = answers->GetItemNoCheck(i);
		if (ans->recType == 1) // A
		{
			if (!dev->name.SetTo(s))
			{
				dev->name = ans->name->Clone();
				me->devUpdated = true;
			}
		}
		else if (ans->recType == 16) //TXT
		{
			if (dev->svcMap.GetNN(ans->name).SetTo(svc))
			{
				if (svc->txtTTL != ans->ttl)
				{
					svc->txtTTL = ans->ttl;
					if (me->currDev == dev)
						me->svcUpdated = true;
				}
				sb.ClearStr();
				sb.Append(ans->rd);
				svc->txtList.FreeAll();
				sarr[1] = sb;
				while (true)
				{
					k = Text::StrSplitTrimP(sarr, 2, sarr[1], ',');
					svc->txtList.Add(Text::String::New(sarr[0].ToCString()));
					if (k != 2)
						break;
				}
			}
		}
		else if (ans->recType == 33) //SRV
		{
			if (dev->svcMap.GetNN(ans->name).SetTo(svc))
			{
				if (svc->srvTTL != ans->ttl)
				{
					svc->srvTTL = ans->ttl;
					if (me->currDev == dev)
						me->svcUpdated = true;
				}
				if (svc->port != ans->port)
				{
					svc->port = ans->port;
					if (me->currDev == dev)
						me->svcUpdated = true;
				}
			}
		}
		else if (ans->recType == 12) //PTR
		{
			if (!ans->name->Equals(CSTR("_services._dns-sd._udp.local")))
			{
				if (dev->svcMap.GetNN(ans->name).SetTo(svc))
				{
					if (svc->ptrTTL != ans->ttl)
					{
						svc->ptrTTL = ans->ttl;
						if (me->currDev == dev)
							me->svcUpdated = true;
					}
				}
				else
				{
					NEW_CLASSNN(svc, ServiceInfo());
					svc->instanceName = ans->rd->Clone();
					svc->serviceName = ans->name->Clone();
					svc->port = 0;
					svc->ptrTTL = ans->ttl;
					svc->srvTTL = 0;
					svc->txtTTL = 0;
					dev->svcMap.PutNN(svc->instanceName, svc);
					if (me->currDev == dev)
						me->svcUpdated = true;
				}
			}
			else
			{
				Sync::MutexUsage mutUsage(me->ptrMut);
				if (me->ptrMap.Get(ans->rd->ToCString()) == 0)
				{
					me->ptrMap.Put(ans->rd->ToCString(), 1);
					me->mdns->SendQuery(ans->rd->ToCString());
				}
			}
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRMDNSForm::OnDevSelChg(AnyType userData)
{
	NN<SSWR::AVIRead::AVIRMDNSForm> me = userData.GetNN<SSWR::AVIRead::AVIRMDNSForm>();
	NN<DeviceInfo> dev;
	if (me->lbDev->GetSelectedItem().GetOpt<DeviceInfo>().SetTo(dev))
	{
		me->currDev = dev;
		me->currSvc = 0;
		me->DisplayServices();
	}
}

void __stdcall SSWR::AVIRead::AVIRMDNSForm::OnServicesSelChg(AnyType userData)
{
	NN<SSWR::AVIRead::AVIRMDNSForm> me = userData.GetNN<SSWR::AVIRead::AVIRMDNSForm>();
	NN<ServiceInfo> svc;
	if (me->lvServices->GetSelectedItem().GetOpt<ServiceInfo>().SetTo(svc))
	{
		me->currSvc = svc;
		me->DisplayTXTList();
	}
}

void __stdcall SSWR::AVIRead::AVIRMDNSForm::OnTimerTick(AnyType userData)
{
	NN<SSWR::AVIRead::AVIRMDNSForm> me = userData.GetNN<SSWR::AVIRead::AVIRMDNSForm>();
	if (me->devUpdated)
	{
		Sync::MutexUsage mutUsage(me->devMut);
		NN<DeviceInfo> dev;
		NN<Text::String> name;
		Text::StringBuilderUTF8 sb;
		me->devUpdated = false;
		me->lbDev->ClearItems();
		UOSInt selIndex = INVALID_INDEX;
		UOSInt i = 0;
		UOSInt j = me->devMap.GetCount();
		while (i < j)
		{
			dev = me->devMap.GetItemNoCheck(i);
			if (dev->name.SetTo(name))
			{
				sb.ClearStr();
				sb.Append(dev->ip);
				sb.AppendC(UTF8STRC(" ("));
				sb.Append(name);
				sb.AppendUTF8Char(')');
				me->lbDev->AddItem(sb.ToCString(), dev);
			}
			else
			{
				me->lbDev->AddItem(dev->ip, dev);
			}
			if (me->currDev == dev)
			{
				selIndex = i;
			}
			i++;
		}
		if (selIndex != INVALID_INDEX)
		{
			me->lbDev->SetSelectedIndex(selIndex);
		}
	}
	if (me->svcUpdated)
	{
		me->svcUpdated = false;
		me->DisplayServices();
	}
}

void __stdcall SSWR::AVIRead::AVIRMDNSForm::FreeServiceInfo(NN<ServiceInfo> svc)
{
	svc->serviceName->Release();
	svc->instanceName->Release();
	svc->txtList.FreeAll();
	svc.Delete();
}

void __stdcall SSWR::AVIRead::AVIRMDNSForm::FreeDeviceInfo(NN<DeviceInfo> dev)
{
	dev->ip->Release();
	OPTSTR_DEL(dev->name);
	dev->svcMap.FreeAll(FreeServiceInfo);
	dev.Delete();
}

void SSWR::AVIRead::AVIRMDNSForm::DisplayServices()
{
	NN<DeviceInfo> dev;
	NN<ServiceInfo> svc;
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	if (this->currDev.SetTo(dev))
	{
		Sync::MutexUsage mutUsage(this->devMut);
		this->lvServices->ClearItems();
		UOSInt i = 0;
		UOSInt j = dev->svcMap.GetCount();
		while (i < j)
		{
			svc = dev->svcMap.GetItemNoCheck(i);
			this->lvServices->AddItem(svc->instanceName, svc);
			if (svc->port != 0)
			{
				sptr = Text::StrUInt16(sbuff, svc->port);
				this->lvServices->SetSubItem(i, 1, CSTRP(sbuff, sptr));
			}
			sptr = Text::StrUInt32(sbuff, svc->ptrTTL);
			this->lvServices->SetSubItem(i, 2, CSTRP(sbuff, sptr));
			this->lvServices->SetSubItem(i, 3, svc->serviceName);
			i++;
		}
	}
}

void SSWR::AVIRead::AVIRMDNSForm::DisplayTXTList()
{
	NN<ServiceInfo> svc;
	if (this->currSvc.SetTo(svc))
	{
		Text::StringBuilderUTF8 sb;
		Sync::MutexUsage mutUsage(this->devMut);
		UOSInt i = 0;
		UOSInt j = svc->txtList.GetCount();
		while (i < j)
		{
			if (i > 0)
				sb.Append(CSTR("\r\n"));
			sb.Append(svc->txtList.GetItemNoCheck(i));
			i++;
			this->txtServices->SetText(sb.ToCString());
			i++;
		}
	}
	else
	{
		this->txtServices->SetText(CSTR(""));
	}
}

SSWR::AVIRead::AVIRMDNSForm::AVIRMDNSForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("mDNS"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lbDev = ui->NewListBox(*this, false);
	this->lbDev->SetRect(0, 0, 150, 23, false);
	this->lbDev->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDev->HandleSelectionChange(OnDevSelChg, this);
	this->hspDev = ui->NewHSplitter(*this, 3, false);
	this->lvServices = ui->NewListView(*this, UI::ListViewStyle::Table, 4);
	this->lvServices->SetRect(0, 0, 100, 100, false);
	this->lvServices->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lvServices->SetShowGrid(true);
	this->lvServices->HandleSelChg(OnServicesSelChg, this);
	this->lvServices->AddColumn(CSTR("Instance"), 300);
	this->lvServices->AddColumn(CSTR("Port"), 100);
	this->lvServices->AddColumn(CSTR("TTL(PTR)"), 100);
	this->lvServices->AddColumn(CSTR("Service"), 300);
	this->vspServices = ui->NewVSplitter(*this, 3, false);
	this->txtServices = ui->NewTextBox(*this, CSTR(""), true);
	this->txtServices->SetReadOnly(true);
	this->txtServices->SetDockType(UI::GUIControl::DOCK_FILL);

	this->devUpdated = false;
	this->svcUpdated = false;
	this->currDev = 0;
	this->currSvc = 0;
	NEW_CLASSNN(this->mdns, Net::MDNSClient(core->GetSocketFactory(), DNSRecordRecv, this));
	if (this->mdns->IsError())
	{
		ui->ShowMsgOK(CSTR("Error in initializing mDNS client"), CSTR("mDNS"), this);
	}
	this->AddTimer(500, OnTimerTick, this);
	this->mdns->SendQuery(CSTR("_services._dns-sd._udp.local"));
}

SSWR::AVIRead::AVIRMDNSForm::~AVIRMDNSForm()
{
	this->mdns.Delete();
	this->devMap.FreeAll(FreeDeviceInfo);
}

void SSWR::AVIRead::AVIRMDNSForm::OnMonitorChanged()
{
	MonitorHandle *hMon = this->GetHMonitor();
	this->SetDPI(this->core->GetMonitorHDPI(hMon), this->core->GetMonitorDDPI(hMon));
}
