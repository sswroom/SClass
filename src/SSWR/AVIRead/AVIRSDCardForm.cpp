#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/Sort/ArtificialQuickSortFunc.h"
#include "IO/DeviceDB.h"
#include "IO/FileStream.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRSDCardForm.h"
#include "Text/MyString.h"

void __stdcall SSWR::AVIRead::AVIRSDCardForm::OnDevicesSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSDCardForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSDCardForm>();
	IO::SDCardInfo *sdCard;
	sdCard = (IO::SDCardInfo*)me->lbDevices->GetSelectedItem().p;
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
		UnsafeArray<UTF8Char> sptr;
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
		sptr = Text::StrInt32(sbuff, sdCard->GetManufacturingYear());
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

OSInt __stdcall SSWR::AVIRead::AVIRSDCardForm::ItemCompare(NN<IO::SDCardInfo> item1, NN<IO::SDCardInfo> item2)
{
	return item1->GetName()->CompareTo(item1->GetName());
}

SSWR::AVIRead::AVIRSDCardForm::AVIRSDCardForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText(CSTR("SD Cards"));
	this->SetFont(0, 0, 8.25, false);

	this->lbDevices = ui->NewListBox(*this, false);
	this->lbDevices->SetRect(0, 0, 100, 23, false);
	this->lbDevices->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbDevices->HandleSelectionChange(OnDevicesSelChg, this);
	this->hspDevices = ui->NewHSplitter(*this, 3, false);
	this->pnlDevices = ui->NewPanel(*this);
	this->pnlDevices->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblName = ui->NewLabel(this->pnlDevices, CSTR("Name"));
	this->lblName->SetRect(4, 4, 100, 23, false);
	this->txtName = ui->NewTextBox(this->pnlDevices, CSTR(""));
	this->txtName->SetRect(104, 4, 100, 23, false);
	this->txtName->SetReadOnly(true);
	this->lblCID = ui->NewLabel(this->pnlDevices, CSTR("CID"));
	this->lblCID->SetRect(4, 28, 100, 23, false);
	this->txtCID = ui->NewTextBox(this->pnlDevices, CSTR(""));
	this->txtCID->SetRect(104, 28, 300, 23, false);
	this->txtCID->SetReadOnly(true);
	this->lblCSD = ui->NewLabel(this->pnlDevices, CSTR("CSD"));
	this->lblCSD->SetRect(4, 52, 100, 23, false);
	this->txtCSD = ui->NewTextBox(this->pnlDevices, CSTR(""));
	this->txtCSD->SetRect(104, 52, 300, 23, false);
	this->txtCSD->SetReadOnly(true);
	this->lblMID = ui->NewLabel(this->pnlDevices, CSTR("MID"));
	this->lblMID->SetRect(4, 76, 100, 23, false);
	this->txtMID = ui->NewTextBox(this->pnlDevices, CSTR(""));
	this->txtMID->SetRect(104, 76, 100, 23, false);
	this->txtMID->SetReadOnly(true);
	this->lblOID = ui->NewLabel(this->pnlDevices, CSTR("OID"));
	this->lblOID->SetRect(4, 100, 100, 23, false);
	this->txtOID = ui->NewTextBox(this->pnlDevices, CSTR(""));
	this->txtOID->SetRect(104, 100, 100, 23, false);
	this->txtOID->SetReadOnly(true);
	this->lblProductName = ui->NewLabel(this->pnlDevices, CSTR("Name (PNM)"));
	this->lblProductName->SetRect(4, 124, 100, 23, false);
	this->txtProductName = ui->NewTextBox(this->pnlDevices, CSTR(""));
	this->txtProductName->SetRect(104, 124, 100, 23, false);
	this->txtProductName->SetReadOnly(true);
	this->lblProductRevision = ui->NewLabel(this->pnlDevices, CSTR("Revision"));
	this->lblProductRevision->SetRect(4, 148, 100, 23, false);
	this->txtProductRevision = ui->NewTextBox(this->pnlDevices, CSTR(""));
	this->txtProductRevision->SetRect(104, 148, 100, 23, false);
	this->txtProductRevision->SetReadOnly(true);
	this->lblSerialNo = ui->NewLabel(this->pnlDevices, CSTR("Serial No.(PSN)"));
	this->lblSerialNo->SetRect(4, 172, 100, 23, false);
	this->txtSerialNo = ui->NewTextBox(this->pnlDevices, CSTR(""));
	this->txtSerialNo->SetRect(104, 172, 100, 23, false);
	this->txtSerialNo->SetReadOnly(true);
	this->lblManuDate = ui->NewLabel(this->pnlDevices, CSTR("Manu Date"));
	this->lblManuDate->SetRect(4, 196, 100, 23, false);
	this->txtManuDate = ui->NewTextBox(this->pnlDevices, CSTR(""));
	this->txtManuDate->SetRect(104, 196, 100, 23, false);
	this->txtManuDate->SetReadOnly(true);
	this->lblCSDVersion = ui->NewLabel(this->pnlDevices, CSTR("CSD Version"));
	this->lblCSDVersion->SetRect(4, 220, 100, 23, false);
	this->txtCSDVersion = ui->NewTextBox(this->pnlDevices, CSTR(""));
	this->txtCSDVersion->SetRect(104, 220, 100, 23, false);
	this->txtCSDVersion->SetReadOnly(true);
	this->lblTranRate = ui->NewLabel(this->pnlDevices, CSTR("Transfer Rate"));
	this->lblTranRate->SetRect(4, 244, 100, 23, false);
	this->txtTranRate = ui->NewTextBox(this->pnlDevices, CSTR(""));
	this->txtTranRate->SetRect(104, 244, 100, 23, false);
	this->txtTranRate->SetReadOnly(true);
	this->lblCardCmdClass = ui->NewLabel(this->pnlDevices, CSTR("Cmd Class"));
	this->lblCardCmdClass->SetRect(4, 268, 100, 23, false);
	this->txtCardCmdClass = ui->NewTextBox(this->pnlDevices, CSTR(""));
	this->txtCardCmdClass->SetRect(104, 268, 100, 23, false);
	this->txtCardCmdClass->SetReadOnly(true);
	this->lblCapacity = ui->NewLabel(this->pnlDevices, CSTR("Capacity"));
	this->lblCapacity->SetRect(4, 292, 100, 23, false);
	this->txtCapacity = ui->NewTextBox(this->pnlDevices, CSTR(""));
	this->txtCapacity->SetRect(104, 292, 100, 23, false);
	this->txtCapacity->SetReadOnly(true);

	this->OnMonitorChanged();

	NN<IO::SDCardInfo> sdCard;
	UOSInt i;
	UOSInt j;
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;


	IO::SDCardMgr::GetCardList(this->sdCardList);
	Data::Sort::ArtificialQuickSortFunc<NN<IO::SDCardInfo>>::Sort(this->sdCardList, ItemCompare);
	
	i = 0;
	j = this->sdCardList.GetCount();
	while (i < j)
	{
		sdCard = this->sdCardList.GetItemNoCheck(i);
		sptr = Text::StrUOSInt(sbuff, i);
		sptr = Text::StrConcatC(sptr, UTF8STRC(" - "));
		sptr = sdCard->GetName()->ConcatTo(sptr);
		this->lbDevices->AddItem(CSTRP(sbuff, sptr), sdCard);
		i++;
	}
}

SSWR::AVIRead::AVIRSDCardForm::~AVIRSDCardForm()
{
	this->sdCardList.DeleteAll();
}

void SSWR::AVIRead::AVIRSDCardForm::OnMonitorChanged()
{
	MonitorHandle *hMon = this->GetHMonitor();
	this->SetDPI(this->core->GetMonitorHDPI(hMon), this->core->GetMonitorDDPI(hMon));
}
