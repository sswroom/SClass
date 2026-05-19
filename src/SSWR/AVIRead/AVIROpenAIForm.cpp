#include "Stdafx.h"
#include "IO/Path.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIROpenAIForm.h"

void __stdcall SSWR::AVIRead::AVIROpenAIForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIROpenAIForm> me = userObj.GetNN<SSWR::AVIRead::AVIROpenAIForm>();
	if (me->cli.NotNull())
	{
		me->cli.Delete();
		me->btnStart->SetText(CSTR("Start"));
		me->txtURL->SetReadOnly(false);
		me->txtKey->SetReadOnly(false);
		me->cboModel->ClearItems();
	}
	else
	{
		Text::StringBuilderUTF8 sbURL;
		Text::StringBuilderUTF8 sbKey;
		me->txtURL->GetText(sbURL);
		me->txtKey->GetText(sbKey);
		if (sbURL.GetLength() == 0)
		{
			me->ui->ShowMsgOK(CSTR("Please enter URL"), CSTR("OpenAI"), me);
			return;
		}
		if (sbKey.GetLength() == 0)
		{
			me->ui->ShowMsgOK(CSTR("Please enter Key"), CSTR("OpenAI"), me);
			return;
		}
		if (!sbURL.StartsWith(CSTR("http://")) && !sbURL.StartsWith(CSTR("https://")))
		{
			me->ui->ShowMsgOK(CSTR("URL should start with http:// or https://"), CSTR("OpenAI"), me);
			return;
		}
		NN<Net::OpenAIClient> cli;
		NEW_CLASSNN(cli, Net::OpenAIClient(me->core->GetTCPClientFactory(), me->ssl, sbURL.ToCString(), sbKey.ToCString()));
		if (cli->IsError())
		{
			me->ui->ShowMsgOK(CSTR("Error in getting OpenAI info"), CSTR("OpenAI"), me);
			cli.Delete();
			return;
		}
		me->cli = cli;
		UIntOS i = 0;
		UIntOS j = cli->GetModelCount();
		while (i < j)
		{
			me->cboModel->AddItem(Text::String::OrEmpty(cli->GetModelName(i)), (AnyType)(void*)i);
			i++;
		}
		if (j > 0)
		{
			me->cboModel->SetSelectedIndex(0);
		}
		me->btnStart->SetText(CSTR("Stop"));
		me->txtURL->SetReadOnly(true);
		me->txtKey->SetReadOnly(true);
		me->tcQuestion->SetSelectedIndex(0);
		me->txtInput->Focus();
	}
}

UI::EventState __stdcall SSWR::AVIRead::AVIROpenAIForm::OnUserKeyDown(AnyType userObj, UInt32 osKey)
{
	NN<SSWR::AVIRead::AVIROpenAIForm> me = userObj.GetNN<SSWR::AVIRead::AVIROpenAIForm>();
	UI::GUIControl::GUIKey key = UI::GUIControl::OSKey2GUIKey(osKey);
	if ((key == UI::GUIControl::GK_ENTER) || (key == UI::GUIControl::GK_NUMPAD_ENTER))
	{
		NN<Net::OpenAIClient> cli;
		if (me->cli.SetTo(cli))
		{
			Text::StringBuilderUTF8 sbUser;
			me->txtInput->GetText(sbUser);
			if (sbUser.GetLength() > 0)
			{
				Text::StringBuilderUTF8 sbSystem;
				me->txtSystem->GetText(sbSystem);
				Net::OpenAIResponse resp(cli->GetCurrModel(), sbUser.ToCString(), sbSystem.GetLength() == 0 ? Text::CString(nullptr) : sbSystem.ToCString());
				UIntOS i = 0;
				UIntOS j = me->fileList.GetCount();
				while (i < j)
				{
					resp.AddFile(me->fileList.GetItemNoCheck(i)->ToCString());
					i++;
				}
				NN<Net::OpenAIResult> result = cli->SendResponses(resp);
				NN<QAPair> qa = MemAllocNN(QAPair);
				qa->question = Text::String::New(sbUser.ToCString());
				qa->result = result;
				me->qaList.Add(qa);
				i = me->lbQAPair->AddItem(qa->question, qa);
				me->lbQAPair->SetSelectedIndex(i);
				me->txtInput->SetText(CSTR(""));
				me->fileList.FreeAll();
				me->lblInputFileNames->SetText(CSTR("No File"));
				me->tcQuestion->SetSelectedIndex(2);
				NN<Text::String> s;
				if (qa->result->GetID().SetTo(s))
				{
					me->txtInputPrevRespId->SetText(s->ToCString());
				}
				else
				{
					me->txtInputPrevRespId->SetText(CSTR(""));
				}
			}
		}
		return UI::EventState::StopEvent;
	}
	return UI::EventState::ContinueEvent;
}

