#include "Stdafx.h"
#include "Parser/FileParser/X509Parser.h"
#include "SSWR/AVIRead/AVIRCertTextForm.h"
#include "Text/TextBinEnc/Base64Enc.h"
#include "Text/TextBinEnc/HexTextBinEnc.h"

void __stdcall SSWR::AVIRead::AVIRCertTextForm::OnLoadClicked(void *userObj)
{
	SSWR::AVIRead::AVIRCertTextForm *me = (SSWR::AVIRead::AVIRCertTextForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtText->GetText(sb);
	if (sb.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter text to load first"), CSTR("Certificate Load from Text"), me);
		return;
	}
	UOSInt encType = me->cboEncType->GetSelectedIndex();
	UInt8 *buff;
	UOSInt buffSize;
	if (encType == 0)
	{
		Text::TextBinEnc::Base64Enc b64;
		buffSize = b64.CalcBinSize(sb.ToString(), sb.GetLength());
		if (buffSize < 20)
		{
			me->ui->ShowMsgOK(CSTR("Data too short"), CSTR("Certificate Load from Text"), me);
			return;
		}
		buff = MemAlloc(UInt8, buffSize);
		b64.DecodeBin(sb.ToString(), sb.GetLength(), buff);
	}
	else if (encType == 1)
	{
		Text::TextBinEnc::HexTextBinEnc hexEnc;
		buffSize = hexEnc.CalcBinSize(sb.ToString(), sb.GetLength());
		if (buffSize < 20)
		{
			me->ui->ShowMsgOK(CSTR("Data too short"), CSTR("Certificate Load from Text"), me);
			return;
		}
		buff = MemAlloc(UInt8, buffSize);
		hexEnc.DecodeBin(sb.ToString(), sb.GetLength(), buff);
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Unknown Enc Type"), CSTR("Certificate Load from Text"), me);
		return;
	}
	NotNullPtr<Crypto::Cert::X509File> file;
	if (file.Set(Parser::FileParser::X509Parser::ParseBinary(Data::ByteArray(buff, buffSize))))
	{
		MemFree(buff);
		me->core->OpenObject(file);
		me->Close();
	}
	else
	{
		MemFree(buff);
		me->ui->ShowMsgOK(CSTR("Unknown Format"), CSTR("Certificate Load from Text"), me);
		return;
	}
}

SSWR::AVIRead::AVIRCertTextForm::AVIRCertTextForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Certificate Load from Text"));
	this->SetFont(0, 0, 8.25, false);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASSNN(this->pnlButton, UI::GUIPanel(ui, *this));
	this->pnlButton->SetRect(0, 0, 100, 31, false);
	this->pnlButton->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASSNN(this->pnlLabel, UI::GUIPanel(ui, *this));
	this->pnlLabel->SetRect(0, 0, 100, 23, false);
	this->pnlLabel->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lblEncType = ui->NewLabel(this->pnlLabel, CSTR("Enc Type"));
	this->lblEncType->SetRect(0, 0, 100, 23, false);
	this->cboEncType = ui->NewComboBox(*this, false);
	this->cboEncType->SetRect(0, 0, 100, 23, false);
	this->cboEncType->SetDockType(UI::GUIControl::DOCK_TOP);
	this->cboEncType->AddItem(CSTR("Base64"), 0);
	this->cboEncType->AddItem(CSTR("Hex"), 0);
	this->cboEncType->SetSelectedIndex(0);
	this->lblText = ui->NewLabel(this->pnlLabel, CSTR("Text"));
	this->lblText->SetRect(0, 24, 100, 23, false);
	this->txtText = ui->NewTextBox(*this, CSTR(""), true);
	this->txtText->SetDockType(UI::GUIControl::DOCK_FILL);
	this->btnLoad = ui->NewButton(this->pnlButton, CSTR("Load"));
	this->btnLoad->SetRect(104, 4, 75, 23, false);
	this->btnLoad->HandleButtonClick(OnLoadClicked, this);
}

SSWR::AVIRead::AVIRCertTextForm::~AVIRCertTextForm()
{
}

void SSWR::AVIRead::AVIRCertTextForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
