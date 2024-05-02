#include "Stdafx.h"
#include "IO/BTUtil.h"
#include "Net/MACInfo.h"
#include "SSWR/AVIRead/AVIRBluetoothForm.h"

void __stdcall SSWR::AVIRead::AVIRBluetoothForm::OnCtrlChanged(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRBluetoothForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBluetoothForm>();
	NN<BTStatus> btStatus;
	if (me->lbCtrl->GetSelectedItem().GetOpt<BTStatus>().SetTo(btStatus))
	{
		Text::StringBuilderUTF8 sb;

		me->txtRadioName->SetText(btStatus->bt->GetName()->ToCString());
		IO::BTUtil::GetAddrText(sb, btStatus->bt->GetAddress());
		me->txtAddr->SetText(sb.ToCString());
		me->txtManu->SetText(IO::BTUtil::GetManufacturerName(btStatus->bt->GetManufacturer()));
		sb.ClearStr();
		sb.AppendC(UTF8STRC("0x"));
		sb.AppendHex16(btStatus->bt->GetSubversion());
		me->txtSubver->SetText(sb.ToCString());
		sb.ClearStr();
		sb.AppendC(UTF8STRC("0x"));
		sb.AppendHex32(btStatus->bt->GetDevClass());
		me->txtClass->SetText(sb.ToCString());

		me->UpdateDevList(btStatus);
	}
	else
	{
		me->txtRadioName->SetText(CSTR(""));
		me->txtAddr->SetText(CSTR(""));
		me->txtManu->SetText(CSTR(""));
		me->txtSubver->SetText(CSTR(""));
		me->txtClass->SetText(CSTR(""));

		me->ClearGUIDs();
		me->lvDevice->ClearItems();
	}
}

void __stdcall SSWR::AVIRead::AVIRBluetoothForm::OnDeviceSrchClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRBluetoothForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBluetoothForm>();
	NN<BTStatus> btStatus;
	NN<IO::BTController::BTDevice> dev;
	UOSInt i;
	if (me->lbCtrl->GetSelectedItem().GetOpt<BTStatus>().SetTo(btStatus))
	{
		i = btStatus->devList->GetCount();
		while (i-- > 0)
		{
			dev = btStatus->devList->GetItemNoCheck(i);
			dev.Delete();
		}
		btStatus->devList->Clear();

		btStatus->bt->CreateDevices(btStatus->devList, true);
		me->UpdateDevList(btStatus);
	}
}

void __stdcall SSWR::AVIRead::AVIRBluetoothForm::OnDeviceUpdateClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRBluetoothForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBluetoothForm>();
	NN<BTStatus> btStatus;
	NN<IO::BTController::BTDevice> dev;
	UOSInt i;
	if (me->lbCtrl->GetSelectedItem().GetOpt<BTStatus>().SetTo(btStatus))
	{
		i = btStatus->devList->GetCount();
		while (i-- > 0)
		{
			dev = btStatus->devList->GetItemNoCheck(i);
			dev.Delete();
		}
		btStatus->devList->Clear();

		btStatus->bt->CreateDevices(btStatus->devList, false);
		me->UpdateDevList(btStatus);
	}
}

