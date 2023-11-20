#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRFontRendererForm.h"

void __stdcall SSWR::AVIRead::AVIRFontRendererForm::OnCharSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRFontRendererForm *me = (SSWR::AVIRead::AVIRFontRendererForm*)userObj;
	Math::Size2D<UOSInt> sz = me->pbChar->GetSizeP();
	Media::StaticImage *newImg = me->font->CreateImage((UTF32Char)(OSInt)me->lbChar->GetSelectedItem(), sz);
	me->pbChar->SetImage(newImg, false);
	me->pbChar->ZoomToFit();
	SDEL_CLASS(me->currImg);
	me->currImg = newImg;
}

SSWR::AVIRead::AVIRFontRendererForm::AVIRFontRendererForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Media::FontRenderer *font) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	this->SetFont(0, 0, 8.25, false);
	this->font = font;
	this->core = core;
	this->currImg = 0;
	this->colorSess = this->core->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetText(CSTR("Font Renderer Form"));

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, *this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpChar = this->tcMain->AddTabPage(CSTR("Char"));
	NEW_CLASS(this->lbChar, UI::GUIListBox(ui, this->tpChar, false));
	this->lbChar->SetRect(0, 0, 150, 23, false);
	this->lbChar->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbChar->HandleSelectionChange(OnCharSelChg, this);
	NEW_CLASS(this->hspChar, UI::GUIHSplitter(ui, this->tpChar, 3, false));
	NEW_CLASS(this->pbChar, UI::GUIPictureBoxDD(ui, this->tpChar, this->colorSess, true, false));
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
	DEL_CLASS(this->font);
	SDEL_CLASS(this->currImg);
	this->ClearChildren();
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRFontRendererForm::OnMonitorChanged()
{
	this->pbChar->ChangeMonitor(this->GetHMonitor());
	this->colorSess->ChangeMonitor(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
