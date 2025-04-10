#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Manage/HiResClock.h"
#include "Net/MACInfo.h"
#include "SSWR/AVIRead/AVIRWifiScanForm.h"
#include "Sync/SimpleThread.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRWifiScanForm::OnScanClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWifiScanForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWifiScanForm>();
	me->WifiScan();
}

void __stdcall SSWR::AVIRead::AVIRWifiScanForm::OnWifiSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWifiScanForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWifiScanForm>();
	NN<Net::WirelessLAN::BSSInfo> bss;
	if (!me->lvWifi->GetSelectedItem().GetOpt<Net::WirelessLAN::BSSInfo>().SetTo(bss))
	{
		me->txtWifi->SetText(CSTR(""));
	}
	else
	{
		UOSInt i;
		UOSInt j;
		NN<Net::WirelessLANIE> ie;
		Text::StringBuilderUTF8 sb;
		i = 0;
		j = bss->GetIECount();
		while (i < j)
		{
			if (bss->GetIE(i).SetTo(ie))
			{
				if (i > 0)
				{
					sb.AppendC(UTF8STRC("\r\n"));
				}
				Net::WirelessLANIE::ToString(ie->GetIEBuff(), sb);
			}
			i++;
		}
		me->txtWifi->SetText(sb.ToCString());
	}
	
}

void SSWR::AVIRead::AVIRWifiScanForm::WifiScan()
{
	this->WifiClear();
	const UInt8 tmpOUI[] = {0, 0, 0};
	NN<Net::WirelessLAN::Interface> wlanInterf;
	if (this->wlanInterf.SetTo(wlanInterf))
	{
		UTF8Char sbuff[64];
		UnsafeArray<UTF8Char> sptr;
		UInt8 id[8];
		UInt64 imac;
		Manage::HiResClock clk;
		NN<Text::String> s;
		UnsafeArray<const UTF8Char> csptr;
		NN<Text::String> ssid;
		Data::ArrayListNN<Net::WirelessLAN::BSSInfo> bssList;
		NN<Net::WirelessLAN::BSSInfo> bss;
		Double t1;
		Double t2;
		UOSInt i;
		UOSInt j;
		wlanInterf->Scan();
		t1 = clk.GetTimeDiff();
		Sync::SimpleThread::Sleep(5000);

		clk.Start();
		wlanInterf->GetBSSList(bssList);
		t2 = clk.GetTimeDiff();

		sptr = Text::StrDouble(sbuff, t1);
		this->txtScanTime->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, t2);
		this->txtResultTime->SetText(CSTRP(sbuff, sptr));

		i = 0;
		j = bssList.GetCount();
		while (i < j)
		{
			bss = bssList.GetItemNoCheck(i);
			ssid = bss->GetSSID();
			MemCopyNO(&id[2], bss->GetMAC().Ptr(), 6);
			id[0] = 0;
			id[1] = 0;
			imac = ReadMUInt64(id);

			this->lvWifi->AddItem(ssid, bss);
			this->bssList.Add(bss);
			sptr = Text::StrUInt32(sbuff, bss->GetPHYId());
			this->lvWifi->SetSubItem(i, 1, CSTRP(sbuff, sptr));
			sptr = Text::StrHexBytes(sbuff, &id[2], 6, ':');
			this->lvWifi->SetSubItem(i, 2, CSTRP(sbuff, sptr));
			sptr = Text::StrInt32(sbuff, bss->GetBSSType());
			NN<const Net::MACInfo::MACEntry> entry = Net::MACInfo::GetMACInfo(imac);
			this->lvWifi->SetSubItem(i, 3, {entry->name, entry->nameLen});
			this->lvWifi->SetSubItem(i, 4, CSTRP(sbuff, sptr));
			sptr = Text::StrInt32(sbuff, bss->GetPHYType());
			this->lvWifi->SetSubItem(i, 5, CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, bss->GetRSSI());
			this->lvWifi->SetSubItem(i, 6, CSTRP(sbuff, sptr));
			sptr = Text::StrUInt32(sbuff, bss->GetLinkQuality());
			this->lvWifi->SetSubItem(i, 7, CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, bss->GetFreq());
			this->lvWifi->SetSubItem(i, 8, CSTRP(sbuff, sptr));
			UnsafeArray<const UInt8> oui;
			if (bss->GetCountry().SetTo(csptr))
				this->lvWifi->SetSubItem(i, 9, Text::CStringNN::FromPtr(csptr));
			oui = bss->GetChipsetOUI(0).Or(tmpOUI);
			if (oui[0] != 0 || oui[1] != 0 || oui[2] != 0)
			{
				entry = Net::MACInfo::GetMACInfoOUI(oui);
				this->lvWifi->SetSubItem(i, 10, {entry->name, entry->nameLen});
			}
			oui = bss->GetChipsetOUI(1).Or(tmpOUI);
			if (oui[0] != 0 || oui[1] != 0 || oui[2] != 0)
			{
				entry = Net::MACInfo::GetMACInfoOUI(oui);
				this->lvWifi->SetSubItem(i, 11, {entry->name, entry->nameLen});
			}
			oui = bss->GetChipsetOUI(2).Or(tmpOUI);
			if (oui[0] != 0 || oui[1] != 0 || oui[2] != 0)
			{
				entry = Net::MACInfo::GetMACInfoOUI(oui);
				this->lvWifi->SetSubItem(i, 12, {entry->name, entry->nameLen});
			}
			if (bss->GetManuf().SetTo(s))
				this->lvWifi->SetSubItem(i, 13, s);
			if (bss->GetModel().SetTo(s))
				this->lvWifi->SetSubItem(i, 14, s);
			if (bss->GetSN().SetTo(s))
				this->lvWifi->SetSubItem(i, 15, s);

			i++;
		}
	}
	else
	{
		this->ui->ShowMsgOK(CSTR("No WiFi interface found"), CSTR("WiFi Scan"), this);
	}	
}