void __stdcall SSWR::AVIRead::AVIRBluetoothForm::OnDeviceSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRBluetoothForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBluetoothForm>();
	NN<IO::BTController::BTDevice> dev;
	me->currDev = me->lvDevice->GetSelectedItem().GetOpt<IO::BTController::BTDevice>();
	me->ClearGUIDs();
	if (me->currDev.SetTo(dev))
	{
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		NN<Data::UUID> guid;
		dev->QueryServices(me->guidList);
		i = 0;
		j = me->guidList.GetCount();
		while (i < j)
		{
			guid = me->guidList.GetItemNoCheck(i);
			sb.ClearStr();
			IO::BTUtil::GetServiceName(sb, guid);
			me->lbDevServices->AddItem(sb.ToCString(), guid);
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRBluetoothForm::OnDevAuthenClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRBluetoothForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBluetoothForm>();
	NN<IO::BTController::BTDevice> currDev;
	if (me->currDev.SetTo(currDev))
	{
		if (currDev->Pair((const UTF8Char*)"0000"))
		{
			OnDeviceUpdateClicked(userObj);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRBluetoothForm::OnDevUnauthenClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRBluetoothForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBluetoothForm>();
	NN<IO::BTController::BTDevice> currDev;
	if (me->currDev.SetTo(currDev))
	{
		if (currDev->Unpair())
		{
			OnDeviceUpdateClicked(userObj);
		}
	}
}

void SSWR::AVIRead::AVIRBluetoothForm::ClearGUIDs()
{
	NN<IO::BTController::BTDevice> currDev;
	if (this->currDev.SetTo(currDev))
	{
		currDev->FreeServices(this->guidList);
		this->lbDevServices->ClearItems();
	}
}

void SSWR::AVIRead::AVIRBluetoothForm::UpdateDevList(NN<BTStatus> btStatus)
{
	Text::StringBuilderUTF8 sb;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	NN<IO::BTController::BTDevice> dev;
	this->ClearGUIDs();
	this->currDev = 0;
	this->lvDevice->ClearItems();
	i = 0;
	j = btStatus->devList->GetCount();
	while (i < j)
	{
		dev = btStatus->devList->GetItemNoCheck(i);
		k = this->lvDevice->AddItem(dev->GetName(), dev);
		sb.ClearStr();
		IO::BTUtil::GetAddrText(sb, dev->GetAddress());
		this->lvDevice->SetSubItem(k, 1, sb.ToCString());
		const Net::MACInfo::MACEntry *mac = Net::MACInfo::GetMACInfo(IO::BTUtil::GetAddrMAC(dev->GetAddress()));
		this->lvDevice->SetSubItem(k, 2, {mac->name, mac->nameLen});
		this->lvDevice->SetSubItem(k, 3, dev->IsConnected()?CSTR("Y"):CSTR("N"));
		this->lvDevice->SetSubItem(k, 4, dev->IsAuthenticated()?CSTR("Y"):CSTR("N"));
		this->lvDevice->SetSubItem(k, 5, dev->IsRemembered()?CSTR("Y"):CSTR("N"));
		i++;
	}
}

SSWR::AVIRead::AVIRBluetoothForm::AVIRBluetoothForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Bluetooth Form"));
	this->SetFont(0, 0, 8.25, false);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lbCtrl = ui->NewListBox(*this, false);
	this->lbCtrl->SetRect(0, 0, 200, 23, false);
	this->lbCtrl->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbCtrl->HandleSelectionChange(OnCtrlChanged, this);
	this->hspMain = ui->NewHSplitter(*this, 3, false);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpInfo = this->tcMain->AddTabPage(CSTR("Info"));
	this->lblRadioName = ui->NewLabel(this->tpInfo, CSTR("Radio Name"));
	this->lblRadioName->SetRect(4, 4, 100, 23, false);
	this->txtRadioName = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtRadioName->SetRect(104, 4, 200, 23, false);
	this->txtRadioName->SetReadOnly(true);
	this->lblAddr = ui->NewLabel(this->tpInfo, CSTR("Address"));
	this->lblAddr->SetRect(4, 28, 100, 23, false);
	this->txtAddr = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtAddr->SetRect(104, 28, 200, 23, false);
	this->txtAddr->SetReadOnly(true);
	this->lblManu = ui->NewLabel(this->tpInfo, CSTR("Manufacturer"));
	this->lblManu->SetRect(4, 52, 100, 23, false);
	this->txtManu = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtManu->SetRect(104, 52, 200, 23, false);
	this->txtManu->SetReadOnly(true);
	this->lblSubver = ui->NewLabel(this->tpInfo, CSTR("Subversion"));
	this->lblSubver->SetRect(4, 76, 100, 23, false);
	this->txtSubver = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtSubver->SetRect(104, 76, 100, 23, false);
	this->txtSubver->SetReadOnly(true);
	this->lblClass = ui->NewLabel(this->tpInfo, CSTR("Class"));
	this->lblClass->SetRect(4, 100, 100, 23, false);
	this->txtClass = ui->NewTextBox(this->tpInfo, CSTR(""));
	this->txtClass->SetRect(104, 100, 100, 23, false);
	this->txtClass->SetReadOnly(true);

	this->tpDevice = this->tcMain->AddTabPage(CSTR("Device"));
	this->pnlDevice = ui->NewPanel(this->tpDevice);
	this->pnlDevice->SetRect(0, 0, 100, 31, false);
	this->pnlDevice->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnDeviceSrch = ui->NewButton(this->pnlDevice, CSTR("Search"));
	this->btnDeviceSrch->SetRect(4, 4, 75, 23, false);
	this->btnDeviceSrch->HandleButtonClick(OnDeviceSrchClicked, this);
	this->btnDeviceUpdate = ui->NewButton(this->pnlDevice, CSTR("Update"));
	this->btnDeviceUpdate->SetRect(84, 4, 75, 23, false);
	this->btnDeviceUpdate->HandleButtonClick(OnDeviceUpdateClicked, this);
	this->pnlDevDetail = ui->NewPanel(this->tpDevice);
	this->pnlDevDetail->SetRect(0, 0, 100, 360, false);
	this->pnlDevDetail->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->vspDevice = ui->NewVSplitter(this->tpDevice, 3, true);
	this->pnlDevCtrl = ui->NewPanel(this->pnlDevDetail);
	this->pnlDevCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlDevCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnDevAuthen = ui->NewButton(this->pnlDevCtrl, CSTR("Pair"));
	this->btnDevAuthen->SetRect(4, 4, 75, 23, false);
	this->btnDevAuthen->HandleButtonClick(OnDevAuthenClicked, this);
	this->btnDevUnauthen = ui->NewButton(this->pnlDevCtrl, CSTR("Unpair"));
	this->btnDevUnauthen->SetRect(84, 4, 75, 23, false);
	this->btnDevUnauthen->HandleButtonClick(OnDevUnauthenClicked, this);
	this->lbDevServices = ui->NewListBox(this->pnlDevDetail, false);
	this->lbDevServices->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDevice = ui->NewListView(this->tpDevice, UI::ListViewStyle::Table, 6);
	this->lvDevice->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDevice->HandleSelChg(OnDeviceSelChg, this);
	this->lvDevice->AddColumn(CSTR("Name"), 150);
	this->lvDevice->AddColumn(CSTR("Address"), 150);
	this->lvDevice->AddColumn(CSTR("Vendor"), 150);
	this->lvDevice->AddColumn(CSTR("Connected"), 80);
	this->lvDevice->AddColumn(CSTR("Authen"), 60);
	this->lvDevice->AddColumn(CSTR("Remember"), 80);

	UOSInt i;
	UOSInt j;
	Data::ArrayListNN<IO::BTController> btList;
	NN<BTStatus> btStatus;
	Text::CString cstr;
	Text::StringBuilderUTF8 sb;
	this->currDev = 0;

	IO::BTManager btMgr;
	btMgr.CreateControllers(btList);
	i = 0;
	j = btList.GetCount();
	while (i < j)
	{
		btStatus = MemAllocNN(BTStatus);
		btStatus->bt = btList.GetItemNoCheck(i);
		NEW_CLASSNN(btStatus->devList, Data::ArrayListNN<IO::BTController::BTDevice>());
		this->btList.Add(btStatus);
		btStatus->bt->CreateDevices(btStatus->devList, false);

		sb.ClearStr();
		sb.Append(btStatus->bt->GetName());
		cstr = IO::BTUtil::GetManufacturerName(btStatus->bt->GetManufacturer());
		if (cstr.v)
		{
			sb.AppendC(UTF8STRC(" ("));
			sb.Append(cstr);
			sb.AppendC(UTF8STRC(")"));
		}
		this->lbCtrl->AddItem(sb.ToCString(), btStatus);
		i++;
	}
}

SSWR::AVIRead::AVIRBluetoothForm::~AVIRBluetoothForm()
{
	UOSInt i;
	UOSInt j;
	NN<BTStatus> btStatus;
	NN<IO::BTController::BTDevice> dev;
	this->ClearGUIDs();
	i = this->btList.GetCount();
	while (i-- > 0)
	{
		btStatus = this->btList.GetItemNoCheck(i);
		j = btStatus->devList->GetCount();
		while (j-- > 0)
		{
			dev = btStatus->devList->GetItemNoCheck(j);
			dev.Delete();
		}
		btStatus->devList.Delete();
		btStatus->bt.Delete();
		MemFreeNN(btStatus);
	}
}

void SSWR::AVIRead::AVIRBluetoothForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
