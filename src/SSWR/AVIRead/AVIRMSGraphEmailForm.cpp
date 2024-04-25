#include "Stdafx.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRMSGraphEmailForm.h"
#include "Text/StringTool.h"

void __stdcall SSWR::AVIRead::AVIRMSGraphEmailForm::OnReadClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRMSGraphEmailForm> me = userObj.GetNN<SSWR::AVIRead::AVIRMSGraphEmailForm>();
	Text::StringBuilderUTF8 sbTenantId;
	Text::StringBuilderUTF8 sbClientId;
	Text::StringBuilderUTF8 sbClientSecret;
	Text::StringBuilderUTF8 sbUserName;
	if (!me->txtTenantId->GetText(sbTenantId) || sbTenantId.leng != 36)
	{
		me->ui->ShowMsgOK(CSTR("Tenant Id invalid"), CSTR("MSGraph Email"), me);
		return;
	}
	if (!me->txtClientId->GetText(sbClientId) || sbClientId.leng != 36)
	{
		me->ui->ShowMsgOK(CSTR("Client Id invalid"), CSTR("MSGraph Email"), me);
		return;
	}
	if (!me->txtClientSecret->GetText(sbClientSecret) || sbClientSecret.leng == 0)
	{
		me->ui->ShowMsgOK(CSTR("Client Secret invalid"), CSTR("MSGraph Email"), me);
		return;
	}
	if (!me->txtUserName->GetText(sbUserName) || !Text::StringTool::IsEmailAddress(sbUserName.v))
	{
		me->ui->ShowMsgOK(CSTR("User Name invalid"), CSTR("MSGraph Email"), me);
		return;
	}

    Net::MSGraphClient cli(me->core->GetSocketFactory(), me->ssl);
	cli.SetLog(me->log);
    NN<Net::MSGraphAccessToken> token;
    if (cli.AccessTokenGet(sbTenantId.ToCString(), sbClientId.ToCString(), sbClientSecret.ToCString(), CSTR_NULL).SetTo(token))
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
            me->ui->ShowMsgOK(CSTR("Error in getting mail message"), CSTR("MSGraph Email"), me);
        }

/*        Data::ArrayListNN<Net::MSGraphMailFolder> folderList;
        if (cli.MailFoldersGet(token, userName, false, folderList))
        {
			Text::StringBuilderUTF8 sb;
            console.WriteLineCStr(CSTR("Mail folders Get:"));
			Data::ArrayIterator<NN<Net::MSGraphMailFolder>> it = folderList.Iterator();
			while (it.HasNext())
			{
				NN<Net::MSGraphMailFolder> folder = it.Next();
				sb.ClearStr();
				sb.AppendOpt(folder->GetDisplayName());
				sb.AppendC(UTF8STRC(" ("));
				sb.AppendOpt(folder->GetId());
				sb.AppendUTF8Char(')');
				console.WriteLineCStr(sb.ToCString());
			}
            folderList.DeleteAll();
        }
        else
        {
            console.WriteLineCStr(CSTR("Error in getting mail folders"));
        }*/
        
        token.Delete();
    }
    else
    {
		me->log.LogMessage(CSTR("Error in getting access token"), IO::LogHandler::LogLevel::Error);
        me->ui->ShowMsgOK(CSTR("Error in getting access token"), CSTR("MSGraph Email"), me);
    }

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
	this->lvMail->ClearItems();
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
			k = this->lvMail->AddItem(sb.ToCString(), msg);
		}
		else
		{
			k = this->lvMail->AddItem(CSTR("Unknown"), msg);
		}
		this->lvMail->SetSubItem(k, 1, msg->GetSubject());
		this->lvMail->SetSubItem(k, 2, msg->GetSentDateTime());
		i++;
	}
}

SSWR::AVIRead::AVIRMSGraphEmailForm::AVIRMSGraphEmailForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 220, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("MSGraph Email"));

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
	this->btnRead = ui->NewButton(this->pnlCtrl, CSTR("Read"));
	this->btnRead->SetRect(104, 100, 75, 23, false);
	this->btnRead->HandleButtonClick(OnReadClicked, this);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpMail = this->tcMain->AddTabPage(CSTR("Mail"));
	this->lvMail = ui->NewListView(this->tpMail, UI::ListViewStyle::Table, 3);
	this->lvMail->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvMail->AddColumn(CSTR("To"), 100);
	this->lvMail->AddColumn(CSTR("Subject"), 300);
	this->lvMail->AddColumn(CSTR("Sent Date"), 100);
	this->lvMail->SetFullRowSelect(true);
	this->lvMail->SetShowGrid(true);

	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	this->txtLog = ui->NewTextBox(this->tpLog, CSTR(""));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->txtLog->SetReadOnly(true);
	this->lbLog = ui->NewListBox(this->tpLog, false);
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSelChg, this);

	this->core = core;
    this->ssl = Net::SSLEngineFactory::Create(core->GetSocketFactory(), false);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	NEW_CLASSNN(logger, UI::ListBoxLogger(*this, this->lbLog, 500, false));
	this->log.AddLogHandler(logger, IO::LogHandler::LogLevel::Raw);
}

SSWR::AVIRead::AVIRMSGraphEmailForm::~AVIRMSGraphEmailForm()
{
	this->msgList.DeleteAll();
	this->log.ClearHandlers();
	this->logger.Delete();
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRMSGraphEmailForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
