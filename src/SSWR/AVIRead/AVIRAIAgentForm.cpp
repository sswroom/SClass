#include "Stdafx.h"
#include "DB/JavaDBUtil.h"
#include "DB/SQLiteFile.h"
#include "IO/Path.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRAIAgentForm.h"
#include "Sync/MutexUsage.h"

void __stdcall SSWR::AVIRead::AVIRAIAgentForm::OnStartClicked(AnyType userObj)
{
	NN<AVIRAIAgentForm> me = userObj.GetNN<AVIRAIAgentForm>();
	if (me->agent.NotNull())
		return;

	Text::StringBuilderUTF8 jdbc;
	Text::StringBuilderUTF8 dbUser;
	Text::StringBuilderUTF8 dbPassword;
	Text::StringBuilderUTF8 schema;
	Text::StringBuilderUTF8 url;
	Text::StringBuilderUTF8 key;
	Text::StringBuilderUTF8 maxCallsText;
	Text::StringBuilderUTF8 mdPath;
	Text::StringBuilderUTF8 prompt;
	me->txtJDBC->GetText(jdbc);
	me->txtDBUser->GetText(dbUser);
	me->txtDBPassword->GetText(dbPassword);
	me->txtSchema->GetText(schema);
	me->txtURL->GetText(url);
	me->txtKey->GetText(key);
	me->txtMaxToolCalls->GetText(maxCallsText);
	me->txtMDPath->GetText(mdPath);
	me->txtPrompt->GetText(prompt);

	UIntOS maxCalls;
	if (url.GetLength() == 0 || key.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Enter the OpenAI URL and API key."), CSTR("AI Agent"), me);
		return;
	}
	if (!url.StartsWith(CSTR("http://")) && !url.StartsWith(CSTR("https://")))
	{
		me->ui->ShowMsgOK(CSTR("OpenAI URL must start with http:// or https://."), CSTR("AI Agent"), me);
		return;
	}
	if (url.StartsWith(CSTR("https://")) && me->ssl.IsNull())
	{
		me->ui->ShowMsgOK(CSTR("TLS is not available for the OpenAI URL."), CSTR("AI Agent"), me);
		return;
	}
	if (!maxCallsText.ToUIntOS(maxCalls) || maxCalls == 0)
	{
		me->ui->ShowMsgOK(CSTR("Enter a valid non-zero maximum tool calls."), CSTR("AI Agent"), me);
		return;
	}
	if (mdPath.GetLength() > 0 && IO::Path::GetPathType(mdPath.ToCString()) != IO::Path::PathType::Directory)
	{
		me->ui->ShowMsgOK(CSTR("Markdown path must be an existing directory."), CSTR("AI Agent"), me);
		return;
	}
	NN<DB::DBTool> db;
	NN<AI::AITool> tool;
	if (jdbc.GetLength() > 0)
	{
		if ((jdbc.StartsWith(CSTR("jdbc:sqlite:")) || jdbc.StartsWith(CSTR("jdbc:mysql://"))) && schema.GetLength() > 0)
		{
			me->ui->ShowMsgOK(CSTR("Leave the schema empty for SQLite and MySQL."), CSTR("AI Agent"), me);
			return;
		}
		NN<Text::String> jdbcStr = Text::String::New(jdbc.ToCString());
		NN<Text::String> userStr = Text::String::New(dbUser.ToCString());
		NN<Text::String> passwordStr = Text::String::New(dbPassword.ToCString());
		me->db = DB::JavaDBUtil::OpenJDBC(jdbcStr, userStr, passwordStr, me->log, me->core->GetTCPClientFactory());
		jdbcStr->Release();
		userStr->Release();
		passwordStr->Release();
		if (!me->db.SetTo(db))
		{
			me->ui->ShowMsgOK(CSTR("Failed to open the database. Check the connection parameters and log."), CSTR("AI Agent"), me);
			return;
		}
		NEW_CLASSNN(tool, AI::AIReadDBTool(db->GetDBConn(), schema.ToCString()));
		me->tools.Add(tool);
	}

	NN<Net::OpenAIClient> openAI;
	NEW_CLASSNN(openAI, Net::OpenAIClient(me->core->GetTCPClientFactory(), me->ssl, url.ToCString(), key.ToCString()));
	me->openAI = openAI;
	NN<Text::String> model;
	if (openAI->IsError() || !openAI->GetModelName(0).SetTo(model))
	{
		me->ui->ShowMsgOK(CSTR("Failed to initialize the OpenAI client or get a model."), CSTR("AI Agent"), me);
		me->Stop(true);
		return;
	}
	me->log.LogMessage(Text::StringBuilderUTF8().Append(CSTR("Using model: "))->Append(model)->ToCString(), IO::LogHandler::LogLevel::Command);

	NN<AI::SimpleAIAgent> agent;
	NEW_CLASSNN(agent, AI::SimpleAIAgent(openAI, model->ToCString()));
	me->agent = agent;
	UIntOS i = 0;
	while (i < me->tools.GetCount())
	{
		if (!agent->AddTool(me->tools.GetItemNoCheck(i)))
		{
			me->ui->ShowMsgOK(CSTR("Failed to add a tool."), CSTR("AI Agent"), me);
			me->Stop(true);
			return;
		}
		i++;
	}
	if (mdPath.GetLength() > 0)
	{
		NEW_CLASSNN(tool, AI::AIReadMDTool(mdPath.ToCString()));
		me->tools.Add(tool);
		if (!agent->AddTool(tool))
		{
			me->ui->ShowMsgOK(CSTR("Failed to add the Markdown tool."), CSTR("AI Agent"), me);
			me->Stop(true);
			return;
		}
	}
	agent->SetMaxToolCalls(maxCalls);
	me->systemPrompt = Text::String::New(prompt.ToCString());
	if (!me->worker.Start())
	{
		me->ui->ShowMsgOK(CSTR("Failed to start the agent worker."), CSTR("AI Agent"), me);
		me->Stop(true);
		return;
	}
	me->pnlSettings->SetEnabled(false);
	me->btnStart->SetEnabled(false);
	me->btnStop->SetEnabled(true);
	me->btnAsk->SetEnabled(true);
	me->lblStatus->SetText(CSTR("Ready"));
	me->tcMain->SetSelectedIndex(1);
	me->log.LogMessage(CSTR("AIAgent ready"), IO::LogHandler::LogLevel::Command);
}

