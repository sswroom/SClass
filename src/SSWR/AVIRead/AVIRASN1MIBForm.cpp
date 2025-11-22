#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "Net/ASN1OIDDB.h"
#include "Net/ASN1Util.h"
#include "SSWR/AVIRead/AVIRASN1MIBForm.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIRASN1MIBForm::OnFileDroped(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRASN1MIBForm> me = userObj.GetNN<SSWR::AVIRead::AVIRASN1MIBForm>();
	me->LoadFile(files[0]->ToCString());
}

void __stdcall SSWR::AVIRead::AVIRASN1MIBForm::OnBrowseClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRASN1MIBForm> me = userObj.GetNN<SSWR::AVIRead::AVIRASN1MIBForm>();
	Text::StringBuilderUTF8 sb;
	me->txtFile->GetText(sb);
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"ASN1MIB", false);
	dlg->AddFilter(CSTR("*.asn"), CSTR("ASN.1 MIB File"));
	dlg->AddFilter(CSTR("*.mib"), CSTR("MIB file"));
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->LoadFile(dlg->GetFileName()->ToCString());
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRASN1MIBForm::OnObjectsSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRASN1MIBForm> me = userObj.GetNN<SSWR::AVIRead::AVIRASN1MIBForm>();
	NN<Net::ASN1MIB::ObjectInfo> obj;
	me->lvObjectsVal->ClearItems();
	if (me->lvObjects->GetSelectedItem().GetOpt<Net::ASN1MIB::ObjectInfo>().SetTo(obj))
	{
		UOSInt i = 0;
		UOSInt j = obj->valName.GetCount();
		while (i < j)
		{
			me->lvObjectsVal->AddItem(Text::String::OrEmpty(obj->valName.GetItem(i)), 0);
			me->lvObjectsVal->SetSubItem(i, 1, Text::String::OrEmpty(obj->valCont.GetItem(i)));
			i++;
		}
	}
}

void SSWR::AVIRead::AVIRASN1MIBForm::LoadFile(Text::CStringNN fileName)
{
	Text::StringBuilderUTF8 sb;
	this->txtFile->SetText(fileName);
	this->mib.UnloadAll();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Error in loading MIB file: "));
	if (this->mib.LoadFile(fileName, sb))
	{

	}
	else
	{
		this->ui->ShowMsgOK(sb.ToCString(), CSTR("SNMP MIB"), this);
	}
	this->lvObjects->ClearItems();
	this->lvOID->ClearItems();
	UOSInt i;
	UOSInt j;
	NN<Net::ASN1MIB::ObjectInfo> obj;
	NN<Net::ASN1MIB::ModuleInfo> module;
	if (!this->mib.GetModuleByFileName(fileName).SetTo(module))
	{
		module = this->mib.GetGlobalModule();
	}
	NN<Data::ArrayListNN<Net::ASN1MIB::ObjectInfo>> objList = module->objValues;
	NN<Text::String> s;
	i = 0;
	j = objList->GetCount();
	while (i < j)
	{
		obj = objList->GetItemNoCheck(i);
		this->lvObjects->AddItem(Text::String::OrEmpty(obj->objectName), obj);
		if (obj->oidLen > 0)
		{
			sb.ClearStr();
			Net::ASN1Util::OIDToString(Data::ByteArrayR(obj->oid, obj->oidLen), sb);
			this->lvObjects->SetSubItem(i, 1, sb.ToCString());
		}
		if (s.Set(obj->typeName))
		{
			this->lvObjects->SetSubItem(i, 2, s);
		}
		if (s.Set(obj->typeVal))
		{
			this->lvObjects->SetSubItem(i, 3, s);
		}
		i++;
	}
	Text::StringBuilderUTF8 sbOIDText;
	i = 0;
	j = module->oidList.GetCount();
	while (i < j)
	{
		obj = module->oidList.GetItemNoCheck(i);
		sb.ClearStr();
		Net::ASN1Util::OIDToString(Data::ByteArrayR(obj->oid, obj->oidLen), sb);
		this->lvOID->AddItem(sb.ToCString(), obj);
		NN<const Net::ASN1OIDDB::OIDInfo> entry;
		if (Net::ASN1OIDDB::OIDGetEntry(Data::ByteArrayR(obj->oid, obj->oidLen)).SetTo(entry))
		{
			this->lvOID->SetSubItem(i, 1, Text::CStringNN::FromPtr((const UTF8Char*)entry->name));
		}
		this->lvOID->SetSubItem(i, 2, Text::String::OrEmpty(obj->objectName));

		Net::ASN1Util::OIDToCPPCode(Data::ByteArrayR(obj->oid, obj->oidLen), obj->objectName->ToCString(), sbOIDText);
		i++;
	}
	this->txtOIDText->SetText(sbOIDText.ToCString());
}

