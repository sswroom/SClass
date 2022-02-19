#include "Stdafx.h"
#include "Math/Math.h"
#include "Media/ColorConv.h"
#include "SSWR/AVIRead/AVIRGISLineEditForm.h"
#include "Text/MyString.h"
#include "UtilUI/ColorDialog.h"
#include "UI/FontDialog.h"

void SSWR::AVIRead::AVIRGISLineEditForm::LineStyleUpdated()
{
	UTF8Char sbuff[16];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	this->lbLayer->ClearItems();
	i = 0;
	j = this->lineLayers->GetCount();
	while (i < j)
	{
		sptr = Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("Layer")), (Int32)i);
		this->lbLayer->AddItem(CSTRP(sbuff, sptr), this->lineLayers->GetItem(i));
		i++;
	}
}

void SSWR::AVIRead::AVIRGISLineEditForm::UpdatePreview()
{
	Media::ColorProfile srcProfile(Media::ColorProfile::CPT_SRGB);
	Media::ColorProfile destProfile(Media::ColorProfile::CPT_PDISPLAY);
	UOSInt w = this->prevImage->GetWidth();
	UOSInt h = this->prevImage->GetHeight();
	Double dpi = this->prevImage->GetHDPI();

	Media::DrawPen *p;
	Media::DrawBrush *b;
	b = this->prevImage->NewBrushARGB(Media::ColorConv::ConvARGB(&srcProfile, &destProfile, this->colorSess, 0xffc0c0c0));
	this->prevImage->DrawRect(0, 0, UOSInt2Double(w), UOSInt2Double(h), 0, b);
	this->prevImage->DelBrush(b);

	UOSInt i;
	UOSInt j;
	Int32 t;
	LineLayer *lyr;
	i = 0;
	j = this->lineLayers->GetCount();
	while (i < j)
	{
		lyr = this->lineLayers->GetItem(i);
		t = Double2Int32(UOSInt2Double(lyr->thick) * dpi / 96.0);
		if (t <= 0)
		{
			t = 1;
		}
		p = this->prevImage->NewPenARGB(Media::ColorConv::ConvARGB(&srcProfile, &destProfile, this->colorSess, lyr->color), t, lyr->pattern, lyr->nPattern);
		this->prevImage->DrawLine(0, UOSInt2Double(h >> 1), UOSInt2Double(w), UOSInt2Double(h >> 1), p);
		this->prevImage->DelPen(p);
		i++;
	}

	if (this->prevsImage)
	{
		this->pbPreview->SetImage(0);
		DEL_CLASS(this->prevsImage);
	}
	this->prevsImage = this->prevImage->ToStaticImage();
	this->pbPreview->SetImage(this->prevsImage);
}

void __stdcall SSWR::AVIRead::AVIRGISLineEditForm::NewLayerClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISLineEditForm *me = (SSWR::AVIRead::AVIRGISLineEditForm*)userObj;
	LineLayer *lyr;
	lyr = MemAlloc(LineLayer, 1);
	lyr->color = 0xff000000;
	lyr->thick = 1;
	lyr->pattern = 0;
	lyr->nPattern = 0;
	UTF8Char sbuff[16];
	UTF8Char *sptr;
	UOSInt i = me->lineLayers->Add(lyr);
	sptr = Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("Layer")), (Int32)i);
	me->lbLayer->SetSelectedIndex(me->lbLayer->AddItem(CSTRP(sbuff, sptr), lyr));
	me->UpdatePreview();
}

void __stdcall SSWR::AVIRead::AVIRGISLineEditForm::RemoveLayerClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISLineEditForm *me = (SSWR::AVIRead::AVIRGISLineEditForm*)userObj;
	UOSInt i = me->lbLayer->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		me->currLayer = 0;
		FreeLayer(me->lineLayers->RemoveAt(i));
		me->LineStyleUpdated();
		me->UpdatePreview();
	}
}