void __stdcall SSWR::AVIRead::AVIRAIAgentForm::OnStopClicked(AnyType userObj)
{
	NN<AVIRAIAgentForm> me = userObj.GetNN<AVIRAIAgentForm>();
	me->Stop(true);
	me->log.LogMessage(CSTR("AIAgent stopped"), IO::LogHandler::LogLevel::Command);
}

void SSWR::AVIRead::AVIRAIAgentForm::Stop(Bool updateUI)
{
	this->worker.Stop();
	OPTSTR_DEL(this->pendingQuestion);
	OPTSTR_DEL(this->answer);
	this->pendingSession.Delete();
	OPTSTR_DEL(this->systemPrompt);
	this->completed = false;
	this->agent.Delete();
	this->tools.DeleteAll();
	this->openAI.Delete();
	this->db.Delete();
	if (updateUI)
	{
		this->pnlSettings->SetEnabled(true);
		this->btnStart->SetEnabled(true);
		this->btnStop->SetEnabled(false);
		this->btnAsk->SetEnabled(false);
		this->lblStatus->SetText(CSTR("Not started"));
	}
}

void __stdcall SSWR::AVIRead::AVIRAIAgentForm::OnAskClicked(AnyType userObj)
{
	NN<AVIRAIAgentForm> me = userObj.GetNN<AVIRAIAgentForm>();
	if (me->agent.IsNull())
		return;
	Text::StringBuilderUTF8 question;
	me->txtQuestion->GetText(question);
	if (question.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Enter a question."), CSTR("AI Agent"), me);
		return;
	}
	{
		Sync::MutexUsage mutUsage(me->resultMut);
		me->pendingQuestion = Text::String::New(question.ToCString());
	}
	me->txtAnswer->SetText(CSTR(""));
	me->lbSession->ClearItems();
	me->txtTrace->SetText(CSTR(""));
	me->displayedSession.Delete();
	me->btnAsk->SetEnabled(false);
	me->btnStop->SetEnabled(false);
	me->lblStatus->SetText(CSTR("Processing..."));
	me->worker.Notify();
}