SSWR::AVIRead::AVIRASN1MIBForm::AVIRASN1MIBForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("ASN.1 MIB"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlRequest = ui->NewPanel(*this);
	this->pnlRequest->SetRect(0, 0, 100, 31, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblFile = ui->NewLabel(this->pnlRequest, CSTR("MIB File"));
	this->lblFile->SetRect(4, 4, 100, 23, false);
	this->txtFile = ui->NewTextBox(this->pnlRequest, CSTR(""));
	this->txtFile->SetRect(104, 4, 500, 23, false);
	this->txtFile->SetReadOnly(true);
	this->btnBrowse = ui->NewButton(this->pnlRequest, CSTR("B&rowse"));
	this->btnBrowse->SetRect(604, 4, 75, 23, false);
	this->btnBrowse->HandleButtonClick(OnBrowseClicked, this);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpObjects = this->tcMain->AddTabPage(CSTR("Objects"));
	this->lvObjectsVal = ui->NewListView(this->tpObjects, UI::ListViewStyle::Table, 2);
	this->lvObjectsVal->SetRect(0, 0, 100, 150, false);
	this->lvObjectsVal->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lvObjectsVal->SetShowGrid(true);
	this->lvObjectsVal->SetFullRowSelect(true);
	this->lvObjectsVal->AddColumn(CSTR("Name"), 200);
	this->lvObjectsVal->AddColumn(CSTR("Value"), 600);
	this->vspObjects = ui->NewVSplitter(this->tpObjects, 3, true);
	this->lvObjects = ui->NewListView(this->tpObjects, UI::ListViewStyle::Table, 4);
	this->lvObjects->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvObjects->SetShowGrid(true);
	this->lvObjects->SetFullRowSelect(true);
	this->lvObjects->AddColumn(CSTR("Name"), 200);
	this->lvObjects->AddColumn(CSTR("OID"), 200);
	this->lvObjects->AddColumn(CSTR("Type"), 200);
	this->lvObjects->AddColumn(CSTR("Value"), 200);
	this->lvObjects->HandleSelChg(OnObjectsSelChg, this);

	this->tpOID = this->tcMain->AddTabPage(CSTR("OID"));
	this->lvOID = ui->NewListView(this->tpOID, UI::ListViewStyle::Table, 3);
	this->lvOID->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvOID->SetShowGrid(true);
	this->lvOID->SetFullRowSelect(true);
	this->lvOID->AddColumn(CSTR("OID"), 200);
	this->lvOID->AddColumn(CSTR("DB Name"), 200);
	this->lvOID->AddColumn(CSTR("Name"), 200);

	this->tpOIDText = this->tcMain->AddTabPage(CSTR("OIDText"));
	this->txtOIDText = ui->NewTextBox(this->tpOIDText, CSTR(""), true);
	this->txtOIDText->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtOIDText->SetReadOnly(true);

	this->HandleDropFiles(OnFileDroped, this);
}

SSWR::AVIRead::AVIRASN1MIBForm::~AVIRASN1MIBForm()
{
}

void SSWR::AVIRead::AVIRASN1MIBForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
