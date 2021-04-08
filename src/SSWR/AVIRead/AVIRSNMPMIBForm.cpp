#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/SNMPInfo.h"
#include "Net/SNMPUtil.h"
#include "SSWR/AVIRead/AVIRSNMPMIBForm.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRSNMPMIBForm::OnFileDroped(void *userObj, const UTF8Char **files, OSInt nFiles)
{
	SSWR::AVIRead::AVIRSNMPMIBForm *me = (SSWR::AVIRead::AVIRSNMPMIBForm*)userObj;
	me->LoadFile(files[0]);
}

void __stdcall SSWR::AVIRead::AVIRSNMPMIBForm::OnBrowseClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSNMPMIBForm *me = (SSWR::AVIRead::AVIRSNMPMIBForm*)userObj;
	Text::StringBuilderUTF8 sb;
	UI::FileDialog *dlg;
	me->txtFile->GetText(&sb);
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"SNMPMIB", false));
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->LoadFile(dlg->GetFileName());
	}
	DEL_CLASS(dlg);
}

void __stdcall SSWR::AVIRead::AVIRSNMPMIBForm::OnObjectsSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRSNMPMIBForm *me = (SSWR::AVIRead::AVIRSNMPMIBForm*)userObj;
	Net::SNMPMIB::ObjectInfo *obj = (Net::SNMPMIB::ObjectInfo*)me->lvObjects->GetSelectedItem();
	me->lvObjectsVal->ClearItems();
	if (obj)
	{
		UOSInt i = 0;
		UOSInt j = obj->valName->GetCount();
		while (i < j)
		{
			me->lvObjectsVal->AddItem(obj->valName->GetItem(i), 0);
			me->lvObjectsVal->SetSubItem(i, 1, obj->valCont->GetItem(i));
			i++;
		}
	}
}

void SSWR::AVIRead::AVIRSNMPMIBForm::LoadFile(const UTF8Char *fileName)
{
	Text::StringBuilderUTF8 sb;
	this->txtFile->SetText(fileName);
	this->mib->UnloadAll();
	sb.ClearStr();
	sb.Append((const UTF8Char*)"Error in loading MIB file: ");
	if (this->mib->LoadFile(fileName, &sb))
	{

	}
	else
	{
		UI::MessageDialog::ShowDialog(sb.ToString(), (const UTF8Char*)"SNMP MIB", this);
	}
	this->lvObjects->ClearItems();
	this->lvOID->ClearItems();
	UOSInt i;
	UOSInt j;
	OSInt k;
	Net::SNMPMIB::ObjectInfo *obj;
	Net::SNMPMIB::ModuleInfo *module = this->mib->GetModuleByFileName(fileName);
	if (module == 0)
	{
		module = this->mib->GetGlobalModule();
	}
	Data::ArrayList<Net::SNMPMIB::ObjectInfo *> *objList = module->objValues;
	i = 0;
	j = objList->GetCount();
	while (i < j)
	{
		obj = objList->GetItem(i);
		this->lvObjects->AddItem(obj->objectName, obj);
		if (obj->oidLen > 0)
		{
			sb.ClearStr();
			Net::SNMPUtil::OIDToString(obj->oid, obj->oidLen, &sb);
			this->lvObjects->SetSubItem(i, 1, sb.ToString());
		}
		if (obj->typeName)
		{
			this->lvObjects->SetSubItem(i, 2, obj->typeName);
		}
		if (obj->typeVal)
		{
			this->lvObjects->SetSubItem(i, 3, obj->typeVal);
		}
		i++;
	}
	Text::StringBuilderUTF8 sbOIDText;
	i = 0;
	j = module->oidList->GetCount();
	while (i < j)
	{
		obj = module->oidList->GetItem(i);
		sb.ClearStr();
		Net::SNMPUtil::OIDToString(obj->oid, obj->oidLen, &sb);
		this->lvOID->AddItem(sb.ToString(), obj);
		this->lvOID->SetSubItem(i, 1, obj->objectName);

		sbOIDText.AppendChar('\t', 1);
		sbOIDText.Append((const UTF8Char*)"{\"");
		sbOIDText.Append(obj->objectName);
		sbOIDText.Append((const UTF8Char*)"\",");
		k = (OSInt)(60 - Text::StrCharCnt(obj->objectName));
		if (k > 0)
		{
			sbOIDText.AppendChar('\t', (UOSInt)(k + 3) >> 2);
		}
		if (obj->oidLen < 10)
		{
			sbOIDText.AppendUOSInt(obj->oidLen);
			sbOIDText.Append((const UTF8Char*)",  {");
		}
		else
		{
			sbOIDText.AppendUOSInt(obj->oidLen);
			sbOIDText.Append((const UTF8Char*)", {");
		}
		k = 0;
		while (k < (OSInt)obj->oidLen)
		{
			if (k > 0)
			{
				sbOIDText.Append((const UTF8Char*)", ");
			}
			sbOIDText.Append((const UTF8Char*)"0x");
			sbOIDText.AppendHex8(obj->oid[k]);
			k++;
		}
		sbOIDText.Append((const UTF8Char*)"}}, // ");
		sbOIDText.Append(sb.ToString());
		sbOIDText.Append((const UTF8Char*)"\r\n");
		i++;
	}
	this->txtOIDText->SetText(sbOIDText.ToString());
}