void __stdcall SSWR::AVIRead::AVIRAIAgentForm::OnSessionSelChg(AnyType userObj)
{
	NN<AVIRAIAgentForm> me = userObj.GetNN<AVIRAIAgentForm>();
	NN<Net::OpenAIMessage> message;
	if (me->lbSession->GetSelectedItem().GetOpt<Net::OpenAIMessage>().SetTo(message))
	{
		NN<Text::String> raw;
		NN<Text::String> toolCallId;
		Text::StringBuilderUTF8 detail;
		if (message->GetToolCallId().SetTo(toolCallId))
		{
			detail.Append(CSTR("Tool call ID: "));
			detail.Append(toolCallId);
			detail.Append(CSTR("\r\n\r\n"));
		}
		if (message->GetRawMessage().SetTo(raw))
			detail.Append(raw);
		else
			detail.Append(message->GetContent());
		me->txtTrace->SetText(detail.ToCString());
	}
	else
	{
		me->txtTrace->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRAIAgentForm::WorkerThread(NN<Sync::Thread> thread)
{
	NN<AVIRAIAgentForm> me = thread->GetUserObj().GetNN<AVIRAIAgentForm>();
	while (!thread->IsStopping())
	{
		NN<Text::String> question;
		{
			Sync::MutexUsage mutUsage(me->resultMut);
			if (!me->pendingQuestion.SetTo(question))
			{
				mutUsage.EndUse();
				thread->Wait(1000);
				continue;
			}
			me->pendingQuestion = nullptr;
		}
		NN<AI::SimpleAIAgent> agent;
		NN<Text::String> prompt;
		if (!me->agent.SetTo(agent) || !me->systemPrompt.SetTo(prompt))
		{
			question->Release();
			me->log.LogMessage(CSTR("AI agent worker is not initialized"), IO::LogHandler::LogLevel::Error);
			Sync::MutexUsage mutUsage(me->resultMut);
			me->completed = true;
			continue;
		}
		NN<AI::AISession> session = agent->CreateSession(prompt->ToCString());
		Optional<Text::String> answer = agent->Ask(session, question->ToCString());
		question->Release();
		{
			Sync::MutexUsage mutUsage(me->resultMut);
			me->answer = answer;
			me->pendingSession = session;
			me->completed = true;
		}
	}
}

UI::EventState __stdcall SSWR::AVIRead::AVIRAIAgentForm::OnQuestionKeyDown(AnyType userObj, UInt32 osKey)
{
	NN<AVIRAIAgentForm> me = userObj.GetNN<AVIRAIAgentForm>();
	if (osKey == 13) // Enter key
	{
		if (me->completed && me->agent.NotNull())
		{
			OnAskClicked(me);
			return UI::EventState::StopEvent;
		}
	}
	return UI::EventState::ContinueEvent;
}

void __stdcall SSWR::AVIRead::AVIRAIAgentForm::OnTimerTick(AnyType userObj)
{
	NN<AVIRAIAgentForm> me = userObj.GetNN<AVIRAIAgentForm>();
	Optional<Text::String> answer;
	Optional<AI::AISession> session;
	{
		Sync::MutexUsage mutUsage(me->resultMut);
		if (!me->completed)
			return;
		answer = me->answer;
		session = me->pendingSession;
		me->answer = nullptr;
		me->pendingSession = nullptr;
		me->completed = false;
	}
	NN<Text::String> s;
	NN<AI::AISession> nnSession;
	if (session.SetTo(nnSession))
	{
		me->displayedSession = nnSession;
		UIntOS i = 0;
		UIntOS count = nnSession->GetMessageCount();
		while (i < count)
		{
			NN<Net::OpenAIMessage> message;
			if (nnSession->GetMessage(i).SetTo(message))
			{
				Text::StringBuilderUTF8 label;
				label.AppendUIntOS(i + 1);
				label.Append(CSTR(". "));
				switch (message->GetRole())
				{
				case Net::OpenAIMessageRole::System: label.Append(CSTR("System")); break;
				case Net::OpenAIMessageRole::User: label.Append(CSTR("User")); break;
				case Net::OpenAIMessageRole::Assistant: label.Append(CSTR("Assistant")); break;
				case Net::OpenAIMessageRole::Tool: label.Append(CSTR("Tool")); break;
				}
				me->lbSession->AddItem(label.ToCString(), message);
			}
			i++;
		}
		if (me->lbSession->GetCount() > 0)
		{
			me->lbSession->SetSelectedIndex(0);
			OnSessionSelChg(me);
		}
	}
	if (answer.SetTo(s))
	{
		me->txtAnswer->SetText(s->ToCString());
		me->lblStatus->SetText(CSTR("Completed"));
	}
	else
	{
		me->lblStatus->SetText(CSTR("Failed"));
		me->log.LogMessage(CSTR("Failed to get an answer from the AI agent"), IO::LogHandler::LogLevel::Error);
		me->ui->ShowMsgOK(CSTR("Failed to get an answer from the AI agent."), CSTR("AI Agent"), me);
	}
	OPTSTR_DEL(answer);
	me->btnAsk->SetEnabled(true);
	me->btnStop->SetEnabled(true);
}

SSWR::AVIRead::AVIRAIAgentForm::AVIRAIAgentForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 700, ui), worker(WorkerThread, this, CSTR("AIAgent"))
{
	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(core->GetTCPClientFactory(), false);
	this->db = nullptr;
	this->openAI = nullptr;
	this->agent = nullptr;
	this->systemPrompt = nullptr;
	this->pendingQuestion = nullptr;
	this->answer = nullptr;
	this->pendingSession = nullptr;
	this->displayedSession = nullptr;
	this->completed = false;
	this->SetText(CSTR("AI Agent"));
	this->SetFont(nullptr, 8.25, false);
	this->SetDPI(core->GetMonitorHDPI(this->GetHMonitor()), core->GetMonitorDDPI(this->GetHMonitor()));
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpControl = this->tcMain->AddTabPage(CSTR("Control"));
	this->pnlSettings = ui->NewPanel(this->tpControl);
	this->pnlSettings->SetRect(0, 0, 780, 510, false);

	NN<UI::GUILabel> lbl;
	lbl = ui->NewLabel(this->pnlSettings, CSTR("JDBC URL"));
	lbl->SetRect(8, 8, 130, 23, false);
	this->txtJDBC = ui->NewTextBox(this->pnlSettings, CSTR(""));
	this->txtJDBC->SetRect(140, 8, 600, 23, false);
	lbl = ui->NewLabel(this->pnlSettings, CSTR("Database user"));
	lbl->SetRect(8, 36, 130, 23, false);
	this->txtDBUser = ui->NewTextBox(this->pnlSettings, CSTR(""));
	this->txtDBUser->SetRect(140, 36, 200, 23, false);
	lbl = ui->NewLabel(this->pnlSettings, CSTR("Database password"));
	lbl->SetRect(350, 36, 130, 23, false);
	this->txtDBPassword = ui->NewTextBox(this->pnlSettings, CSTR(""));
	this->txtDBPassword->SetRect(480, 36, 260, 23, false);
	this->txtDBPassword->SetPasswordChar('*');
	lbl = ui->NewLabel(this->pnlSettings, CSTR("Database schema"));
	lbl->SetRect(8, 64, 130, 23, false);
	this->txtSchema = ui->NewTextBox(this->pnlSettings, CSTR(""));
	this->txtSchema->SetRect(140, 64, 200, 23, false);
	lbl = ui->NewLabel(this->pnlSettings, CSTR("Markdown directory"));
	lbl->SetRect(8, 92, 130, 23, false);
	this->txtMDPath = ui->NewTextBox(this->pnlSettings, CSTR(""));
	this->txtMDPath->SetRect(140, 92, 600, 23, false);
	lbl = ui->NewLabel(this->pnlSettings, CSTR("OpenAI URL"));
	lbl->SetRect(8, 120, 130, 23, false);
	this->txtURL = ui->NewTextBox(this->pnlSettings, CSTR("http://localhost:1234/v1"));
	this->txtURL->SetRect(140, 120, 600, 23, false);
	lbl = ui->NewLabel(this->pnlSettings, CSTR("OpenAI API key"));
	lbl->SetRect(8, 148, 130, 23, false);
	this->txtKey = ui->NewTextBox(this->pnlSettings, CSTR("AnyKey"));
	this->txtKey->SetRect(140, 148, 600, 23, false);
	this->txtKey->SetPasswordChar('*');
	lbl = ui->NewLabel(this->pnlSettings, CSTR("Max tool calls"));
	lbl->SetRect(8, 176, 130, 23, false);
	this->txtMaxToolCalls = ui->NewTextBox(this->pnlSettings, CSTR("20"));
	this->txtMaxToolCalls->SetRect(140, 176, 100, 23, false);
	lbl = ui->NewLabel(this->pnlSettings, CSTR("System prompt"));
	lbl->SetRect(8, 204, 130, 23, false);
	this->txtPrompt = ui->NewTextBox(this->pnlSettings, CSTR(""), true);
	this->txtPrompt->SetRect(140, 204, 600, 250, false);
	this->btnStart = ui->NewButton(this->tpControl, CSTR("Start"));
	this->btnStart->SetRect(140, 490, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	this->btnStop = ui->NewButton(this->tpControl, CSTR("Stop"));
	this->btnStop->SetRect(225, 490, 75, 23, false);
	this->btnStop->HandleButtonClick(OnStopClicked, this);
	this->btnStop->SetEnabled(false);

	this->tpQuestion = this->tcMain->AddTabPage(CSTR("Question"));
	this->grpQuestion = ui->NewGroupBox(this->tpQuestion, CSTR("Question"));
	this->grpQuestion->SetRect(8, 8, 736, 160, false);
	this->grpQuestion->SetDockType(UI::GUIControl::DOCK_TOP);
	this->pnlQuestion = ui->NewPanel(this->grpQuestion);
	this->pnlQuestion->SetRect(8, 32, 720, 32, false);
	this->pnlQuestion->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnAsk = ui->NewButton(this->pnlQuestion, CSTR("Ask"));
	this->btnAsk->SetRect(8, 0, 75, 23, false);
	this->btnAsk->HandleButtonClick(OnAskClicked, this);
	this->btnAsk->SetEnabled(false);
	this->lblStatus = ui->NewLabel(this->pnlQuestion, CSTR("Not started"));
	this->lblStatus->SetRect(96, 0, 400, 23, false);
	this->txtQuestion = ui->NewTextBox(this->grpQuestion, CSTR(""), true);
	this->txtQuestion->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtQuestion->HandleKeyDown(OnQuestionKeyDown, this);
	this->grpAnswer = ui->NewGroupBox(this->tpQuestion, CSTR("Answer"));
	this->grpAnswer->SetRect(8, 176, 736, 160, false);
	this->grpAnswer->SetDockType(UI::GUIControl::DOCK_TOP);
	this->txtAnswer = ui->NewTextBox(this->grpAnswer, CSTR(""), true);
	this->txtAnswer->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtAnswer->SetReadOnly(true);
	this->grpSession = ui->NewGroupBox(this->tpQuestion, CSTR("Session"));
	this->grpSession->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbSession = ui->NewListBox(this->grpSession, false);
	this->lbSession->SetRect(8, 360, 180, 200, false);
	this->lbSession->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbSession->HandleSelectionChange(OnSessionSelChg, this);
	this->txtTrace = ui->NewTextBox(this->grpSession, CSTR(""), true);
	this->txtTrace->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtTrace->SetReadOnly(true);

	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	this->lbLog = ui->NewListBox(this->tpLog, false);
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASSNN(this->logger, UI::ListBoxLogger(*this, this->lbLog, 500, true));
	this->log.AddLogHandler(this->logger, IO::LogHandler::LogLevel::Raw);
	this->AddTimer(200, OnTimerTick, this);
}

SSWR::AVIRead::AVIRAIAgentForm::~AVIRAIAgentForm()
{
	this->Stop(false);
	this->displayedSession.Delete();
	this->log.RemoveLogHandler(this->logger);
	this->logger.Delete();
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRAIAgentForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
