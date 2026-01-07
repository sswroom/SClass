#include "Stdafx.h"
#include "Text/MyString.h"
#include "IO/EXEFile.h"
#include "IO/Path.h"
#include "UI/GUILabel.h"
#include "UI/GUIButton.h"
#include "SSWR/AVIReadCE/AVIRCEAboutForm.h"

void __stdcall SSWR::AVIReadCE::AVIRCEAboutForm::OKClicked(AnyType userObj)
{
	NN<SSWR::AVIReadCE::AVIRCEAboutForm> me = userObj.GetNN<SSWR::AVIReadCE::AVIRCEAboutForm>();
	me->Close();
}

SSWR::AVIReadCE::AVIRCEAboutForm::AVIRCEAboutForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui) : UI::GUIForm(parent, 240, 120, ui)
{
	this->SetText(CSTR("About"));
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);
	
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<UI::GUILabel> lbl;
	Data::DateTime t;
	lbl = ui->NewLabel(*this, CSTR("AVIReadCE by sswroom"));
	lbl->SetRect(8, 8, 220, 23, false);
	IO::Path::GetProcessFileName(sbuff);
	IO::EXEFile::GetFileTime(CSTRP(sbuff, sptr), t);
	sptr = t.ToString(Text::StrConcatC(sbuff, UTF8STRC("Build date: ")), "yyyy-MM-dd HH:mm:ss");
	lbl = ui->NewLabel(*this, CSTRP(sbuff, sptr));
	lbl->SetRect(8, 32, 220, 23, false);
	btn = ui->NewButton(*this, CSTR("OK"));
	btn->SetRect(160, 60, 75, 23, false);
	btn->HandleButtonClick(OKClicked, this);
}

SSWR::AVIReadCE::AVIRCEAboutForm::~AVIRCEAboutForm()
{
}
