#include "Stdafx.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRMSGraphEmailForm.h"
#include "Text/StringTool.h"
#include "UI/GUIFileDialog.h"

#define MSGBOX_TITLE CSTR("MSGraph Email")

void __stdcall SSWR::AVIRead::AVIRMSGraphEmailForm::OnReadClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMSGraphEmailForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMSGraphEmailForm>();
	Text::StringBuilderUTF8 sbUserName;
	if (!me->txtUserName->GetText(sbUserName) || !Text::StringTool::IsEmailAddress(sbUserName.v))
	{
		me->ui->ShowMsgOK(CSTR("User Name invalid"), MSGBOX_TITLE, me);
		return;
	}

    NN<Net::MSGraphAccessToken> token;
    Net::MSGraphClient cli(me->core->GetTCPClientFactory(), me->ssl);
	cli.SetLog(me->log, true);
    if (me->GetToken(cli).SetTo(token))
    {
        Data::ArrayListNN<Net::MSGraphEventMessageRequest> msgList;
        Bool hasNext;
        if (cli.MailMessagesGet(token, sbUserName.ToCString(), 0, 0, msgList, hasNext))
        {
			me->msgList.DeleteAll();
			me->msgList.AddAll(msgList);
			me->UpdateMessages();
        }
        else
        {
            me->ui->ShowMsgOK(CSTR("Error in getting mail message"), MSGBOX_TITLE, me);
        }

/*        Data::ArrayListNN<Net::MSGraphMailFolder> folderList;
        if (cli.MailFoldersGet(token, userName, false, folderList))
        {
			Text::StringBuilderUTF8 sb;
            console.WriteLine(CSTR("Mail folders Get:"));
			Data::ArrayIterator<NN<Net::MSGraphMailFolder>> it = folderList.Iterator();
			while (it.HasNext())
			{
				NN<Net::MSGraphMailFolder> folder = it.Next();
				sb.ClearStr();
				sb.AppendOpt(folder->GetDisplayName());
				sb.AppendC(UTF8STRC(" ("));
				sb.AppendOpt(folder->GetId());
				sb.AppendUTF8Char(')');
				console.WriteLine(sb.ToCString());
			}
            folderList.DeleteAll();
        }
        else
        {
            console.WriteLine(CSTR("Error in getting mail folders"));
        }*/
    }
}

