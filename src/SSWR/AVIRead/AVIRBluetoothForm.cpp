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

		me->txtRadioName->SetText(btStatus->bt->GetName()->v);
		IO::BTUtil::GetAddrText(&sb, btStatus->bt->GetAddress());
		me->txtAddr->SetText(sb.ToString());
		me->txtManu->SetText(IO::BTUtil::GetManufacturerName(btStatus->bt->GetManufacturer()).v);
		sb.ClearStr();
		sb.AppendC(UTF8STRC("0x"));
		sb.AppendHex16(btStatus->bt->GetSubversion());
		me->txtSubver->SetText(sb.ToString());
		sb.ClearStr();
		sb.AppendC(UTF8STRC("0x"));
		sb.AppendHex32(btStatus->bt->GetDevClass());
		me->txtClass->SetText(sb.ToString());

		me->UpdateDevList(btStatus);
	}
	else
	{
		me->txtRadioName->SetText((const UTF8Char*)"");
		me->txtAddr->SetText((const UTF8Char*)"");
		me->txtManu->SetText((const UTF8Char*)"");
		me->txtSubver->SetText((const UTF8Char*)"");
		me->txtClass->SetText((const UTF8Char*)"");

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
		dev->QueryServices(me->guidList);
		i = 0;
		j = me->guidList->GetCount();
		while (i < j)
		{
			guid = me->guidList->GetItem(i);
			sb.ClearStr();
			IO::BTUtil::GetServiceName(&sb, guid);
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
		this->currDev->FreeServices(this->guidList);
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
		IO::BTUtil::GetAddrText(&sb, dev->GetAddress());
		this->lvDevice->SetSubItem(k, 1, sb.ToString());
		const Net::MACInfo::MACEntry *mac = Net::MACInfo::GetMACInfo(IO::BTUtil::GetAddrMAC(dev->GetAddress()));
		this->lvDevice->SetSubItem(k, 2, mac->name);
		this->lvDevice->SetSubItem(k, 3, dev->IsConnected()?(const UTF8Char*)"Y":(const UTF8Char*)"N");
		this->lvDevice->SetSubItem(k, 4, dev->IsAuthenticated()?(const UTF8Char*)"Y":(const UTF8Char*)"N");
		this->lvDevice->SetSubItem(k, 5, dev->IsRemembered()?(const UTF8Char*)"Y":(const UTF8Char*)"N");
		i++;
	}
}

