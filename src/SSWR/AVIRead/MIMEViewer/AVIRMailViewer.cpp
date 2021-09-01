#include "Stdafx.h"
#include "SSWR/AVIRead/MIMEViewer/AVIRMailViewer.h"

SSWR::AVIRead::MIMEViewer::AVIRMailViewer::AVIRMailViewer(SSWR::AVIRead::AVIRCore *core, UI::GUICore *ui, UI::GUIClientControl *ctrl, Media::ColorManagerSess *sess, Text::MIMEObj::MailMessage *mail) : SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer(core, ctrl, mail)
{
	this->mail = mail;

	NEW_CLASS(this->tcHeader, UI::GUITabControl(ui, ctrl));
	this->tcHeader->SetRect(0,0, 100, 96, false);
	this->tcHeader->SetDockType(UI::GUIControl::DOCK_TOP);

	this->tpHeading = this->tcHeader->AddTabPage((const UTF8Char*)"Heading");
	NEW_CLASS(this->lvRecp, UI::GUIListView(ui, this->tpHeading, UI::GUIListView::LVSTYLE_TABLE, 3));
	this->lvRecp->SetRect(0, 0, 300, 23, false);
	this->lvRecp->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->lvRecp->AddColumn((const UTF8Char*)"Type", 40);
	this->lvRecp->AddColumn((const UTF8Char*)"Name", 100);
	this->lvRecp->AddColumn((const UTF8Char*)"Email", 150);
	this->lvRecp->SetShowGrid(true);
	this->lvRecp->SetFullRowSelect(true);
	NEW_CLASS(this->pnlFrom, UI::GUIPanel(ui, this->tpHeading));
	this->pnlFrom->SetRect(0, 0, 100, 24, false);
	this->pnlFrom->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblFrom, UI::GUILabel(ui, this->pnlFrom, (const UTF8Char*)"From"));
	this->lblFrom->SetRect(0, 0, 100, 23, false);
	this->lblFrom->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->txtFrom, UI::GUITextBox(ui, this->pnlFrom, (const UTF8Char*)""));
	this->txtFrom->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtFrom->SetReadOnly(true);
	NEW_CLASS(this->pnlSubject, UI::GUIPanel(ui, this->tpHeading));
	this->pnlSubject->SetRect(0, 0, 100, 24, false);
	this->pnlSubject->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblSubject, UI::GUILabel(ui, this->pnlSubject, (const UTF8Char*)"Subject"));
	this->lblSubject->SetRect(0, 0, 100, 23, false);
	this->lblSubject->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->txtSubject, UI::GUITextBox(ui, this->pnlSubject, (const UTF8Char*)""));
	this->txtSubject->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtSubject->SetReadOnly(true);
	NEW_CLASS(this->pnlDate, UI::GUIPanel(ui, this->tpHeading));
	this->pnlDate->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lblDate, UI::GUILabel(ui, this->pnlDate, (const UTF8Char*)"Date"));
	this->lblDate->SetRect(0, 0, 100, 23, false);
	this->lblDate->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->txtDate, UI::GUITextBox(ui, this->pnlDate, (const UTF8Char*)""));
	this->txtDate->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtDate->SetReadOnly(true);

	this->tpRAW = this->tcHeader->AddTabPage((const UTF8Char*)"RAW Headers");
	NEW_CLASS(this->lvRAW, UI::GUIListView(ui, this->tpRAW, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvRAW->SetDockType(UI::GUIControl::DOCK_RIGHT);
	this->lvRAW->AddColumn((const UTF8Char*)"Name", 100);
	this->lvRAW->AddColumn((const UTF8Char*)"Value", 300);
	this->lvRAW->SetShowGrid(true);
	this->lvRAW->SetFullRowSelect(true);

	NEW_CLASS(this->pnlContent, UI::GUIPanel(ui, ctrl));
	this->pnlContent->SetDockType(UI::GUIControl::DOCK_FILL);
	this->viewer = SSWR::AVIRead::MIMEViewer::AVIRMIMEViewer::CreateViewer(this->core, ui, this->pnlContent, sess, this->mail->GetRAWContent());

	UTF8Char sbuff[512];
	Data::DateTime dt;
	if (this->mail->GetFromAddr(sbuff))
	{
		this->txtFrom->SetText(sbuff);
	}
	if (this->mail->GetSubject(sbuff))
	{
		this->txtSubject->SetText(sbuff);
	}
	if (this->mail->GetDate(&dt))
	{
		dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff zzzz");
		this->txtDate->SetText(sbuff);
	}
	Data::ArrayList<Text::MIMEObj::MailMessage::MailAddress *> addrList;
	Text::MIMEObj::MailMessage::MailAddress *addr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	this->mail->GetRecpList(&addrList);
	i = 0;
	j = addrList.GetCount();
	while (i < j)
	{
		addr = addrList.GetItem(i);
		if (addr->type == Text::MIMEObj::MailMessage::AT_TO)
		{
			k = this->lvRecp->AddItem((const UTF8Char*)"To", 0);
		}
		else if (addr->type == Text::MIMEObj::MailMessage::AT_CC)
		{
			k = this->lvRecp->AddItem((const UTF8Char*)"CC", 0);
		}
		else
		{
			k = this->lvRecp->AddItem((const UTF8Char*)"Unk", 0);
		}
		if (addr->name)
		{
			this->lvRecp->SetSubItem(k, 1, addr->name);
		}
		this->lvRecp->SetSubItem(k, 2, addr->address);
		i++;
	}
	i = 0;
	j = this->mail->GetHeaderCount();
	while (i < j)
	{
		k = this->lvRAW->AddItem(this->mail->GetHeaderName(i), 0);
		this->lvRAW->SetSubItem(k, 1, this->mail->GetHeaderValue(i));
		i++;
	}
}

SSWR::AVIRead::MIMEViewer::AVIRMailViewer::~AVIRMailViewer()
{
	SDEL_CLASS(this->viewer);
}
