#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRFontRendererForm.h"

void __stdcall SSWR::AVIRead::AVIRFontRendererForm::OnCharSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRFontRendererForm> me = userObj.GetNN<SSWR::AVIRead::AVIRFontRendererForm>();
	Math::Size2D<UOSInt> sz = me->pbChar->GetSizeP();
	Optional<Media::StaticImage> newImg = me->font->CreateImage((UTF32Char)(OSInt)me->lbChar->GetSelectedItem().p, sz);
	me->pbChar->SetImage(newImg, false);
	me->pbChar->ZoomToFit();
	me->currImg.Delete();
	me->currImg = newImg;
}

SSWR::AVIRead::AVIRFontRendererForm::AVIRFontRendererForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Media::FontRenderer> font) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	this->SetFont(nullptr, 8.25, false);
	this->font = font;
	this->core = core;
	this->currImg = 0;
	this->colorSess = this->core->GetColorManager()->CreateSess(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetText(CSTR("Font Renderer Form"));

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpChar = this->tcMain->AddTabPage(CSTR("Char"));
	this->lbChar = ui->NewListBox(this->tpChar, false);
	this->lbChar->SetRect(0, 0, 150, 23, false);
	this->lbChar->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbChar->HandleSelectionChange(OnCharSelChg, this);
	this->hspChar = ui->NewHSplitter(this->tpChar, 3, false);
	this->pbChar = ui->NewPictureBoxDD(this->tpChar, this->colorSess, true, false);
	this->pbChar->SetDockType(UI::GUIControl::DOCK_FILL);

	UTF32Char i = font->GetMinChar();
	UTF32Char j = font->GetMaxChar();
	while (i <= j)
	{
		sptr = Text::StrHexVal32V(Text::StrConcatC(sbuff, UTF8STRC("0x")), (UInt32)i);
		this->lbChar->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)i);
		i++;
	}
}

SSWR::AVIRead::AVIRFontRendererForm::~AVIRFontRendererForm()
{
	this->font.Delete();
	this->currImg.Delete();
	this->ClearChildren();
	this->core->GetDrawEngine()->EndColorSess(this->colorSess);
	this->core->GetColorManager()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRFontRendererForm::OnMonitorChanged()
{
	this->pbChar->ChangeMonitor(this->GetHMonitor());
	this->colorSess->ChangeMonitor(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
