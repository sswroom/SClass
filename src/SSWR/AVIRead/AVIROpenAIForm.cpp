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
	}
}

UI::EventState __stdcall SSWR::AVIRead::AVIROpenAIForm::OnQuestionKeyDown(AnyType userObj, UInt32 osKey)
{
	NN<SSWR::AVIRead::AVIROpenAIForm> me = userObj.GetNN<SSWR::AVIRead::AVIROpenAIForm>();
	UI::GUIControl::GUIKey key = UI::GUIControl::OSKey2GUIKey(osKey);
	if (key == UI::GUIControl::GK_ENTER)
	{
		NN<Net::OpenAIClient> cli;
		if (me->cli.SetTo(cli))
		{
			Text::StringBuilderUTF8 sb;
			me->txtQuestion->GetText(sb);
			if (sb.GetLength() > 0)
			{
				Net::OpenAIResponse resp(cli->GetCurrModel(), sb.ToCString());
				UIntOS i = 0;
				UIntOS j = me->fileList.GetCount();
				while (i < j)
				{
					resp.AddFile(me->fileList.GetItemNoCheck(i)->ToCString());
					i++;
				}
				NN<Net::OpenAIResult> result = cli->SendResponses(resp);
				NN<Text::String> output = result->GetOutputText();
				me->txtAnswer->SetText(output->ToCString());
				output->Release();
				result.Delete();
				me->txtQuestion->SetText(CSTR(""));
				me->fileList.FreeAll();
				me->lblFileNames->SetText(CSTR("No File"));
			}
		}
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
			me->lblFileNames->SetText(me->fileList.GetItemNoCheck(0)->ToCString());
		}
		else if (me->fileList.GetCount() > 1)
		{
			me->lblFileNames->SetText(Text::StringBuilderUTF8().AppendUIntOS(me->fileList.GetCount())->Append(CSTR(" Files"))->ToCString());
		}
		else
		{
			me->lblFileNames->SetText(CSTR("No File"));
		}
	}
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
	this->pnlQuestion = this->ui->NewPanel(*this);
	this->pnlQuestion->SetRect(0, 0, 100, 100, false);
	this->pnlQuestion->SetDockType(UI::GUIControl::DOCK_TOP);
	this->pnlQuestionFile = this->ui->NewPanel(this->pnlQuestion);
	this->pnlQuestionFile->SetRect(0, 0, 100, 24, false);
	this->pnlQuestionFile->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->txtQuestion = this->ui->NewTextBox(this->pnlQuestion, CSTR(""), true);
	this->txtQuestion->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtQuestion->HandleKeyDown(OnQuestionKeyDown, this);
	this->lblFiles = this->ui->NewLabel(this->pnlQuestionFile, CSTR("Files"));
	this->lblFiles->SetRect(4, 4, 100, 24, false);
	this->lblFiles->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lblFileNames = this->ui->NewLabel(this->pnlQuestionFile, CSTR("No File"));
	this->lblFileNames->SetDockType(UI::GUIControl::DOCK_FILL);
	this->vspQuestion = this->ui->NewVSplitter(*this, 3, false);
	this->txtAnswer = this->ui->NewTextBox(*this, CSTR(""), true);
	this->txtAnswer->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtAnswer->SetReadOnly(true);
	this->HandleDropFiles(OnFiles, this);
}

SSWR::AVIRead::AVIROpenAIForm::~AVIROpenAIForm()
{
	this->cli.Delete();
	this->ssl.Delete();
	this->fileList.FreeAll();
}

void SSWR::AVIRead::AVIROpenAIForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
