#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/IniFile.h"
#include "IO/Path.h"
#include "Manage/Process.h"
#include "SSWR/ProcMonForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"

void SSWR::ProcMonForm::AddProg(Text::CString progName, Text::CString progPath)
{
	SSWR::ProcMonForm::ProgInfo *prog;
	prog = MemAlloc(ProgInfo, 1);
	prog->progName = Text::String::New(progName);
	prog->procId = 0;
	if (progPath.leng > 0)
	{
		prog->progPath = Text::String::New(progPath).Ptr();
	}
	else
	{
		prog->progPath = 0;
	}
	this->progList->Add(prog);
	this->lbProg->AddItem(prog->progName, prog);

	if (progPath.leng > 0)
	{
		this->SearchProcId(prog);
	}
}

Bool SSWR::ProcMonForm::SearchProcId(SSWR::ProcMonForm::ProgInfo *prog)
{
	if (prog->progPath == 0)
		return false;

	UTF8Char sbuff[512];
	UOSInt i;
	Bool ret = false;
	Manage::Process::ProcessInfo info;
	i = prog->progPath->LastIndexOf(IO::Path::PATH_SEPERATOR);
	Manage::Process::FindProcSess *sess = Manage::Process::FindProcess(prog->progPath->ToCString().Substring(i + 1));
	if (sess)
	{
		Text::StringBuilderUTF8 sb;
		while (Manage::Process::FindProcessNext(sbuff, sess, &info))
		{
			Manage::Process proc(info.processId, false);
			sb.ClearStr();
			if (proc.GetFilename(&sb))
			{
				if (sb.Equals(prog->progPath))
				{
					Text::StringBuilderUTF8 sb;
					prog->procId = info.processId;
					ret = true;
					sb.AppendC(UTF8STRC("Prog "));
					sb.Append(prog->progName);
					sb.AppendC(UTF8STRC(": Updated procId as "));
					sb.AppendUOSInt(prog->procId);
					this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);
					break;
				}
			}
		}
		Manage::Process::FindProcessClose(sess);
	}
	return ret;
}

void SSWR::ProcMonForm::SetByProcId(ProgInfo *prog, UOSInt procId)
{
	Manage::Process proc(procId, false);
	if (proc.IsRunning())
	{
		Text::StringBuilderUTF8 sb;
		if (proc.GetFilename(&sb))
		{
			SDEL_STRING(prog->progPath);
			prog->progPath = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
			prog->procId = procId;
			this->txtProgPath->SetText(sb.ToCString());
			this->SaveProgList();
		}
	}
}

void SSWR::ProcMonForm::LoadProgList()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Text::PString sarr[2];
	Text::StringBuilderUTF8 sb;

	IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::ReplaceExt(sbuff, UTF8STRC("prg"));
	IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyAll, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		Text::UTF8Reader reader(&fs);
		
		while (true)
		{
			sb.ClearStr();
			if (!reader.ReadLine(&sb, 4096))
				break;
			if (Text::StrSplitP(sarr, 2, sb, ',') == 2)
			{
				if (sarr[1].v[0])
				{
					this->AddProg(sarr[0].ToCString(), sarr[1].ToCString());
				}
				else
				{
					this->AddProg(sarr[0].ToCString(), sarr[1].ToCString());
				}
			}
		}
	}
}

void SSWR::ProcMonForm::SaveProgList()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 sb;
	UOSInt i;
	UOSInt j;
	ProgInfo *prog;

	IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::ReplaceExt(sbuff, UTF8STRC("prg"));
	IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyAll, IO::FileStream::BufferType::NoWriteBuffer);
	Text::UTF8Writer writer(&fs);
	i = 0;
	j = this->progList->GetCount();
	while (i < j)
	{
		prog = this->progList->GetItem(i);
		sb.ClearStr();
		sb.Append(prog->progName);
		sb.AppendC(UTF8STRC(","));
		if (prog->progPath)
		{
			sb.Append(prog->progPath);
		}
		writer.WriteLineC(sb.ToString(), sb.GetLength());
		i++;
	}
}