void __stdcall SSWR::AVIRead::AVIROpenAIForm::OnModelSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIROpenAIForm> me = userObj.GetNN<SSWR::AVIRead::AVIROpenAIForm>();
	NN<Net::OpenAIClient> cli;
	if (me->cli.SetTo(cli))
	{
		Text::StringBuilderUTF8 sb;
		me->cboModel->GetText(sb);
		cli->SetModel(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIROpenAIForm::OnFiles(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIROpenAIForm> me = userObj.GetNN<SSWR::AVIRead::AVIROpenAIForm>();
	Bool chg = false;
	UIntOS i = 0;
	UIntOS j = files.GetCount();
	while (i < j)
	{
		if (IO::Path::GetPathType(files[i]->ToCString()) == IO::Path::PathType::File)
		{
			me->fileList.Add(files[i]->Clone());
			chg = true;
		}
		i++;
	}
	if (chg)
	{
		if (me->fileList.GetCount() == 1)
		{
			me->lblInputFileNames->SetText(me->fileList.GetItemNoCheck(0)->ToCString());
		}
		else if (me->fileList.GetCount() > 1)
		{
			me->lblInputFileNames->SetText(Text::StringBuilderUTF8().AppendUIntOS(me->fileList.GetCount())->Append(CSTR(" Files"))->ToCString());
		}
		else
		{
			me->lblInputFileNames->SetText(CSTR("No File"));
		}
	}
}

void __stdcall SSWR::AVIRead::AVIROpenAIForm::OnQAPairSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIROpenAIForm> me = userObj.GetNN<SSWR::AVIRead::AVIROpenAIForm>();
	NN<QAPair> qa;
	NN<Text::String> s;
	Double f64;
	Int64 i64;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	me->lvResultValues->ClearItems();
	if (me->lbQAPair->GetSelectedItem().GetOpt<QAPair>().SetTo(qa))
	{
		me->txtOutputMessage->SetText(qa->result->GetOutputMessage()->ToCString());
		me->lvResultValues->AddItem(CSTR("question"), nullptr);
		me->lvResultValues->SetSubItem(0, 1, qa->question->ToCString());
		me->lvResultValues->AddItem(CSTR("id"), nullptr);
		me->lvResultValues->SetSubItem(1, 1, qa->result->GetID().SetTo(s)?s->ToCString():CSTR("null"));
		me->lvResultValues->AddItem(CSTR("object"), nullptr);
		me->lvResultValues->SetSubItem(2, 1, qa->result->GetObject().SetTo(s)?s->ToCString():CSTR("null"));
		me->lvResultValues->AddItem(CSTR("status"), nullptr);
		me->lvResultValues->SetSubItem(3, 1, qa->result->GetStatus().SetTo(s)?s->ToCString():CSTR("null"));
		me->lvResultValues->AddItem(CSTR("model"), nullptr);
		me->lvResultValues->SetSubItem(4, 1, qa->result->GetModel().SetTo(s)?s->ToCString():CSTR("null"));
		me->lvResultValues->AddItem(CSTR("createdAt"), nullptr);
		if (qa->result->GetCreatedAt(i64))
		{
			sptr = Data::Timestamp::FromEpochSec(i64, Data::DateTimeUtil::GetLocalTzQhr()).ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
			me->lvResultValues->SetSubItem(5, 1, CSTRP(sbuff, sptr));
		}
		else
		{
			me->lvResultValues->SetSubItem(5, 1, CSTR("null"));
		}
		me->lvResultValues->AddItem(CSTR("completedAt"), nullptr);
		if (qa->result->GetCompletedAt(i64))
		{
			sptr = Data::Timestamp::FromEpochSec(i64, Data::DateTimeUtil::GetLocalTzQhr()).ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
			me->lvResultValues->SetSubItem(6, 1, CSTRP(sbuff, sptr));
		}
		else
		{
			me->lvResultValues->SetSubItem(6, 1, CSTR("null"));
		}
		me->lvResultValues->AddItem(CSTR("previousResponseId"), nullptr);
		me->lvResultValues->SetSubItem(7, 1, qa->result->GetPreviousResponseId().SetTo(s)?s->ToCString():CSTR("null"));
		me->lvResultValues->AddItem(CSTR("toolChoice"), nullptr);
		me->lvResultValues->SetSubItem(8, 1, qa->result->GetToolChoice().SetTo(s)?s->ToCString():CSTR("null"));
		me->lvResultValues->AddItem(CSTR("truncation"), nullptr);
		me->lvResultValues->SetSubItem(9, 1, qa->result->GetTruncation().SetTo(s)?s->ToCString():CSTR("null"));
		me->lvResultValues->AddItem(CSTR("parallelToolCalls"), nullptr);
		me->lvResultValues->SetSubItem(10, 1, qa->result->IsParallelToolCalls()?CSTR("true"):CSTR("false"));
		me->lvResultValues->AddItem(CSTR("topP"), nullptr);
		if (Math::IsNAN(f64 = qa->result->GetTopP()))
		{
			me->lvResultValues->SetSubItem(11, 1, CSTR("null"));
		}
		else
		{
			sptr = Text::StrDouble(sbuff, f64);
			me->lvResultValues->SetSubItem(11, 1, CSTRP(sbuff, sptr));
		}
		me->lvResultValues->AddItem(CSTR("presencePenalty"), nullptr);
		if (Math::IsNAN(f64 = qa->result->GetPresencePenalty()))
		{
			me->lvResultValues->SetSubItem(12, 1, CSTR("null"));
		}
		else
		{
			sptr = Text::StrDouble(sbuff, f64);
			me->lvResultValues->SetSubItem(12, 1, CSTRP(sbuff, sptr));
		}
		me->lvResultValues->AddItem(CSTR("frequencyPenalty"), nullptr);
		if (Math::IsNAN(f64 = qa->result->GetFrequencyPenalty()))
		{
			me->lvResultValues->SetSubItem(13, 1, CSTR("null"));
		}
		else
		{
			sptr = Text::StrDouble(sbuff, f64);
			me->lvResultValues->SetSubItem(13, 1, CSTRP(sbuff, sptr));
		}
		me->lvResultValues->AddItem(CSTR("topLogprobs"), nullptr);
		if (Math::IsNAN(f64 = qa->result->GetTopLogprobs()))
		{
			me->lvResultValues->SetSubItem(14, 1, CSTR("null"));
		}
		else
		{
			sptr = Text::StrDouble(sbuff, f64);
			me->lvResultValues->SetSubItem(14, 1, CSTRP(sbuff, sptr));
		}
		me->lvResultValues->AddItem(CSTR("temperature"), nullptr);
		if (Math::IsNAN(f64 = qa->result->GetTemperature()))
		{
			me->lvResultValues->SetSubItem(15, 1, CSTR("null"));
		}
		else
		{
			sptr = Text::StrDouble(sbuff, f64);
			me->lvResultValues->SetSubItem(15, 1, CSTRP(sbuff, sptr));
		}
		me->lvResultValues->AddItem(CSTR("inputTokens"), nullptr);
		if (qa->result->GetInputTokens(i64))
		{
			sptr = Text::StrInt64(sbuff, i64);
			me->lvResultValues->SetSubItem(16, 1, CSTRP(sbuff, sptr));
		}
		else
		{
			me->lvResultValues->SetSubItem(16, 1, CSTR("null"));
		}
		me->lvResultValues->AddItem(CSTR("outputTokens"), nullptr);
		if (qa->result->GetOutputTokens(i64))
		{
			sptr = Text::StrInt64(sbuff, i64);
			me->lvResultValues->SetSubItem(17, 1, CSTRP(sbuff, sptr));
		}
		else
		{
			me->lvResultValues->SetSubItem(17, 1, CSTR("null"));
		}
		me->lvResultValues->AddItem(CSTR("totalTokens"), nullptr);
		if (qa->result->GetTotalTokens(i64))
		{
			sptr = Text::StrInt64(sbuff, i64);
			me->lvResultValues->SetSubItem(18, 1, CSTRP(sbuff, sptr));
		}
		else
		{
			me->lvResultValues->SetSubItem(18, 1, CSTR("null"));
		}
		me->lvResultValues->AddItem(CSTR("inputCachedTokens"), nullptr);
		if (qa->result->GetInputCachedTokens(i64))
		{
			sptr = Text::StrInt64(sbuff, i64);
			me->lvResultValues->SetSubItem(19, 1, CSTRP(sbuff, sptr));
		}
		else
		{
			me->lvResultValues->SetSubItem(19, 1, CSTR("null"));
		}
		me->lvResultValues->AddItem(CSTR("outputReasoningTokens"), nullptr);
		if (qa->result->GetOutputReasoningTokens(i64))
		{
			sptr = Text::StrInt64(sbuff, i64);
			me->lvResultValues->SetSubItem(20, 1, CSTRP(sbuff, sptr));
		}
		else
		{
			me->lvResultValues->SetSubItem(20, 1, CSTR("null"));
		}
		me->lvResultValues->AddItem(CSTR("store"), nullptr);
		me->lvResultValues->SetSubItem(21, 1, qa->result->IsStore()?CSTR("true"):CSTR("false"));
		me->lvResultValues->AddItem(CSTR("background"), nullptr);
		me->lvResultValues->SetSubItem(22, 1, qa->result->IsBackground()?CSTR("true"):CSTR("false"));
		me->lvResultValues->AddItem(CSTR("serviceTier"), nullptr);
		me->lvResultValues->SetSubItem(23, 1, qa->result->GetServiceTier().SetTo(s)?s->ToCString():CSTR("null"));

		me->txtReasoning->SetText(qa->result->GetOutputReasoning().SetTo(s)?s->ToCString():CSTR(""));
		me->txtResultInstructions->SetText(qa->result->GetInstructions().SetTo(s)?s->ToCString():CSTR(""));
		me->txtResultError->SetText(qa->result->GetError().SetTo(s)?s->ToCString():CSTR(""));
	}
	else
	{
		me->txtOutputMessage->SetText(CSTR(""));
		me->txtReasoning->SetText(CSTR(""));
		me->txtResultInstructions->SetText(CSTR(""));
		me->txtResultError->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIROpenAIForm::OnQAPairClearClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIROpenAIForm> me = userObj.GetNN<SSWR::AVIRead::AVIROpenAIForm>();
	me->lbQAPair->ClearItems();
	me->qaList.FreeAll(FreeQAPair);
	me->txtOutputMessage->SetText(CSTR(""));
	me->txtReasoning->SetText(CSTR(""));
	me->txtResultInstructions->SetText(CSTR(""));
	me->txtResultError->SetText(CSTR(""));
}

void __stdcall SSWR::AVIRead::AVIROpenAIForm::FreeQAPair(NN<QAPair> qa)
{
	qa->question->Release();
	qa->result.Delete();
	MemFreeNN(qa);
}

SSWR::AVIRead::AVIROpenAIForm::AVIROpenAIForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("OpenAI Client"));
	this->SetFont(nullptr, 8.25, false);

	this->core = core;
	this->cli = nullptr;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetTCPClientFactory(), false);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlControl = this->ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 76, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblURL = this->ui->NewLabel(this->pnlControl, CSTR("URL"));
	this->lblURL->SetRect(4, 4, 100, 23, false);
	this->txtURL = this->ui->NewTextBox(this->pnlControl, CSTR("http://localhost:1234/v1"));
	this->txtURL->SetRect(104, 4, 400, 23, false);
	this->txtURL->HandleAccept(OnStartClicked, this);
	this->lblKey = this->ui->NewLabel(this->pnlControl, CSTR("Key"));
	this->lblKey->SetRect(4, 28, 100, 23, false);
	this->txtKey = this->ui->NewTextBox(this->pnlControl, CSTR("AnyKey"));
	this->txtKey->SetRect(104, 28, 200, 23, false);
	this->btnStart = this->ui->NewButton(this->pnlControl, CSTR("Start"));
	this->btnStart->SetRect(304, 28, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	this->lblModel = this->ui->NewLabel(this->pnlControl, CSTR("Model"));
	this->lblModel->SetRect(4, 52, 100, 23, false);
	this->cboModel = this->ui->NewComboBox(this->pnlControl, false);
	this->cboModel->SetRect(104, 52, 200, 23, false);
	this->cboModel->HandleSelectionChange(OnModelSelChg, this);
	this->tcQuestion = this->ui->NewTabControl(*this);
	this->tcQuestion->SetRect(0, 0, 100, 124, false);
	this->tcQuestion->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpInput = this->tcQuestion->AddTabPage(CSTR("Input"));
	this->pnlInputParam = this->ui->NewPanel(this->tpInput);
	this->pnlInputParam->SetRect(0, 0, 100, 24, false);
	this->pnlInputParam->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblInputPrevRespId = this->ui->NewLabel(this->pnlInputParam, CSTR("Prev Resp ID"));
	this->lblInputPrevRespId->SetRect(4, 0, 100, 24, false);
	this->txtInputPrevRespId = this->ui->NewTextBox(this->pnlInputParam, CSTR(""), false);
	this->txtInputPrevRespId->SetRect(104, 0, 200, 24, false);
	this->pnlInputFile = this->ui->NewPanel(this->tpInput);
	this->pnlInputFile->SetRect(0, 0, 100, 24, false);
	this->pnlInputFile->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->txtInput = this->ui->NewTextBox(this->tpInput, CSTR(""), true);
	this->txtInput->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtInput->HandleKeyDown(OnUserKeyDown, this);
	this->lblInputFiles = this->ui->NewLabel(this->pnlInputFile, CSTR("Files"));
	this->lblInputFiles->SetRect(4, 4, 100, 24, false);
	this->lblInputFiles->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lblInputFileNames = this->ui->NewLabel(this->pnlInputFile, CSTR("No File"));
	this->lblInputFileNames->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpSystem = this->tcQuestion->AddTabPage(CSTR("System"));
	this->txtSystem = this->ui->NewTextBox(this->tpSystem, CSTR(""), true);
	this->txtSystem->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpResult = this->tcQuestion->AddTabPage(CSTR("Result"));
	this->pnlQAPair = this->ui->NewPanel(this->tpResult);
	this->pnlQAPair->SetRect(0, 0, 100, 24, false);
	this->pnlQAPair->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->btnQAPairClear = this->ui->NewButton(this->pnlQAPair, CSTR("Clear"));
	this->btnQAPairClear->SetRect(4, 4, 75, 23, false);
	this->btnQAPairClear->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnQAPairClear->HandleButtonClick(OnQAPairClearClicked, this);
	this->lbQAPair = this->ui->NewListBox(this->pnlQAPair, false);
	this->lbQAPair->SetRect(0, 0, 100, 100, false);
	this->lbQAPair->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbQAPair->HandleSelectionChange(OnQAPairSelChg, this);
	this->lvResultValues = this->ui->NewListView(this->tpResult, UI::ListViewStyle::Table, 2);
	this->lvResultValues->SetRect(0, 0, 100, 150, false);
	this->lvResultValues->SetShowGrid(true);
	this->lvResultValues->SetFullRowSelect(true);
	this->lvResultValues->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lvResultValues->AddColumn(CSTR("Name"), 100);
	this->lvResultValues->AddColumn(CSTR("Value"), 400);
	this->tcResult = this->ui->NewTabControl(this->tpResult);
	this->tcResult->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpOutputMessage = this->tcResult->AddTabPage(CSTR("Output Message"));
	this->txtOutputMessage = this->ui->NewTextBox(this->tpOutputMessage, CSTR(""), true);
	this->txtOutputMessage->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtOutputMessage->SetReadOnly(true);
	this->tpReasoning = this->tcResult->AddTabPage(CSTR("Reasoning"));
	this->txtReasoning = this->ui->NewTextBox(this->tpReasoning, CSTR(""), true);
	this->txtReasoning->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtReasoning->SetReadOnly(true);
	this->tpResultInstructions = this->tcResult->AddTabPage(CSTR("Instructions"));
	this->txtResultInstructions = this->ui->NewTextBox(this->tpResultInstructions, CSTR(""), true);
	this->txtResultInstructions->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtResultInstructions->SetReadOnly(true);
	this->tpResultError = this->tcResult->AddTabPage(CSTR("Error"));
	this->txtResultError = this->ui->NewTextBox(this->tpResultError, CSTR(""), true);
	this->txtResultError->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtResultError->SetReadOnly(true);
	this->HandleDropFiles(OnFiles, this);
}

SSWR::AVIRead::AVIROpenAIForm::~AVIROpenAIForm()
{
	this->cli.Delete();
	this->ssl.Delete();
	this->fileList.FreeAll();
	this->qaList.FreeAll(FreeQAPair);
}

void SSWR::AVIRead::AVIROpenAIForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
