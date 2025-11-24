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

void SSWR::ProcMonForm::AddProg(Text::CStringNN progName, Text::CString progPath)
{
	NN<SSWR::ProcMonForm::ProgInfo> prog;
	prog = MemAllocNN(ProgInfo);
	prog->progName = Text::String::New(progName);
	prog->procId = 0;
	prog->progPath = Text::String::NewOrNull(progPath);
	this->progList.Add(prog);
	this->lbProg->AddItem(prog->progName, prog);

	if (progPath.leng > 0)
	{
		this->SearchProcId(prog);
	}
}

Bool SSWR::ProcMonForm::SearchProcId(NN<SSWR::ProcMonForm::ProgInfo> prog)
{
	NN<Text::String> progPath;
	if (!prog->progPath.SetTo(progPath))
		return false;

	UTF8Char sbuff[512];
	UOSInt i;
	Bool ret = false;
	Manage::Process::ProcessInfo info;
	i = progPath->LastIndexOf(IO::Path::PATH_SEPERATOR);
	NN<Manage::Process::FindProcSess> sess;
	if (Manage::Process::FindProcess(progPath->ToCString().Substring(i + 1)).SetTo(sess))
	{
		Text::StringBuilderUTF8 sb;
		while (Manage::Process::FindProcessNext(sbuff, sess, info).NotNull())
		{
			Manage::Process proc(info.processId, false);
			sb.ClearStr();
			if (proc.GetFilename(sb))
			{
				if (sb.Equals(progPath))
				{
					Text::StringBuilderUTF8 sb;
					prog->procId = info.processId;
					ret = true;
					sb.AppendC(UTF8STRC("Prog "));
					sb.Append(prog->progName);
					sb.AppendC(UTF8STRC(": Updated procId as "));
					sb.AppendUOSInt(prog->procId);
					this->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);
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
		if (proc.GetFilename(sb))
		{
			OPTSTR_DEL(prog->progPath);
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
	UnsafeArray<UTF8Char> sptr;
	Text::PString sarr[2];
	Text::StringBuilderUTF8 sb;

	IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::ReplaceExt(sbuff, UTF8STRC("prg"));
	IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyAll, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		Text::UTF8Reader reader(fs);
		
		while (true)
		{
			sb.ClearStr();
			if (!reader.ReadLine(sb, 4096))
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
	UnsafeArray<UTF8Char> sptr;
	Text::StringBuilderUTF8 sb;
	UOSInt i;
	UOSInt j;
	NN<ProgInfo> prog;
	NN<Text::String> progPath;

	IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::ReplaceExt(sbuff, UTF8STRC("prg"));
	IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyAll, IO::FileStream::BufferType::NoWriteBuffer);
	Text::UTF8Writer writer(fs);
	i = 0;
	j = this->progList.GetCount();
	while (i < j)
	{
		prog = this->progList.GetItemNoCheck(i);
		sb.ClearStr();
		sb.Append(prog->progName);
		sb.AppendC(UTF8STRC(","));
		if (prog->progPath.SetTo(progPath))
		{
			sb.Append(progPath);
		}
		writer.WriteLine(sb.ToCString());
		i++;
	}
}

void __stdcall SSWR::ProcMonForm::OnProgSelChange(AnyType userObj)
{
	NN<SSWR::ProcMonForm> me = userObj.GetNN<SSWR::ProcMonForm>();
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> progPath;
	ProgInfo *prog = (ProgInfo*)me->lbProg->GetSelectedItem().p;
	if (prog && prog->progPath.SetTo(progPath))
	{
		me->txtProgPath->SetText(progPath->ToCString());
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

void __stdcall SSWR::ProcMonForm::OnProcIdClicked(AnyType userObj)
{
	NN<SSWR::ProcMonForm> me = userObj.GetNN<SSWR::ProcMonForm>();
	ProgInfo *prog = (ProgInfo*)me->lbProg->GetSelectedItem().p;
	if (prog)
	{
		Text::StringBuilderUTF8 sb;
		UInt32 procId;
		me->txtProcId->GetText(sb);
		if (sb.ToUInt32(procId))
		{
			me->SetByProcId(prog, procId);
		}
	}
}

void __stdcall SSWR::ProcMonForm::OnProgAddClicked(AnyType userObj)
{
	NN<SSWR::ProcMonForm> me = userObj.GetNN<SSWR::ProcMonForm>();
	Text::StringBuilderUTF8 sb;
	UInt32 procId;
	me->txtProgAddId->GetText(sb);
	if (sb.ToUInt32(procId))
	{
		sb.ClearStr();
		me->txtProgAddName->GetText(sb);
		if (sb.GetLength() > 0)
		{
			Manage::Process proc(procId, false);
			if (proc.IsRunning())
			{
				Text::StringBuilderUTF8 sb2;
				if (proc.GetFilename(sb2))
				{
					me->AddProg(sb.ToCString(), sb2.ToCString());
					me->SaveProgList();
					me->txtProgAddId->SetText(CSTR(""));
				}
			}
		}
	}
}

void __stdcall SSWR::ProcMonForm::OnLogSelChg(AnyType userObj)
{
	NN<SSWR::ProcMonForm> me = userObj.GetNN<SSWR::ProcMonForm>();
	NN<Text::String> s = Text::String::OrEmpty(me->lbLog->GetSelectedItemTextNew());
	me->txtLog->SetText(s->ToCString());
	s->Release();
}

void __stdcall SSWR::ProcMonForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::ProcMonForm> me = userObj.GetNN<SSWR::ProcMonForm>();
	UOSInt i;
	NN<ProgInfo> prog;
	NN<Text::String> progPath;
	i = me->progList.GetCount();
	while (i-- > 0)
	{
		prog = me->progList.GetItemNoCheck(i);
		if (prog->progPath.SetTo(progPath))
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
					me->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);
				}
			}
			if (prog->procId == 0)
			{
				if (!me->SearchProcId(prog))
				{
					Manage::Process proc(progPath->v);
					if (proc.IsRunning())
					{
						prog->procId = proc.GetProcId();
						Text::StringBuilderUTF8 sb;
						sb.AppendC(UTF8STRC("Prog "));
						sb.Append(prog->progName);
						sb.AppendC(UTF8STRC(" restarted, procId = "));
						sb.AppendUOSInt(prog->procId);
						me->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);
						
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

SSWR::ProcMonForm::ProcMonForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Process Monitor"));
	this->SetFont(0, 0, 8.25, false);

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	this->txtLog = ui->NewTextBox(this->tpLog, CSTR(""));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->txtLog->SetReadOnly(true);
	this->lbLog = ui->NewListBox(this->tpLog, false);
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSelChg, this);

	this->tpProg = this->tcMain->AddTabPage(CSTR("Prog"));
	this->lbProg = ui->NewListBox(this->tpProg, false);
	this->lbProg->SetRect(0, 0, 150, 100, false);
	this->lbProg->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbProg->HandleSelectionChange(OnProgSelChange, this);
	this->pnlProg = ui->NewPanel(this->tpProg);
	this->pnlProg->SetDockType(UI::GUIControl::DOCK_FILL);
	this->grpProgAdd = ui->NewGroupBox(this->pnlProg, CSTR("Add Prog"));
	this->grpProgAdd->SetRect(0, 0, 100, 64, false);
	this->grpProgAdd->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblProgAddName = ui->NewLabel(this->grpProgAdd, CSTR("Prog Name"));
	this->lblProgAddName->SetRect(0, 0, 100, 23, false);
	this->txtProgAddName = ui->NewTextBox(this->grpProgAdd, CSTR(""));
	this->txtProgAddName->SetRect(100, 0, 200, 23, false);
	this->lblProgAddId = ui->NewLabel(this->grpProgAdd, CSTR("Process Id"));
	this->lblProgAddId->SetRect(0, 24, 100, 23, false);
	this->txtProgAddId = ui->NewTextBox(this->grpProgAdd, CSTR(""));
	this->txtProgAddId->SetRect(100, 24, 100, 23, false);
	this->btnProgAdd = ui->NewButton(this->grpProgAdd, CSTR("&Add"));
	this->btnProgAdd->SetRect(200, 24, 75, 23, false);
	this->btnProgAdd->HandleButtonClick(OnProgAddClicked, this);
	this->lblProcId = ui->NewLabel(this->pnlProg, CSTR("Process Id"));
	this->lblProcId->SetRect(4, 68, 100, 23, false);
	this->txtProcId = ui->NewTextBox(this->pnlProg, CSTR(""));
	this->txtProcId->SetRect(104, 68, 100, 23, false);
	this->btnProcId = ui->NewButton(this->pnlProg, CSTR("Set"));
	this->btnProcId->SetRect(204, 68, 75, 23, false);
	this->btnProcId->HandleButtonClick(OnProcIdClicked, this);
	this->lblProgPath = ui->NewLabel(this->pnlProg, CSTR("Path"));
	this->lblProgPath->SetRect(4, 92, 100, 23, false);
	this->txtProgPath = ui->NewTextBox(this->pnlProg, CSTR(""));
	this->txtProgPath->SetRect(104, 92, 600, 23, false);
	this->txtProgPath->SetReadOnly(true);

	this->notifyCmd = 0;
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Log"));
	sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
	sb.AppendC(UTF8STRC("ProgLog"));
	this->log.AddFileLog(sb.ToCString(), IO::LogHandler::LogType::PerDay, IO::LogHandler::LogGroup::PerMonth, IO::LogHandler::LogLevel::Raw, "yyyy-MM-dd HH:mm:ss.fff", false);
	NEW_CLASSNN(this->logger, UI::ListBoxLogger(*this, this->lbLog, 512, false));
	this->log.AddLogHandler(this->logger, IO::LogHandler::LogLevel::Raw);
	this->LoadProgList();

	NN<IO::ConfigFile> cfg;
	if (IO::IniFile::ParseProgConfig(0).SetTo(cfg))
	{
		NN<Text::String> s;
		if (cfg->GetValue(CSTR("NotifyCmd")).SetTo(s))
		{
			this->notifyCmd = s->Clone().Ptr();
		}
		cfg.Delete();
	}
	this->AddTimer(30000, OnTimerTick, this);
}

SSWR::ProcMonForm::~ProcMonForm()
{
	NN<ProgInfo> prog;
	UOSInt i = this->progList.GetCount();
	while (i-- > 0)
	{
		prog = this->progList.GetItemNoCheck(i);
		OPTSTR_DEL(prog->progPath);
		prog->progName->Release();
		MemFreeNN(prog);
	}
	this->logger.Delete();
	SDEL_STRING(this->notifyCmd);
}
