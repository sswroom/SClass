#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "IO/DeviceDB.h"
#include "IO/FileStream.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRSDCardForm.h"
#include "Text/MyString.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRSDCardForm::OnDevicesSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRSDCardForm *me = (SSWR::AVIRead::AVIRSDCardForm*)userObj;
	IO::SDCardInfo *sdCard;
	sdCard = (IO::SDCardInfo*)me->lbDevices->GetSelectedItem();
	if (sdCard == 0)
	{
		me->txtName->SetText(CSTR(""));
		me->txtCID->SetText(CSTR(""));
		me->txtCSD->SetText(CSTR(""));
		me->txtMID->SetText(CSTR(""));
		me->txtOID->SetText(CSTR(""));
		me->txtProductName->SetText(CSTR(""));
		me->txtProductRevision->SetText(CSTR(""));
		me->txtSerialNo->SetText(CSTR(""));
		me->txtManuDate->SetText(CSTR(""));
		me->txtCSDVersion->SetText(CSTR(""));
		me->txtTranRate->SetText(CSTR(""));
		me->txtCardCmdClass->SetText(CSTR(""));
		me->txtCapacity->SetText(CSTR(""));
	}
	else
	{
		UInt8 buff[32];
		UTF8Char sbuff[64];
		UTF8Char *sptr;
		me->txtName->SetText(sdCard->GetName()->ToCString());
		sdCard->GetCID(buff);
		sptr = Text::StrHexBytes(sbuff, buff, 16, 0);
		me->txtCID->SetText(CSTRP(sbuff, sptr));
		sdCard->GetCSD(buff);
		sptr = Text::StrHexBytes(sbuff, buff, 16, 0);
		me->txtCSD->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUInt16(sbuff, sdCard->GetManufacturerID());
		me->txtMID->SetText(CSTRP(sbuff, sptr));
		WriteMInt16(sbuff, sdCard->GetOEMID());
		if (sbuff[0] >= 'A' && sbuff[0] <= 'z' && sbuff[1] >= 'A' && sbuff[1] <= 'z')
		{
			sbuff[2] = 0;
			me->txtOID->SetText({sbuff, 2});
		}
		else
		{
			sptr = Text::StrHexVal16(Text::StrConcatC(sbuff, UTF8STRC("0x")), sdCard->GetOEMID());
			me->txtOID->SetText(CSTRP(sbuff, sptr));
		}
		sptr = (UTF8Char*)sdCard->GetProductName((Char*)sbuff);
		me->txtProductName->SetText(CSTRP(sbuff, sptr));
		buff[0] = sdCard->GetProductRevision();
		sptr = Text::StrUInt16(sbuff, (UInt8)(buff[0] >> 4));
		*sptr++ = '.';
		sptr = Text::StrUInt16(sptr, buff[0] & 15);
		me->txtProductRevision->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrHexVal32(sbuff, sdCard->GetSerialNo());
		me->txtSerialNo->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUInt32(sbuff, sdCard->GetManufacturingYear());
		*sptr++ = '/';
		sptr = Text::StrUInt16(sptr, sdCard->GetManufacturingMonth());
		me->txtManuDate->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUInt16(sbuff, sdCard->GetCSDVersion());
		me->txtCSDVersion->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt64(sbuff, sdCard->GetMaxTranRate());
		me->txtTranRate->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrUInt16(sbuff, sdCard->GetCardCmdClass());
		me->txtCardCmdClass->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt64(sbuff, sdCard->GetCardCapacity());
		me->txtCapacity->SetText(CSTRP(sbuff, sptr));
	}
}

OSInt __stdcall SSWR::AVIRead::AVIRSDCardForm::ItemCompare(void *item1, void *item2)
{
	IO::SDCardInfo *sdCard1 = (IO::SDCardInfo*)item1;
	IO::SDCardInfo *sdCard2 = (IO::SDCardInfo*)item2;
	return sdCard1->GetName()->CompareTo(sdCard2->GetName().Ptr());
}

