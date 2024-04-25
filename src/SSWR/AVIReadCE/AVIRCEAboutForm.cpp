#include "Stdafx.h"
#include "Text/MyString.h"
#include "IO/EXEFile.h"
#include "IO/Path.h"
#include "UI/GUILabel.h"
#include "UI/GUIButton.h"
#include "SSWR/AVIReadCE/AVIRCEAboutForm.h"

void __stdcall SSWR::AVIReadCE::AVIRCEAboutForm::OKClicked(void *userObj)
{
	SSWR::AVIReadCE::AVIRCEAboutForm *me = (SSWR::AVIReadCE::AVIRCEAboutForm*)userObj;
	me->Close();
}

SSWR::AVIReadCE::AVIRCEAboutForm::AVIRCEAboutForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui) : UI::GUIForm(parent, 240, 120, ui)
{
	this->SetText(CSTR("About");
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	
	UTF8Char sbuff[512];
	UI::GUILabel *lbl;
	Data::DateTime t;
	NEW_CLASS(lbl = ui->NewLabel(this, CSTR("AVIReadCE by sswroom")));
	lbl->SetRect(8, 8, 220, 23, false);
	IO::Path::GetProcessFileName(sbuff);
	IO::EXEFile::GetFileTime(sbuff, &t);
	t.ToString(Text::StrConcatC(sbuff, UTF8STRC("Build date: ")), "yyyy-MM-dd HH:mm:ss");
	NEW_CLASS(lbl = ui->NewLabel(this, sbuff));
	lbl->SetRect(8, 32, 220, 23, false);
	NEW_CLASS(btn = ui->NewButton(this, CSTR("OK")));
	btn->SetRect(160, 60, 75, 23, false);
	btn->HandleButtonClick(OKClicked, this);
}

SSWR::AVIReadCE::AVIRCEAboutForm::~AVIRCEAboutForm()
{
}
