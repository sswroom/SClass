#include "Stdafx.h"
#include "Map/DrawMapRenderer.h"
#include "Media/ImageList.h"
#include "SSWR/AVIRead/AVIRGISExportImageForm.h"

void __stdcall SSWR::AVIRead::AVIRGISExportImageForm::OnExportClicked(AnyType userObj)
{
	NN<AVIRGISExportImageForm> me = userObj.GetNN<AVIRGISExportImageForm>();
	Text::StringBuilderUTF8 sb;
	Double x;
	Double y;
	Double scale;
	Double w;
	Double h;
	Double dpi;
	me->txtCenterX->GetText(sb);
	if (!sb.ToDouble(x))
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid X"), CSTR("Export Image"), me);
		return;
	}
	sb.ClearStr();
	me->txtCenterY->GetText(sb);
	if (!sb.ToDouble(y))
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid Y"), CSTR("Export Image"), me);
		return;
	}
	sb.ClearStr();
	me->txtScale->GetText(sb);
	if (!sb.ToDouble(scale) || scale <= 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid scale"), CSTR("Export Image"), me);
		return;
	}
	sb.ClearStr();
	me->txtDimensionW->GetText(sb);
	w = sb.ToDoubleOrNAN();
	sb.ClearStr();
	me->txtDimensionH->GetText(sb);
	h = sb.ToDoubleOrNAN();
	sb.ClearStr();
	me->txtDPI->GetText(sb);
	dpi = sb.ToDoubleOrNAN();
	if (w <= 0 || h <= 0 || dpi <= 0 || Math::IsNAN(w) || Math::IsNAN(h) || Math::IsNAN(dpi))
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid dimension and DPI"), CSTR("Export Image"), me);
		return;
	}
	Math::Unit::Distance::DistanceUnit unit = (Math::Unit::Distance::DistanceUnit)me->cboDimensionUnit->GetSelectedItem().GetIntOS();
	Double pxW;
	Double pxH;
	if (unit == Math::Unit::Distance::DU_PIXEL)
	{
		pxW = w;
		pxH = h;
	}
	else
	{
		pxW = Math::Unit::Distance::Convert(unit, Math::Unit::Distance::DU_PIXEL, w) * dpi / 96.0;
		pxH = Math::Unit::Distance::Convert(unit, Math::Unit::Distance::DU_PIXEL, h) * dpi / 96.0;
	}
	NN<Media::DrawEngine> deng = me->core->GetDrawEngine();
	NN<Media::DrawImage> dimg;
	if (!deng->CreateImage32(Math::Size2D<UIntOS>((UIntOS)Double2IntOS(pxW), (UIntOS)Double2IntOS(pxH)), Media::AlphaType::AT_IGNORE_ALPHA).SetTo(dimg))
	{
		me->ui->ShowMsgOK(CSTR("Error in creating image"), CSTR("Export Image"), me);
		return;
	}
	dimg->SetHDPI(dpi);
	dimg->SetVDPI(dpi);
	Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
	Map::DrawMapRenderer renderer(deng, me->env, color, nullptr, Map::DrawMapRenderer::DrawType::DT_PIXELDRAW);
	NN<Map::MapView> view = me->env->CreateMapView(Math::Size2DDbl(pxW, pxH));
	view->SetDPI(dpi, 96.0);
	view->SetCenterXY(Math::Coord2DDbl(x, y));
	view->SetMapScale(scale);
	if (renderer.DrawMap(dimg, view, nullptr))
	{
		me->env->WaitForLoad(60000);
		renderer.DrawMap(dimg, view, nullptr);
	}
	view.Delete();
	NN<Media::StaticImage> simg;
	if (!dimg->ToStaticImage().SetTo(simg))
	{
		deng->DeleteImage(dimg);
		me->ui->ShowMsgOK(CSTR("Error in converting image"), CSTR("Export Image"), me);
		return;
	}
	deng->DeleteImage(dimg);
	NN<Media::ImageList> imgList;
	NEW_CLASSNN(imgList, Media::ImageList(CSTR("MapImage")));
	imgList->AddImage(simg, 0);
	me->core->OpenObject(imgList);
	me->Close();
}

