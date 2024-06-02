#include "Stdafx.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMailViewer.h"

SSWR::AVIRead::MIMEViewer::AVIRMailViewer::AVIRMailViewer(NN<SSWR::AVIRead::AVIRCore> core, NN<UI::GUICore> ui, NN<UI::GUIClientControl> ctrl, NN<Media::ColorManagerSess> sess, NN<Text::MIMEObj::MailMessage> mail) : SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer(core, ctrl, mail)
{
	this->mail = mail;

	this->tcHeader = ui->NewTabControl(ctrl);
	this->tcHeader->SetRect(0,0, 100, 96, false);
	this->tcHeader->SetDockType(UI::GUIControl::DOCK_TOP);

	this->tpHeading = this->tcHeader->AddTabPage(CSTR("Heading"));
	this->lvRecp = ui->NewListView(this->tpHeading, UI::ListViewStyle::Table, 3);
	this->lvRecp->SetRect(0, 0, 300, 23, false);
	this->lvRecp->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->lvRecp->AddColumn(CSTR("Type"), 40);
	this->lvRecp->AddColumn(CSTR("Name"), 100);
	this->lvRecp->AddColumn(CSTR("Email"), 150);
	this->lvRecp->SetShowGrid(true);
	this->lvRecp->SetFullRowSelect(true);
	this->pnlFrom = ui->NewPanel(this->tpHeading);
	this->pnlFrom->SetRect(0, 0, 100, 24, false);
	this->pnlFrom->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblFrom = ui->NewLabel(this->pnlFrom, CSTR("From"));
	this->lblFrom->SetRect(0, 0, 100, 23, false);
	this->lblFrom->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->txtFrom = ui->NewTextBox(this->pnlFrom, CSTR(""));
	this->txtFrom->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtFrom->SetReadOnly(true);
	this->pnlSubject = ui->NewPanel(this->tpHeading);
	this->pnlSubject->SetRect(0, 0, 100, 24, false);
	this->pnlSubject->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblSubject = ui->NewLabel(this->pnlSubject, CSTR("Subject"));
	this->lblSubject->SetRect(0, 0, 100, 23, false);
	this->lblSubject->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->txtSubject = ui->NewTextBox(this->pnlSubject, CSTR(""));
	this->txtSubject->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtSubject->SetReadOnly(true);
	this->pnlDate = ui->NewPanel(this->tpHeading);
	this->pnlDate->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblDate = ui->NewLabel(this->pnlDate, CSTR("Date"));
	this->lblDate->SetRect(0, 0, 100, 23, false);
	this->lblDate->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->txtDate = ui->NewTextBox(this->pnlDate, CSTR(""));
	this->txtDate->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtDate->SetReadOnly(true);

	this->tpRAW = this->tcHeader->AddTabPage(CSTR("RAW Headers"));
	this->lvRAW = ui->NewListView(this->tpRAW, UI::ListViewStyle::Table, 2);
	this->lvRAW->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvRAW->AddColumn(CSTR("Name"), 100);
	this->lvRAW->AddColumn(CSTR("Value"), 600);
	this->lvRAW->SetShowGrid(true);
	this->lvRAW->SetFullRowSelect(true);

	this->pnlContent = ui->NewPanel(ctrl);
	this->pnlContent->SetDockType(UI::GUIControl::DOCK_FILL);
	this->viewer = 0;
	NN<Text::IMIMEObj> content;
	if (this->mail->GetRAWContent().SetTo(content))
	{
		this->viewer = SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer::CreateViewer(this->core, ui, this->pnlContent, sess, content);
	}

	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Data::DateTime dt;
	if (this->mail->GetFromAddr(sbuff).SetTo(sptr))
	{
		this->txtFrom->SetText(CSTRP(sbuff, sptr));
	}
	if (this->mail->GetSubject(sbuff).SetTo(sptr))
	{
		this->txtSubject->SetText(CSTRP(sbuff, sptr));
	}
	if (this->mail->GetDate(&dt))
	{
		sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff zzzz");
		this->txtDate->SetText(CSTRP(sbuff, sptr));
	}
	Data::ArrayListNN<Text::MIMEObj::MailMessage::MailAddress> addrList;
	NN<Text::MIMEObj::MailMessage::MailAddress> addr;
	NN<Text::String> s;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	this->mail->GetRecpList(addrList);
	i = 0;
	j = addrList.GetCount();
	while (i < j)
	{
		addr = addrList.GetItemNoCheck(i);
		if (addr->type == Text::MIMEObj::MailMessage::AT_TO)
		{
			k = this->lvRecp->AddItem(CSTR("To"), 0);
		}
		else if (addr->type == Text::MIMEObj::MailMessage::AT_CC)
		{
			k = this->lvRecp->AddItem(CSTR("CC"), 0);
		}
		else
		{
			k = this->lvRecp->AddItem(CSTR("Unk"), 0);
		}
		if (s.Set(addr->name))
		{
			this->lvRecp->SetSubItem(k, 1, s);
		}
		this->lvRecp->SetSubItem(k, 2, addr->address);
		i++;
	}
	this->mail->FreeRecpList(addrList);
	i = 0;
	j = this->mail->GetHeaderCount();
	while (i < j)
	{
		k = this->lvRAW->AddItem(Text::String::OrEmpty(this->mail->GetHeaderName(i)), 0);
		this->lvRAW->SetSubItem(k, 1, Text::String::OrEmpty(this->mail->GetHeaderValue(i)));
		i++;
	}
}

SSWR::AVIRead::MIMEViewer::AVIRMailViewer::~AVIRMailViewer()
{
	this->viewer.Delete();
}