SSWR::AVIRead::AVIRSDCardForm::AVIRSDCardForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText(CSTR("SD Cards"));
	this->SetFont(0, 0, 8.25, false);

	NEW_CLASS(this->lbDevices, UI::GUIListBox(ui, this, false));
	this->lbDevices->SetRect(0, 0, 100, 23, false);
	this->lbDevices->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDevices->HandleSelectionChange(OnDevicesSelChg, this);
	NEW_CLASS(this->hspDevices, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->pnlDevices, UI::GUIPanel(ui, this));
	this->pnlDevices->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lblName, UI::GUILabel(ui, this->pnlDevices, CSTR("Name")));
	this->lblName->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtName, UI::GUITextBox(ui, this->pnlDevices, CSTR("")));
	this->txtName->SetRect(104, 4, 100, 23, false);
	this->txtName->SetReadOnly(true);
	NEW_CLASS(this->lblCID, UI::GUILabel(ui, this->pnlDevices, CSTR("CID")));
	this->lblCID->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtCID, UI::GUITextBox(ui, this->pnlDevices, CSTR("")));
	this->txtCID->SetRect(104, 28, 300, 23, false);
	this->txtCID->SetReadOnly(true);
	NEW_CLASS(this->lblCSD, UI::GUILabel(ui, this->pnlDevices, CSTR("CSD")));
	this->lblCSD->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtCSD, UI::GUITextBox(ui, this->pnlDevices, CSTR("")));
	this->txtCSD->SetRect(104, 52, 300, 23, false);
	this->txtCSD->SetReadOnly(true);
	NEW_CLASS(this->lblMID, UI::GUILabel(ui, this->pnlDevices, CSTR("MID")));
	this->lblMID->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtMID, UI::GUITextBox(ui, this->pnlDevices, CSTR("")));
	this->txtMID->SetRect(104, 76, 100, 23, false);
	this->txtMID->SetReadOnly(true);
	NEW_CLASS(this->lblOID, UI::GUILabel(ui, this->pnlDevices, CSTR("OID")));
	this->lblOID->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtOID, UI::GUITextBox(ui, this->pnlDevices, CSTR("")));
	this->txtOID->SetRect(104, 100, 100, 23, false);
	this->txtOID->SetReadOnly(true);
	NEW_CLASS(this->lblProductName, UI::GUILabel(ui, this->pnlDevices, CSTR("Name (PNM)")));
	this->lblProductName->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtProductName, UI::GUITextBox(ui, this->pnlDevices, CSTR("")));
	this->txtProductName->SetRect(104, 124, 100, 23, false);
	this->txtProductName->SetReadOnly(true);
	NEW_CLASS(this->lblProductRevision, UI::GUILabel(ui, this->pnlDevices, CSTR("Revision")));
	this->lblProductRevision->SetRect(4, 148, 100, 23, false);
	NEW_CLASS(this->txtProductRevision, UI::GUITextBox(ui, this->pnlDevices, CSTR("")));
	this->txtProductRevision->SetRect(104, 148, 100, 23, false);
	this->txtProductRevision->SetReadOnly(true);
	NEW_CLASS(this->lblSerialNo, UI::GUILabel(ui, this->pnlDevices, CSTR("Serial No.(PSN)")));
	this->lblSerialNo->SetRect(4, 172, 100, 23, false);
	NEW_CLASS(this->txtSerialNo, UI::GUITextBox(ui, this->pnlDevices, CSTR("")));
	this->txtSerialNo->SetRect(104, 172, 100, 23, false);
	this->txtSerialNo->SetReadOnly(true);
	NEW_CLASS(this->lblManuDate, UI::GUILabel(ui, this->pnlDevices, CSTR("Manu Date")));
	this->lblManuDate->SetRect(4, 196, 100, 23, false);
	NEW_CLASS(this->txtManuDate, UI::GUITextBox(ui, this->pnlDevices, CSTR("")));
	this->txtManuDate->SetRect(104, 196, 100, 23, false);
	this->txtManuDate->SetReadOnly(true);
	NEW_CLASS(this->lblCSDVersion, UI::GUILabel(ui, this->pnlDevices, CSTR("CSD Version")));
	this->lblCSDVersion->SetRect(4, 220, 100, 23, false);
	NEW_CLASS(this->txtCSDVersion, UI::GUITextBox(ui, this->pnlDevices, CSTR("")));
	this->txtCSDVersion->SetRect(104, 220, 100, 23, false);
	this->txtCSDVersion->SetReadOnly(true);
	NEW_CLASS(this->lblTranRate, UI::GUILabel(ui, this->pnlDevices, CSTR("Transfer Rate")));
	this->lblTranRate->SetRect(4, 244, 100, 23, false);
	NEW_CLASS(this->txtTranRate, UI::GUITextBox(ui, this->pnlDevices, CSTR("")));
	this->txtTranRate->SetRect(104, 244, 100, 23, false);
	this->txtTranRate->SetReadOnly(true);
	NEW_CLASS(this->lblCardCmdClass, UI::GUILabel(ui, this->pnlDevices, CSTR("Cmd Class")));
	this->lblCardCmdClass->SetRect(4, 268, 100, 23, false);
	NEW_CLASS(this->txtCardCmdClass, UI::GUITextBox(ui, this->pnlDevices, CSTR("")));
	this->txtCardCmdClass->SetRect(104, 268, 100, 23, false);
	this->txtCardCmdClass->SetReadOnly(true);
	NEW_CLASS(this->lblCapacity, UI::GUILabel(ui, this->pnlDevices, CSTR("Capacity")));
	this->lblCapacity->SetRect(4, 292, 100, 23, false);
	NEW_CLASS(this->txtCapacity, UI::GUITextBox(ui, this->pnlDevices, CSTR("")));
	this->txtCapacity->SetRect(104, 292, 100, 23, false);
	this->txtCapacity->SetReadOnly(true);

	this->OnMonitorChanged();

	IO::SDCardInfo *sdCard;
	UOSInt i;
	UOSInt j;
	UTF8Char sbuff[32];
	UTF8Char *sptr;

	NEW_CLASS(this->sdCardList, Data::ArrayList<IO::SDCardInfo*>());

	IO::SDCardMgr::GetCardList(this->sdCardList);
	void **arr = (void**)this->sdCardList->GetArray(&j);
	ArtificialQuickSort_SortCmp(arr, ItemCompare, 0, (OSInt)j - 1);
	
	i = 0;
	j = this->sdCardList->GetCount();
	while (i < j)
	{
		sdCard = this->sdCardList->GetItem(i);
		sptr = Text::StrUOSInt(sbuff, i);
		sptr = Text::StrConcatC(sptr, UTF8STRC(" - "));
		sptr = sdCard->GetName()->ConcatTo(sptr);
		this->lbDevices->AddItem(CSTRP(sbuff, sptr), sdCard);
		i++;
	}
}

SSWR::AVIRead::AVIRSDCardForm::~AVIRSDCardForm()
{
	IO::SDCardInfo *sdCard;
	UOSInt i = this->sdCardList->GetCount();
	while (i-- > 0)
	{
		sdCard = this->sdCardList->GetItem(i);
		DEL_CLASS(sdCard);
	}
	DEL_CLASS(this->sdCardList);
}

void SSWR::AVIRead::AVIRSDCardForm::OnMonitorChanged()
{
	MonitorHandle *hMon = this->GetHMonitor();
	this->SetDPI(this->core->GetMonitorHDPI(hMon), this->core->GetMonitorDDPI(hMon));
}
