#include "Stdafx.h"
#include "Math/Math.h"
#include "Media/ColorConv.h"
#include "SSWR/AVIRead/AVIRGISLineEditForm.h"
#include "Text/MyString.h"
#include "UtilUI/ColorDialog.h"

void SSWR::AVIRead::AVIRGISLineEditForm::LineStyleUpdated()
{
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	this->lbLayer->ClearItems();
	i = 0;
	j = this->lineLayers.GetCount();
	while (i < j)
	{
		sptr = Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("Layer")), (Int32)i);
		this->lbLayer->AddItem(CSTRP(sbuff, sptr), this->lineLayers.GetItemNoCheck(i));
		i++;
	}
}

void SSWR::AVIRead::AVIRGISLineEditForm::UpdatePreview()
{
	NN<Media::DrawImage> prevImage;
	if (!this->prevImage.SetTo(prevImage))
		return;
	Media::ColorProfile srcProfile(Media::ColorProfile::CPT_SRGB);
	Media::ColorProfile destProfile(Media::ColorProfile::CPT_PDISPLAY);
	UOSInt w = prevImage->GetWidth();
	UOSInt h = prevImage->GetHeight();
	Double dpi = prevImage->GetHDPI();

	NN<Media::DrawPen> p;
	NN<Media::DrawBrush> b;
	b = prevImage->NewBrushARGB(Media::ColorConv::ConvARGB(srcProfile, destProfile, this->colorSess.Ptr(), 0xffc0c0c0));
	prevImage->DrawRect(Math::Coord2DDbl(0, 0), Math::Size2DDbl(UOSInt2Double(w), UOSInt2Double(h)), 0, b);
	prevImage->DelBrush(b);

	UOSInt i;
	UOSInt j;
	Int32 t;
	NN<LineLayer> lyr;
	i = 0;
	j = this->lineLayers.GetCount();
	while (i < j)
	{
		lyr = this->lineLayers.GetItemNoCheck(i);
		t = Double2Int32(UOSInt2Double(lyr->thick) * dpi / 96.0);
		if (t <= 0)
		{
			t = 1;
		}
		p = prevImage->NewPenARGB(Media::ColorConv::ConvARGB(srcProfile, destProfile, this->colorSess.Ptr(), lyr->color), t, lyr->pattern, lyr->nPattern);
		prevImage->DrawLine(0, UOSInt2Double(h >> 1), UOSInt2Double(w), UOSInt2Double(h >> 1), p);
		prevImage->DelPen(p);
		i++;
	}

	this->pbPreview->SetImage(0);
	this->prevsImage.Delete();
	this->prevsImage = prevImage->ToStaticImage();
	this->pbPreview->SetImage(this->prevsImage);
}

void __stdcall SSWR::AVIRead::AVIRGISLineEditForm::NewLayerClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISLineEditForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISLineEditForm>();
	NN<LineLayer> lyr;
	lyr = MemAllocNN(LineLayer);
	lyr->color = 0xff000000;
	lyr->thick = 1;
	lyr->pattern = 0;
	lyr->nPattern = 0;
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i = me->lineLayers.Add(lyr);
	sptr = Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("Layer")), (Int32)i);
	me->lbLayer->SetSelectedIndex(me->lbLayer->AddItem(CSTRP(sbuff, sptr), lyr));
	me->UpdatePreview();
}

void __stdcall SSWR::AVIRead::AVIRGISLineEditForm::RemoveLayerClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISLineEditForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISLineEditForm>();
	UOSInt i = me->lbLayer->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		me->currLayer = 0;
		FreeLayer(me->lineLayers.GetItemNoCheck(i));
		me->lineLayers.RemoveAt(i);
		me->LineStyleUpdated();
		me->UpdatePreview();
	}
}