void __stdcall SSWR::ProcMonForm::OnProgSelChange(void *userObj)
{
	SSWR::ProcMonForm *me = (SSWR::ProcMonForm *)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	ProgInfo *prog = (ProgInfo*)me->lbProg->GetSelectedItem();
	if (prog && prog->progPath)
	{
		me->txtProgPath->SetText(prog->progPath->ToCString());
	}
	else
	{
		me->txtProgPath->SetText(CSTR(""));
	}
	if (prog)
	{
		sptr = Text::StrUOSInt(sbuff, prog->procId);
		me->txtProcId->SetText(CSTRP(sbuff, sptr));
	}
	else
	{
		me->txtProcId->SetText(CSTR(""));
	}
}

void __stdcall SSWR::ProcMonForm::OnProcIdClicked(void *userObj)
{
	SSWR::ProcMonForm *me = (SSWR::ProcMonForm *)userObj;
	ProgInfo *prog = (ProgInfo*)me->lbProg->GetSelectedItem();
	if (prog)
	{
		Text::StringBuilderUTF8 sb;
		UInt32 procId;
		me->txtProcId->GetText(&sb);
		if (sb.ToUInt32(&procId))
		{
			me->SetByProcId(prog, procId);
		}
	}
}

void __stdcall SSWR::ProcMonForm::OnProgAddClicked(void *userObj)
{
	SSWR::ProcMonForm *me = (SSWR::ProcMonForm *)userObj;
	Text::StringBuilderUTF8 sb;
	UInt32 procId;
	me->txtProgAddId->GetText(&sb);
	if (sb.ToUInt32(&procId))
	{
		sb.ClearStr();
		me->txtProgAddName->GetText(&sb);
		if (sb.GetLength() > 0)
		{
			Manage::Process proc(procId, false);
			if (proc.IsRunning())
			{
				Text::StringBuilderUTF8 sb2;
				if (proc.GetFilename(&sb2))
				{
					me->AddProg(sb.ToCString(), sb2.ToCString());
					me->SaveProgList();
					me->txtProgAddId->SetText(CSTR(""));
				}
			}
		}
	}
}

void __stdcall SSWR::ProcMonForm::OnLogSelChg(void *userObj)
{
	SSWR::ProcMonForm *me = (SSWR::ProcMonForm *)userObj;
	NotNullPtr<Text::String> s = Text::String::OrEmpty(me->lbLog->GetSelectedItemTextNew());
	me->txtLog->SetText(s->ToCString());
	s->Release();
}

void __stdcall SSWR::ProcMonForm::OnTimerTick(void *userObj)
{
	SSWR::ProcMonForm *me = (SSWR::ProcMonForm *)userObj;
	UOSInt i;
	ProgInfo *prog;
	i = me->progList->GetCount();
	while (i-- > 0)
	{
		prog = me->progList->GetItem(i);
		if (prog->progPath != 0)
		{
			if (prog->procId != 0)
			{
				Manage::Process proc(prog->procId, false);
				if (!proc.IsRunning())
				{
					prog->procId = 0;
					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("Prog "));
					sb.Append(prog->progName);
					sb.AppendC(UTF8STRC(" stopped"));
					me->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);
				}
			}
			if (prog->procId == 0)
			{
				if (!me->SearchProcId(prog))
				{
					Manage::Process proc(prog->progPath->v);
					if (proc.IsRunning())
					{
						prog->procId = proc.GetProcId();
						Text::StringBuilderUTF8 sb;
						sb.AppendC(UTF8STRC("Prog "));
						sb.Append(prog->progName);
						sb.AppendC(UTF8STRC(" restarted, procId = "));
						sb.AppendUOSInt(prog->procId);
						me->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);
						
						if (me->notifyCmd)
						{
							sb.ClearStr();
							sb.Append(me->notifyCmd);
							sb.AppendC(UTF8STRC(" \"Prog "));
							sb.Append(prog->progName);
							sb.AppendC(UTF8STRC(" restarted\""));
							Manage::Process proc2(sb.ToString());
						}
					}
				}
			}
		}
	}
}