SSWR::AVIRead::AVIRBluetoothForm::AVIRBluetoothForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText((const UTF8Char*)"Bluetooth Form");
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
	NEW_CLASS(this->lblRadioName, UI::GUILabel(ui, this->tpInfo, (const UTF8Char*)"Radio Name"));
	this->lblRadioName->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtRadioName, UI::GUITextBox(ui, this->tpInfo, CSTR("")));
	this->txtRadioName->SetRect(104, 4, 200, 23, false);
	this->txtRadioName->SetReadOnly(true);
	NEW_CLASS(this->lblAddr, UI::GUILabel(ui, this->tpInfo, (const UTF8Char*)"Address"));
	this->lblAddr->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtAddr, UI::GUITextBox(ui, this->tpInfo, CSTR("")));
	this->txtAddr->SetRect(104, 28, 200, 23, false);
	this->txtAddr->SetReadOnly(true);
	NEW_CLASS(this->lblManu, UI::GUILabel(ui, this->tpInfo, (const UTF8Char*)"Manufacturer"));
	this->lblManu->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtManu, UI::GUITextBox(ui, this->tpInfo, CSTR("")));
	this->txtManu->SetRect(104, 52, 200, 23, false);
	this->txtManu->SetReadOnly(true);
	NEW_CLASS(this->lblSubver, UI::GUILabel(ui, this->tpInfo, (const UTF8Char*)"Subversion"));
	this->lblSubver->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtSubver, UI::GUITextBox(ui, this->tpInfo, CSTR("")));
	this->txtSubver->SetRect(104, 76, 100, 23, false);
	this->txtSubver->SetReadOnly(true);
	NEW_CLASS(this->lblClass, UI::GUILabel(ui, this->tpInfo, (const UTF8Char*)"Class"));
	this->lblClass->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtClass, UI::GUITextBox(ui, this->tpInfo, CSTR("")));
	this->txtClass->SetRect(104, 100, 100, 23, false);
	this->txtClass->SetReadOnly(true);

	this->tpDevice = this->tcMain->AddTabPage(CSTR("Device"));
	NEW_CLASS(this->pnlDevice, UI::GUIPanel(ui, this->tpDevice));
	this->pnlDevice->SetRect(0, 0, 100, 31, false);
	this->pnlDevice->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnDeviceSrch, UI::GUIButton(ui, this->pnlDevice, (const UTF8Char*)"Search"));
	this->btnDeviceSrch->SetRect(4, 4, 75, 23, false);
	this->btnDeviceSrch->HandleButtonClick(OnDeviceSrchClicked, this);
	NEW_CLASS(this->btnDeviceUpdate, UI::GUIButton(ui, this->pnlDevice, (const UTF8Char*)"Update"));
	this->btnDeviceUpdate->SetRect(84, 4, 75, 23, false);
	this->btnDeviceUpdate->HandleButtonClick(OnDeviceUpdateClicked, this);
	NEW_CLASS(this->pnlDevDetail, UI::GUIPanel(ui, this->tpDevice));
	this->pnlDevDetail->SetRect(0, 0, 100, 360, false);
	this->pnlDevDetail->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->vspDevice, UI::GUIVSplitter(ui, this->tpDevice, 3, true));
	NEW_CLASS(this->pnlDevCtrl, UI::GUIPanel(ui, this->pnlDevDetail));
	this->pnlDevCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlDevCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnDevAuthen, UI::GUIButton(ui, this->pnlDevCtrl, (const UTF8Char*)"Pair"));
	this->btnDevAuthen->SetRect(4, 4, 75, 23, false);
	this->btnDevAuthen->HandleButtonClick(OnDevAuthenClicked, this);
	NEW_CLASS(this->btnDevUnauthen, UI::GUIButton(ui, this->pnlDevCtrl, (const UTF8Char*)"Unpair"));
	this->btnDevUnauthen->SetRect(84, 4, 75, 23, false);
	this->btnDevUnauthen->HandleButtonClick(OnDevUnauthenClicked, this);
	NEW_CLASS(this->lbDevServices, UI::GUIListBox(ui, this->pnlDevDetail, false));
	this->lbDevServices->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lvDevice, UI::GUIListView(ui, this->tpDevice, UI::GUIListView::LVSTYLE_TABLE, 6));
	this->lvDevice->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDevice->HandleSelChg(OnDeviceSelChg, this);
	this->lvDevice->AddColumn((const UTF8Char*)"Name", 150);
	this->lvDevice->AddColumn((const UTF8Char*)"Address", 150);
	this->lvDevice->AddColumn((const UTF8Char*)"Vendor", 150);
	this->lvDevice->AddColumn((const UTF8Char*)"Connected", 80);
	this->lvDevice->AddColumn((const UTF8Char*)"Authen", 60);
	this->lvDevice->AddColumn((const UTF8Char*)"Remember", 80);

	UOSInt i;
	UOSInt j;
	NEW_CLASS(this->btList, Data::ArrayList<BTStatus*>());
	NEW_CLASS(this->guidList, Data::ArrayList<void *>());
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
		this->btList->Add(btStatus);
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
	i = this->btList->GetCount();
	while (i-- > 0)
	{
		btStatus = this->btList->GetItem(i);
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
	DEL_CLASS(this->btList);
	DEL_CLASS(this->guidList);
}

void SSWR::AVIRead::AVIRBluetoothForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