void __stdcall SSWR::AVIRead::AVIRGISLineEditForm::LayerSelChanged(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISLineEditForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISLineEditForm>();
	UOSInt i = me->lbLayer->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		Media::ColorProfile srcProfile(Media::ColorProfile::CPT_SRGB);
		Media::ColorProfile destProfile(Media::ColorProfile::CPT_PDISPLAY);
		UTF8Char sbuff[256];
		UnsafeArray<UTF8Char> sptr;
		NN<LineLayer> currLayer;
		me->currLayer = currLayer = me->lineLayers.GetItemNoCheck(i);
		me->pbColor->SetBGColor(Media::ColorConv::ConvARGB(srcProfile, destProfile, me->colorSess.Ptr(), currLayer->color | 0xff000000));
		me->pbColor->Redraw();
		me->hsbAlpha->SetPos((currLayer->color >> 24) & 255);
		sptr = Text::StrDouble(sbuff, currLayer->thick);
		me->txtThick->SetText(CSTRP(sbuff, sptr));
		UnsafeArray<UInt8> pattern;
		if (currLayer->nPattern == 0 || !currLayer->pattern.SetTo(pattern))
		{
			me->txtPattern->SetText(CSTR(""));
		}
		else
		{
			sptr = Text::StrUInt16(sbuff, pattern[0]);
			i = 1;
			while (i < currLayer->nPattern)
			{
				sptr = Text::StrUInt16(Text::StrConcatC(sptr, UTF8STRC(",")), pattern[i]);
				i++;
			}
			me->txtPattern->SetText(CSTRP(sbuff, sptr));
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGISLineEditForm::ThickChanged(AnyType userObj)
{
	UTF8Char sbuff[16];
	NN<SSWR::AVIRead::AVIRGISLineEditForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISLineEditForm>();
	NN<LineLayer> currLayer;
	if (me->currLayer.SetTo(currLayer) && !me->thickChging)
	{
		me->thickChging = true;
		me->txtThick->GetText(sbuff);
		currLayer->thick = Text::StrToDoubleOr(sbuff, 0);
		if (currLayer->thick < 0)
			currLayer->thick = 0;
		if (currLayer->thick >= 0 && currLayer->thick <= 20)
		{
			me->hsbThick->SetPos((UOSInt)Double2OSInt(currLayer->thick * 10));
		}
		me->thickChging = false;
		me->UpdatePreview();
	}
}

void __stdcall SSWR::AVIRead::AVIRGISLineEditForm::OnThickScrolled(AnyType userObj, UOSInt newPos)
{
	NN<SSWR::AVIRead::AVIRGISLineEditForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISLineEditForm>();
	NN<LineLayer> currLayer;
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	if (me->currLayer.SetTo(currLayer) && !me->thickChging)
	{
		me->thickChging = true;
		currLayer->thick = UOSInt2Double(newPos) * 0.1;
		sptr = Text::StrDouble(sbuff, currLayer->thick);
		me->txtThick->SetText(CSTRP(sbuff, sptr));
		me->thickChging = false;
		me->UpdatePreview();
	}
}

Bool __stdcall SSWR::AVIRead::AVIRGISLineEditForm::ColorClicked(AnyType userObj, Math::Coord2D<OSInt> scnPos, UI::GUIControl::MouseButton btn)
{
	NN<SSWR::AVIRead::AVIRGISLineEditForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISLineEditForm>();
	NN<LineLayer> currLayer;
	if (btn == UI::GUIPictureBox::MBTN_LEFT && me->currLayer.SetTo(currLayer))
	{
		Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
		UtilUI::ColorDialog dlg(0, me->ui, me->core->GetColorMgr(), me->core->GetDrawEngine(), UtilUI::ColorDialog::CCT_PHOTO, color, me->core->GetMonitorMgr());
		dlg.SetColor32(currLayer->color);
		if (dlg.ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			Media::ColorProfile srcProfile(Media::ColorProfile::CPT_SRGB);
			Media::ColorProfile destProfile(Media::ColorProfile::CPT_PDISPLAY);

			currLayer->color = dlg.GetColor32();
			me->pbColor->SetBGColor(Media::ColorConv::ConvARGB(srcProfile, destProfile, me->colorSess.Ptr(), currLayer->color | 0xff000000));
			me->pbColor->Redraw();
			me->UpdatePreview();
		}
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRGISLineEditForm::PatternChanged(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISLineEditForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISLineEditForm>();
	UOSInt npattern;
	NN<LineLayer> currLayer;
	UnsafeArray<UInt8> pattern;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sarr[32];
	if (!me->currLayer.SetTo(currLayer))
		return;
	sbuff[0] = 0;
	me->txtPattern->GetText(sbuff);
	if (currLayer->pattern.SetTo(pattern))
	{
		MemFreeArr(pattern);
	}
	if (sbuff[0] == 0)
	{
		currLayer->pattern = 0;
		currLayer->nPattern = 0;
		me->UpdatePreview();
	}
	else
	{
		UOSInt i;
		npattern = Text::StrSplit(sarr, 32, sbuff, ',');
		i = npattern;
		currLayer->pattern = pattern = MemAllocArr(UInt8, npattern);
		currLayer->nPattern = npattern;
		while (i-- > 0)
		{
			pattern[i] = (UInt8)Text::StrToUInt32(sarr[i]);
			if (pattern[i] <= 0)
				pattern[i] = 1;
		}
		me->UpdatePreview();
	}
}

void __stdcall SSWR::AVIRead::AVIRGISLineEditForm::OKClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISLineEditForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISLineEditForm>();
	NN<LineLayer> lyr;
	Text::StringBuilderUTF8 sb;
	UOSInt i = me->lineLayers.GetCount();
	UOSInt j = me->env->GetLineStyleLayerCnt(me->lineStyle);
	while (j > i)
	{
		me->env->RemoveLineStyleLayer(me->lineStyle, --j);
	}
	i = 0;
	while (i < j)
	{
		lyr = me->lineLayers.GetItemNoCheck(i);
		me->env->ChgLineStyleLayer(me->lineStyle, i, lyr->color, lyr->thick, lyr->pattern, lyr->nPattern);
		i++;
	}
	j = me->lineLayers.GetCount();
	while (i < j)
	{
		lyr = me->lineLayers.GetItemNoCheck(i);
		me->env->AddLineStyleLayer(me->lineStyle, lyr->color, lyr->thick, lyr->pattern, lyr->nPattern);
		i++;
	}
	sb.ClearStr();
	me->txtName->GetText(sb);
	me->env->SetLineStyleName(me->lineStyle, sb.ToCString());
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRGISLineEditForm::CancelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISLineEditForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISLineEditForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIRGISLineEditForm::FreeLayer(NN<LineLayer> lyr)
{
	UnsafeArray<UInt8> pattern;
	if (lyr->pattern.SetTo(pattern))
	{
		MemFreeArr(pattern);
	}
	MemFreeNN(lyr);
}

SSWR::AVIRead::AVIRGISLineEditForm::AVIRGISLineEditForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::MapEnv> env, NN<Media::DrawEngine> eng, UOSInt lineStyle) : UI::GUIForm(parent, 462, 334, ui)
{
	this->core = core;
	this->env = env;
	this->eng = eng;
	this->lineStyle = lineStyle;
	this->currLayer = 0;
	this->prevsImage = 0;
	this->thickChging = false;
	this->colorSess = this->core->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->colorSess->AddHandler(*this);

	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetText(CSTR("Edit Line Style"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->pnlButtons = ui->NewPanel(*this);
	this->pnlButtons->SetRect(0, 0, 456, 32, false);
	this->pnlButtons->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->pnlStyle = ui->NewPanel(*this);
	this->pnlStyle->SetRect(0, 0, 456, 56, false);
	this->pnlStyle->SetDockType(UI::GUIControl::DOCK_TOP);
	this->grpLayer = ui->NewGroupBox(*this, CSTR("Layers"));
	this->grpLayer->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pnlLayers = ui->NewPanel(this->grpLayer);
	this->pnlLayers->SetRect(0, 0, 152, 157, false);
	this->pnlLayers->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->pnlLayersButton = ui->NewPanel(this->pnlLayers);
	this->pnlLayersButton->SetRect(0, 0, 152, 32, false);
	this->pnlLayersButton->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->pnlLayer = ui->NewPanel(this->grpLayer);
	this->pnlLayer->SetDockType(UI::GUIControl::DOCK_FILL);

	Math::Size2D<UOSInt> sz;
	this->pbPreview = ui->NewPictureBox(this->pnlStyle, this->eng, false, false);
	this->pbPreview->SetRect(0, 0, 100, 24, false);
	this->pbPreview->SetDockType(UI::GUIControl::DOCK_TOP);
	sz = this->pbPreview->GetSizeP();
	this->prevImage = this->eng->CreateImage32(sz, Media::AT_ALPHA_ALL_FF);
	NN<Media::DrawImage> img;
	if (this->prevImage.SetTo(img))
	{
		img->SetHDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
	}
	this->lblName = ui->NewLabel(this->pnlStyle, CSTR("Name:"));
	this->lblName->SetRect(0, 32, 100, 23, false);
	this->txtName = ui->NewTextBox(this->pnlStyle, CSTR(""));
	this->txtName->SetRect(100, 32, 112, 23, false);

	this->lbLayer = ui->NewListBox(this->pnlLayers, false);
	this->lbLayer->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbLayer->HandleSelectionChange(LayerSelChanged, this);
	this->btnNewLayer = ui->NewButton(this->pnlLayersButton, CSTR("&Add"));
	this->btnNewLayer->SetRect(8, 8, 64, 23, false);
	this->btnNewLayer->HandleButtonClick(NewLayerClicked, this);
	this->btnRemoveLayer = ui->NewButton(this->pnlLayersButton, CSTR("&Remove"));
	this->btnRemoveLayer->SetRect(80, 8, 64, 23, false);
	this->btnRemoveLayer->HandleButtonClick(RemoveLayerClicked, this);

	this->lblColor = ui->NewLabel(this->pnlLayer, CSTR("Color"));
	this->lblColor->SetRect(0, 8, 88, 23, false);
	this->lblAlpha = ui->NewLabel(this->pnlLayer, CSTR("Alpha"));
	this->lblAlpha->SetRect(0, 32, 88, 23, false);
	this->lblThick = ui->NewLabel(this->pnlLayer, CSTR("Thick"));
	this->lblThick->SetRect(0, 56, 88, 23, false);
	this->lblPattern = ui->NewLabel(this->pnlLayer, CSTR("Pattern"));
	this->lblPattern->SetRect(0, 80, 88, 23, false);
	this->pbColor = ui->NewPictureBox(this->pnlLayer, this->eng, true, false);
	this->pbColor->SetRect(96, 8, 72, 24, false);
	this->pbColor->SetBGColor(0);
	this->pbColor->HandleMouseDown(ColorClicked, this);
	this->hsbAlpha = ui->NewHScrollBar(this->pnlLayer, 144);
	this->hsbAlpha->SetPosition(96, 32);
	this->hsbAlpha->InitScrollBar(0, 255, 255, 1);
	this->txtThick = ui->NewTextBox(this->pnlLayer, CSTR("1"));
	this->txtThick->SetRect(96, 56, 40, 23, false);
	this->txtThick->HandleTextChanged(ThickChanged, this);
	this->hsbThick = ui->NewHScrollBar(this->pnlLayer, 120);
	this->hsbThick->SetPosition(136, 56);
	this->hsbThick->InitScrollBar(0, 210, 10, 10);
	this->hsbThick->HandlePosChanged(OnThickScrolled, this);
	this->txtPattern = ui->NewTextBox(this->pnlLayer, CSTR(""));
	this->txtPattern->SetRect(96, 80, 100, 23, false);
	this->txtPattern->HandleTextChanged(PatternChanged, this);

	this->btnOK = ui->NewButton(this->pnlButtons, CSTR("OK"));
	this->btnOK->SetRect(150, 8, 75, 23, false);
	this->btnOK->HandleButtonClick(OKClicked, this);
	this->btnCancel = ui->NewButton(this->pnlButtons, CSTR("Cancel"));
	this->btnCancel->SetRect(231, 8, 75, 23, false);
	this->btnCancel->HandleButtonClick(CancelClicked, this);
	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);

	UOSInt i;
	UOSInt j;
	NN<LineLayer> lyr;
	i = 0;
	j = this->env->GetLineStyleLayerCnt(this->lineStyle);
	while (i < j)
	{
		UInt32 color;
		Double thick;
		UnsafeArrayOpt<UInt8> pattern = 0;
		UnsafeArray<UInt8> nnpattern;
		UnsafeArray<UInt8> lpattern;
		UOSInt npattern;
		this->env->GetLineStyleLayer(this->lineStyle, i, color, thick, pattern, npattern);

		lyr = MemAllocNN(LineLayer);
		lyr->color = color;
		lyr->thick = thick;
		if (pattern.SetTo(nnpattern))
		{
			lyr->pattern = lpattern = MemAllocArr(UInt8, npattern);
			lyr->nPattern = npattern;
			MemCopyNO(lpattern.Ptr(), nnpattern.Ptr(), npattern);
		}
		else
		{
			lyr->pattern = 0;
			lyr->nPattern = 0;
		}
		this->lineLayers.Add(lyr);

		i++;
	}
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	if (this->env->GetLineStyleName(this->lineStyle, sbuff).SetTo(sptr))
	{
		this->txtName->SetText(CSTRP(sbuff, sptr));
	}
	this->LineStyleUpdated();
	if (this->lineLayers.GetCount() > 0)
	{
		this->lbLayer->SetSelectedIndex(0);
	}
	this->UpdatePreview();
}

SSWR::AVIRead::AVIRGISLineEditForm::~AVIRGISLineEditForm()
{
	this->lineLayers.FreeAll(FreeLayer);
	NN<Media::DrawImage> img;
	if (this->prevImage.SetTo(img))
	{
		this->eng->DeleteImage(img);
	}
	this->prevsImage.Delete();
	this->colorSess->RemoveHandler(*this);
	this->ClearChildren();
	this->core->GetDrawEngine()->EndColorSess(this->colorSess);
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRGISLineEditForm::OnMonitorChanged()
{
	this->colorSess->ChangeMonitor(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	Math::Size2D<UOSInt> sz = this->pbPreview->GetSizeP();
	NN<Media::DrawImage> img;
	if (this->prevImage.SetTo(img))
	{
		this->eng->DeleteImage(img);
	}
	this->prevImage = this->eng->CreateImage32(sz, Media::AT_ALPHA_ALL_FF);
	if (this->prevImage.SetTo(img))
	{
		img->SetHDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
	}
	NN<LineLayer> currLayer;
	if (this->currLayer.SetTo(currLayer))
	{
		Media::ColorProfile srcProfile(Media::ColorProfile::CPT_SRGB);
		Media::ColorProfile destProfile(Media::ColorProfile::CPT_PDISPLAY);

		this->pbColor->SetBGColor(Media::ColorConv::ConvARGB(srcProfile, destProfile, this->colorSess.Ptr(), currLayer->color | 0xff000000));
		this->pbColor->Redraw();
		this->UpdatePreview();
	}
}

void SSWR::AVIRead::AVIRGISLineEditForm::YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuvParam)
{
}

void SSWR::AVIRead::AVIRGISLineEditForm::RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgbParam)
{
	NN<LineLayer> currLayer;
	if (this->currLayer.SetTo(currLayer))
	{
		Media::ColorProfile srcProfile(Media::ColorProfile::CPT_SRGB);
		Media::ColorProfile destProfile(Media::ColorProfile::CPT_PDISPLAY);

		this->pbColor->SetBGColor(Media::ColorConv::ConvARGB(srcProfile, destProfile, this->colorSess.Ptr(), currLayer->color | 0xff000000));
		this->pbColor->Redraw();
		this->UpdatePreview();
	}
}
