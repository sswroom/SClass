#include "Stdafx.h"
#include "IO/BTUtil.h"
#include "Net/MACInfo.h"
#include "SSWR/AVIRead/AVIRBluetoothForm.h"

void __stdcall SSWR::AVIRead::AVIRBluetoothForm::OnCtrlChanged(void *userObj)
{
	SSWR::AVIRead::AVIRBluetoothForm *me = (SSWR::AVIRead::AVIRBluetoothForm*)userObj;
	BTStatus *btStatus = (BTStatus*)me->lbCtrl->GetSelectedItem();
	if (btStatus)
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

void __stdcall SSWR::AVIRead::AVIRBluetoothForm::OnDeviceSrchClicked(void *userObj)
{
	SSWR::AVIRead::AVIRBluetoothForm *me = (SSWR::AVIRead::AVIRBluetoothForm*)userObj;
	BTStatus *btStatus = (BTStatus*)me->lbCtrl->GetSelectedItem();
	IO::BTController::BTDevice *dev;
	UOSInt i;
	if (btStatus)
	{
		i = btStatus->devList->GetCount();
		while (i-- > 0)
		{
			dev = btStatus->devList->GetItem(i);
			DEL_CLASS(dev);
		}
		btStatus->devList->Clear();

		btStatus->bt->CreateDevices(btStatus->devList, true);
		me->UpdateDevList(btStatus);
	}
}

void __stdcall SSWR::AVIRead::AVIRBluetoothForm::OnDeviceUpdateClicked(void *userObj)
{
	SSWR::AVIRead::AVIRBluetoothForm *me = (SSWR::AVIRead::AVIRBluetoothForm*)userObj;
	BTStatus *btStatus = (BTStatus*)me->lbCtrl->GetSelectedItem();
	IO::BTController::BTDevice *dev;
	UOSInt i;
	if (btStatus)
	{
		i = btStatus->devList->GetCount();
		while (i-- > 0)
		{
			dev = btStatus->devList->GetItem(i);
			DEL_CLASS(dev);
		}
		btStatus->devList->Clear();

		btStatus->bt->CreateDevices(btStatus->devList, false);
		me->UpdateDevList(btStatus);
	}
}

void __stdcall SSWR::AVIRead::AVIRBluetoothForm::OnDeviceSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRBluetoothForm *me = (SSWR::AVIRead::AVIRBluetoothForm*)userObj;
	IO::BTController::BTDevice *dev = (IO::BTController::BTDevice*)me->lvDevice->GetSelectedItem();
	me->ClearGUIDs();
	me->currDev = dev;
	if (dev)
	{
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		void *guid;
		dev->QueryServices(&me->guidList);
		i = 0;
		j = me->guidList.GetCount();
		while (i < j)
		{
			guid = me->guidList.GetItem(i);
			sb.ClearStr();
			IO::BTUtil::GetServiceName(sb, guid);
			me->lbDevServices->AddItem(sb.ToCString(), guid);
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRBluetoothForm::OnDevAuthenClicked(void *userObj)
{
	SSWR::AVIRead::AVIRBluetoothForm *me = (SSWR::AVIRead::AVIRBluetoothForm*)userObj;
	if (me->currDev)
	{
		if (me->currDev->Pair((const UTF8Char*)"0000"))
		{
			OnDeviceUpdateClicked(userObj);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRBluetoothForm::OnDevUnauthenClicked(void *userObj)
{
	SSWR::AVIRead::AVIRBluetoothForm *me = (SSWR::AVIRead::AVIRBluetoothForm*)userObj;
	if (me->currDev)
	{
		if (me->currDev->Unpair())
		{
			OnDeviceUpdateClicked(userObj);
		}
	}
}

void SSWR::AVIRead::AVIRBluetoothForm::ClearGUIDs()
{
	if (this->currDev)
	{
		this->currDev->FreeServices(&this->guidList);
		this->lbDevServices->ClearItems();
	}
}

void SSWR::AVIRead::AVIRBluetoothForm::UpdateDevList(BTStatus *btStatus)
{
	Text::StringBuilderUTF8 sb;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	IO::BTController::BTDevice *dev;
	this->ClearGUIDs();
	this->currDev = 0;
	this->lvDevice->ClearItems();
	i = 0;
	j = btStatus->devList->GetCount();
	while (i < j)
	{
		dev = btStatus->devList->GetItem(i);
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

SSWR::AVIRead::AVIRBluetoothForm::AVIRBluetoothForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Bluetooth Form"));
	this->SetFont(0, 0, 8.25, false);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lbCtrl, UI::GUIListBox(ui, this, false));
	this->lbCtrl->SetRect(0, 0, 200, 23, false);
	this->lbCtrl->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbCtrl->HandleSelectionChange(OnCtrlChanged, this);
	NEW_CLASS(this->hspMain, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpInfo = this->tcMain->AddTabPage(CSTR("Info"));
	NEW_CLASS(this->lblRadioName, UI::GUILabel(ui, this->tpInfo, CSTR("Radio Name")));
	this->lblRadioName->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtRadioName, UI::GUITextBox(ui, this->tpInfo, CSTR("")));
	this->txtRadioName->SetRect(104, 4, 200, 23, false);
	this->txtRadioName->SetReadOnly(true);
	NEW_CLASS(this->lblAddr, UI::GUILabel(ui, this->tpInfo, CSTR("Address")));
	this->lblAddr->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtAddr, UI::GUITextBox(ui, this->tpInfo, CSTR("")));
	this->txtAddr->SetRect(104, 28, 200, 23, false);
	this->txtAddr->SetReadOnly(true);
	NEW_CLASS(this->lblManu, UI::GUILabel(ui, this->tpInfo, CSTR("Manufacturer")));
	this->lblManu->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtManu, UI::GUITextBox(ui, this->tpInfo, CSTR("")));
	this->txtManu->SetRect(104, 52, 200, 23, false);
	this->txtManu->SetReadOnly(true);
	NEW_CLASS(this->lblSubver, UI::GUILabel(ui, this->tpInfo, CSTR("Subversion")));
	this->lblSubver->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtSubver, UI::GUITextBox(ui, this->tpInfo, CSTR("")));
	this->txtSubver->SetRect(104, 76, 100, 23, false);
	this->txtSubver->SetReadOnly(true);
	NEW_CLASS(this->lblClass, UI::GUILabel(ui, this->tpInfo, CSTR("Class")));
	this->lblClass->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtClass, UI::GUITextBox(ui, this->tpInfo, CSTR("")));
	this->txtClass->SetRect(104, 100, 100, 23, false);
	this->txtClass->SetReadOnly(true);

	this->tpDevice = this->tcMain->AddTabPage(CSTR("Device"));
	NEW_CLASS(this->pnlDevice, UI::GUIPanel(ui, this->tpDevice));
	this->pnlDevice->SetRect(0, 0, 100, 31, false);
	this->pnlDevice->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnDeviceSrch, UI::GUIButton(ui, this->pnlDevice, CSTR("Search")));
	this->btnDeviceSrch->SetRect(4, 4, 75, 23, false);
	this->btnDeviceSrch->HandleButtonClick(OnDeviceSrchClicked, this);
	NEW_CLASS(this->btnDeviceUpdate, UI::GUIButton(ui, this->pnlDevice, CSTR("Update")));
	this->btnDeviceUpdate->SetRect(84, 4, 75, 23, false);
	this->btnDeviceUpdate->HandleButtonClick(OnDeviceUpdateClicked, this);
	NEW_CLASS(this->pnlDevDetail, UI::GUIPanel(ui, this->tpDevice));
	this->pnlDevDetail->SetRect(0, 0, 100, 360, false);
	this->pnlDevDetail->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->vspDevice, UI::GUIVSplitter(ui, this->tpDevice, 3, true));
	NEW_CLASS(this->pnlDevCtrl, UI::GUIPanel(ui, this->pnlDevDetail));
	this->pnlDevCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlDevCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnDevAuthen, UI::GUIButton(ui, this->pnlDevCtrl, CSTR("Pair")));
	this->btnDevAuthen->SetRect(4, 4, 75, 23, false);
	this->btnDevAuthen->HandleButtonClick(OnDevAuthenClicked, this);
	NEW_CLASS(this->btnDevUnauthen, UI::GUIButton(ui, this->pnlDevCtrl, CSTR("Unpair")));
	this->btnDevUnauthen->SetRect(84, 4, 75, 23, false);
	this->btnDevUnauthen->HandleButtonClick(OnDevUnauthenClicked, this);
	NEW_CLASS(this->lbDevServices, UI::GUIListBox(ui, this->pnlDevDetail, false));
	this->lbDevServices->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lvDevice, UI::GUIListView(ui, this->tpDevice, UI::GUIListView::LVSTYLE_TABLE, 6));
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
	Data::ArrayList<IO::BTController*> btList;
	BTStatus *btStatus;
	Text::CString cstr;
	Text::StringBuilderUTF8 sb;
	this->currDev = 0;

	IO::BTManager btMgr;
	btMgr.CreateControllers(&btList);
	i = 0;
	j = btList.GetCount();
	while (i < j)
	{
		btStatus = MemAlloc(BTStatus, 1);
		btStatus->bt = btList.GetItem(i);
		NEW_CLASS(btStatus->devList, Data::ArrayList<IO::BTController::BTDevice*>());
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
	BTStatus *btStatus;
	IO::BTController::BTDevice *dev;
	this->ClearGUIDs();
	i = this->btList.GetCount();
	while (i-- > 0)
	{
		btStatus = this->btList.GetItem(i);
		j = btStatus->devList->GetCount();
		while (j-- > 0)
		{
			dev = btStatus->devList->GetItem(j);
			DEL_CLASS(dev);
		}
		DEL_CLASS(btStatus->devList);
		DEL_CLASS(btStatus->bt);
		MemFree(btStatus);
	}
}

void SSWR::AVIRead::AVIRBluetoothForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
