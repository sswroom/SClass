#include "Stdafx.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRCodeProjectForm.h"
#include "Text/MyString.h"
#include "Text/Cpp/CppEnv.h"


void __stdcall SSWR::AVIRead::AVIRCodeProjectForm::OnItemSelected(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCodeProjectForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCodeProjectForm>();
	Optional<Text::CodeProjectCfg> cfg = me->cboConfig->GetSelectedItem().GetOpt<Text::CodeProjectCfg>();
	NN<UI::GUITreeView::TreeItem> tvi;
	if (me->tvMain->GetSelectedItem().SetTo(tvi))
	{
		NN<Text::CodeObject> obj = tvi->GetItemObj().GetNN<Text::CodeObject>();
		if (obj->GetObjectType() == Text::CodeObject::OT_FILE)
		{
			Text::Cpp::CppEnv *env;
			NN<Text::Cpp::CppParseStatus> status;
			Text::Cpp::CppCodeParser *parser;
			NN<Text::CodeFile> file = NN<Text::CodeFile>::ConvertFrom(obj);
			UTF8Char sbuff[512];
			UnsafeArray<UTF8Char> sptr;
			NN<Text::String> s;
			sptr = me->proj->GetSourceName(sbuff);
			s = file->GetFileName();
			sptr = IO::Path::AppendPath(sbuff, sptr, s->ToCString());
			if (Text::StrEndsWithICaseC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC(".CPP")))
			{
				Text::StringBuilderUTF8 sb;
				Data::ArrayListStringNN errMsgs;
				NEW_CLASS(env, Text::Cpp::CppEnv(me->proj, cfg));
				NEW_CLASS(parser, Text::Cpp::CppCodeParser(env));
				NEW_CLASSNN(status, Text::Cpp::CppParseStatus(me->proj->GetSourceNameObj()));
				env->InitEnvStatus(status);
				status->AddGlobalDef(CSTR("__STDC__"), CSTR("0"));
				status->AddGlobalDef(CSTR("__cplusplus"), CSTR("201103"));
				parser->ParseFile(CSTRP(sbuff, sptr), errMsgs, status);
				if (errMsgs.GetCount() > 0)
				{
					sb.AppendC(UTF8STRC("Parse Error:\r\n"));
					Data::ArrayIterator<NN<Text::String>> it = errMsgs.Iterator();
					while (it.HasNext())
					{
						sb.Append(it.Next());
						sb.AppendC(UTF8STRC("\r\n"));
					}
					sb.AppendC(UTF8STRC("\r\n"));
					sb.AppendC(UTF8STRC("\r\n"));
				}
				me->DisplayStatus(sb, status);
				me->txtMessage->SetText(sb.ToCString());
				parser->FreeErrMsgs(errMsgs);
				status.Delete();
				DEL_CLASS(parser);
				DEL_CLASS(env);
			}
			else if (Text::StrEndsWithICaseC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC(".C")))
			{
				Text::StringBuilderUTF8 sb;
				Data::ArrayListStringNN errMsgs;
				NEW_CLASS(env, Text::Cpp::CppEnv(me->proj, cfg));
				NEW_CLASS(parser, Text::Cpp::CppCodeParser(env));
				NEW_CLASSNN(status, Text::Cpp::CppParseStatus(me->proj->GetSourceNameObj()));
				env->InitEnvStatus(status);
				status->AddGlobalDef(CSTR("__STDC__"), CSTR("1"));
				parser->ParseFile(CSTRP(sbuff, sptr), errMsgs, status);
				if (errMsgs.GetCount() == 0)
				{
					me->DisplayStatus(sb, status);
				}
				else
				{
					Data::ArrayIterator<NN<Text::String>> it = errMsgs.Iterator();
					while (it.HasNext())
					{
						sb.Append(it.Next());
						sb.AppendC(UTF8STRC("\r\n"));
					}
				}
				me->txtMessage->SetText(sb.ToCString());
				parser->FreeErrMsgs(errMsgs);
				status.Delete();
				DEL_CLASS(parser);
				DEL_CLASS(env);
			}
			else
			{
				me->txtMessage->SetText(CSTR(""));
			}
		}
		else
		{
			me->txtMessage->SetText(CSTR(""));
		}
	}
}

