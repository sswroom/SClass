#include "Stdafx.h"
#include "Map/VectorLayer.h"
#include "Math/Geometry/VectorImage.h"
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

void __stdcall SSWR::AVIRead::AVIRGISPropForm::OnOKClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISPropForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISPropForm>();
	Map::MapEnv::LayerItem setting;
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	if (me->env->GetLayerProp(setting, me->group, me->index))
	{
		setting.labelCol = (UIntOS)me->cboColName->GetSelectedIndex();
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
		setting.minScale = Text::StrToDoubleOr(sbuff, 0);
		me->txtMaxScale->GetText(sbuff);
		setting.maxScale = Text::StrToDoubleOr(sbuff, 0);
		if (me->txtPriority->GetText(sbuff).SetTo(sptr))
			setting.priority = Text::StrToInt32(sbuff);
		else
		{
			sptr = sbuff;
			setting.priority = 0;
		}
		if (setting.minScale == 0 || setting.maxScale == 0 || (setting.priority == 0 && !Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("0"))))
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

void __stdcall SSWR::AVIRead::AVIRGISPropForm::OnCancelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISPropForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISPropForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

UI::EventState __stdcall SSWR::AVIRead::AVIRGISPropForm::OnFillClicked(AnyType userObj, Math::Coord2D<IntOS> scnPos, UI::GUIPictureBox::MouseButton btn)
{
	NN<SSWR::AVIRead::AVIRGISPropForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISPropForm>();
	if (btn == UI::GUIPictureBox::MBTN_LEFT)
	{
		Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
		UtilUI::ColorDialog dlg(nullptr, me->ui, me->core->GetColorManager(), me->core->GetDrawEngine(), UtilUI::ColorDialog::CCT_PHOTO, color, me->core->GetMonitorMgr());
		dlg.ShowAlpha();
		dlg.SetColor32(me->fillStyle);
		if (dlg.ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			me->fillStyle = dlg.GetColor32();
			me->pbFillStyle->SetBGColor(me->colorConv->ConvRGB8(me->fillStyle));
			me->pbFillStyle->Redraw();
		}
	}
	return UI::EventState::ContinueEvent;
}

UI::EventState __stdcall SSWR::AVIRead::AVIRGISPropForm::OnLineDown(AnyType userObj, Math::Coord2D<IntOS> scnPos, UI::GUIPictureBox::MouseButton btn)
{
//	NN<SSWR::AVIRead::AVIRGISPropForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISPropForm>();
	if (btn == UI::GUIPictureBox::MBTN_LEFT)
	{
		OnLineModifyClicked(userObj);
	}
	return UI::EventState::ContinueEvent;
}

void __stdcall SSWR::AVIRead::AVIRGISPropForm::OnLineModifyClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISPropForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISPropForm>();
	SSWR::AVIRead::AVIRGISLineForm frm(nullptr, me->ui, me->core, me->core->GetDrawEngine(), me->lineThick, me->lineColor);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		Math::Size2D<UIntOS> sz;
		me->lineType = 1;
		me->lineThick = frm.GetLineThick();
		me->lineColor = frm.GetLineColor();
		me->imgLine.Delete();
		sz = me->pbLineStyle->GetSizeP();
		NN<Media::DrawImage> dimg;
		if (me->eng->CreateImage32(sz, Media::AT_ALPHA_ALL_FF).SetTo(dimg))
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

void __stdcall SSWR::AVIRead::AVIRGISPropForm::OnLineStyleClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISPropForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISPropForm>();
	SSWR::AVIRead::AVIRGISLineStyleForm frm(nullptr, me->ui, me->core, me->env, me->core->GetDrawEngine(), me->lineStyle);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK || frm.IsChanged())
	{
		Math::Size2D<UIntOS> sz;
		me->lineType = 0;
		me->lineStyle = frm.GetLineStyle();
		me->imgLine.Delete();
		sz = me->pbLineStyle->GetSizeP();
		NN<Media::DrawImage> dimg;
		if (me->eng->CreateImage32(sz, Media::AT_ALPHA_ALL_FF).SetTo(dimg))
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

UI::EventState __stdcall SSWR::AVIRead::AVIRGISPropForm::OnIconClicked(AnyType userObj, Math::Coord2D<IntOS> scnPos, UI::GUIPictureBox::MouseButton btn)
{
	NN<SSWR::AVIRead::AVIRGISPropForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISPropForm>();
	if (btn == UI::GUIPictureBox::MBTN_LEFT)
	{
		SSWR::AVIRead::AVIRGISImageForm frm(nullptr, me->ui, me->core, me->env, me->imgIndex);
		if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			me->imgIndex = frm.GetImgIndex();
			me->pbIcon->SetImage(me->env->GetImage(me->imgIndex, 0));
		}
	}
	return UI::EventState::ContinueEvent;
}

