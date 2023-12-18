#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRGISFontForm.h"
#include "SSWR/AVIRead/AVIRGISFontStyleForm.h"
#include "SSWR/AVIRead/AVIRGISImageForm.h"
#include "SSWR/AVIRead/AVIRGISLineForm.h"
#include "SSWR/AVIRead/AVIRGISLineStyleForm.h"
#include "SSWR/AVIRead/AVIRGISPropForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringUtil.h"
#include "UtilUI/ColorDialog.h"

void __stdcall SSWR::AVIRead::AVIRGISPropForm::OnOKClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISPropForm *me = (SSWR::AVIRead::AVIRGISPropForm*)userObj;
	Map::MapEnv::LayerItem setting;
	UTF8Char sbuff[16];
	UTF8Char *sptr;
	if (me->env->GetLayerProp(setting, me->group, me->index))
	{
		setting.labelCol = (UOSInt)me->cboColName->GetSelectedIndex();
		setting.flags = Map::MapEnv::SFLG_NONE;
		if (me->chkShowLabel->IsChecked())
		{
			setting.flags |= Map::MapEnv::SFLG_SHOWLABEL;
		}
		if (me->chkSmart->IsChecked())
		{
			setting.flags |= Map::MapEnv::SFLG_SMART;
		}
		if (me->chkRotate->IsChecked())
		{
			setting.flags |= Map::MapEnv::SFLG_ROTATE;
		}
		if (me->chkAlign->IsChecked())
		{
			setting.flags |= Map::MapEnv::SFLG_ALIGN;
		}
		if (me->chkTrim->IsChecked())
		{
			setting.flags |= Map::MapEnv::SFLG_TRIM;
		}
		if (me->chkCapital->IsChecked())
		{
			setting.flags |= Map::MapEnv::SFLG_CAPITAL;
		}
		if (me->chkHide->IsChecked())
		{
			setting.flags |= Map::MapEnv::SFLG_HIDESHAPE;
		}
		me->txtMinScale->GetText(sbuff);
		setting.minScale = Text::StrToDouble(sbuff);
		me->txtMaxScale->GetText(sbuff);
		setting.maxScale = Text::StrToDouble(sbuff);
		sptr = me->txtPriority->GetText(sbuff);
		setting.priority = Text::StrToInt32(sbuff);
		if (setting.minScale == 0 || setting.maxScale == 0 || (setting.priority == 0 && !Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("0"))))
		{
			me->ui->ShowMsgOK(CSTR("Input value invalid"), CSTR("Properties"), me);
			return;
		}
		setting.lineType = me->lineType;
		setting.lineStyle = me->lineStyle;
		setting.lineThick = me->lineThick;
		setting.lineColor = me->lineColor;
		setting.fillStyle = me->fillStyle;
		setting.fontType = me->fontType;
		setting.fontStyle = me->fontStyle;
		setting.fontName = me->fontName;
		setting.fontSizePt = me->fontSizePt;
		setting.fontColor = me->fontColor;
		setting.imgIndex = me->imgIndex;
		if (me->env->SetLayerProp(setting, me->group, me->index))
		{
			me->SetDialogResult(UI::GUIForm::DR_OK);
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("Error in storing properties"), CSTR("Properties"), me);
		}
	}	
}