SSWR::AVIRead::AVIRGISExportImageForm::AVIRGISExportImageForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::MapEnv> env, Math::Coord2DDbl center, Double scale) : UI::GUIForm(parent, 480, 200, ui)
{
	Text::StringBuilderUTF8 sb;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->env = env;
	this->center = center;
	this->scale = scale;

	this->SetText(CSTR("Export Image"));
	this->SetFont(nullptr, 8.25, false);
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;

	this->lblCenterX = ui->NewLabel(*this, CSTR("Center X"));
	this->lblCenterX->SetRect(4, 4, 100, 23, false);
	sptr = Text::StrDouble(sbuff, center.x);
	this->txtCenterX = ui->NewTextBox(*this, CSTRP(sbuff, sptr));
	this->txtCenterX->SetRect(104, 4, 150, 23, false);
	this->lblCenterY = ui->NewLabel(*this, CSTR("Center Y"));
	this->lblCenterY->SetRect(4, 28, 100, 23, false);
	sptr = Text::StrDouble(sbuff, center.y);
	this->txtCenterY = ui->NewTextBox(*this, CSTRP(sbuff, sptr));
	this->txtCenterY->SetRect(104, 28, 150, 23, false);
	this->lblScale = ui->NewLabel(*this, CSTR("Scale"));
	this->lblScale->SetRect(4, 52, 100, 23, false);
	sptr = Text::StrDouble(sbuff, scale);
	this->txtScale = ui->NewTextBox(*this, CSTRP(sbuff, sptr));
	this->txtScale->SetRect(104, 52, 150, 23, false);
	this->lblDimension = ui->NewLabel(*this, CSTR("Dimension"));
	this->lblDimension->SetRect(4, 76, 100, 23, false);
	this->txtDimensionW = ui->NewTextBox(*this, CSTR("1024"));
	this->txtDimensionW->SetRect(104, 76, 100, 23, false);
	this->txtDimensionH = ui->NewTextBox(*this, CSTR("768"));
	this->txtDimensionH->SetRect(204, 76, 100, 23, false);
	this->cboDimensionUnit = ui->NewComboBox(*this, false);
	this->cboDimensionUnit->SetRect(304, 76, 100, 23, false);
	Math::Unit::Distance::DistanceUnit du = Math::Unit::Distance::DU_FIRST;
	while (du <= Math::Unit::Distance::DU_LAST)
	{
		this->cboDimensionUnit->AddItem(Math::Unit::Distance::GetUnitName(du), (void*)(IntOS)du);
		du = (Math::Unit::Distance::DistanceUnit)((IntOS)du + 1);
	}
	this->cboDimensionUnit->SetSelectedIndex(Math::Unit::Distance::DistanceUnit::DU_PIXEL);
	this->lblDPI = ui->NewLabel(*this, CSTR("DPI"));
	this->lblDPI->SetRect(4, 100, 100, 23, false);
	this->txtDPI = ui->NewTextBox(*this, CSTR("96"));
	this->txtDPI->SetRect(104, 100, 150, 23, false);
	this->btnExport = ui->NewButton(*this, CSTR("Export"));
	this->btnExport->SetRect(4, 124, 100, 23, false);
	this->btnExport->HandleButtonClick(OnExportClicked, this);
}

SSWR::AVIRead::AVIRGISExportImageForm::~AVIRGISExportImageForm()
{
}

void SSWR::AVIRead::AVIRGISExportImageForm::SetExportSize(Math::Size2D<UIntOS> size, Double dpi)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrUIntOS(sbuff, size.x);
	this->txtDimensionW->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrUIntOS(sbuff, size.y);
	this->txtDimensionH->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, dpi);
	this->txtDPI->SetText(CSTRP(sbuff, sptr));
}

void SSWR::AVIRead::AVIRGISExportImageForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
