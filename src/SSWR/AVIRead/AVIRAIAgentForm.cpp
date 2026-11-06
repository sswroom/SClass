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
	OPTSTR_DEL(this->trace);
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
	me->txtTrace->SetText(CSTR(""));
	me->btnAsk->SetEnabled(false);
	me->btnStop->SetEnabled(false);
	me->lblStatus->SetText(CSTR("Processing..."));
	me->worker.Notify();
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
		Text::StringBuilderUTF8 trace;
		UIntOS i = 0;
		UIntOS count = session->GetMessageCount();
		while (i < count)
		{
			NN<Net::OpenAIMessage> message;
			if (session->GetMessage(i).SetTo(message))
			{
				switch (message->GetRole())
				{
				case Net::OpenAIMessageRole::System: trace.Append(CSTR("System")); break;
				case Net::OpenAIMessageRole::User: trace.Append(CSTR("User")); break;
				case Net::OpenAIMessageRole::Assistant: trace.Append(CSTR("Assistant")); break;
				case Net::OpenAIMessageRole::Tool: trace.Append(CSTR("Tool")); break;
				}
				trace.Append(CSTR(": "));
				NN<Text::String> raw;
				if (message->GetRawMessage().SetTo(raw))
					trace.Append(raw);
				else
					trace.Append(message->GetContent());
				trace.Append(CSTR("\r\n\r\n"));
			}
			i++;
		}
		session.Delete();
		question->Release();
		{
			Sync::MutexUsage mutUsage(me->resultMut);
			me->answer = answer;
			me->trace = Text::String::New(trace.ToCString());
			me->completed = true;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRAIAgentForm::OnTimerTick(AnyType userObj)
{
	NN<AVIRAIAgentForm> me = userObj.GetNN<AVIRAIAgentForm>();
	Optional<Text::String> answer;
	Optional<Text::String> trace;
	{
		Sync::MutexUsage mutUsage(me->resultMut);
		if (!me->completed)
			return;
		answer = me->answer;
		trace = me->trace;
		me->answer = nullptr;
		me->trace = nullptr;
		me->completed = false;
	}
	NN<Text::String> s;
	if (trace.SetTo(s))
		me->txtTrace->SetText(s->ToCString());
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
	OPTSTR_DEL(trace);
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
	this->trace = nullptr;
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
	lbl = ui->NewLabel(this->tpQuestion, CSTR("Question"));
	lbl->SetRect(8, 8, 100, 23, false);
	this->txtQuestion = ui->NewTextBox(this->tpQuestion, CSTR(""), true);
	this->txtQuestion->SetRect(8, 32, 732, 90, false);
	this->btnAsk = ui->NewButton(this->tpQuestion, CSTR("Ask"));
	this->btnAsk->SetRect(8, 128, 75, 23, false);
	this->btnAsk->HandleButtonClick(OnAskClicked, this);
	this->btnAsk->SetEnabled(false);
	this->lblStatus = ui->NewLabel(this->tpQuestion, CSTR("Not started"));
	this->lblStatus->SetRect(96, 128, 400, 23, false);
	lbl = ui->NewLabel(this->tpQuestion, CSTR("Answer"));
	lbl->SetRect(8, 156, 100, 23, false);
	this->txtAnswer = ui->NewTextBox(this->tpQuestion, CSTR(""), true);
	this->txtAnswer->SetRect(8, 180, 732, 150, false);
	this->txtAnswer->SetReadOnly(true);
	lbl = ui->NewLabel(this->tpQuestion, CSTR("Session"));
	lbl->SetRect(8, 336, 100, 23, false);
	this->txtTrace = ui->NewTextBox(this->tpQuestion, CSTR(""), true);
	this->txtTrace->SetRect(8, 360, 732, 200, false);
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
	this->log.RemoveLogHandler(this->logger);
	this->logger.Delete();
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRAIAgentForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