void __stdcall SSWR::AVIRead::AVIRGISPropForm::OnCancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISPropForm *me = (SSWR::AVIRead::AVIRGISPropForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

Bool __stdcall SSWR::AVIRead::AVIRGISPropForm::OnFillClicked(void *userObj, Math::Coord2D<OSInt> scnPos, UI::GUIPictureBox::MouseButton btn)
{
	SSWR::AVIRead::AVIRGISPropForm *me = (SSWR::AVIRead::AVIRGISPropForm*)userObj;
	if (btn == UI::GUIPictureBox::MBTN_LEFT)
	{
		Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
		UtilUI::ColorDialog dlg(0, me->ui, me->core->GetColorMgr(), me->core->GetDrawEngine(), UtilUI::ColorDialog::CCT_PHOTO, color, me->core->GetMonitorMgr());
		dlg.ShowAlpha();
		dlg.SetColor32(me->fillStyle);
		if (dlg.ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			me->fillStyle = dlg.GetColor32();
			me->pbFillStyle->SetBGColor(me->colorConv->ConvRGB8(me->fillStyle));
			me->pbFillStyle->Redraw();
		}
	}
	return false;
}

Bool __stdcall SSWR::AVIRead::AVIRGISPropForm::OnLineDown(void *userObj, Math::Coord2D<OSInt> scnPos, UI::GUIPictureBox::MouseButton btn)
{
//	SSWR::AVIRead::AVIRGISPropForm *me = (SSWR::AVIRead::AVIRGISPropForm*)userObj;
	if (btn == UI::GUIPictureBox::MBTN_LEFT)
	{
		OnLineModifyClicked(userObj);
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRGISPropForm::OnLineModifyClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISPropForm *me = (SSWR::AVIRead::AVIRGISPropForm*)userObj;
	SSWR::AVIRead::AVIRGISLineForm frm(0, me->ui, me->core, me->core->GetDrawEngine(), me->lineThick, me->lineColor);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		Math::Size2D<UOSInt> sz;
		me->lineType = 1;
		me->lineThick = frm.GetLineThick();
		me->lineColor = frm.GetLineColor();

		if (me->imgLine)
		{
			DEL_CLASS(me->imgLine);
		}
		sz = me->pbLineStyle->GetSizeP();
		NotNullPtr<Media::DrawImage> dimg;
		if (dimg.Set(me->eng->CreateImage32(sz, Media::AT_NO_ALPHA)))
		{
			dimg->SetHDPI(me->GetHDPI() / me->GetDDPI() * 96.0);
			dimg->SetVDPI(me->GetHDPI() / me->GetDDPI() * 96.0);
			me->core->GenLinePreview(dimg, me->eng, me->lineThick, me->lineColor, me->colorConv);
			me->imgLine = dimg->ToStaticImage();
			me->pbLineStyle->SetImage(me->imgLine);
			me->eng->DeleteImage(dimg);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGISPropForm::OnLineStyleClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISPropForm *me = (SSWR::AVIRead::AVIRGISPropForm*)userObj;
	SSWR::AVIRead::AVIRGISLineStyleForm frm(0, me->ui, me->core, me->env, me->core->GetDrawEngine(), me->lineStyle);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK || frm.IsChanged())
	{
		Math::Size2D<UOSInt> sz;
		me->lineType = 0;
		me->lineStyle = frm.GetLineStyle();

		if (me->imgLine)
		{
			DEL_CLASS(me->imgLine);
		}
		sz = me->pbLineStyle->GetSizeP();
		NotNullPtr<Media::DrawImage> dimg;
		if (dimg.Set(me->eng->CreateImage32(sz, Media::AT_NO_ALPHA)))
		{
			dimg->SetHDPI(me->GetHDPI() / me->GetDDPI() * 96.0);
			dimg->SetVDPI(me->GetHDPI() / me->GetDDPI() * 96.0);
			me->core->GenLineStylePreview(dimg, me->eng, me->env, me->lineStyle, me->colorConv);
			me->imgLine = dimg->ToStaticImage();
			me->pbLineStyle->SetImage(me->imgLine);
			me->eng->DeleteImage(dimg);
		}
	}
}

Bool __stdcall SSWR::AVIRead::AVIRGISPropForm::OnIconClicked(void *userObj, Math::Coord2D<OSInt> scnPos, UI::GUIPictureBox::MouseButton btn)
{
	SSWR::AVIRead::AVIRGISPropForm *me = (SSWR::AVIRead::AVIRGISPropForm*)userObj;
	if (btn == UI::GUIPictureBox::MBTN_LEFT)
	{
		SSWR::AVIRead::AVIRGISImageForm frm(0, me->ui, me->core, me->env, me->imgIndex);
		if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			me->imgIndex = frm.GetImgIndex();
			me->pbIcon->SetImage(me->env->GetImage(me->imgIndex, 0));
		}
	}
	return false;
}

Bool __stdcall SSWR::AVIRead::AVIRGISPropForm::OnFontModifyDown(void *userObj, Math::Coord2D<OSInt> scnPos, UI::GUIPictureBox::MouseButton btn)
{
//	SSWR::AVIRead::AVIRGISPropForm *me = (SSWR::AVIRead::AVIRGISPropForm*)userObj;
	if (btn == UI::GUIPictureBox::MBTN_LEFT)
	{
		OnFontModifyClicked(userObj);
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRGISPropForm::OnFontModifyClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISPropForm *me = (SSWR::AVIRead::AVIRGISPropForm*)userObj;
	SSWR::AVIRead::AVIRGISFontForm frm(0, me->ui, me->core, me->core->GetDrawEngine(), me->fontName, me->fontSizePt, me->fontColor);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		Math::Size2D<UOSInt> sz;
		me->fontType = Map::MapEnv::FontType::LayerStyle;
		SDEL_STRING(me->fontName);
		me->fontName = frm.GetFontName()->Clone().Ptr();
		me->fontSizePt = frm.GetFontSizePt();
		me->fontColor = frm.GetFontColor();

		if (me->imgFont)
		{
			DEL_CLASS(me->imgFont);
		}
		sz = me->pbFontStyle->GetSizeP();
		NotNullPtr<Media::DrawImage> dimg;
		if (dimg.Set(me->eng->CreateImage32(sz, Media::AT_NO_ALPHA)))
		{
			dimg->SetHDPI(me->GetHDPI() / me->GetDDPI() * 96.0);
			dimg->SetVDPI(me->GetHDPI() / me->GetDDPI() * 96.0);
			me->core->GenFontPreview(dimg, me->eng, me->fontName->ToCString(), me->fontSizePt, me->fontColor, me->colorConv);
			me->imgFont = dimg->ToStaticImage();
			me->pbFontStyle->SetImage(me->imgFont);
			me->eng->DeleteImage(dimg);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGISPropForm::OnFontStyleClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISPropForm *me = (SSWR::AVIRead::AVIRGISPropForm*)userObj;
	SSWR::AVIRead::AVIRGISFontStyleForm frm(0, me->ui, me->core, me->env, me->core->GetDrawEngine(), me->fontStyle);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK || frm.IsChanged())
	{
		Math::Size2D<UOSInt> sz;
		me->fontStyle = frm.GetFontStyle();
		me->fontType = Map::MapEnv::FontType::GlobalStyle;

		if (me->imgFont)
		{
			DEL_CLASS(me->imgFont);
		}
		sz = me->pbFontStyle->GetSizeP();
		NotNullPtr<Media::DrawImage> dimg;
		if (dimg.Set(me->eng->CreateImage32(sz, Media::AT_NO_ALPHA)))
		{
			dimg->SetHDPI(me->GetHDPI() / me->GetDDPI() * 96.0);
			dimg->SetVDPI(me->GetHDPI() / me->GetDDPI() * 96.0);
			me->core->GenFontStylePreview(dimg, me->eng, me->env, me->fontStyle, me->colorConv);
			me->imgFont = dimg->ToStaticImage();
			me->pbFontStyle->SetImage(me->imgFont);
			me->eng->DeleteImage(dimg);
		}
	}
}

SSWR::AVIRead::AVIRGISPropForm::AVIRGISPropForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Map::MapEnv> env, Map::MapEnv::GroupItem *group, UOSInt index) : UI::GUIForm(parent, 512, 320, ui)
{
	UI::GUILabel *lbl;
	this->SetNoResize(true);
	this->env = env;
	this->group = group;
	this->index = index;
	this->core = core;
	this->eng = this->core->GetDrawEngine();
	this->colorSess = this->core->GetColorMgr()->CreateSess(this->GetHMonitor());
	Media::ColorProfile srcProfile(Media::ColorProfile::CPT_SRGB);
	Media::ColorProfile destProfile(Media::ColorProfile::CPT_PDISPLAY);
	NEW_CLASS(this->colorConv, Media::ColorConv(srcProfile, destProfile, this->colorSess.Ptr()));
	this->colorSess->AddHandler(*this);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->SetText(CSTR("Layer Properties"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->SetSize(512, 320);


	NEW_CLASS(lbl, UI::GUILabel(ui, *this, CSTR("Min Visible Scale")));
	lbl->SetRect(8, 0, 100, 23, false);
	NEW_CLASS(this->txtMinScale, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtMinScale->SetRect(112, 0, 100, 21, false);
	NEW_CLASS(lbl, UI::GUILabel(ui, *this, CSTR("Max Visible Scale")));
	lbl->SetRect(8, 24, 100, 23, false);
	NEW_CLASS(this->txtMaxScale, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtMaxScale->SetRect(112, 24, 100, 21, false);
	NEW_CLASS(this->chkHide, UI::GUICheckBox(ui, *this, CSTR("Hide"), false));
	this->chkHide->SetRect(240, 8, 104, 24, false);
	NEW_CLASS(lbl, UI::GUILabel(ui, *this, CSTR("Font Style")));
	lbl->SetRect(8, 48, 100, 23, false);
	NEW_CLASS(this->pbFontStyle, UI::GUIPictureBox(ui, *this, this->core->GetDrawEngine(), true, false));
	this->pbFontStyle->SetRect(112, 48, 160, 20, false);
	this->btnFontModify = ui->NewButton(*this, CSTR("Modify"));
	this->btnFontModify->SetRect(280, 48, 75, 20, false);
	this->btnFontStyle = ui->NewButton(*this, CSTR("Style"));
	this->btnFontStyle->SetRect(360, 48, 75, 20, false);
	NEW_CLASS(this->lblLineStyle, UI::GUILabel(ui, *this, CSTR("Line Style")));
	this->lblLineStyle->SetRect(8, 72, 100, 23, false);
	NEW_CLASS(this->pbLineStyle, UI::GUIPictureBox(ui, *this, this->core->GetDrawEngine(), true, false));
	this->pbLineStyle->SetRect(112, 72, 160, 20, false);
	this->btnLineModify = ui->NewButton(*this, CSTR("Modify"));
	this->btnLineModify->SetRect(280, 72, 75, 20, false);
	this->btnLineStyle = ui->NewButton(*this, CSTR("Style"));
	this->btnLineStyle->SetRect(360, 72, 75, 20, false);
	NEW_CLASS(this->lblFillStyle, UI::GUILabel(ui, *this, CSTR("Fill Style")));
	this->lblFillStyle->SetRect(8, 96, 100, 23, false);
	NEW_CLASS(this->pbFillStyle, UI::GUIPictureBox(ui, *this, this->core->GetDrawEngine(), true, false));
	this->pbFillStyle->SetRect(112, 96, 160, 20, false);
	NEW_CLASS(lbl, UI::GUILabel(ui, *this, CSTR("Label Column")));
	lbl->SetRect(8, 120, 100, 23, false);
	NEW_CLASS(this->cboColName, UI::GUIComboBox(ui, *this, false));
	this->cboColName->SetRect(112, 120, 121, 20, false);
	NEW_CLASS(this->chkShowLabel, UI::GUICheckBox(ui, *this, CSTR("Show Label"), false));
	this->chkShowLabel->SetRect(240, 120, 104, 24, false);
	NEW_CLASS(lbl, UI::GUILabel(ui, *this, CSTR("Label Priority")));
	lbl->SetRect(8, 144, 100, 23, false);
	NEW_CLASS(this->txtPriority, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtPriority->SetRect(112, 144, 100, 21, false);
	NEW_CLASS(this->lblIcon, UI::GUILabel(ui, *this, CSTR("Point Icon")));
	this->lblIcon->SetRect(8, 168, 72, 23, false);
	NEW_CLASS(this->pbIcon, UI::GUIPictureBox(ui, *this, this->core->GetDrawEngine(), true, true));
	this->pbIcon->SetRect(112, 168, 200, 128, false);

	this->grpLabel = ui->NewGroupBox(*this, CSTR("Label"));
	this->grpLabel->SetRect(352, 92, 120, 144, false);
	NEW_CLASS(this->chkSmart, UI::GUICheckBox(ui, this->grpLabel, CSTR("Smart"), false));
	this->chkSmart->SetRect(4, 4, 104, 24, false);
	NEW_CLASS(this->chkRotate, UI::GUICheckBox(ui, this->grpLabel, CSTR("Rotate"), false));
	this->chkRotate->SetRect(4, 28, 104, 24, false);
	NEW_CLASS(this->chkAlign, UI::GUICheckBox(ui, this->grpLabel, CSTR("Align"), false));
	this->chkAlign->SetRect(4, 52, 104, 24, false);
	NEW_CLASS(this->chkTrim, UI::GUICheckBox(ui, this->grpLabel, CSTR("Trim"), false));
	this->chkTrim->SetRect(4, 76, 104, 24, false);
	NEW_CLASS(this->chkCapital, UI::GUICheckBox(ui, this->grpLabel, CSTR("Capital"), false));
	this->chkCapital->SetRect(4, 100, 104, 24, false);
	this->btnOK = ui->NewButton(*this, CSTR("OK"));
	this->btnOK->SetRect(336, 244, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->btnCancel = ui->NewButton(*this, CSTR("Cancel"));
	this->btnCancel->SetRect(424, 244, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);

	this->pbFillStyle->HandleMouseDown(OnFillClicked, this);
	this->pbFontStyle->HandleMouseDown(OnFontModifyDown, this);
	this->btnFontModify->HandleButtonClick(OnFontModifyClicked, this);
	this->btnFontStyle->HandleButtonClick(OnFontStyleClicked, this);
	this->pbIcon->HandleMouseDown(OnIconClicked, this);
	this->pbLineStyle->HandleMouseDown(OnLineDown, this);
	this->btnLineModify->HandleButtonClick(OnLineModifyClicked, this);
	this->btnLineStyle->HandleButtonClick(OnLineStyleClicked, this);
	this->SetCancelButton(this->btnCancel);
	this->SetDefaultButton(this->btnOK);

	this->imgLine = 0;
	this->imgFont = 0;
	Map::MapEnv::LayerItem setting;
	Map::MapEnv::LayerItem *lyr;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	if (this->env->GetLayerProp(setting, this->group, this->index))
	{
		lyr = (Map::MapEnv::LayerItem*)this->env->GetItem(this->group, this->index);
		UOSInt j = lyr->layer->GetColumnCnt();
		UOSInt i = 0;
		while (i < j)
		{
			if ((sptr = lyr->layer->GetColumnName(sbuff, i)) != 0)
			{
				this->cboColName->AddItem(CSTRP(sbuff, sptr), 0);
			}
			else
			{
				this->cboColName->AddItem(CSTR("Unknown"), 0);
			}
			i++;
		}

		sptr = Text::StrDouble(sbuff, setting.minScale);
		this->txtMinScale->SetText(CSTRP(sbuff, sptr));
		sptr =Text::StrDouble(sbuff, setting.maxScale);
		this->txtMaxScale->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrInt32(sbuff, setting.priority);
		this->txtPriority->SetText(CSTRP(sbuff, sptr));

		NotNullPtr<Media::DrawImage> dimg;
		Math::Size2D<UOSInt> sz = this->pbLineStyle->GetSizeP();
		this->lineType = setting.lineType;
		this->lineStyle = setting.lineStyle;
		this->lineThick = setting.lineThick;
		this->lineColor = setting.lineColor;
		if (dimg.Set(this->eng->CreateImage32(sz, Media::AT_NO_ALPHA)))
		{
			dimg->SetHDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
			dimg->SetVDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
			if (this->lineType == 0)
			{
				this->core->GenLineStylePreview(dimg, this->eng, this->env, this->lineStyle, this->colorConv);
			}
			else
			{
				this->core->GenLinePreview(dimg, this->eng, this->lineThick, this->lineColor, this->colorConv);
			}
			this->imgLine = dimg->ToStaticImage();
			this->pbLineStyle->SetImage(this->imgLine);
			this->eng->DeleteImage(dimg);
		}

		sz = this->pbFontStyle->GetSizeP();
		if (dimg.Set(this->eng->CreateImage32(sz, Media::AT_NO_ALPHA)))
		{
			dimg->SetHDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
			dimg->SetVDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
			if (setting.fontType == Map::MapEnv::FontType::GlobalStyle)
			{
				this->core->GenFontStylePreview(dimg, this->eng, this->env, setting.fontStyle, this->colorConv);
			}
			else if (setting.fontType == Map::MapEnv::FontType::LayerStyle)
			{
				this->core->GenFontPreview(dimg, this->eng, setting.fontName->ToCString(), setting.fontSizePt, setting.fontColor, this->colorConv);
			}
			this->imgFont = dimg->ToStaticImage();
			this->pbFontStyle->SetImage(this->imgFont);
			this->eng->DeleteImage(dimg);
		}

		this->pbFillStyle->SetBGColor(this->colorConv->ConvRGB8(setting.fillStyle));
		this->fillStyle = setting.fillStyle;
		this->fontType = setting.fontType;
		this->fontStyle = setting.fontStyle;
		if (setting.fontName)
		{
			this->fontName = setting.fontName->Clone().Ptr();
		}
		else
		{
			this->fontName = 0;
		}
		this->fontSizePt = setting.fontSizePt;
		this->fontColor = setting.fontColor;
		this->imgIndex = setting.imgIndex;
		Map::DrawLayerType layerType = setting.layer->GetLayerType();
		if (layerType == Map::DRAW_LAYER_POINT || layerType == Map::DRAW_LAYER_POINT3D)
		{
			UInt32 imgDurMS;
			this->pbLineStyle->SetVisible(false);
			this->lblLineStyle->SetEnabled(false);
			this->pbFillStyle->SetVisible(false);
			this->lblFillStyle->SetEnabled(false);
			if (this->imgIndex < this->env->GetImageCnt())
			{
				this->pbIcon->SetImage(this->env->GetImage(this->imgIndex, imgDurMS));
			}
		}
		else if (layerType == Map::DRAW_LAYER_POLYLINE || layerType == Map::DRAW_LAYER_POLYLINE3D)
		{
			this->pbFillStyle->SetVisible(false);
			this->lblFillStyle->SetEnabled(false);
			this->pbIcon->SetVisible(false);
			this->lblIcon->SetEnabled(false);
		}
		else if (layerType == Map::DRAW_LAYER_POLYGON)
		{
			this->pbIcon->SetVisible(false);
			this->lblIcon->SetEnabled(false);
		}
		else if (layerType == Map::DRAW_LAYER_IMAGE)
		{
			this->pbLineStyle->SetVisible(false);
			this->lblLineStyle->SetEnabled(false);
			this->pbFillStyle->SetVisible(false);
			this->lblFillStyle->SetEnabled(false);
			this->pbIcon->SetVisible(false);
			this->lblIcon->SetEnabled(false);
			this->cboColName->SetEnabled(false);
		}
		if (setting.labelCol < j)
		{
			this->cboColName->SetSelectedIndex(setting.labelCol);
		}
		if (setting.flags & Map::MapEnv::SFLG_SHOWLABEL)
		{
			this->chkShowLabel->SetChecked(true);
		}
		if (setting.flags & Map::MapEnv::SFLG_SMART)
		{
			this->chkSmart->SetChecked(true);
		}
		if (setting.flags & Map::MapEnv::SFLG_ROTATE)
		{
			this->chkRotate->SetChecked(true);
		}
		if (setting.flags & Map::MapEnv::SFLG_ALIGN)
		{
			this->chkAlign->SetChecked(true);
		}
		if (setting.flags & Map::MapEnv::SFLG_TRIM)
		{
			this->chkTrim->SetChecked(true);
		}
		if (setting.flags & Map::MapEnv::SFLG_CAPITAL)
		{
			this->chkCapital->SetChecked(true);
		}
		if (setting.flags & Map::MapEnv::SFLG_HIDESHAPE)
		{
			this->chkHide->SetChecked(true);
		}
	}
	else
	{
		this->ui->ShowMsgOK(CSTR("Error in getting layer properties"), CSTR("Properties"), this);
		this->SetDialogResult(UI::GUIForm::DR_CANCEL);
	}
}

SSWR::AVIRead::AVIRGISPropForm::~AVIRGISPropForm()
{
	if (this->imgLine)
	{
		DEL_CLASS(this->imgLine);
		this->imgLine = 0;
	}
	if (this->imgFont)
	{
		DEL_CLASS(this->imgFont);
		this->imgFont = 0;
	}
	SDEL_STRING(this->fontName);
	DEL_CLASS(this->colorConv);
	this->colorSess->RemoveHandler(*this);
	this->ClearChildren();
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRGISPropForm::OnMonitorChanged()
{
	this->colorSess->ChangeMonitor(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRGISPropForm::YUVParamChanged(NotNullPtr<const Media::IColorHandler::YUVPARAM> yuvParam)
{
}

void SSWR::AVIRead::AVIRGISPropForm::RGBParamChanged(NotNullPtr<const Media::IColorHandler::RGBPARAM2> rgbParam)
{
	Math::Size2D<UOSInt> sz;
	this->colorConv->RGBParamChanged(rgbParam);
	this->pbFillStyle->SetBGColor(this->colorConv->ConvRGB8(this->fillStyle));
	this->pbFillStyle->Redraw();

	if (this->imgLine)
	{
		DEL_CLASS(this->imgLine);
	}
	sz = this->pbLineStyle->GetSizeP();
	NotNullPtr<Media::DrawImage> dimg;
	if (dimg.Set(this->eng->CreateImage32(sz, Media::AT_NO_ALPHA)))
	{
		dimg->SetHDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
		dimg->SetVDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
		if (this->lineType == 0)
		{
			this->core->GenLineStylePreview(dimg, this->eng, this->env, this->lineStyle, this->colorConv);
		}
		else
		{
			this->core->GenLinePreview(dimg, this->eng, this->lineThick, this->lineColor, this->colorConv);
		}
		this->imgLine = dimg->ToStaticImage();
		this->pbLineStyle->SetImage(this->imgLine);
		this->eng->DeleteImage(dimg);
	}

	if (this->imgFont)
	{
		DEL_CLASS(this->imgFont);
	}
	sz = this->pbFontStyle->GetSizeP();
	if (dimg.Set(this->eng->CreateImage32(sz, Media::AT_NO_ALPHA)))
	{
		dimg->SetHDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
		dimg->SetVDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
		if (this->fontType == Map::MapEnv::FontType::GlobalStyle)
		{
			this->core->GenFontStylePreview(dimg, this->eng, this->env, this->fontStyle, this->colorConv);
		}
		else if (this->fontType == Map::MapEnv::FontType::LayerStyle)
		{
			this->core->GenFontPreview(dimg, this->eng, this->fontName->ToCString(), this->fontSizePt, this->fontColor, this->colorConv);
		}
		this->imgFont = dimg->ToStaticImage();
		this->pbFontStyle->SetImage(this->imgFont);
		this->eng->DeleteImage(dimg);
	}
}