SSWR::ProcMonForm::ProcMonForm(UI::GUIClientControl *parent, UI::GUICore *ui) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Process Monitor"));
	this->SetFont(0, 0, 8.25, false);

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	NEW_CLASS(this->txtLog, UI::GUITextBox(ui, this->tpLog, CSTR("")));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->txtLog->SetReadOnly(true);
	NEW_CLASS(this->lbLog, UI::GUIListBox(ui, this->tpLog, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSelChg, this);

	this->tpProg = this->tcMain->AddTabPage(CSTR("Prog"));
	NEW_CLASS(this->lbProg, UI::GUIListBox(ui, this->tpProg, false));
	this->lbProg->SetRect(0, 0, 150, 100, false);
	this->lbProg->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbProg->HandleSelectionChange(OnProgSelChange, this);
	NEW_CLASS(this->pnlProg, UI::GUIPanel(ui, this->tpProg));
	this->pnlProg->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->grpProgAdd, UI::GUIGroupBox(ui, this->pnlProg, CSTR("Add Prog")));
	this->grpProgAdd->SetRect(0, 0, 100, 64, false);
	this->grpProgAdd->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblProgAddName, UI::GUILabel(ui, this->grpProgAdd, CSTR("Prog Name")));
	this->lblProgAddName->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->txtProgAddName, UI::GUITextBox(ui, this->grpProgAdd, CSTR("")));
	this->txtProgAddName->SetRect(100, 0, 200, 23, false);
	NEW_CLASS(this->lblProgAddId, UI::GUILabel(ui, this->grpProgAdd, CSTR("Process Id")));
	this->lblProgAddId->SetRect(0, 24, 100, 23, false);
	NEW_CLASS(this->txtProgAddId, UI::GUITextBox(ui, this->grpProgAdd, CSTR("")));
	this->txtProgAddId->SetRect(100, 24, 100, 23, false);
	NEW_CLASS(this->btnProgAdd, UI::GUIButton(ui, this->grpProgAdd, CSTR("&Add")));
	this->btnProgAdd->SetRect(200, 24, 75, 23, false);
	this->btnProgAdd->HandleButtonClick(OnProgAddClicked, this);
	NEW_CLASS(this->lblProcId, UI::GUILabel(ui, this->pnlProg, CSTR("Process Id")));
	this->lblProcId->SetRect(4, 68, 100, 23, false);
	NEW_CLASS(this->txtProcId, UI::GUITextBox(ui, this->pnlProg, CSTR("")));
	this->txtProcId->SetRect(104, 68, 100, 23, false);
	NEW_CLASS(this->btnProcId, UI::GUIButton(ui, this->pnlProg, CSTR("Set")));
	this->btnProcId->SetRect(204, 68, 75, 23, false);
	this->btnProcId->HandleButtonClick(OnProcIdClicked, this);
	NEW_CLASS(this->lblProgPath, UI::GUILabel(ui, this->pnlProg, CSTR("Path")));
	this->lblProgPath->SetRect(4, 92, 100, 23, false);
	NEW_CLASS(this->txtProgPath, UI::GUITextBox(ui, this->pnlProg, CSTR("")));
	this->txtProgPath->SetRect(104, 92, 600, 23, false);
	this->txtProgPath->SetReadOnly(true);

	this->notifyCmd = 0;
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Log"));
	sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
	sb.AppendC(UTF8STRC("ProgLog"));
	NEW_CLASS(this->log, IO::LogTool());
	this->log->AddFileLog(sb.ToCString(), IO::LogHandler::LogType::PerDay, IO::LogHandler::LogGroup::PerMonth, IO::LogHandler::LogLevel::Raw, "yyyy-MM-dd HH:mm:ss.fff", false);
	NEW_CLASS(this->logger, UI::ListBoxLogger(this, this->lbLog, 512, false));
	this->log->AddLogHandler(this->logger, IO::LogHandler::LogLevel::Raw);
	NEW_CLASS(this->progList, Data::ArrayList<ProgInfo*>());
	this->LoadProgList();

	IO::ConfigFile *cfg = IO::IniFile::ParseProgConfig(0);
	if (cfg)
	{
		Text::String *s = cfg->GetValue(CSTR("NotifyCmd"));
		if (s)
		{
			this->notifyCmd = s->Clone().Ptr();
		}
		DEL_CLASS(cfg);
	}
	this->AddTimer(30000, OnTimerTick, this);
}

SSWR::ProcMonForm::~ProcMonForm()
{
	ProgInfo *prog;
	UOSInt i = this->progList->GetCount();
	while (i-- > 0)
	{
		prog = this->progList->GetItem(i);
		SDEL_STRING(prog->progPath);
		prog->progName->Release();
		MemFree(prog);
	}
	DEL_CLASS(this->progList);
	DEL_CLASS(this->log);
	DEL_CLASS(this->logger);
	SDEL_STRING(this->notifyCmd);
}