UI::EventState __stdcall SSWR::AVIRead::AVIRGISPropForm::OnFontModifyDown(AnyType userObj, Math::Coord2D<IntOS> scnPos, UI::GUIPictureBox::MouseButton btn)
{
//	NN<SSWR::AVIRead::AVIRGISPropForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISPropForm>();
	if (btn == UI::GUIPictureBox::MBTN_LEFT)
	{
		OnFontModifyClicked(userObj);
	}
	return UI::EventState::ContinueEvent;
}

void __stdcall SSWR::AVIRead::AVIRGISPropForm::OnFontModifyClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISPropForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISPropForm>();
	NN<Text::String> fontName;
	SSWR::AVIRead::AVIRGISFontForm frm(nullptr, me->ui, me->core, me->core->GetDrawEngine(), me->fontName, me->fontSizePt, me->fontColor);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
	{
		Math::Size2D<UIntOS> sz;
		me->fontType = Map::MapEnv::FontType::LayerStyle;
		OPTSTR_DEL(me->fontName);
		me->fontName = fontName = frm.GetFontName()->Clone();
		me->fontSizePt = frm.GetFontSizePt();
		me->fontColor = frm.GetFontColor();
		me->imgFont.Delete();
		sz = me->pbFontStyle->GetSizeP();
		NN<Media::DrawImage> dimg;
		if (me->eng->CreateImage32(sz, Media::AT_ALPHA_ALL_FF).SetTo(dimg))
		{
			dimg->SetHDPI(me->GetHDPI() / me->GetDDPI() * 96.0);
			dimg->SetVDPI(me->GetHDPI() / me->GetDDPI() * 96.0);
			me->core->GenFontPreview(dimg, me->eng, fontName->ToCString(), me->fontSizePt, me->fontColor, me->colorConv);
			me->imgFont = dimg->ToStaticImage();
			me->pbFontStyle->SetImage(me->imgFont);
			me->eng->DeleteImage(dimg);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGISPropForm::OnFontStyleClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISPropForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISPropForm>();
	SSWR::AVIRead::AVIRGISFontStyleForm frm(nullptr, me->ui, me->core, me->env, me->core->GetDrawEngine(), me->fontStyle);
	if (frm.ShowDialog(me) == UI::GUIForm::DR_OK || frm.IsChanged())
	{
		Math::Size2D<UIntOS> sz;
		me->fontStyle = frm.GetFontStyle();
		me->fontType = Map::MapEnv::FontType::GlobalStyle;
		me->imgFont.Delete();
		sz = me->pbFontStyle->GetSizeP();
		NN<Media::DrawImage> dimg;
		if (me->eng->CreateImage32(sz, Media::AT_ALPHA_ALL_FF).SetTo(dimg))
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

SSWR::AVIRead::AVIRGISPropForm::AVIRGISPropForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::MapEnv> env, Optional<Map::MapEnv::GroupItem> group, UIntOS index) : UI::GUIForm(parent, 512, 320, ui)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	NN<UI::GUILabel> lbl;
	this->SetNoResize(true);
	this->env = env;
	this->group = group;
	this->index = index;
	this->core = core;
	this->eng = this->core->GetDrawEngine();
	this->colorSess = this->core->GetColorManager()->CreateSess(this->GetHMonitor());
	Media::ColorProfile srcProfile(Media::ColorProfile::CPT_SRGB);
	Media::ColorProfile destProfile(Media::ColorProfile::CPT_PDISPLAY);
	NEW_CLASSNN(this->colorConv, Media::ColorConv(srcProfile, destProfile, this->colorSess.Ptr()));
	this->colorSess->AddHandler(*this);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->SetText(CSTR("Layer Properties"));
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);
	this->SetSize(512, 320);

	this->pnlCtrl = ui->NewPanel(*this);
	this->pnlCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnOK = ui->NewButton(this->pnlCtrl, CSTR("OK"));
	this->btnOK->SetRect(336, 4, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->btnCancel = ui->NewButton(this->pnlCtrl, CSTR("Cancel"));
	this->btnCancel->SetRect(424, 4, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpLayer = this->tcMain->AddTabPage(CSTR("Layer"));
	lbl = ui->NewLabel(this->tpLayer, CSTR("Min Visible Scale"));
	lbl->SetRect(8, 0, 100, 23, false);
	this->txtMinScale = ui->NewTextBox(this->tpLayer, CSTR(""));
	this->txtMinScale->SetRect(112, 0, 100, 21, false);
	lbl = ui->NewLabel(this->tpLayer, CSTR("Max Visible Scale"));
	lbl->SetRect(8, 24, 100, 23, false);
	this->txtMaxScale = ui->NewTextBox(this->tpLayer, CSTR(""));
	this->txtMaxScale->SetRect(112, 24, 100, 21, false);
	this->chkHide = ui->NewCheckBox(this->tpLayer, CSTR("Hide"), false);
	this->chkHide->SetRect(240, 8, 104, 24, false);
	lbl = ui->NewLabel(this->tpLayer, CSTR("Font Style"));
	lbl->SetRect(8, 48, 100, 23, false);
	this->pbFontStyle = ui->NewPictureBox(this->tpLayer, this->core->GetDrawEngine(), true, false);
	this->pbFontStyle->SetRect(112, 48, 160, 20, false);
	this->btnFontModify = ui->NewButton(this->tpLayer, CSTR("Modify"));
	this->btnFontModify->SetRect(280, 48, 75, 20, false);
	this->btnFontStyle = ui->NewButton(this->tpLayer, CSTR("Style"));
	this->btnFontStyle->SetRect(360, 48, 75, 20, false);
	this->lblLineStyle = ui->NewLabel(this->tpLayer, CSTR("Line Style"));
	this->lblLineStyle->SetRect(8, 72, 100, 23, false);
	this->pbLineStyle = ui->NewPictureBox(this->tpLayer, this->core->GetDrawEngine(), true, false);
	this->pbLineStyle->SetRect(112, 72, 160, 20, false);
	this->btnLineModify = ui->NewButton(this->tpLayer, CSTR("Modify"));
	this->btnLineModify->SetRect(280, 72, 75, 20, false);
	this->btnLineStyle = ui->NewButton(this->tpLayer, CSTR("Style"));
	this->btnLineStyle->SetRect(360, 72, 75, 20, false);
	this->lblFillStyle = ui->NewLabel(this->tpLayer, CSTR("Fill Style"));
	this->lblFillStyle->SetRect(8, 96, 100, 23, false);
	this->pbFillStyle = ui->NewPictureBox(this->tpLayer, this->core->GetDrawEngine(), true, false);
	this->pbFillStyle->SetRect(112, 96, 160, 20, false);
	lbl = ui->NewLabel(this->tpLayer, CSTR("Label Column"));
	lbl->SetRect(8, 120, 100, 23, false);
	this->cboColName = ui->NewComboBox(this->tpLayer, false);
	this->cboColName->SetRect(112, 120, 121, 20, false);
	this->chkShowLabel = ui->NewCheckBox(this->tpLayer, CSTR("Show Label"), false);
	this->chkShowLabel->SetRect(240, 120, 104, 24, false);
	lbl = ui->NewLabel(this->tpLayer, CSTR("Label Priority"));
	lbl->SetRect(8, 144, 100, 23, false);
	this->txtPriority = ui->NewTextBox(this->tpLayer, CSTR(""));
	this->txtPriority->SetRect(112, 144, 100, 21, false);
	this->lblIcon = ui->NewLabel(this->tpLayer, CSTR("Point Icon"));
	this->lblIcon->SetRect(8, 168, 72, 23, false);
	this->pbIcon = ui->NewPictureBox(this->tpLayer, this->core->GetDrawEngine(), true, true);
	this->pbIcon->SetRect(112, 168, 200, 128, false);

	this->grpLabel = ui->NewGroupBox(this->tpLayer, CSTR("Label"));
	this->grpLabel->SetRect(352, 92, 120, 144, false);
	this->chkSmart = ui->NewCheckBox(this->grpLabel, CSTR("Smart"), false);
	this->chkSmart->SetRect(4, 4, 104, 24, false);
	this->chkRotate = ui->NewCheckBox(this->grpLabel, CSTR("Rotate"), false);
	this->chkRotate->SetRect(4, 28, 104, 24, false);
	this->chkAlign = ui->NewCheckBox(this->grpLabel, CSTR("Align"), false);
	this->chkAlign->SetRect(4, 52, 104, 24, false);
	this->chkTrim = ui->NewCheckBox(this->grpLabel, CSTR("Trim"), false);
	this->chkTrim->SetRect(4, 76, 104, 24, false);
	this->chkCapital = ui->NewCheckBox(this->grpLabel, CSTR("Capital"), false);
	this->chkCapital->SetRect(4, 100, 104, 24, false);

	Math::RectAreaDbl bounds;
	if (!env->GetLayerBounds(group, index, bounds))
	{
		bounds = Math::RectAreaDbl(0, 0, 0, 0);
	}
	this->tpCSys = this->tcMain->AddTabPage(CSTR("CSys"));
	this->pnlCSysBounds = ui->NewPanel(this->tpCSys);
	this->pnlCSysBounds->SetRect(0, 0, 100, 51, false);
	this->pnlCSysBounds->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblCSysBounds = ui->NewLabel(this->pnlCSysBounds, CSTR("Bounds"));
	this->lblCSysBounds->SetRect(4, 4, 100, 23, false);
	this->lblCSysMin = ui->NewLabel(this->pnlCSysBounds, CSTR("Min"));
	this->lblCSysMin->SetRect(104, 4, 100, 23, false);
	sptr = Text::StrDouble(sbuff, bounds.min.x);
	this->txtCSysMinX = ui->NewTextBox(this->pnlCSysBounds, CSTRP(sbuff, sptr));
	this->txtCSysMinX->SetRect(204, 4, 100, 23, false);
	this->txtCSysMinX->SetReadOnly(true);
	sptr = Text::StrDouble(sbuff, bounds.min.y);
	this->txtCSysMinY = ui->NewTextBox(this->pnlCSysBounds, CSTRP(sbuff, sptr));
	this->txtCSysMinY->SetRect(304, 4, 100, 23, false);
	this->txtCSysMinY->SetReadOnly(true);
	this->lblCSysMax = ui->NewLabel(this->pnlCSysBounds, CSTR("Max"));
	this->lblCSysMax->SetRect(104, 28, 100, 23, false);
	sptr = Text::StrDouble(sbuff, bounds.max.x);
	this->txtCSysMaxX = ui->NewTextBox(this->pnlCSysBounds, CSTRP(sbuff, sptr));
	this->txtCSysMaxX->SetRect(204, 28, 100, 23, false);
	this->txtCSysMaxX->SetReadOnly(true);
	sptr = Text::StrDouble(sbuff, bounds.max.y);
	this->txtCSysMaxY = ui->NewTextBox(this->pnlCSysBounds, CSTRP(sbuff, sptr));
	this->txtCSysMaxY->SetRect(304, 28, 100, 23, false);
	this->txtCSysMaxY->SetReadOnly(true);
	this->txtCSysCurr = ui->NewTextBox(this->tpCSys, CSTR(""), true);
	this->txtCSysCurr->SetRect(0, 0, 100, 240, false);
	this->txtCSysCurr->SetDockType(UI::GUIControl::DOCK_TOP);
	this->txtCSysCurr->SetReadOnly(true);

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

	this->imgLine = nullptr;
	this->imgFont = nullptr;
	Map::MapEnv::LayerItem setting;
	NN<Map::MapEnv::LayerItem> lyr;
	if (this->env->GetLayerProp(setting, this->group, this->index) && Optional<Map::MapEnv::LayerItem>::ConvertFrom(this->env->GetItem(this->group, this->index)).SetTo(lyr))
	{
		if (lyr->layer->GetObjectClass() == Map::MapDrawLayer::OC_VECTOR_LAYER)
		{
			NN<Map::VectorLayer> vecLyr = NN<Map::VectorLayer>::ConvertFrom(lyr->layer);
			if (vecLyr->GetLayerType() == Map::DRAW_LAYER_IMAGE)
			{
				Data::ArrayListInt64 ids;
				if (vecLyr->GetAllObjectIds(ids, 0) == 1)
				{
					NN<Map::GetObjectSess> sess = vecLyr->BeginGetObject();
					NN<Math::Geometry::Vector2D> vec;
					if (vecLyr->GetNewVectorById(sess, ids.GetItem(0)).SetTo(vec))
					{
						NN<Media::StaticImage> simg;
						if (vec->GetVectorType() == Math::Geometry::Vector2D::VectorType::Image && NN<Math::Geometry::VectorImage>::ConvertFrom(vec)->GetImage(0).SetTo(simg))
						{
							Text::StringBuilderUTF8 sb;
							simg->ToString(sb);
							this->tpImage = this->tcMain->AddTabPage(CSTR("Image"));
							this->txtImage = ui->NewTextBox(this->tpImage, sb.ToCString(), true);
							this->txtImage->SetDockType(UI::GUIControl::DOCK_FILL);
						}
						vec.Delete();
					}
					vecLyr->EndGetObject(sess);
				}
			}
		}

		UIntOS j = lyr->layer->GetColumnCnt();
		UIntOS i = 0;
		while (i < j)
		{
			if (lyr->layer->GetColumnName(sbuff, i).SetTo(sptr))
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

		NN<Media::DrawImage> dimg;
		Math::Size2D<UIntOS> sz = this->pbLineStyle->GetSizeP();
		this->lineType = setting.lineType;
		this->lineStyle = setting.lineStyle;
		this->lineThick = setting.lineThick;
		this->lineColor = setting.lineColor;
		if (this->eng->CreateImage32(sz, Media::AT_ALPHA_ALL_FF).SetTo(dimg))
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
		if (this->eng->CreateImage32(sz, Media::AT_ALPHA_ALL_FF).SetTo(dimg))
		{
			dimg->SetHDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
			dimg->SetVDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
			if (setting.fontType == Map::MapEnv::FontType::GlobalStyle)
			{
				this->core->GenFontStylePreview(dimg, this->eng, this->env, setting.fontStyle, this->colorConv);
			}
			else if (setting.fontType == Map::MapEnv::FontType::LayerStyle)
			{
				this->core->GenFontPreview(dimg, this->eng, Text::String::OrEmpty(setting.fontName)->ToCString(), setting.fontSizePt, setting.fontColor, this->colorConv);
			}
			this->imgFont = dimg->ToStaticImage();
			this->pbFontStyle->SetImage(this->imgFont);
			this->eng->DeleteImage(dimg);
		}

		this->pbFillStyle->SetBGColor(this->colorConv->ConvRGB8(setting.fillStyle));
		this->fillStyle = setting.fillStyle;
		this->fontType = setting.fontType;
		this->fontStyle = setting.fontStyle;
		NN<Text::String> s;
		if (setting.fontName.SetTo(s))
		{
			this->fontName = s->Clone();
		}
		else
		{
			this->fontName = nullptr;
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
	this->imgLine.Delete();
	this->imgFont.Delete();
	OPTSTR_DEL(this->fontName);
	this->colorConv.Delete();
	this->colorSess->RemoveHandler(*this);
	this->ClearChildren();
	this->eng->EndColorSess(this->colorSess);
	this->core->GetColorManager()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRGISPropForm::OnMonitorChanged()
{
	this->colorSess->ChangeMonitor(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRGISPropForm::YUVParamChanged(NN<const Media::ColorHandler::YUVPARAM> yuvParam)
{
}

void SSWR::AVIRead::AVIRGISPropForm::RGBParamChanged(NN<const Media::ColorHandler::RGBPARAM2> rgbParam)
{
	Math::Size2D<UIntOS> sz;
	this->colorConv->RGBParamChanged(rgbParam);
	this->pbFillStyle->SetBGColor(this->colorConv->ConvRGB8(this->fillStyle));
	this->pbFillStyle->Redraw();

	this->imgLine.Delete();
	sz = this->pbLineStyle->GetSizeP();
	NN<Media::DrawImage> dimg;
	if (this->eng->CreateImage32(sz, Media::AT_ALPHA_ALL_FF).SetTo(dimg))
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

	this->imgFont.Delete();
	sz = this->pbFontStyle->GetSizeP();
	if (this->eng->CreateImage32(sz, Media::AT_ALPHA_ALL_FF).SetTo(dimg))
	{
		dimg->SetHDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
		dimg->SetVDPI(this->GetHDPI() / this->GetDDPI() * 96.0);
		if (this->fontType == Map::MapEnv::FontType::GlobalStyle)
		{
			this->core->GenFontStylePreview(dimg, this->eng, this->env, this->fontStyle, this->colorConv);
		}
		else if (this->fontType == Map::MapEnv::FontType::LayerStyle)
		{
			this->core->GenFontPreview(dimg, this->eng, Text::String::OrEmpty(this->fontName)->ToCString(), this->fontSizePt, this->fontColor, this->colorConv);
		}
		this->imgFont = dimg->ToStaticImage();
		this->pbFontStyle->SetImage(this->imgFont);
		this->eng->DeleteImage(dimg);
	}
}
