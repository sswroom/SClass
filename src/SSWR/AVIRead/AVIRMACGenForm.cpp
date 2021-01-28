#include "Stdafx.h"
#include "Data/RandomMT19937.h"
#include "Data/Sort/ArtificialQuickSortC.h"
#include "Net/ConnectionInfo.h"
#include "SSWR/AVIRead/AVIRMACGenForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRMACGenForm::OnGenerateClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMACGenForm *me = (SSWR::AVIRead::AVIRMACGenForm*)userObj;
	Data::ArrayList<Net::MACInfo::MACEntry*> *macArr;
	UTF8Char sbuff[20];
	Int64 iMAC;
	UInt32 irand;
	UInt8 macBuff[8];
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	Data::RandomMT19937 random((UInt32)(dt.ToTicks() & 0xffffffff));
	irand = random.NextInt32();
	macArr = (Data::ArrayList<Net::MACInfo::MACEntry*>*)me->cboVendor->GetSelectedItem();
	if (macArr == 0)
	{
		return;
	}
	OSInt cnt = macArr->GetCount();
	Net::MACInfo::MACEntry *ent = macArr->GetItem((irand >> 24) % cnt);
	iMAC = (ent->rangeStart & 0xffffff000000) | (irand & 0xffffff);
	WriteMInt64(macBuff, iMAC);
	Text::StrHexBytes(sbuff, &macBuff[2], 6, ':');
	me->txtColonFormat->SetText(sbuff);
	Text::StrHexBytes(sbuff, &macBuff[2], 6, 0);
	me->txtPlainFormat->SetText(sbuff);
}

void __stdcall SSWR::AVIRead::AVIRMACGenForm::OnAdapterSetClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMACGenForm *me = (SSWR::AVIRead::AVIRMACGenForm*)userObj;
	UInt8 buff[12];
	Text::StringBuilderUTF8 sb1;
	Text::StringBuilderUTF8 sb2;
	me->txtPlainFormat->GetText(&sb1);
	me->cboAdapter->GetText(&sb2);
	if (sb1.GetLength() > 0 && sb2.GetLength() > 0)
	{
		Text::StrHex2Bytes(sb1.ToString(), buff);
		if (me->core->GetSocketFactory()->AdapterSetHWAddr(sb2.ToString(), buff))
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Address set", (const UTF8Char*)"MAC Generator", me);
		}
		else
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in setting address", (const UTF8Char*)"MAC Generator", me);
		}
	}	
}

OSInt __stdcall SSWR::AVIRead::AVIRMACGenForm::ListCompare(void *list1, void *list2)
{
	return Text::StrCompare(((Data::ArrayList<Net::MACInfo::MACEntry*>*)list1)->GetItem(0)->name, ((Data::ArrayList<Net::MACInfo::MACEntry*>*)list2)->GetItem(0)->name);
}

SSWR::AVIRead::AVIRMACGenForm::AVIRMACGenForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 480, 136, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"MAC Generator");
	this->SetNoResize(true);

	this->core = core;

	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblVendor, UI::GUILabel(ui, this, (const UTF8Char*)"Vendor"));
	this->lblVendor->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboVendor, UI::GUIComboBox(ui, this, false));
	this->cboVendor->SetRect(104, 4, 300, 23, false);
	NEW_CLASS(this->btnGenerate, UI::GUIButton(ui, this, (const UTF8Char*)"Generate"));
	this->btnGenerate->SetRect(104, 28, 75, 23, false);
	this->btnGenerate->HandleButtonClick(OnGenerateClicked, this);
	NEW_CLASS(this->lblColonFormat, UI::GUILabel(ui, this, (const UTF8Char*)"Colon Format"));
	this->lblColonFormat->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtColonFormat, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtColonFormat->SetRect(104, 52, 150, 23, false);
	this->txtColonFormat->SetReadOnly(true);
	NEW_CLASS(this->lblPlainFormat, UI::GUILabel(ui, this, (const UTF8Char*)"Plain Format"));
	this->lblPlainFormat->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtPlainFormat, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtPlainFormat->SetRect(104, 76, 150, 23, false);
	this->txtPlainFormat->SetReadOnly(true);
	NEW_CLASS(this->lblAdapter, UI::GUILabel(ui, this, (const UTF8Char*)"Adapter"));
	this->lblAdapter->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->cboAdapter, UI::GUIComboBox(ui, this, false));
	this->cboAdapter->SetRect(104, 100, 200, 23, false);
	NEW_CLASS(this->btnAdapterSet, UI::GUIButton(ui, this, (const UTF8Char*)"Set"));
	this->btnAdapterSet->SetRect(304, 100, 75, 23, false);
	this->btnAdapterSet->HandleButtonClick(OnAdapterSetClicked, this);

	Data::ArrayList<Net::MACInfo::MACEntry*> **macList;
	Data::ArrayList<Net::MACInfo::MACEntry*> *macArr;
	UOSInt macCnt;
	UOSInt i;
	Net::MACInfo::MACEntry *entList;
	NEW_CLASS(this->macMap, Data::BTreeUTF8Map<Data::ArrayList<Net::MACInfo::MACEntry*>*>());
	entList = Net::MACInfo::GetMACEntryList(&macCnt);
	i = 1;
	macCnt--;
	while (i < macCnt)
	{
		if (entList[i].name[0])
		{
			macArr = this->macMap->Get((const UTF8Char*)entList[i].name);
			if (macArr == 0)
			{
				NEW_CLASS(macArr, Data::ArrayList<Net::MACInfo::MACEntry*>());
				this->macMap->Put((const UTF8Char*)entList[i].name, macArr);
			}
			macArr->Add(&entList[i]);
		}
		i++;
	}

	macList = this->macMap->ToArray(&macCnt);
	ArtificialQuickSort_SortCmp((void**)macList, ListCompare, 0, macCnt - 1);
	i = 0;
	while (i < macCnt)
	{
		this->cboVendor->AddItem((const UTF8Char*)macList[i]->GetItem(0)->name, macList[i]);
		i++;
	}
	MemFree(macList);
	if (macCnt > 0)
	{
		this->cboVendor->SetSelectedIndex(0);
	}

	Net::SocketFactory *sockf = this->core->GetSocketFactory();
	Data::ArrayList<Net::ConnectionInfo*> connInfoList;
	Net::ConnectionInfo *connInfo;
	UTF8Char sbuff[64];
	UOSInt j;
	sockf->GetConnInfoList(&connInfoList);
	i = 0;
	j = connInfoList.GetCount();
	while (i < j)
	{
		connInfo = connInfoList.GetItem(i);
		if (connInfo->GetConnectionType() != Net::ConnectionInfo::CT_LOOPBACK)
		{
			connInfo->GetName(sbuff);
			this->cboAdapter->AddItem(sbuff, 0);
		}
		DEL_CLASS(connInfo);
		i++;
	}
	if (this->cboAdapter->GetCount() > 0)
	{
		this->cboAdapter->SetSelectedIndex(0);
	}	
}

SSWR::AVIRead::AVIRMACGenForm::~AVIRMACGenForm()
{
	Data::ArrayList<Net::MACInfo::MACEntry*> **macList;
	UOSInt macCnt;
	UOSInt i;
	macList = this->macMap->ToArray(&macCnt);
	i = 0;
	while (i < macCnt)
	{
		DEL_CLASS(macList[i]);
		i++;
	}
	MemFree(macList);
	DEL_CLASS(this->macMap);
}

void SSWR::AVIRead::AVIRMACGenForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
