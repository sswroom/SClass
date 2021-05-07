#include "Stdafx.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRCodeProjectForm.h"
#include "Text/MyString.h"
#include "Text/Cpp/CppEnv.h"
#include "UI/MessageDialog.h"


void __stdcall SSWR::AVIRead::AVIRCodeProjectForm::OnItemSelected(void *userObj)
{
	SSWR::AVIRead::AVIRCodeProjectForm *me = (SSWR::AVIRead::AVIRCodeProjectForm*)userObj;
	UI::GUITreeView::TreeItem *tvi = me->tvMain->GetSelectedItem();
	Text::CodeProjectCfg *cfg = (Text::CodeProjectCfg*)me->cboConfig->GetSelectedItem();
	if (tvi)
	{
		Text::CodeObject *obj = (Text::CodeObject*)tvi->GetItemObj();
		if (obj->GetObjectType() == Text::CodeObject::OT_FILE)
		{
			UOSInt i;
			UOSInt j;
			Text::Cpp::CppEnv *env;
			Text::Cpp::CppParseStatus *status;
			Text::Cpp::CppCodeParser *parser;
			Text::CodeFile *file = (Text::CodeFile*)obj;
			UTF8Char u8buff[512];
			me->proj->GetSourceName(u8buff);
			IO::Path::AppendPath(u8buff, file->GetFileName());
			if (Text::StrEndsWithICase(u8buff, (const UTF8Char*)".CPP"))
			{
				Text::StringBuilderUTF8 sb;
				Data::ArrayListStrUTF8 errMsgs;
				NEW_CLASS(env, Text::Cpp::CppEnv(me->proj, cfg));
				NEW_CLASS(parser, Text::Cpp::CppCodeParser(env));
				NEW_CLASS(status, Text::Cpp::CppParseStatus(me->proj->GetSourceNameObj()));
				env->InitEnvStatus(status);
				status->AddGlobalDef((const UTF8Char*)"__STDC__", (const UTF8Char*)"0");
				status->AddGlobalDef((const UTF8Char*)"__cplusplus", (const UTF8Char*)"201103");
				parser->ParseFile(u8buff, &errMsgs, status);
				i = 0;
				j = errMsgs.GetCount();
				if (j > 0)
				{
					sb.Append((const UTF8Char*)"Parse Error:\r\n");
					while (i < j)
					{
						sb.Append(errMsgs.GetItem(i));
						sb.Append((const UTF8Char*)"\r\n");
						i++;
					}
					sb.Append((const UTF8Char*)"\r\n");
					sb.Append((const UTF8Char*)"\r\n");
				}
				me->DisplayStatus(&sb, status);
				me->txtMessage->SetText(sb.ToString());
				parser->FreeErrMsgs(&errMsgs);
				DEL_CLASS(status);
				DEL_CLASS(parser);
				DEL_CLASS(env);
			}
			else if (Text::StrEndsWithICase(u8buff, (const UTF8Char*)".C"))
			{
				Text::StringBuilderUTF8 sb;
				Data::ArrayListStrUTF8 errMsgs;
				NEW_CLASS(env, Text::Cpp::CppEnv(me->proj, cfg));
				NEW_CLASS(parser, Text::Cpp::CppCodeParser(env));
				NEW_CLASS(status, Text::Cpp::CppParseStatus(me->proj->GetSourceNameObj()));
				env->InitEnvStatus(status);
				status->AddGlobalDef((const UTF8Char*)"__STDC__", (const UTF8Char*)"1");
				parser->ParseFile(u8buff, &errMsgs, status);
				i = 0;
				j = errMsgs.GetCount();
				if (j == 0)
				{
					me->DisplayStatus(&sb, status);
				}
				else
				{
					while (i < j)
					{
						sb.Append(errMsgs.GetItem(i));
						sb.Append((const UTF8Char*)"\r\n");
						i++;
					}
				}
				me->txtMessage->SetText(sb.ToString());
				parser->FreeErrMsgs(&errMsgs);
				DEL_CLASS(status);
				DEL_CLASS(parser);
				DEL_CLASS(env);
			}
			else
			{
				me->txtMessage->SetText((const UTF8Char*)"");
			}
		}
		else
		{
			me->txtMessage->SetText((const UTF8Char*)"");
		}
	}
}

