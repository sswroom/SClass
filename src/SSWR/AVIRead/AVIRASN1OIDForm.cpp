#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/ASN1Util.h"
#include "SSWR/AVIRead/AVIRASN1OIDForm.h"

void __stdcall SSWR::AVIRead::AVIRASN1OIDForm::OnConvertClicked(void *userObj)
{
	SSWR::AVIRead::AVIRASN1OIDForm *me = (SSWR::AVIRead::AVIRASN1OIDForm*)userObj;
	Text::StringBuilderUTF8 sbCPP;
	Text::StringBuilderUTF8 sb;
	me->txtSource->GetText(sb);
	Text::PString lines[2];
	Text::PString sarr[10];
	UOSInt firstRowCnt;
	UOSInt rowCnt;
	UOSInt lineCnt = Text::StrSplitLineP(lines, 2, sb);
	UOSInt oidCol = 10;
	UOSInt nameCol;
	UInt8 oid[32];
	UOSInt oidLen;
	firstRowCnt = Text::StrSplitTrimP(sarr, 10, lines[0], '\t');
	rowCnt = 0;
	while (rowCnt < firstRowCnt)
	{
		oidLen = Net::ASN1Util::OIDText2PDU(sarr[rowCnt].v, sarr[rowCnt].leng, oid);
		if (oidLen != 0)
		{
			oidCol = rowCnt;
			break;
		}
		rowCnt++;
	}
	if (oidCol == 10)
	{
		me->ui->ShowMsgOK(CSTR("OID Column not found"), CSTR("ASN.1 OID"), me);
		return;
	}
	if (oidCol == 0)
	{
		nameCol = 1;
	}
	else
	{
		nameCol = 0;
	}
	Net::ASN1Util::OIDToCPPCode(oid, oidLen, sarr[nameCol].v, sarr[nameCol].leng, sbCPP);
	while (lineCnt == 2)
	{
		lineCnt = Text::StrSplitLineP(lines, 2, lines[1]);
		rowCnt = Text::StrSplitTrimP(sarr, 10, lines[0], '\t');
		if (rowCnt == firstRowCnt)
		{
			oidLen = Net::ASN1Util::OIDText2PDU(sarr[oidCol].v, sarr[oidCol].leng, oid);
			if (oidLen != 0)
			{
				Net::ASN1Util::OIDToCPPCode(oid, oidLen, sarr[nameCol].v, sarr[nameCol].leng, sbCPP);
			}
		}
	}
	me->txtCPP->SetText(sbCPP.ToCString());
	me->tcMain->SetSelectedIndex(1);
}

SSWR::AVIRead::AVIRASN1OIDForm::AVIRASN1OIDForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("ASN.1 OID"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
		
	this->tpSource = this->tcMain->AddTabPage(CSTR("Source"));
	this->pnlSource = ui->NewPanel(this->tpSource);
	this->pnlSource->SetRect(0, 0, 100, 31, false);
	this->pnlSource->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnConvert = ui->NewButton(this->pnlSource, CSTR("Convert"));
	this->btnConvert->SetRect(4, 4, 75, 23, false);
	this->btnConvert->HandleButtonClick(OnConvertClicked, this);
	this->txtSource = ui->NewTextBox(this->tpSource, CSTR(""), true);
	this->txtSource->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpCPP = this->tcMain->AddTabPage(CSTR("CPP"));
	this->txtCPP = ui->NewTextBox(this->tpCPP, CSTR(""), true);
	this->txtCPP->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtCPP->SetReadOnly(true);
}

SSWR::AVIRead::AVIRASN1OIDForm::~AVIRASN1OIDForm()
{
}

void SSWR::AVIRead::AVIRASN1OIDForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