void SSWR::AVIRead::AVIRCodeProjectForm::DisplayStatus(NN<Text::StringBuilderUTF8> sb, NN<Text::Cpp::CppParseStatus> status)
{
	UOSInt i;
	UOSInt j;
	j = status->GetFileCount();
	if (j > 0)
	{
		i = 0;
		sb->AppendC(UTF8STRC("Files:\r\n"));
		while (i < j)
		{
			sb->AppendOpt(status->GetFileName(i));
			sb->AppendC(UTF8STRC("\r\n"));

			i++;
		}
		sb->AppendC(UTF8STRC("\r\n"));
	}
	j = status->GetDefineCount();
	if (j > 0)
	{
		Text::Cpp::CppParseStatus::DefineInfo defInfo;
		i = 0;
		sb->AppendC(UTF8STRC("Defines:\r\n"));
		while (i < j)
		{
			status->GetDefineInfo(i, defInfo);
			sb->Append(defInfo.defineName);
			sb->AppendC(UTF8STRC("\t"));
			if (defInfo.fileName == 0)
			{
				sb->AppendC(UTF8STRC("By compiler\r\n"));
			}
			else
			{
				sb->Append(defInfo.fileName);
				sb->AppendC(UTF8STRC(" ("));
				sb->AppendI32(defInfo.lineNum);
				sb->AppendC(UTF8STRC(")\r\n"));
			}

			i++;
		}
		sb->AppendC(UTF8STRC("\r\n"));
	}
}

void SSWR::AVIRead::AVIRCodeProjectForm::AddTreeObj(Optional<UI::GUITreeView::TreeItem> parent, NN<Text::CodeContainer> container)
{
	Optional<UI::GUITreeView::TreeItem> tviLast = nullptr;
	NN<UI::GUITreeView::TreeItem> tvi;
	NN<Text::CodeObject> obj;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	i = 0;
	j = container->GetChildCount();
	while (i < j)
	{
		obj = container->GetChildNoCheck(i);
		if (obj->GetObjectType() == Text::CodeObject::OT_FILE)
		{
			NN<Text::CodeFile> file = NN<Text::CodeFile>::ConvertFrom(obj);
			NN<Text::String> fileName = file->GetFileName();
			k = fileName->LastIndexOf(IO::Path::PATH_SEPERATOR);
			tviLast = this->tvMain->InsertItem(parent, tviLast, fileName->ToCString().Substring(k + 1), obj);
		}
		else if (obj->GetObjectType() == Text::CodeObject::OT_CONTAINER)
		{
			NN<Text::CodeContainer> childCont = NN<Text::CodeContainer>::ConvertFrom(obj);
			if (this->tvMain->InsertItem(parent, tviLast, childCont->GetContainerName(), obj).SetTo(tvi))
			{
				tviLast = tvi;
				AddTreeObj(tvi, childCont);
				this->tvMain->ExpandItem(tvi);
			}
		}
		i++;
	}
}

SSWR::AVIRead::AVIRCodeProjectForm::AVIRCodeProjectForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Text::CodeProject> proj) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Code Project"));
	this->SetFont(nullptr, 8.25, false);

	this->core = core;
	this->proj = proj;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	
	this->pnlCtrl = ui->NewPanel(*this);
	this->pnlCtrl->SetRect(0, 0, 100, 23, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblConfig = ui->NewLabel(this->pnlCtrl, CSTR("Config"));
	this->lblConfig->SetRect(100, 0, 100, 23, false);
	this->cboConfig = ui->NewComboBox(this->pnlCtrl, false);
	this->cboConfig->SetRect(200, 0, 200, 23, false);
	NEW_CLASSNN(this->tvMain, UI::GUITreeView(ui, *this));
	this->tvMain->SetRect(0, 0, 200, 23, false);
	this->tvMain->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->tvMain->SetHasLines(true);
	this->tvMain->SetHasButtons(true);
	this->tvMain->HandleSelectionChange(OnItemSelected, this);
	this->hspMain = ui->NewHSplitter(*this, 3, false);
	this->txtMessage = ui->NewTextBox(*this, CSTR(""), true);
	this->txtMessage->SetReadOnly(true);
	this->txtMessage->SetDockType(UI::GUIControl::DOCK_FILL);

	NN<UI::GUITreeView::TreeItem> tvi;
	if (this->tvMain->InsertItem(nullptr, nullptr, proj->GetContainerName(), proj).SetTo(tvi))
	{
		AddTreeObj(tvi, proj);
		this->tvMain->ExpandItem(tvi);
	}
	UOSInt i;
	UOSInt j;
	NN<Text::CodeProjectCfg> cfg;
	i = 0;
	j = this->proj->GetConfigCnt();
	while (i < j)
	{
		if (this->proj->GetConfig(i).SetTo(cfg))
		{
			this->cboConfig->AddItem(cfg->GetCfgName(), cfg);
		}
		i++;
	}
	if (j > 0)
	{
		this->cboConfig->SetSelectedIndex(0);
	}
}

SSWR::AVIRead::AVIRCodeProjectForm::~AVIRCodeProjectForm()
{
	this->proj.Delete();
}

void SSWR::AVIRead::AVIRCodeProjectForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