void __stdcall SSWR::AVIRead::AVIRGISLineEditForm::LayerSelChanged(void *userObj)
{
	SSWR::AVIRead::AVIRGISLineEditForm *me = (SSWR::AVIRead::AVIRGISLineEditForm*)userObj;
	UOSInt i = me->lbLayer->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		Media::ColorProfile srcProfile(Media::ColorProfile::CPT_SRGB);
		Media::ColorProfile destProfile(Media::ColorProfile::CPT_PDISPLAY);
		UTF8Char sbuff[256];
		UTF8Char *sptr;
		me->currLayer = me->lineLayers->GetItem(i);
		me->pbColor->SetBGColor(Media::ColorConv::ConvARGB(&srcProfile, &destProfile, me->colorSess, me->currLayer->color | 0xff000000));
		me->pbColor->Redraw();
		me->hsbAlpha->SetPos((me->currLayer->color >> 24) & 255);
		sptr = Text::StrUOSInt(sbuff, me->currLayer->thick);
		me->txtThick->SetText(CSTRP(sbuff, sptr));
		if (me->currLayer->nPattern == 0)
		{
			me->txtPattern->SetText(CSTR(""));
		}
		else
		{
			sptr = Text::StrUInt16(sbuff, me->currLayer->pattern[0]);
			i = 1;
			while (i < me->currLayer->nPattern)
			{
				sptr = Text::StrUInt16(Text::StrConcatC(sptr, UTF8STRC(",")), me->currLayer->pattern[i]);
				i++;
			}
			me->txtPattern->SetText(CSTRP(sbuff, sptr));
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGISLineEditForm::ThickChanged(void *userObj)
{
	UTF8Char sbuff[16];
	SSWR::AVIRead::AVIRGISLineEditForm *me = (SSWR::AVIRead::AVIRGISLineEditForm*)userObj;
	if (me->currLayer && !me->thickChging)
	{
		me->thickChging = true;
		me->txtThick->GetText(sbuff);
		me->currLayer->thick = Text::StrToUInt32(sbuff);
		if (me->currLayer->thick < 0)
			me->currLayer->thick = 0;
		if (me->currLayer->thick >= 0 && me->currLayer->thick <= 20)
		{
			me->hsbThick->SetPos(me->currLayer->thick);
		}
		me->thickChging = false;
		me->UpdatePreview();
	}
}

void __stdcall SSWR::AVIRead::AVIRGISLineEditForm::OnThickScrolled(void *userObj, UOSInt newPos)
{
	SSWR::AVIRead::AVIRGISLineEditForm *me = (SSWR::AVIRead::AVIRGISLineEditForm*)userObj;
	UTF8Char sbuff[16];
	UTF8Char *sptr;
	if (me->currLayer && !me->thickChging)
	{
		me->thickChging = true;
		sptr = Text::StrUOSInt(sbuff, newPos);
		me->txtThick->SetText(CSTRP(sbuff, sptr));
		me->currLayer->thick = newPos;
		me->thickChging = false;
		me->UpdatePreview();
	}
}

Bool __stdcall SSWR::AVIRead::AVIRGISLineEditForm::ColorClicked(void *userObj, OSInt x, OSInt y, UI::GUIControl::MouseButton btn)
{
	SSWR::AVIRead::AVIRGISLineEditForm *me = (SSWR::AVIRead::AVIRGISLineEditForm*)userObj;
	if (btn == UI::GUIPictureBox::MBTN_LEFT && me->currLayer)
	{
		UtilUI::ColorDialog *dlg;
		Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
		NEW_CLASS(dlg, UtilUI::ColorDialog(0, me->ui, me->core->GetColorMgr(), me->core->GetDrawEngine(), UtilUI::ColorDialog::CCT_PHOTO, &color, me->core->GetMonitorMgr()));
		dlg->SetColor32(me->currLayer->color);
		if (dlg->ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			Media::ColorProfile srcProfile(Media::ColorProfile::CPT_SRGB);
			Media::ColorProfile destProfile(Media::ColorProfile::CPT_PDISPLAY);

			me->currLayer->color = dlg->GetColor32();
			me->pbColor->SetBGColor(Media::ColorConv::ConvARGB(&srcProfile, &destProfile, me->colorSess, me->currLayer->color | 0xff000000));
			me->pbColor->Redraw();
			me->UpdatePreview();
		}
		DEL_CLASS(dlg);
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRGISLineEditForm::PatternChanged(void *userObj)
{
	SSWR::AVIRead::AVIRGISLineEditForm *me = (SSWR::AVIRead::AVIRGISLineEditForm*)userObj;
	UOSInt npattern;
	UTF8Char sbuff[256];
	UTF8Char *sarr[32];
	if (me->currLayer == 0)
		return;
	sbuff[0] = 0;
	me->txtPattern->GetText(sbuff);
	if (me->currLayer->pattern)
	{
		MemFree(me->currLayer->pattern);
	}
	if (sbuff[0] == 0)
	{
		me->currLayer->pattern = 0;
		me->currLayer->nPattern = 0;
		me->UpdatePreview();
	}
	else
	{
		UOSInt i;
		npattern = Text::StrSplit(sarr, 32, sbuff, ',');
		i = npattern;
		me->currLayer->pattern = MemAlloc(UInt8, npattern);
		me->currLayer->nPattern = npattern;
		while (i-- > 0)
		{
			me->currLayer->pattern[i] = (UInt8)Text::StrToUInt32(sarr[i]);
			if (me->currLayer->pattern[i] <= 0)
				me->currLayer->pattern[i] = 1;
		}
		me->UpdatePreview();
	}
}

void __stdcall SSWR::AVIRead::AVIRGISLineEditForm::OKClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISLineEditForm *me = (SSWR::AVIRead::AVIRGISLineEditForm*)userObj;
	LineLayer *lyr;
	Text::StringBuilderUTF8 sb;
	UOSInt i = me->lineLayers->GetCount();
	UOSInt j = me->env->GetLineStyleLayerCnt(me->lineStyle);
	while (j > i)
	{
		me->env->RemoveLineStyleLayer(me->lineStyle, --j);
	}
	i = 0;
	while (i < j)
	{
		lyr = me->lineLayers->GetItem(i);
		me->env->ChgLineStyleLayer(me->lineStyle, i, lyr->color, lyr->thick, lyr->pattern, lyr->nPattern);
		i++;
	}
	j = me->lineLayers->GetCount();
	while (i < j)
	{
		lyr = me->lineLayers->GetItem(i);
		me->env->AddLineStyleLayer(me->lineStyle, lyr->color, lyr->thick, lyr->pattern, lyr->nPattern);
		i++;
	}
	sb.ClearStr();
	me->txtName->GetText(&sb);
	me->env->SetLineStyleName(me->lineStyle, sb.ToString());
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRGISLineEditForm::CancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISLineEditForm *me = (SSWR::AVIRead::AVIRGISLineEditForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void SSWR::AVIRead::AVIRGISLineEditForm::FreeLayer(LineLayer *lyr)
{
	if (lyr->pattern)
	{
		MemFree(lyr->pattern);
	}
	MemFree(lyr);
}

SSWR::AVIRead::AVIRGISLineEditForm::AVIRGISLineEditForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Map::MapEnv *env, Media::DrawEngine *eng, UOSInt lineStyle) : UI::GUIForm(parent, 462, 334, ui)
{
	this->core = core;
	this->env = env;
	this->eng = eng;
	this->lineStyle = lineStyle;
	this->currLayer = 0;
	this->prevsImage = 0;
	this->thickChging = false;
	this->colorSess = this->core->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->colorSess->AddHandler(this);

	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetText(CSTR("Edit Line Style"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	NEW_CLASS(this->pnlButtons, UI::GUIPanel(ui, this));
	this->pnlButtons->SetRect(0, 0, 456, 32, false);
	this->pnlButtons->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->pnlStyle, UI::GUIPanel(ui, this));
	this->pnlStyle->SetRect(0, 0, 456, 56, false);
	this->pnlStyle->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->grpLayer, UI::GUIGroupBox(ui, this, CSTR("Layers")));
	this->grpLayer->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->pnlLayers, UI::GUIPanel(ui, this->grpLayer));
	this->pnlLayers->SetRect(0, 0, 152, 157, false);
	this->pnlLayers->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->pnlLayersButton, UI::GUIPanel(ui, this->pnlLayers));
	this->pnlLayersButton->SetRect(0, 0, 152, 32, false);
	this->pnlLayersButton->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->pnlLayer, UI::GUIPanel(ui, this->grpLayer));
	this->pnlLayer->SetDockType(UI::GUIControl::DOCK_FILL);

	UOSInt w;
	UOSInt h;
	NEW_CLASS(this->pbPreview, UI::GUIPictureBox(ui, this->pnlStyle, this->eng, false, false));
	this->pbPreview->SetRect(0, 0, 100, 24, false);
	this->pbPreview->SetDockType(UI::GUIControl::DOCK_TOP);
	this->pbPreview->GetSizeP(&w, &h);
	this->prevImage = this->eng->CreateImage32(w, h, Media::AT_NO_ALPHA);
	this->prevImage->SetHDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
	NEW_CLASS(this->lblName, UI::GUILabel(ui, this->pnlStyle, CSTR("Name:")));
	this->lblName->SetRect(0, 32, 100, 23, false);
	NEW_CLASS(this->txtName, UI::GUITextBox(ui, this->pnlStyle, CSTR("")));
	this->txtName->SetRect(100, 32, 112, 23, false);

	NEW_CLASS(this->lbLayer, UI::GUIListBox(ui, this->pnlLayers, false));
	this->lbLayer->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLayer->HandleSelectionChange(LayerSelChanged, this);
	NEW_CLASS(this->btnNewLayer, UI::GUIButton(ui, this->pnlLayersButton, CSTR("&Add")));
	this->btnNewLayer->SetRect(8, 8, 64, 23, false);
	this->btnNewLayer->HandleButtonClick(NewLayerClicked, this);
	NEW_CLASS(this->btnRemoveLayer, UI::GUIButton(ui, this->pnlLayersButton, CSTR("&Remove")));
	this->btnRemoveLayer->SetRect(80, 8, 64, 23, false);
	this->btnRemoveLayer->HandleButtonClick(RemoveLayerClicked, this);

	NEW_CLASS(this->lblColor, UI::GUILabel(ui, this->pnlLayer, CSTR("Color")));
	this->lblColor->SetRect(0, 8, 88, 23, false);
	NEW_CLASS(this->lblAlpha, UI::GUILabel(ui, this->pnlLayer, CSTR("Alpha")));
	this->lblAlpha->SetRect(0, 32, 88, 23, false);
	NEW_CLASS(this->lblThick, UI::GUILabel(ui, this->pnlLayer, CSTR("Thick")));
	this->lblThick->SetRect(0, 56, 88, 23, false);
	NEW_CLASS(this->lblPattern, UI::GUILabel(ui, this->pnlLayer, CSTR("Pattern")));
	this->lblPattern->SetRect(0, 80, 88, 23, false);
	NEW_CLASS(this->pbColor, UI::GUIPictureBox(ui, this->pnlLayer, this->eng, true, false));
	this->pbColor->SetRect(96, 8, 72, 24, false);
	this->pbColor->SetBGColor(0);
	this->pbColor->HandleMouseDown(ColorClicked, this);
	NEW_CLASS(this->hsbAlpha, UI::GUIHScrollBar(ui, this->pnlLayer, 144));
	this->hsbAlpha->SetPosition(96, 32);
	this->hsbAlpha->InitScrollBar(0, 255, 255, 1);
	NEW_CLASS(this->txtThick, UI::GUITextBox(ui, this->pnlLayer, CSTR("1")));
	this->txtThick->SetRect(96, 56, 40, 23, false);
	this->txtThick->HandleTextChanged(ThickChanged, this);
	NEW_CLASS(this->hsbThick, UI::GUIHScrollBar(ui, this->pnlLayer, 120));
	this->hsbThick->SetPosition(136, 56);
	this->hsbThick->InitScrollBar(0, 21, 1, 1);
	this->hsbThick->HandlePosChanged(OnThickScrolled, this);
	NEW_CLASS(this->txtPattern, UI::GUITextBox(ui, this->pnlLayer, CSTR("")));
	this->txtPattern->SetRect(96, 80, 100, 23, false);
	this->txtPattern->HandleTextChanged(PatternChanged, this);

	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this->pnlButtons, CSTR("OK")));
	this->btnOK->SetRect(150, 8, 75, 23, false);
	this->btnOK->HandleButtonClick(OKClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this->pnlButtons, CSTR("Cancel")));
	this->btnCancel->SetRect(231, 8, 75, 23, false);
	this->btnCancel->HandleButtonClick(CancelClicked, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	UOSInt i;
	UOSInt j;
	LineLayer *lyr;
	NEW_CLASS(this->lineLayers, Data::ArrayList<LineLayer*>());
	i = 0;
	j = this->env->GetLineStyleLayerCnt(this->lineStyle);
	while (i < j)
	{
		UInt32 color;
		UOSInt thick;
		UInt8 *pattern = 0;
		UOSInt npattern;
		this->env->GetLineStyleLayer(this->lineStyle, i, &color, &thick, &pattern, &npattern);

		lyr = MemAlloc(LineLayer, 1);
		lyr->color = color;
		lyr->thick = thick;
		if (pattern)
		{
			lyr->pattern = MemAlloc(UInt8, npattern);
			lyr->nPattern = npattern;
			MemCopyNO(lyr->pattern, pattern, npattern);
		}
		else
		{
			lyr->pattern = 0;
			lyr->nPattern = 0;
		}
		this->lineLayers->Add(lyr);

		i++;
	}
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	if ((sptr = this->env->GetLineStyleName(this->lineStyle, sbuff)) != 0)
	{
		this->txtName->SetText(CSTRP(sbuff, sptr));
	}
	this->LineStyleUpdated();
	if (this->lineLayers->GetCount() > 0)
	{
		this->lbLayer->SetSelectedIndex(0);
	}
	this->UpdatePreview();
}