void __stdcall SSWR::AVIRead::AVIRMSGraphEmailForm::OnSendClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMSGraphEmailForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMSGraphEmailForm>();
	Text::StringBuilderUTF8 sbUserName;
	if (!me->txtUserName->GetText(sbUserName) || !Text::StringTool::IsEmailAddress(sbUserName.v))
	{
		me->ui->ShowMsgOK(CSTR("User Name invalid"), MSGBOX_TITLE, me);
		return;
	}
	Text::StringBuilderUTF8 sb;
	Bool found = false;
	Net::Email::EmailMessage msg;
	msg.SetFrom(0, sbUserName.ToCString());
	me->txtSendTo->GetText(sb);
	if (sb.leng > 0)
	{
		found = true;
		if (!msg.AddToList(sb.ToCString()))
		{
			me->ui->ShowMsgOK(CSTR("To invalid"), MSGBOX_TITLE, me);
			return;
		}
	}
	sb.ClearStr();
	me->txtSendCC->GetText(sb);
	if (sb.leng > 0)
	{
		found = true;
		if (!msg.AddCcList(sb.ToCString()))
		{
			me->ui->ShowMsgOK(CSTR("CC invalid"), MSGBOX_TITLE, me);
			return;
		}
	}
	sb.ClearStr();
	me->txtSendBCC->GetText(sb);
	if (sb.leng > 0)
	{
		found = true;
		if (!msg.AddBccList(sb.ToCString()))
		{
			me->ui->ShowMsgOK(CSTR("BCC invalid"), MSGBOX_TITLE, me);
			return;
		}
	}
	if (!found)
	{
		me->ui->ShowMsgOK(CSTR("No Recepiant address found"), MSGBOX_TITLE, me);
		return;
	}
	sb.ClearStr();
	me->txtSendSubject->GetText(sb);
	if (sb.leng == 0)
	{
		me->ui->ShowMsgOK(CSTR("Subject cannot be empty"), MSGBOX_TITLE, me);
		return;
	}
	msg.SetSubject(sb.ToCString());
	sb.ClearStr();
	me->txtSendContent->GetText(sb);
	msg.SetContent(sb.ToCString(), me->chkSendHTML->IsChecked()?CSTR("text/html"):CSTR("text/plain"));
	NN<Text::String> s;
	UOSInt i = 0;
	UOSInt j = me->atts.GetCount();
	while (i < j)
	{
		s = me->atts.GetItemNoCheck(i);
		if (msg.AddAttachment(s->ToCString()).IsNull())
		{
			sb.ClearStr();
			sb.Append(CSTR("Error in reading as attachment: "));
			sb.Append(s);
			me->ui->ShowMsgOK(sb.ToCString(), MSGBOX_TITLE, me);
			return;
		}
		i++;
	}

    NN<Net::MSGraphAccessToken> token;
    Net::MSGraphClient cli(me->core->GetTCPClientFactory(), me->ssl);
	cli.SetLog(me->log, true);
    if (me->GetToken(cli).SetTo(token))
    {
		if (cli.SendEmail(token, sbUserName.ToCString(), msg))
		{
			me->log.LogMessage(CSTR("Success"), IO::LogHandler::LogLevel::Action);
		}
		else
		{
			me->log.LogMessage(CSTR("Failed"), IO::LogHandler::LogLevel::Action);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMSGraphEmailForm::OnSendAttAddClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMSGraphEmailForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMSGraphEmailForm>();
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"sswr", L"AVIRead", L"MSGraphEmailAtt", false);
	NN<Text::String> s;
	dlg->SetAllowMultiSel(true);
	if (dlg->ShowDialog(me->GetHandle()))
	{
		UOSInt i = 0;
		UOSInt j = dlg->GetFileNameCount();
		while (i < j)
		{
			if (dlg->GetFileNames(i).SetTo(s))
			{
				me->atts.Add(s->Clone());
			}
			i++;
		}
		Text::StringBuilderUTF8 sb;
		j = me->atts.GetCount();
		sb.AppendUOSInt(j);
		if (j == 1)
		{
			sb.Append(CSTR(" attachment"));
		}
		else
		{
			sb.Append(CSTR(" attachments"));
		}
		me->txtSendAtt->SetText(sb.ToCString());
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRMSGraphEmailForm::OnSendAttClearClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMSGraphEmailForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMSGraphEmailForm>();
	me->atts.FreeAll();
	me->txtSendAtt->SetText(CSTR("0 attachments"));
}

void __stdcall SSWR::AVIRead::AVIRMSGraphEmailForm::OnLogSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMSGraphEmailForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMSGraphEmailForm>();
	NN<Text::String> s;
	if (me->lbLog->GetSelectedItemTextNew().SetTo(s))
	{
		me->txtLog->SetText(s->ToCString());
		s->Release();
	}
	else
	{
		me->txtLog->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRMSGraphEmailForm::OnFiles(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRMSGraphEmailForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMSGraphEmailForm>();
	UOSInt i = 0;
	UOSInt j = files.GetCount();
	while (i < j)
	{
		me->atts.Add(files[i]->Clone());
		i++;
	}
	Text::StringBuilderUTF8 sb;
	j = me->atts.GetCount();
	sb.AppendUOSInt(j);
	if (j == 1)
	{
		sb.Append(CSTR(" attachment"));
	}
	else
	{
		sb.Append(CSTR(" attachments"));
	}
	me->txtSendAtt->SetText(sb.ToCString());
}

void SSWR::AVIRead::AVIRMSGraphEmailForm::DisplayRecipient(NN<Text::StringBuilderUTF8> sb, NN<Net::MSGraphRecipient> rcpt)
{
	NN<Net::MSGraphEmailAddress> addr;
	if (rcpt->GetEmailAddress().SetTo(addr))
	{
		sb->AppendUTF8Char('"');
		sb->Append(addr->GetName());
		sb->Append(CSTR("\" <"));
		sb->Append(addr->GetAddress());
		sb->AppendUTF8Char('>');
	}
	else
	{
		sb->Append(CSTR("<error>"));
	}
}

void SSWR::AVIRead::AVIRMSGraphEmailForm::UpdateMessages()
{
	this->lvRead->ClearItems();
	NN<Net::MSGraphEventMessageRequest> msg;
	NN<const Data::ArrayListNN<Net::MSGraphRecipient>> rcptList;
	Text::StringBuilderUTF8 sb;
	UOSInt i = 0;
	UOSInt j = this->msgList.GetCount();
	UOSInt k;
	UOSInt l;
	UOSInt m;
	while (i < j)
	{
		msg = this->msgList.GetItemNoCheck(i);
		if (msg->GetToRecipients().SetTo(rcptList))
		{
			sb.ClearStr();
			l = 0;
			m = rcptList->GetCount();
			while (l < m)
			{
				if (l > 0)
					sb.Append(CSTR(", "));
				DisplayRecipient(sb, rcptList->GetItemNoCheck(l));
				l++;
			}
			k = this->lvRead->AddItem(sb.ToCString(), msg);
		}
		else
		{
			k = this->lvRead->AddItem(CSTR("Unknown"), msg);
		}
		this->lvRead->SetSubItem(k, 1, msg->GetSubject());
		this->lvRead->SetSubItem(k, 2, msg->GetSentDateTime());
		i++;
	}
}

Optional<Net::MSGraphAccessToken> SSWR::AVIRead::AVIRMSGraphEmailForm::GetToken(NN<Net::MSGraphClient> cli)
{
    NN<Net::MSGraphAccessToken> token;
	if (this->token.SetTo(token))
	{
		if (token->IsExpired())
		{
			token.Delete();
			this->token = 0;
		}
		else
		{
			return token;
		}
	}
	Text::StringBuilderUTF8 sbTenantId;
	Text::StringBuilderUTF8 sbClientId;
	Text::StringBuilderUTF8 sbClientSecret;
	if (!this->txtTenantId->GetText(sbTenantId) || sbTenantId.leng != 36)
	{
		this->ui->ShowMsgOK(CSTR("Tenant Id invalid"), MSGBOX_TITLE, this);
		return 0;
	}
	if (!this->txtClientId->GetText(sbClientId) || sbClientId.leng != 36)
	{
		this->ui->ShowMsgOK(CSTR("Client Id invalid"), MSGBOX_TITLE, this);
		return 0;
	}
	if (!this->txtClientSecret->GetText(sbClientSecret) || sbClientSecret.leng == 0)
	{
		this->ui->ShowMsgOK(CSTR("Client Secret invalid"), MSGBOX_TITLE, this);
		return 0;
	}

    if (cli->AccessTokenGet(sbTenantId.ToCString(), sbClientId.ToCString(), sbClientSecret.ToCString(), CSTR_NULL).SetTo(token))
    {
		this->token = token;
		return token;
    }
    else
    {
		this->log.LogMessage(CSTR("Error in getting access token"), IO::LogHandler::LogLevel::Error);
        this->ui->ShowMsgOK(CSTR("Error in getting access token"), MSGBOX_TITLE, this);
		return 0;
    }
}

SSWR::AVIRead::AVIRMSGraphEmailForm::AVIRMSGraphEmailForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 600, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("MSGraph Email"));

	this->token = 0;

	this->pnlCtrl = ui->NewPanel(*this);
	this->pnlCtrl->SetRect(0, 0, 100, 128, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblTenantId = ui->NewLabel(this->pnlCtrl, CSTR("Tenant Id"));
	this->lblTenantId->SetRect(4, 4, 100, 23, false);
	this->txtTenantId = ui->NewTextBox(this->pnlCtrl, CSTR(""));
	this->txtTenantId->SetRect(104, 4, 300, 23, false);
	this->lblClientId = ui->NewLabel(this->pnlCtrl, CSTR("Client Id"));
	this->lblClientId->SetRect(4, 28, 100, 23, false);
	this->txtClientId = ui->NewTextBox(this->pnlCtrl, CSTR(""));
	this->txtClientId->SetRect(104, 28, 300, 23, false);
	this->lblClientSecret = ui->NewLabel(this->pnlCtrl, CSTR("Client Secret"));
	this->lblClientSecret->SetRect(4, 52, 100, 23, false);
	this->txtClientSecret = ui->NewTextBox(this->pnlCtrl, CSTR(""));
	this->txtClientSecret->SetRect(104, 52, 300, 23, false);
	this->txtClientSecret->SetPasswordChar('*');
	this->lblUserName = ui->NewLabel(this->pnlCtrl, CSTR("User Name"));
	this->lblUserName->SetRect(4, 76, 100, 23, false);
	this->txtUserName = ui->NewTextBox(this->pnlCtrl, CSTR(""));
	this->txtUserName->SetRect(104, 76, 300, 23, false);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpRead = this->tcMain->AddTabPage(CSTR("Read"));
	this->pnlRead = ui->NewPanel(this->tpRead);
	this->pnlRead->SetRect(0, 0, 100, 31, false);
	this->pnlRead->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnRead = ui->NewButton(this->pnlRead, CSTR("Read"));
	this->btnRead->SetRect(4, 4, 75, 23, false);
	this->btnRead->HandleButtonClick(OnReadClicked, this);
	this->lvRead = ui->NewListView(this->tpRead, UI::ListViewStyle::Table, 3);
	this->lvRead->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvRead->AddColumn(CSTR("To"), 100);
	this->lvRead->AddColumn(CSTR("Subject"), 300);
	this->lvRead->AddColumn(CSTR("Sent Date"), 100);
	this->lvRead->SetFullRowSelect(true);
	this->lvRead->SetShowGrid(true);

	this->tpSend = this->tcMain->AddTabPage(CSTR("Send"));
	this->pnlSendPre = ui->NewPanel(this->tpSend);
	this->pnlSendPre->SetRect(0, 0, 100, 120, false);
	this->pnlSendPre->SetDockType(UI::GUIControl::DOCK_TOP);
	this->pnlSendPost = ui->NewPanel(this->tpSend);
	this->pnlSendPost->SetRect(0, 0, 100, 48, false);
	this->pnlSendPost->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lblSendTo = ui->NewLabel(this->pnlSendPre, CSTR("To"));
	this->lblSendTo->SetRect(0, 0, 100, 23, false);
	this->txtSendTo = ui->NewTextBox(this->pnlSendPre, CSTR(""));
	this->txtSendTo->SetRect(100, 0, 800, 23, false);
	this->lblSendCC = ui->NewLabel(this->pnlSendPre, CSTR("CC"));
	this->lblSendCC->SetRect(0, 24, 100, 23, false);
	this->txtSendCC = ui->NewTextBox(this->pnlSendPre, CSTR(""));
	this->txtSendCC->SetRect(100, 24, 800, 23, false);
	this->lblSendBCC = ui->NewLabel(this->pnlSendPre, CSTR("BCC"));
	this->lblSendBCC->SetRect(0, 48, 100, 23, false);
	this->txtSendBCC = ui->NewTextBox(this->pnlSendPre, CSTR(""));
	this->txtSendBCC->SetRect(100, 48, 800, 23, false);
	this->lblSendSubject = ui->NewLabel(this->pnlSendPre, CSTR("Subject"));
	this->lblSendSubject->SetRect(0, 72, 100, 23, false);
	this->txtSendSubject = ui->NewTextBox(this->pnlSendPre, CSTR(""));
	this->txtSendSubject->SetRect(100, 72, 800, 23, false);
	this->chkSendHTML = ui->NewCheckBox(this->pnlSendPre, CSTR("HTML"), false);
	this->chkSendHTML->SetRect(100, 96, 200, 23, false);
	this->txtSendContent = ui->NewTextBox(this->tpSend, CSTR(""), true);
	this->txtSendContent->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblSendAtt = ui->NewLabel(this->pnlSendPost, CSTR("Attachment"));
	this->lblSendAtt->SetRect(0, 0, 100, 23, false);
	this->txtSendAtt = ui->NewTextBox(this->pnlSendPost, CSTR("0 attachments"));
	this->txtSendAtt->SetRect(100, 0, 400, 23, false);
	this->txtSendAtt->SetReadOnly(true);
	this->btnSendAttAdd = ui->NewButton(this->pnlSendPost, CSTR("Add"));
	this->btnSendAttAdd->SetRect(500, 0, 75, 23, false);
	this->btnSendAttAdd->HandleButtonClick(OnSendAttAddClicked, this);
	this->btnSendAttClear = ui->NewButton(this->pnlSendPost, CSTR("Clear"));
	this->btnSendAttClear->SetRect(580, 0, 75, 23, false);
	this->btnSendAttClear->HandleButtonClick(OnSendAttClearClicked, this);
	this->btnSend = ui->NewButton(this->pnlSendPost, CSTR("Send"));
	this->btnSend->SetRect(100, 24, 75, 23, false);
	this->btnSend->HandleButtonClick(OnSendClicked, this);

	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	this->txtLog = ui->NewTextBox(this->tpLog, CSTR(""));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->txtLog->SetReadOnly(true);
	this->lbLog = ui->NewListBox(this->tpLog, false);
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSelChg, this);

	this->core = core;
    this->ssl = Net::SSLEngineFactory::Create(core->GetTCPClientFactory(), false);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	NEW_CLASSNN(logger, UI::ListBoxLogger(*this, this->lbLog, 500, false));
	this->log.AddLogHandler(logger, IO::LogHandler::LogLevel::Raw);
	this->HandleDropFiles(OnFiles, this);
}

SSWR::AVIRead::AVIRMSGraphEmailForm::~AVIRMSGraphEmailForm()
{
	this->msgList.DeleteAll();
	this->log.ClearHandlers();
	this->logger.Delete();
	this->ssl.Delete();
	this->atts.FreeAll();
	this->token.Delete();
}

void SSWR::AVIRead::AVIRMSGraphEmailForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
