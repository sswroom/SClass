#include "Stdafx.h"
#include "IO/MemoryReadingStream.h"
#include "Net/SAMLUtil.h"
#include "SSWR/AVIRead/AVIRSAMLReqDecodeForm.h"
#include "Text/XMLReader.h"

void __stdcall SSWR::AVIRead::AVIRSAMLReqDecodeForm::OnDecodeClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRSAMLReqDecodeForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSAMLReqDecodeForm>();
	Text::StringBuilderUTF8 sb;
	me->txtRAWRequest->GetText(sb);
	if (sb.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please input raw response"), CSTR("SAML Response Decrypt"), me);
		return;
	}
	Text::StringBuilderUTF8 sbResult;
	if (Net::SAMLUtil::DecodeRequest(sb.ToCString(), sbResult))
	{
		me->txtResult->SetText(sbResult.ToCString());
		sb.ClearStr();
		IO::MemoryReadingStream mstm(sbResult.v, sbResult.leng);
		Text::XMLReader::XMLWellFormat(me->core->GetEncFactory(), mstm, 0, sb);
		me->txtWellFormat->SetText(sb.ToCString());
	}
	else
	{
		me->txtResult->SetText(CSTR("Error in decoding the request"));
	}
}

SSWR::AVIRead::AVIRSAMLReqDecodeForm::AVIRSAMLReqDecodeForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText(CSTR("SAML Request Decode"));
	this->SetFont(0, 0, 8.25, false);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblRAWRequest = ui->NewLabel(*this, CSTR("RAW Request"));
	this->lblRAWRequest->SetRect(0, 0, 100, 23, false);
	this->txtRAWRequest = ui->NewTextBox(*this, CSTR(""), true);
	this->txtRAWRequest->SetRect(100, 0, 500, 119, false);
	this->btnDecode = ui->NewButton(*this, CSTR("Decode"));
	this->btnDecode->SetRect(100, 120, 75, 23, false);
	this->btnDecode->HandleButtonClick(OnDecodeClicked, this);
	this->lblResult = ui->NewLabel(*this, CSTR("Result"));
	this->lblResult->SetRect(0, 144, 100, 23, false);
	this->txtResult = ui->NewTextBox(*this, CSTR(""), true);
	this->txtResult->SetRect(100, 144, 500, 119, false);
	this->lblWellFormat = ui->NewLabel(*this, CSTR("Well Format"));
	this->lblWellFormat->SetRect(0, 264, 100, 23, false);
	this->txtWellFormat = ui->NewTextBox(*this, CSTR(""), true);
	this->txtWellFormat->SetRect(100, 264, 500, 119, false);
}

SSWR::AVIRead::AVIRSAMLReqDecodeForm::~AVIRSAMLReqDecodeForm()
{
}

void SSWR::AVIRead::AVIRSAMLReqDecodeForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