void SSWR::AVIRead::AVIRCodeProjectForm::DisplayStatus(Text::StringBuilderUTF *sb, Text::Cpp::CppParseStatus *status)
{
	OSInt i;
	OSInt j;
	j = status->GetFileCount();
	if (j > 0)
	{
		i = 0;
		sb->Append((const UTF8Char*)"Files:\r\n");
		while (i < j)
		{
			sb->Append(status->GetFileName(i));
			sb->Append((const UTF8Char*)"\r\n");

			i++;
		}
		sb->Append((const UTF8Char*)"\r\n");
	}
	j = status->GetDefineCount();
	if (j > 0)
	{
		Text::Cpp::CppParseStatus::DefineInfo defInfo;
		i = 0;
		sb->Append((const UTF8Char*)"Defines:\r\n");
		while (i < j)
		{
			status->GetDefineInfo(i, &defInfo);
			sb->Append(defInfo.defineName);
			sb->Append((const UTF8Char*)"\t");
			if (defInfo.fileName == 0)
			{
				sb->Append((const UTF8Char*)"By compiler\r\n");
			}
			else
			{
				sb->Append(defInfo.fileName);
				sb->Append((const UTF8Char*)" (");
				sb->AppendI32(defInfo.lineNum);
				sb->Append((const UTF8Char*)")\r\n");
			}

			i++;
		}
		sb->Append((const UTF8Char*)"\r\n");
	}
}

void SSWR::AVIRead::AVIRCodeProjectForm::AddTreeObj(UI::GUITreeView::TreeItem *parent, Text::CodeContainer *container)
{
	UI::GUITreeView::TreeItem *tviLast = 0;
	Text::CodeObject *obj;
	UOSInt i;
	UOSInt j;
	OSInt k;
	i = 0;
	j = container->GetChildCount();
	while (i < j)
	{
		obj = container->GetChildObj(i);
		if (obj->GetObjectType() == Text::CodeObject::OT_FILE)
		{
			Text::CodeFile *file = (Text::CodeFile*)obj;
			const UTF8Char *fileName = file->GetFileName();
			k = Text::StrLastIndexOf(fileName, IO::Path::PATH_SEPERATOR);
			tviLast = this->tvMain->InsertItem(parent, tviLast, &fileName[k + 1], obj);
		}
		else if (obj->GetObjectType() == Text::CodeObject::OT_CONTAINER)
		{
			Text::CodeContainer *childCont = (Text::CodeContainer*)obj;
			tviLast = this->tvMain->InsertItem(parent, tviLast, childCont->GetContainerName(), obj);
			AddTreeObj(tviLast, childCont);
			this->tvMain->ExpandItem(tviLast);
		}
		i++;
	}
}

SSWR::AVIRead::AVIRCodeProjectForm::AVIRCodeProjectForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Text::CodeProject *proj) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText((const UTF8Char*)"Code Project");
	this->SetFont(0, 8.25, false);

	this->core = core;
	this->proj = proj;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	
	NEW_CLASS(this->pnlCtrl, UI::GUIPanel(ui, this));
	this->pnlCtrl->SetRect(0, 0, 100, 23, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblConfig, UI::GUILabel(ui, this->pnlCtrl, (const UTF8Char*)"Config"));
	this->lblConfig->SetRect(100, 0, 100, 23, false);
	NEW_CLASS(this->cboConfig, UI::GUIComboBox(ui, this->pnlCtrl, false));
	this->cboConfig->SetRect(200, 0, 200, 23, false);
	NEW_CLASS(this->tvMain, UI::GUITreeView(ui, this));
	this->tvMain->SetRect(0, 0, 200, 23, false);
	this->tvMain->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->tvMain->SetHasLines(true);
	this->tvMain->SetHasButtons(true);
	this->tvMain->HandleSelectionChange(OnItemSelected, this);
	NEW_CLASS(this->hspMain, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->txtMessage, UI::GUITextBox(ui, this, (const UTF8Char*)"", true));
	this->txtMessage->SetReadOnly(true);
	this->txtMessage->SetDockType(UI::GUIControl::DOCK_FILL);

	UI::GUITreeView::TreeItem *tvi;
	AddTreeObj(tvi = this->tvMain->InsertItem(0, 0, proj->GetContainerName(), proj), proj);
	this->tvMain->ExpandItem(tvi);
	OSInt i;
	OSInt j;
	Text::CodeProjectCfg *cfg;
	i = 0;
	j = this->proj->GetConfigCnt();
	while (i < j)
	{
		cfg = this->proj->GetConfig(i);
		this->cboConfig->AddItem(cfg->GetCfgName(), cfg);
		
		i++;
	}
	if (j > 0)
	{
		this->cboConfig->SetSelectedIndex(0);
	}
}

SSWR::AVIRead::AVIRCodeProjectForm::~AVIRCodeProjectForm()
{
	DEL_CLASS(this->proj);
}

void SSWR::AVIRead::AVIRCodeProjectForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
