#include "Stdafx.h"
#include "IO/Path.h"
#include "Manage/HiResClock.h"
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
				NN<QuestionInfo> question;
				NEW_CLASSNN(question, QuestionInfo());
				question->question = Text::String::New(sbUser.ToCString());
				sbUser.ClearStr();
				me->txtSystem->GetText(sbUser);
				question->systemPrompt = (sbUser.leng > 0)?Text::String::New(sbUser.ToCString()):Optional<Text::String>(nullptr);
				sbUser.ClearStr();
				me->txtInputPrevRespId->GetText(sbUser);
				question->prevRespId = (sbUser.leng > 0)?Text::String::New(sbUser.ToCString()):Optional<Text::String>(nullptr);
				question->reasoning = (Net::OpenAIReasoningEffort)me->cboInputReasoning->GetSelectedItem().GetIntOS();
				sbUser.ClearStr();
				me->txtInputMaxTokens->GetText(sbUser);
				question->maxTokens = (sbUser.leng > 0)?sbUser.ToUIntOS():0;
				sbUser.ClearStr();
				me->txtInputTemperature->GetText(sbUser);
				question->temperature = (sbUser.leng > 0)?sbUser.ToDoubleOrNAN():NAN;
				sbUser.ClearStr();
				me->txtInputTopP->GetText(sbUser);
				question->topP = (sbUser.leng > 0)?sbUser.ToDoubleOrNAN():NAN;
				question->background = me->chkInputBackground->IsChecked();
				question->stream = me->chkInputStream->IsChecked();
				question->fileList.AddAll(me->fileList);
				{
					Sync::MutexUsage mutUsage(me->questionListMut);
					me->questionList.Add(question);
				}
				me->workerThread.Notify();
				me->txtInput->SetText(CSTR(""));
				me->fileList.Clear();
				me->lblInputFileNames->SetText(CSTR("No File"));
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
	if (me->lbQAPair->GetSelectedItem().GetOpt<QAPair>().SetTo(qa))
	{
		me->currQAPair = qa;
		me->DisplayQAPair(qa);
	}
	else
	{
		me->currQAPair = nullptr;
		me->lvResultValues->ClearItems();
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
	me->txtOutputMessage->SetText(CSTR(""));
	me->txtReasoning->SetText(CSTR(""));
	me->txtResultInstructions->SetText(CSTR(""));
	me->txtResultError->SetText(CSTR(""));
	me->currQAPair = nullptr;
	{
		Sync::MutexUsage mutUsage(me->qaListMut);
		UIntOS i = me->qaList.GetCount();
		while (i-- > 0)
		{
			NN<QAPair> qa = me->qaList.GetItemNoCheck(i);
			if (qa->finish)
			{
				FreeQAPair(qa);
				me->qaList.RemoveAt(i);
			}
		}
		i = 0;
		UIntOS j = me->qaList.GetCount();
		while (i < j)
		{
			NN<QAPair> qa = me->qaList.GetItemNoCheck(i);
			me->lbQAPair->AddItem(qa->question, qa);
			i++;
		}
		if (j > 0)
		{
			me->lbQAPair->SetSelectedIndex(j - 1);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIROpenAIForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIROpenAIForm> me = userObj.GetNN<SSWR::AVIRead::AVIROpenAIForm>();
	UIntOS i;
	UIntOS j;
	UIntOS k;
	NN<QAPair> qa;
	if (me->qaListUpdated)
	{
		Sync::MutexUsage mutUsage(me->qaListMut);
		me->qaListUpdated = false;
		me->lbQAPair->ClearItems();
		k = 0;
		i = 0;
		j = me->qaList.GetCount();
		while (i < j)
		{
			qa = me->qaList.GetItemNoCheck(i);
			k = me->lbQAPair->AddItem(qa->question, qa);
			i++;
		}
		if (j > 0)
		{
			qa = me->qaList.GetItemNoCheck(j - 1);
			me->lbQAPair->SetSelectedIndex(k);
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
	else if (me->currQAPair.SetTo(qa))
	{
		if (qa->messageUpdated)
		{
			qa->messageUpdated = false;
			Sync::MutexUsage mutUsage;
			qa->result->LockAccess(mutUsage);
			me->txtOutputMessage->SetText(qa->result->GetOutputMessage());
			me->tcResult->SetSelectedIndex(0);
		}
		if (qa->reasoningUpdated)
		{
			qa->reasoningUpdated = false;
			Sync::MutexUsage mutUsage;
			qa->result->LockAccess(mutUsage);
			me->txtReasoning->SetText(qa->result->GetOutputReasoning().OrEmpty());
			me->tcResult->SetSelectedIndex(1);
		}
		if (qa->allUpdated)
		{
			qa->allUpdated = false;
			me->DisplayQAPair(qa);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIROpenAIForm::WorkerThread(NN<Sync::Thread> thread)
{
	NN<SSWR::AVIRead::AVIROpenAIForm> me = thread->GetUserObj().GetNN<SSWR::AVIRead::AVIROpenAIForm>();
	Bool found;
	NN<Net::OpenAIClient> cli;
	NN<Text::String> s;
	UIntOS i;
	UIntOS j;
	while (!thread->IsStopping())
	{
		found = false;
		if (me->cli.SetTo(cli))
		{
			Sync::MutexUsage mutUsage(me->questionListMut);
			NN<QuestionInfo> question;
			if (me->questionList.GetCount() > 0 && me->cli.NotNull() && me->questionList.RemoveAt(0).SetTo(question))
			{
				mutUsage.EndUse();
				Net::OpenAIResponse resp(cli->GetCurrModel(), question->question->ToCString(), OPTSTR_CSTR(question->systemPrompt));
				if (question->prevRespId.SetTo(s))
				{
					resp.SetPreviousResponseId(s->ToCString());
				}	
				resp.SetReasoningEffort(question->reasoning);
				if (question->maxTokens > 0)
				{
					resp.SetMaxTokens(question->maxTokens);
				}
				if (!Math::IsNAN(question->temperature))
				{
					resp.SetTemperature(question->temperature);
				}
				if (!Math::IsNAN(question->topP))
				{
					resp.SetTopP(question->topP);
				}
				resp.SetBackground(question->background);
				resp.SetStream(question->stream);
				i = 0;
				j = question->fileList.GetCount();
				while (i < j)
				{
					resp.AddFile(question->fileList.GetItemNoCheck(i)->ToCString());
					i++;
				}				Manage::HiResClock clk;
				NN<Net::OpenAIResult> result = cli->SendResponses(resp);
				NN<QAPair> qa = MemAllocNN(QAPair);
				qa->duration = clk.GetTimeDiff();
				qa->question = question->question->Clone();
				qa->result = result;
				qa->messageUpdated = false;
				qa->reasoningUpdated = false;
				qa->allUpdated = false;
				if (qa->result->IsStream())
				{
					qa->finish = false;
					NN<Net::OpenAIStreamResult> streamResult = NN<Net::OpenAIStreamResult>::ConvertFrom(result);
					{
						Sync::MutexUsage mutUsage(me->qaListMut);
						me->qaList.Add(qa);
						me->qaListUpdated = true;
					}
					while (streamResult->NextEvent().SetTo(s))
					{
						if (thread->IsStopping())
						{
							break;
						}
						if (s->Equals(CSTR("response.reasoning_text.delta")))
						{
							qa->reasoningUpdated = true;
						}
						else if (s->Equals(CSTR("response.output_text.delta")))
						{
							qa->messageUpdated = true;
						}
						else if (s->Equals(CSTR("response.completed")))
						{
							qa->allUpdated = true;
							break;
						}
					}
					qa->duration = clk.GetTimeDiff();
					qa->finish = true;
				}
				else
				{
					qa->finish = true;
					if (qa->result->IsQueuedOrInProgress() && qa->result->GetID().SetTo(s))
					{
						Data::Timestamp nextTime = Data::Timestamp::Now().AddSecond(5);
						UIntOS retryCount = 0;
						while (!thread->IsStopping())
						{
							if (Data::Timestamp::Now() >= nextTime)
							{
								result = cli->GetResponseResult(s->ToCString());
								if (result->IsQueuedOrInProgress())
								{
									nextTime = Data::Timestamp::Now().AddSecond(5);
									retryCount++;
									if (retryCount >= 5 && result->IsQueued())
									{
										result.Delete();
										break;
									}
									result.Delete();
								}
								else
								{
									qa->duration += clk.GetTimeDiff();
									qa->result.Delete();
									qa->result = result;
									break;
								}
							}
							thread->Wait(1000);
						}
					}

					{
						Sync::MutexUsage mutUsage(me->qaListMut);
						me->qaList.Add(qa);
						me->qaListUpdated = true;
					}
				}
				FreeQuestionInfo(question);
				found = true;
			}
		}
		if (!found)
		{
			thread->Wait(1000);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIROpenAIForm::FreeQuestionInfo(NN<QuestionInfo> question)
{
	question->question->Release();
	OPTSTR_DEL(question->systemPrompt);
	OPTSTR_DEL(question->prevRespId);
	question->fileList.FreeAll();
	question.Delete();
}

void __stdcall SSWR::AVIRead::AVIROpenAIForm::FreeQAPair(NN<QAPair> qa)
{
	qa->question->Release();
	qa->result.Delete();
	MemFreeNN(qa);
}

void SSWR::AVIRead::AVIROpenAIForm::DisplayQAPair(NN<QAPair> qa)
{
	NN<Text::String> s;
	Double f64;
	Int64 i64;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	Sync::MutexUsage mutUsage;
	qa->result->LockAccess(mutUsage);
	this->txtOutputMessage->SetText(qa->result->GetOutputMessage());
	this->lvResultValues->ClearItems();
	this->lvResultValues->AddItem(CSTR("question"), nullptr);
	this->lvResultValues->SetSubItem(0, 1, qa->question->ToCString());
	this->lvResultValues->AddItem(CSTR("duration"), nullptr);
	sptr = Text::StrDouble(sbuff, qa->duration);
	this->lvResultValues->SetSubItem(1, 1, CSTRP(sbuff, sptr));
	this->lvResultValues->AddItem(CSTR("id"), nullptr);
	this->lvResultValues->SetSubItem(2, 1, qa->result->GetID().SetTo(s)?s->ToCString():CSTR("null"));
	this->lvResultValues->AddItem(CSTR("object"), nullptr);
	this->lvResultValues->SetSubItem(3, 1, qa->result->GetObject().SetTo(s)?s->ToCString():CSTR("null"));
	this->lvResultValues->AddItem(CSTR("status"), nullptr);
	this->lvResultValues->SetSubItem(4, 1, qa->result->GetStatus().SetTo(s)?s->ToCString():CSTR("null"));
	this->lvResultValues->AddItem(CSTR("model"), nullptr);
	this->lvResultValues->SetSubItem(5, 1, qa->result->GetModel().SetTo(s)?s->ToCString():CSTR("null"));
	this->lvResultValues->AddItem(CSTR("createdAt"), nullptr);
	if (qa->result->GetCreatedAt(i64))
	{
		sptr = Data::Timestamp::FromEpochSec(i64, Data::DateTimeUtil::GetLocalTzQhr()).ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
		this->lvResultValues->SetSubItem(6, 1, CSTRP(sbuff, sptr));
	}
	else
	{
		this->lvResultValues->SetSubItem(6, 1, CSTR("null"));
	}
	this->lvResultValues->AddItem(CSTR("completedAt"), nullptr);
	if (qa->result->GetCompletedAt(i64))
	{
		sptr = Data::Timestamp::FromEpochSec(i64, Data::DateTimeUtil::GetLocalTzQhr()).ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
		this->lvResultValues->SetSubItem(7, 1, CSTRP(sbuff, sptr));
	}
	else
	{
		this->lvResultValues->SetSubItem(7, 1, CSTR("null"));
	}
	this->lvResultValues->AddItem(CSTR("previousResponseId"), nullptr);
	this->lvResultValues->SetSubItem(8, 1, qa->result->GetPreviousResponseId().SetTo(s)?s->ToCString():CSTR("null"));
	this->lvResultValues->AddItem(CSTR("toolChoice"), nullptr);
	this->lvResultValues->SetSubItem(9, 1, qa->result->GetToolChoice().SetTo(s)?s->ToCString():CSTR("null"));
	this->lvResultValues->AddItem(CSTR("truncation"), nullptr);
	this->lvResultValues->SetSubItem(10, 1, qa->result->GetTruncation().SetTo(s)?s->ToCString():CSTR("null"));
	this->lvResultValues->AddItem(CSTR("parallelToolCalls"), nullptr);
	this->lvResultValues->SetSubItem(11, 1, qa->result->IsParallelToolCalls()?CSTR("true"):CSTR("false"));
	this->lvResultValues->AddItem(CSTR("topP"), nullptr);
	if (Math::IsNAN(f64 = qa->result->GetTopP()))
	{
		this->lvResultValues->SetSubItem(12, 1, CSTR("null"));
	}
	else
	{
		sptr = Text::StrDouble(sbuff, f64);
		this->lvResultValues->SetSubItem(12, 1, CSTRP(sbuff, sptr));
	}
	this->lvResultValues->AddItem(CSTR("presencePenalty"), nullptr);
	if (Math::IsNAN(f64 = qa->result->GetPresencePenalty()))
	{
		this->lvResultValues->SetSubItem(13, 1, CSTR("null"));
	}
	else
	{
		sptr = Text::StrDouble(sbuff, f64);
		this->lvResultValues->SetSubItem(13, 1, CSTRP(sbuff, sptr));
	}
	this->lvResultValues->AddItem(CSTR("frequencyPenalty"), nullptr);
	if (Math::IsNAN(f64 = qa->result->GetFrequencyPenalty()))
	{
		this->lvResultValues->SetSubItem(14, 1, CSTR("null"));
	}
	else
	{
		sptr = Text::StrDouble(sbuff, f64);
		this->lvResultValues->SetSubItem(14, 1, CSTRP(sbuff, sptr));
	}
	this->lvResultValues->AddItem(CSTR("topLogprobs"), nullptr);
	if (Math::IsNAN(f64 = qa->result->GetTopLogprobs()))
	{
		this->lvResultValues->SetSubItem(15, 1, CSTR("null"));
	}
	else
	{
		sptr = Text::StrDouble(sbuff, f64);
		this->lvResultValues->SetSubItem(15, 1, CSTRP(sbuff, sptr));
	}
	this->lvResultValues->AddItem(CSTR("temperature"), nullptr);
	if (Math::IsNAN(f64 = qa->result->GetTemperature()))
	{
		this->lvResultValues->SetSubItem(16, 1, CSTR("null"));
	}
	else
	{
		sptr = Text::StrDouble(sbuff, f64);
		this->lvResultValues->SetSubItem(16, 1, CSTRP(sbuff, sptr));
	}
	this->lvResultValues->AddItem(CSTR("inputTokens"), nullptr);
	if (qa->result->GetInputTokens(i64))
	{
		sptr = Text::StrInt64(sbuff, i64);
		this->lvResultValues->SetSubItem(17, 1, CSTRP(sbuff, sptr));
	}
	else
	{
		this->lvResultValues->SetSubItem(17, 1, CSTR("null"));
	}
	this->lvResultValues->AddItem(CSTR("outputTokens"), nullptr);
	if (qa->result->GetOutputTokens(i64))
	{
		sptr = Text::StrInt64(sbuff, i64);
		this->lvResultValues->SetSubItem(18, 1, CSTRP(sbuff, sptr));
	}
	else
	{
		this->lvResultValues->SetSubItem(18, 1, CSTR("null"));
	}
	this->lvResultValues->AddItem(CSTR("totalTokens"), nullptr);
	if (qa->result->GetTotalTokens(i64))
	{
		sptr = Text::StrInt64(sbuff, i64);
		this->lvResultValues->SetSubItem(19, 1, CSTRP(sbuff, sptr));
	}
	else
	{
		this->lvResultValues->SetSubItem(19, 1, CSTR("null"));
	}
	this->lvResultValues->AddItem(CSTR("inputCachedTokens"), nullptr);
	if (qa->result->GetInputCachedTokens(i64))
	{
		sptr = Text::StrInt64(sbuff, i64);
		this->lvResultValues->SetSubItem(20, 1, CSTRP(sbuff, sptr));
	}
	else
	{
		this->lvResultValues->SetSubItem(20, 1, CSTR("null"));
	}
	this->lvResultValues->AddItem(CSTR("outputReasoningTokens"), nullptr);
	if (qa->result->GetOutputReasoningTokens(i64))
	{
		sptr = Text::StrInt64(sbuff, i64);
		this->lvResultValues->SetSubItem(21, 1, CSTRP(sbuff, sptr));
	}
	else
	{
		this->lvResultValues->SetSubItem(21, 1, CSTR("null"));
	}
	this->lvResultValues->AddItem(CSTR("store"), nullptr);
	this->lvResultValues->SetSubItem(22, 1, qa->result->IsStore()?CSTR("true"):CSTR("false"));
	this->lvResultValues->AddItem(CSTR("background"), nullptr);
	this->lvResultValues->SetSubItem(23, 1, qa->result->IsBackground()?CSTR("true"):CSTR("false"));
	this->lvResultValues->AddItem(CSTR("serviceTier"), nullptr);
	this->lvResultValues->SetSubItem(24, 1, qa->result->GetServiceTier().SetTo(s)?s->ToCString():CSTR("null"));

	this->txtReasoning->SetText(qa->result->GetOutputReasoning().OrEmpty());
	this->txtResultInstructions->SetText(qa->result->GetInstructions().SetTo(s)?s->ToCString():CSTR(""));
	this->txtResultError->SetText(qa->result->GetError().SetTo(s)?s->ToCString():CSTR(""));
}

SSWR::AVIRead::AVIROpenAIForm::AVIROpenAIForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui), workerThread(WorkerThread, this, CSTR("OpenAIWorker"))
{
	this->SetText(CSTR("OpenAI Client"));
	this->SetFont(nullptr, 8.25, false);

	this->core = core;
	this->cli = nullptr;
	this->currQAPair = nullptr;
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
	this->pnlInputParam->SetRect(0, 0, 100, 168, false);
	this->pnlInputParam->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblInputPrevRespId = this->ui->NewLabel(this->pnlInputParam, CSTR("Prev Resp ID"));
	this->lblInputPrevRespId->SetRect(4, 0, 100, 24, false);
	this->txtInputPrevRespId = this->ui->NewTextBox(this->pnlInputParam, CSTR(""), false);
	this->txtInputPrevRespId->SetRect(104, 0, 400, 24, false);
	this->lblInputReasoning = this->ui->NewLabel(this->pnlInputParam, CSTR("Reasoning"));
	this->lblInputReasoning->SetRect(4, 24, 100, 24, false);
	this->cboInputReasoning = this->ui->NewComboBox(this->pnlInputParam, false);
	this->cboInputReasoning->SetRect(104, 24, 200, 24, false);
	this->cboInputReasoning->AddItem(CSTR("Default"), (void*)Net::OpenAIReasoningEffort::Default);
	this->cboInputReasoning->AddItem(CSTR("None"), (void*)Net::OpenAIReasoningEffort::None);
	this->cboInputReasoning->AddItem(CSTR("Low"), (void*)Net::OpenAIReasoningEffort::Low);
	this->cboInputReasoning->AddItem(CSTR("Medium"), (void*)Net::OpenAIReasoningEffort::Medium);
	this->cboInputReasoning->AddItem(CSTR("High"), (void*)Net::OpenAIReasoningEffort::High);
	this->cboInputReasoning->SetSelectedIndex(0);
	this->lblInputMaxTokens = this->ui->NewLabel(this->pnlInputParam, CSTR("Max Tokens"));
	this->lblInputMaxTokens->SetRect(4, 48, 100, 24, false);
	this->txtInputMaxTokens = this->ui->NewTextBox(this->pnlInputParam, CSTR(""), false);
	this->txtInputMaxTokens->SetRect(104, 48, 100, 24, false);
	this->lblInputTemperature = this->ui->NewLabel(this->pnlInputParam, CSTR("Temperature"));
	this->lblInputTemperature->SetRect(4, 72, 100, 24, false);
	this->txtInputTemperature = this->ui->NewTextBox(this->pnlInputParam, CSTR(""), false);
	this->txtInputTemperature->SetRect(104, 72, 100, 24, false);
	this->lblInputTopP = this->ui->NewLabel(this->pnlInputParam, CSTR("Top P"));
	this->lblInputTopP->SetRect(4, 96, 100, 24, false);
	this->txtInputTopP = this->ui->NewTextBox(this->pnlInputParam, CSTR(""), false);
	this->txtInputTopP->SetRect(104, 96, 100, 24, false);
	this->lblInputBackground = this->ui->NewLabel(this->pnlInputParam, CSTR("Background"));
	this->lblInputBackground->SetRect(4, 120, 100, 24, false);
	this->chkInputBackground = this->ui->NewCheckBox(this->pnlInputParam, CSTR(""), false);
	this->chkInputBackground->SetRect(104, 120, 100, 24, false);
	this->lblInputStream = this->ui->NewLabel(this->pnlInputParam, CSTR("Stream"));
	this->lblInputStream->SetRect(4, 144, 100, 24, false);
	this->chkInputStream = this->ui->NewCheckBox(this->pnlInputParam, CSTR(""), false);
	this->chkInputStream->SetRect(104, 144, 100, 24, false);
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
	this->AddTimer(500, OnTimerTick, this);
	this->workerThread.Start();
}

SSWR::AVIRead::AVIROpenAIForm::~AVIROpenAIForm()
{
	this->workerThread.Stop();
	this->cli.Delete();
	this->ssl.Delete();
	this->fileList.FreeAll();
	this->qaList.FreeAll(FreeQAPair);
	this->questionList.FreeAll(FreeQuestionInfo);
}

void SSWR::AVIRead::AVIROpenAIForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