void SSWR::AVIRead::AVIRWifiScanForm::WifiClear()
{
	this->bssList.DeleteAll();
	if (this->children.GetCount() > 0)
	{
		this->lvWifi->ClearItems();
	}
}

SSWR::AVIRead::AVIRWifiScanForm::AVIRWifiScanForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Wifi Scan"));

	this->core = core;
	Data::ArrayListNN<Net::WirelessLAN::Interface> interfList;
	this->wlan.GetInterfaces(interfList);
	this->wlanInterf = interfList.GetItem(0);
	UOSInt i = interfList.GetCount();
	while (i-- > 1)
	{
		NN<Net::WirelessLAN::Interface> interf = interfList.GetItemNoCheck(i);
		interf.Delete();
	}

	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnScan = ui->NewButton(this->pnlControl, CSTR("Scan"));
	this->btnScan->SetRect(4, 4, 75, 23, false);
	this->btnScan->HandleButtonClick(OnScanClicked, this);
	this->lblScanTime = ui->NewLabel(this->pnlControl, CSTR("Scan Time"));
	this->lblScanTime->SetRect(104, 4, 100, 23, false);
	this->txtScanTime = ui->NewTextBox(this->pnlControl, CSTR(""));
	this->txtScanTime->SetReadOnly(true);
	this->txtScanTime->SetRect(204, 4, 100, 23, false);
	this->lblResultTime = ui->NewLabel(this->pnlControl, CSTR("Result Time"));
	this->lblResultTime->SetRect(304, 4, 100, 23, false);
	this->txtResultTime = ui->NewTextBox(this->pnlControl, CSTR(""));
	this->txtResultTime->SetReadOnly(true);
	this->txtResultTime->SetRect(404, 4, 100, 23, false);
	this->txtWifi = ui->NewTextBox(*this, CSTR(""), true);
	this->txtWifi->SetRect(0, 0, 100, 128, false);
	this->txtWifi->SetReadOnly(true);
	this->txtWifi->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->vspWifi = ui->NewVSplitter(*this, 3, true);
	this->lvWifi = ui->NewListView(*this, UI::ListViewStyle::Table, 16);
	this->lvWifi->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvWifi->SetShowGrid(true);
	this->lvWifi->SetFullRowSelect(true);
	this->lvWifi->AddColumn(CSTR("SSID"), 200);
	this->lvWifi->AddColumn(CSTR("PhyId"), 40);
	this->lvWifi->AddColumn(CSTR("MAC"), 120);
	this->lvWifi->AddColumn(CSTR("Vendor"), 120);
	this->lvWifi->AddColumn(CSTR("BSSType"), 60);
	this->lvWifi->AddColumn(CSTR("PHYType"), 60);
	this->lvWifi->AddColumn(CSTR("RSSI"), 60);
	this->lvWifi->AddColumn(CSTR("Link Quality"), 60);
	this->lvWifi->AddColumn(CSTR("Frequency"), 80);
	this->lvWifi->AddColumn(CSTR("Country"), 60);
	this->lvWifi->AddColumn(CSTR("Vendor1"), 120);
	this->lvWifi->AddColumn(CSTR("Vendor2"), 120);
	this->lvWifi->AddColumn(CSTR("Vendor3"), 120);
	this->lvWifi->AddColumn(CSTR("Manufacturer"), 100);
	this->lvWifi->AddColumn(CSTR("Model"), 100);
	this->lvWifi->AddColumn(CSTR("S/N"), 100);
	this->lvWifi->HandleSelChg(OnWifiSelChg, this);

	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

SSWR::AVIRead::AVIRWifiScanForm::~AVIRWifiScanForm()
{
	this->WifiClear();
	this->wlanInterf.Delete();
}

void SSWR::AVIRead::AVIRWifiScanForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