SSWR::AVIRead::AVIRSNMPMIBForm::AVIRSNMPMIBForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"SNMP MIB");

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	NEW_CLASS(this->mib, Net::SNMPMIB());

	NEW_CLASS(this->pnlRequest, UI::GUIPanel(ui, this));
	this->pnlRequest->SetRect(0, 0, 100, 31, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblFile, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"MIB File"));
	this->lblFile->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtFile, UI::GUITextBox(ui, this->pnlRequest, (const UTF8Char*)""));
	this->txtFile->SetRect(104, 4, 500, 23, false);
	this->txtFile->SetReadOnly(true);
	NEW_CLASS(this->btnBrowse, UI::GUIButton(ui, this->pnlRequest, (const UTF8Char*)"B&rowse"));
	this->btnBrowse->SetRect(604, 4, 75, 23, false);
	this->btnBrowse->HandleButtonClick(OnBrowseClicked, this);
	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpObjects = this->tcMain->AddTabPage((const UTF8Char*)"Objects");
	NEW_CLASS(this->lvObjectsVal, UI::GUIListView(ui, this->tpObjects, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvObjectsVal->SetRect(0, 0, 100, 150, false);
	this->lvObjectsVal->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lvObjectsVal->SetShowGrid(true);
	this->lvObjectsVal->SetFullRowSelect(true);
	this->lvObjectsVal->AddColumn((const UTF8Char*)"Name", 200);
	this->lvObjectsVal->AddColumn((const UTF8Char*)"Value", 600);
	NEW_CLASS(this->vspObjects, UI::GUIVSplitter(ui, this->tpObjects, 3, true));
	NEW_CLASS(this->lvObjects, UI::GUIListView(ui, this->tpObjects, UI::GUIListView::LVSTYLE_TABLE, 3));
	this->lvObjects->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvObjects->SetShowGrid(true);
	this->lvObjects->SetFullRowSelect(true);
	this->lvObjects->AddColumn((const UTF8Char*)"Name", 200);
	this->lvObjects->AddColumn((const UTF8Char*)"OID", 200);
	this->lvObjects->AddColumn((const UTF8Char*)"Type", 200);
	this->lvObjects->AddColumn((const UTF8Char*)"Value", 200);
	this->lvObjects->HandleSelChg(OnObjectsSelChg, this);

	this->tpOID = this->tcMain->AddTabPage((const UTF8Char*)"OID");
	NEW_CLASS(this->lvOID, UI::GUIListView(ui, this->tpOID, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvOID->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvOID->SetShowGrid(true);
	this->lvOID->SetFullRowSelect(true);
	this->lvOID->AddColumn((const UTF8Char*)"OID", 200);
	this->lvOID->AddColumn((const UTF8Char*)"Name", 200);

	this->tpOIDText = this->tcMain->AddTabPage((const UTF8Char*)"OIDText");
	NEW_CLASS(this->txtOIDText, UI::GUITextBox(ui, this->tpOIDText, (const UTF8Char*)"", true));
	this->txtOIDText->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtOIDText->SetReadOnly(true);

	this->HandleDropFiles(OnFileDroped, this);
}

SSWR::AVIRead::AVIRSNMPMIBForm::~AVIRSNMPMIBForm()
{
	DEL_CLASS(this->mib);
}

void SSWR::AVIRead::AVIRSNMPMIBForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
