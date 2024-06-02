#include "Stdafx.h"
#include "IO/BuildTime.h"
#include "IO/EXEFile.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRAboutForm.h"
#include "Text/MyString.h"
#include "UI/GUIButton.h"
#include "UI/GUILabel.h"

#if defined(CPU_X86_32) || defined(CPU_X86_64)
extern "C" Int32 UseAVX;
extern "C" Int32 CPUBrand;
#endif

void __stdcall SSWR::AVIRead::AVIRAboutForm::OKClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRAboutForm> me = userObj.GetNN<SSWR::AVIRead::AVIRAboutForm>();
	me->Close();
}

SSWR::AVIRead::AVIRAboutForm::AVIRAboutForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 480, 144, ui)
{
	this->SetText(CSTR("About"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<UI::GUILabel> lbl;
	Data::DateTime t;
#if defined(CPU_X86_32) || defined(CPU_X86_64)
	sptr = Text::StrConcatC(sbuff, UTF8STRC("AVIRead v4 by sswroom"));
	if (CPUBrand == 1)
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC(" (Optimized for Intel CPU)"));
	}
	else if (CPUBrand == 2)
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC(" (AMD CPU)"));
	}
	if (UseAVX)
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC(" (AVX Optimized)"));
	}
#else
	sptr = Text::StrConcatC(sbuff, UTF8STRC("AVIRead v4 by sswroom"));
#endif
	lbl = ui->NewLabel(*this, CSTRP(sbuff, sptr));
	lbl->SetRect(8, 8, 400, 23, false);

	sptr = Text::StrConcatC(sbuff, UTF8STRC("Path: "));
	sptr = IO::Path::GetProcessFileName(sptr).Or(sptr);
	lbl = ui->NewLabel(*this, CSTRP(sbuff, sptr));
	lbl->SetRect(8, 32, 400, 23, false);

	IO::BuildTime::GetBuildTime(&t);
	sptr = t.ToString(Text::StrConcatC(sbuff, UTF8STRC("Build date: ")), "yyyy-MM-dd HH:mm:ss");
	lbl = ui->NewLabel(*this, CSTRP(sbuff, sptr));
	lbl->SetRect(8, 56, 400, 23, false);

	btn = ui->NewButton(*this, CSTR("OK"));
	btn->SetRect(385, 74, 75, 23, false);
	btn->HandleButtonClick(OKClicked, this);
}

SSWR::AVIRead::AVIRAboutForm::~AVIRAboutForm()
{
}

void SSWR::AVIRead::AVIRAboutForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
