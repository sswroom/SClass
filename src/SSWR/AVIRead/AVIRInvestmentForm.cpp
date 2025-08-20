#include "Stdafx.h"
#include "IO/Path.h"
#include "IO/Registry.h"
#include "SSWR/AVIRead/AVIRInvestmentForm.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"

#define TITLE CSTR("Investment")

void __stdcall SSWR::AVIRead::AVIRInvestmentForm::OnDirClicked(AnyType userObj)
{
	NN<AVIRInvestmentForm> me = userObj.GetNN<AVIRInvestmentForm>();
	if (me->path.NotNull())
	{
		me->FreeAll();
		me->txtDir->SetReadOnly(false);
		return;
	}
	Text::StringBuilderUTF8 sb;
	me->txtDir->GetText(sb);
	if (IO::Path::GetPathType(sb.ToCString()) != IO::Path::PathType::Directory)
	{
		me->ui->ShowMsgOK(CSTR("Input is not a directory"), TITLE, me);
		return;
	}

	NN<IO::Registry> reg;
	if (IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"SSWR", L"AVIRead").SetTo(reg))
	{
		UnsafeArray<const WChar> s = Text::StrToWCharNew(sb.v);
		reg->SetValue(L"Investment", s);
		Text::StrDelNew(s);
		IO::Registry::CloseRegistry(reg);
	}
	me->path = Text::String::New(sb.ToCString());
	me->txtDir->SetReadOnly(true);
}

Bool SSWR::AVIRead::AVIRInvestmentForm::SaveSettings()
{
	return false;
}

void SSWR::AVIRead::AVIRInvestmentForm::FreeAll()
{
	OPTSTR_DEL(this->path);
}

SSWR::AVIRead::AVIRInvestmentForm::AVIRInvestmentForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(TITLE);

	this->core = core;
	this->path = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	
	this->pnlDir = ui->NewPanel(*this);
	this->pnlDir->SetRect(0, 0, 100, 31, false);
	this->pnlDir->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblDir = ui->NewLabel(this->pnlDir, CSTR("Directory"));
	this->lblDir->SetRect(4, 4, 100, 23, false);
	this->txtDir = ui->NewTextBox(this->pnlDir, CSTR(""));
	this->txtDir->SetRect(104, 4, 400, 23, false);
	this->btnDir = ui->NewButton(this->pnlDir, CSTR("Load"));
	this->btnDir->SetRect(504, 4, 75, 23, false);
	this->btnDir->HandleButtonClick(OnDirClicked, this);

	NN<IO::Registry> reg;
	if (IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"SSWR", L"AVIRead").SetTo(reg))
	{
		WChar wbuff[512];
		UnsafeArray<WChar> wptr;
		if (reg->GetValueStr(L"Investment", wbuff).SetTo(wptr))
		{
			NN<Text::String> s = Text::String::NewW(wbuff, (UOSInt)(wptr - wbuff));
			this->txtDir->SetText(s->ToCString());
			s->Release();
			OnDirClicked(this);
		}
		IO::Registry::CloseRegistry(reg);
	}
}

SSWR::AVIRead::AVIRInvestmentForm::~AVIRInvestmentForm()
{
	this->FreeAll();
}

void SSWR::AVIRead::AVIRInvestmentForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
