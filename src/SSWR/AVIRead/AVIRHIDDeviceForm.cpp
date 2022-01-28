#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "IO/DeviceDB.h"
#include "IO/FileStream.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRHIDDeviceForm.h"
#include "Text/MyString.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRHIDDeviceForm::OnDevicesSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRHIDDeviceForm *me = (SSWR::AVIRead::AVIRHIDDeviceForm*)userObj;
	IO::HIDInfo *hid;
	hid = (IO::HIDInfo*)me->lbDevices->GetSelectedItem();
	if (hid == 0)
	{
		me->txtVendorId->SetText((const UTF8Char*)"");
		me->txtProductId->SetText((const UTF8Char*)"");
		me->txtDevPath->SetText((const UTF8Char*)"");
	}
	else
	{
		UTF8Char sbuff[32];
		Text::StrHexVal16(sbuff, hid->GetVendorId());
		me->txtVendorId->SetText(sbuff);
		Text::StrHexVal16(sbuff, hid->GetProductId());
		me->txtProductId->SetText(sbuff);
		me->txtDevPath->SetText(hid->GetDevPath()->v);
	}
}

OSInt __stdcall SSWR::AVIRead::AVIRHIDDeviceForm::ItemCompare(void *item1, void *item2)
{
	IO::HIDInfo *hid1 = (IO::HIDInfo*)item1;
	IO::HIDInfo *hid2 = (IO::HIDInfo*)item2;
	if (hid1->GetVendorId() > hid2->GetVendorId())
	{
		return 1;
	}
	else if (hid1->GetVendorId() < hid2->GetVendorId())
	{
		return -1;
	}
	if (hid1->GetProductId() > hid2->GetProductId())
	{
		return 1;
	}
	else if (hid1->GetProductId() < hid2->GetProductId())
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

SSWR::AVIRead::AVIRHIDDeviceForm::AVIRHIDDeviceForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText((const UTF8Char*)"HID Devices");
	this->SetFont(0, 0, 8.25, false);

	NEW_CLASS(this->lbDevices, UI::GUIListBox(ui, this, false));
	this->lbDevices->SetRect(0, 0, 100, 23, false);
	this->lbDevices->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDevices->HandleSelectionChange(OnDevicesSelChg, this);
	NEW_CLASS(this->hspDevices, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->pnlDevices, UI::GUIPanel(ui, this));
	this->pnlDevices->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lblVendorId, UI::GUILabel(ui, this->pnlDevices, (const UTF8Char*)"VendorId"));
	this->lblVendorId->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtVendorId, UI::GUITextBox(ui, this->pnlDevices, (const UTF8Char*)""));
	this->txtVendorId->SetRect(104, 4, 100, 23, false);
	this->txtVendorId->SetReadOnly(true);
	NEW_CLASS(this->lblProductId, UI::GUILabel(ui, this->pnlDevices, (const UTF8Char*)"ProductId"));
	this->lblProductId->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtProductId, UI::GUITextBox(ui, this->pnlDevices, (const UTF8Char*)""));
	this->txtProductId->SetRect(104, 28, 100, 23, false);
	this->txtProductId->SetReadOnly(true);
	NEW_CLASS(this->lblDevPath, UI::GUILabel(ui, this->pnlDevices, (const UTF8Char*)"Dev Path"));
	this->lblDevPath->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtDevPath, UI::GUITextBox(ui, this->pnlDevices, (const UTF8Char*)""));
	this->txtDevPath->SetRect(104, 52, 600, 23, false);
	this->txtDevPath->SetReadOnly(true);

	this->OnMonitorChanged();

	IO::HIDInfo *hid;
	UOSInt i;
	UOSInt j;
	UTF8Char sbuff[32];
	UTF8Char *sptr;

	NEW_CLASS(this->hidList, Data::ArrayList<IO::HIDInfo*>());

	IO::HIDInfo::GetHIDList(this->hidList);
	void **arr = (void**)this->hidList->GetArray(&j);
	ArtificialQuickSort_SortCmp(arr, ItemCompare, 0, (OSInt)j - 1);
	
	i = 0;
	j = this->hidList->GetCount();
	while (i < j)
	{
		hid = this->hidList->GetItem(i);
		sptr = Text::StrHexVal16(sbuff, hid->GetVendorId());
		*sptr++ = ':';
		sptr = Text::StrHexVal16(sptr, hid->GetProductId());
		this->lbDevices->AddItem(sbuff, hid);
		i++;
	}
}

SSWR::AVIRead::AVIRHIDDeviceForm::~AVIRHIDDeviceForm()
{
	IO::HIDInfo *hid;
	UOSInt i = this->hidList->GetCount();
	while (i-- > 0)
	{
		hid = this->hidList->GetItem(i);
		DEL_CLASS(hid);
	}
	DEL_CLASS(this->hidList);
}

void SSWR::AVIRead::AVIRHIDDeviceForm::OnMonitorChanged()
{
	MonitorHandle *hMon = this->GetHMonitor();
	this->SetDPI(this->core->GetMonitorHDPI(hMon), this->core->GetMonitorDDPI(hMon));
}
