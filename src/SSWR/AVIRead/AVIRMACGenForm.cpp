#include "Stdafx.h"
#include "Data/RandomMT19937.h"
#include "Data/Sort/ArtificialQuickSortFunc.h"
#include "Net/ConnectionInfo.h"
#include "SSWR/AVIRead/AVIRMACGenForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRMACGenForm::OnGenerateClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMACGenForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMACGenForm>();
	NN<Data::ArrayList<Net::MACInfo::MACEntry*>> macArr;
	UTF8Char sbuff[20];
	UnsafeArray<UTF8Char> sptr;
	UInt64 iMAC;
	UInt32 irand;
	UInt8 macBuff[8];
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	Data::RandomMT19937 random((UInt32)(dt.ToTicks() & 0xffffffff));
	irand = (UInt32)random.NextInt32();
	if (!me->cboVendor->GetSelectedItem().GetOpt<Data::ArrayList<Net::MACInfo::MACEntry*>>().SetTo(macArr))
	{
		return;
	}
	UOSInt cnt = macArr->GetCount();
	Net::MACInfo::MACEntry *ent = macArr->GetItem((irand >> 24) % cnt);
	iMAC = (ent->rangeStart & 0xffffff000000) | (irand & 0xffffff);
	WriteMUInt64(macBuff, iMAC);
	sptr = Text::StrHexBytes(sbuff, &macBuff[2], 6, ':');
	me->txtColonFormat->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrHexBytes(sbuff, &macBuff[2], 6, 0);
	me->txtPlainFormat->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRMACGenForm::OnAdapterSetClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMACGenForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMACGenForm>();
	UInt8 buff[12];
	Text::StringBuilderUTF8 sb1;
	Text::StringBuilderUTF8 sb2;
	me->txtPlainFormat->GetText(sb1);
	me->cboAdapter->GetText(sb2);
	if (sb1.GetLength() > 0 && sb2.GetLength() > 0)
	{
		Text::StrHex2Bytes(sb1.ToString(), buff);
		if (me->core->GetSocketFactory()->AdapterSetHWAddr(sb2.ToCString(), buff))
		{
			me->ui->ShowMsgOK(CSTR("Address set"), CSTR("MAC Generator"), me);
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("Error in setting address"), CSTR("MAC Generator"), me);
		}
	}	
}

OSInt __stdcall SSWR::AVIRead::AVIRMACGenForm::ListCompare(Data::ArrayListNN<Net::MACInfo::MACEntry> *list1, Data::ArrayListNN<Net::MACInfo::MACEntry> *list2)
{
	return Text::StrCompare(list1->GetItemNoCheck(0)->name, list2->GetItemNoCheck(0)->name);
}

SSWR::AVIRead::AVIRMACGenForm::AVIRMACGenForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 480, 136, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("MAC Generator"));
	this->SetNoResize(true);

	this->core = core;

	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblVendor = ui->NewLabel(*this, CSTR("Vendor"));
	this->lblVendor->SetRect(4, 4, 100, 23, false);
	this->cboVendor = ui->NewComboBox(*this, false);
	this->cboVendor->SetRect(104, 4, 300, 23, false);
	this->btnGenerate = ui->NewButton(*this, CSTR("Generate"));
	this->btnGenerate->SetRect(104, 28, 75, 23, false);
	this->btnGenerate->HandleButtonClick(OnGenerateClicked, this);
	this->lblColonFormat = ui->NewLabel(*this, CSTR("Colon Format"));
	this->lblColonFormat->SetRect(4, 52, 100, 23, false);
	this->txtColonFormat = ui->NewTextBox(*this, CSTR(""));
	this->txtColonFormat->SetRect(104, 52, 150, 23, false);
	this->txtColonFormat->SetReadOnly(true);
	this->lblPlainFormat = ui->NewLabel(*this, CSTR("Plain Format"));
	this->lblPlainFormat->SetRect(4, 76, 100, 23, false);
	this->txtPlainFormat = ui->NewTextBox(*this, CSTR(""));
	this->txtPlainFormat->SetRect(104, 76, 150, 23, false);
	this->txtPlainFormat->SetReadOnly(true);
	this->lblAdapter = ui->NewLabel(*this, CSTR("Adapter"));
	this->lblAdapter->SetRect(4, 100, 100, 23, false);
	this->cboAdapter = ui->NewComboBox(*this, false);
	this->cboAdapter->SetRect(104, 100, 200, 23, false);
	this->btnAdapterSet = ui->NewButton(*this, CSTR("Set"));
	this->btnAdapterSet->SetRect(304, 100, 75, 23, false);
	this->btnAdapterSet->HandleButtonClick(OnAdapterSetClicked, this);

	Data::ArrayListNN<Net::MACInfo::MACEntry> **macList;
	Data::ArrayListNN<Net::MACInfo::MACEntry> *macArr;
	UOSInt macCnt;
	UOSInt i;
	UnsafeArray<Net::MACInfo::MACEntry> entList;
	entList = Net::MACInfo::GetMACEntryList(macCnt);
	i = 1;
	macCnt--;
	while (i < macCnt)
	{
		if (entList[i].nameLen > 0)
		{
			macArr = this->macMap.Get({entList[i].name, entList[i].nameLen});
			if (macArr == 0)
			{
				NEW_CLASS(macArr, Data::ArrayListNN<Net::MACInfo::MACEntry>());
				this->macMap.Put({entList[i].name, entList[i].nameLen}, macArr);
			}
			macArr->Add(entList[i]);
		}
		i++;
	}

	macList = this->macMap.ToArray(macCnt);
	Data::Sort::ArtificialQuickSortFunc<Data::ArrayListNN<Net::MACInfo::MACEntry>*>::Sort(macList, ListCompare, 0, (OSInt)macCnt - 1);
	i = 0;
	while (i < macCnt)
	{
		NN<Net::MACInfo::MACEntry> ent = macList[i]->GetItemNoCheck(0);
		this->cboVendor->AddItem({ent->name, ent->nameLen}, macList[i]);
		i++;
	}
	MemFree(macList);
	if (macCnt > 0)
	{
		this->cboVendor->SetSelectedIndex(0);
	}

	NN<Net::SocketFactory> sockf = this->core->GetSocketFactory();
	Data::ArrayListNN<Net::ConnectionInfo> connInfoList;
	NN<Net::ConnectionInfo> connInfo;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UOSInt j;
	sockf->GetConnInfoList(connInfoList);
	i = 0;
	j = connInfoList.GetCount();
	while (i < j)
	{
		connInfo = connInfoList.GetItemNoCheck(i);
		if (connInfo->GetConnectionType() != Net::ConnectionInfo::ConnectionType::Loopback)
		{
			sptr = connInfo->GetName(sbuff).Or(sbuff);
			this->cboAdapter->AddItem(CSTRP(sbuff, sptr), 0);
		}
		connInfo.Delete();
		i++;
	}
	if (this->cboAdapter->GetCount() > 0)
	{
		this->cboAdapter->SetSelectedIndex(0);
	}	
}

SSWR::AVIRead::AVIRMACGenForm::~AVIRMACGenForm()
{
	Data::ArrayListNN<Net::MACInfo::MACEntry> **macList;
	UOSInt macCnt;
	UOSInt i;
	macList = this->macMap.ToArray(macCnt);
	i = 0;
	while (i < macCnt)
	{
		DEL_CLASS(macList[i]);
		i++;
	}
	MemFree(macList);
}

void SSWR::AVIRead::AVIRMACGenForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