SSWR::AVIRead::AVIRGISLineEditForm::~AVIRGISLineEditForm()
{
	UOSInt i;
	i = this->lineLayers->GetCount();
	while (i-- > 0)
	{
		FreeLayer(this->lineLayers->GetItem(i));
	}
	DEL_CLASS(this->lineLayers);
	this->eng->DeleteImage(this->prevImage);
	if (this->prevsImage)
	{
		DEL_CLASS(this->prevsImage);
		this->prevsImage = 0;
	}
	this->colorSess->RemoveHandler(this);
	this->ClearChildren();
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRGISLineEditForm::OnMonitorChanged()
{
	this->colorSess->ChangeMonitor(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	UOSInt w;
	UOSInt h;
	this->pbPreview->GetSizeP(&w, &h);
	this->eng->DeleteImage(this->prevImage);
	this->prevImage = this->eng->CreateImage32(w, h, Media::AT_NO_ALPHA);
	this->prevImage->SetHDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
	if (this->currLayer)
	{
		Media::ColorProfile srcProfile(Media::ColorProfile::CPT_SRGB);
		Media::ColorProfile destProfile(Media::ColorProfile::CPT_PDISPLAY);

		this->pbColor->SetBGColor(Media::ColorConv::ConvARGB(&srcProfile, &destProfile, this->colorSess, this->currLayer->color | 0xff000000));
		this->pbColor->Redraw();
		this->UpdatePreview();
	}
}

void SSWR::AVIRead::AVIRGISLineEditForm::YUVParamChanged(const Media::IColorHandler::YUVPARAM *yuvParam)
{
}

void SSWR::AVIRead::AVIRGISLineEditForm::RGBParamChanged(const Media::IColorHandler::RGBPARAM2 *rgbParam)
{
	if (this->currLayer)
	{
		Media::ColorProfile srcProfile(Media::ColorProfile::CPT_SRGB);
		Media::ColorProfile destProfile(Media::ColorProfile::CPT_PDISPLAY);

		this->pbColor->SetBGColor(Media::ColorConv::ConvARGB(&srcProfile, &destProfile, this->colorSess, this->currLayer->color | 0xff000000));
		this->pbColor->Redraw();
		this->UpdatePreview();
	}
}
