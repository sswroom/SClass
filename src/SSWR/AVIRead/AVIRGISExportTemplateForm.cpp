#include "Stdafx.h"
#include "IO/FileStream.h"
#include "Map/DrawMapRenderer.h"
#include "SSWR/AVIRead/AVIRGISExportTemplateForm.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIRGISExportTemplateForm::OnFiles(AnyType userObj, Data::DataArray<NN<Text::String>> fileNames)
{
	NN<AVIRGISExportTemplateForm> me = userObj.GetNN<AVIRGISExportTemplateForm>();
	NN<Media::SVGDocument> doc;
	Text::EncodingFactory encFact;
	UIntOS i = 0;
	while (i < fileNames.GetSize())
	{
		NN<Text::String> s = fileNames.GetItem(i);
		if (Media::SVGDocument::ParseFile(s->ToCString(), encFact, me->core->GetDrawEngine()).SetTo(doc))
		{
			me->doc.Delete();
			me->doc = doc;
			me->txtTemplate->SetText(s->ToCString());
			me->cboId->ClearItems();
			Data::ArrayListNN<Media::SVGElement> elements;
			doc->FindElementName(CSTR("rect"), elements);
			NN<Media::SVGRect> rect;
			NN<Text::String> id;
			i = 0;
			UIntOS j = elements.GetCount();
			Bool found = false;
			while (i < j)
			{
				rect = NN<Media::SVGRect>::ConvertFrom(elements.GetItemNoCheck(i));
				if (rect->GetID().SetTo(id))
				{
					me->cboId->AddItem(id, rect);
					found = true;
				}
				i++;
			}
			if (found)
			{
				me->cboId->SetSelectedIndex(0);
			}
			return;
		}
		i++;
	}
	me->ui->ShowMsgOK(CSTR("Error in loading SVG file"), CSTR("Export with Template"), me);
}

void __stdcall SSWR::AVIRead::AVIRGISExportTemplateForm::OnIdSelChg(AnyType userObj)
{
	NN<AVIRGISExportTemplateForm> me = userObj.GetNN<AVIRGISExportTemplateForm>();
	NN<Media::SVGRect> rect;
	NN<Media::SVGContainer> parent;
	NN<Media::SVGDocument> doc;
	if (!me->cboId->GetSelectedItem().GetOpt<Media::SVGRect>().SetTo(rect))
	{
		return;
	}
	if (!rect->GetParent().SetTo(parent))
	{
		me->txtDimensionW->SetText(CSTR("No Parent"));
		me->txtDimensionH->SetText(CSTR(""));
		return;
	}
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	doc = parent->GetDoc();
	sptr = Text::StrDouble(sbuff, rect->GetWidth() * doc->GetHDrawScale());
	me->txtDimensionW->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, rect->GetHeight() * doc->GetVDrawScale());
	me->txtDimensionH->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRGISExportTemplateForm::OnExportClicked(AnyType userObj)
{
	NN<AVIRGISExportTemplateForm> me = userObj.GetNN<AVIRGISExportTemplateForm>();
	NN<Media::SVGDocument> doc;
	NN<Media::SVGRect> rect;
	NN<Media::SVGContainer> parent;
	NN<Media::SVGGroup> group;
	NN<Text::String> id;
	if (!me->doc.SetTo(doc))
	{
		me->ui->ShowMsgOK(CSTR("Please load a SVG file"), CSTR("Export with Template"), me);
		return;
	}
	if (!me->cboId->GetSelectedItem().GetOpt<Media::SVGRect>().SetTo(rect))
	{
		me->ui->ShowMsgOK(CSTR("Please select a area"), CSTR("Export with Template"), me);
		return;
	}
	if (!rect->GetParent().SetTo(parent))
	{
		me->ui->ShowMsgOK(CSTR("The selected area has no parent"), CSTR("Export with Template"), me);
		return;
	}
	if (!rect->GetID().SetTo(id))
	{
		me->ui->ShowMsgOK(CSTR("The selected area has no ID"), CSTR("Export with Template"), me);
		return;
	}
	Text::StringBuilderUTF8 sb;
	Double x;
	Double y;
	Double scale;
	me->txtCenterX->GetText(sb);
	if (!sb.ToDouble(x))
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid X"), CSTR("Export with Template"), me);
		return;
	}
	sb.ClearStr();
	me->txtCenterY->GetText(sb);
	if (!sb.ToDouble(y))
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid Y"), CSTR("Export with Template"), me);
		return;
	}
	sb.ClearStr();
	me->txtScale->GetText(sb);
	if (!sb.ToDouble(scale) || scale <= 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid scale"), CSTR("Export with Template"), me);
		return;
	}
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"GISExportTemplate", true);
	dlg->AddFilter(CSTR("*.svg"), CSTR("SVG File"));
	if (dlg->ShowDialog(me->GetHandle()))
	{
		NN<Media::SVGRect> clipRect;
		NN<Media::SVGUnknownContainer> clipPath;
		NEW_CLASSNN(clipPath, Media::SVGUnknownContainer(parent, doc->GetDrawEngine(), doc, CSTR("clipPath")));
		sb.ClearStr();
		sb.Append(id);
		sb.Append(CSTR("Clip"));
		clipPath->AddAttr(CSTR("id"), sb.ToCString());
		NEW_CLASSNN(clipRect, Media::SVGRect(clipPath, rect->GetTL(), rect->GetSize(), nullptr, nullptr));
		clipPath->AddElement(clipRect);
		parent->AddElement(clipPath);
		NEW_CLASSNN(group, Media::SVGGroup(parent, doc->GetDrawEngine(), doc));
		sb.ClearStr();
		sb.AppendC(UTF8STRC("url(#"));
		sb.Append(id);
		sb.AppendC(UTF8STRC("Clip)"));
		group->SetClipPath(sb.ToCString());
		group->SetDrawRect(rect->GetRect());
		parent->AddElement(group);
		Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
		group->SetColorProfile(color);
		NN<Map::MapView> view = me->env->CreateMapView(Math::Size2DDbl(group->GetWidth(), group->GetHeight()));
		view->SetDPI(96.0, 96.0);
		view->SetCenterXY(Math::Coord2DDbl(x, y));
		view->SetMapScale(scale);
		Map::DrawMapRenderer renderer(me->core->GetDrawEngine(), me->env, color, nullptr, Map::DrawMapRenderer::DrawType::DT_VECTORDRAW);
		if (renderer.DrawMap(group, view, nullptr))
		{
			me->env->WaitForLoad(60000);
			group->ClearElements();
			renderer.DrawMap(group, view, nullptr);
		}
		view.Delete();
		sb.ClearStr();
		doc->ToString(sb);
		{
			IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			fs.Write(sb.ToByteArray());
		}
		me->Close();
	}
	dlg.Delete();
}

SSWR::AVIRead::AVIRGISExportTemplateForm::AVIRGISExportTemplateForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::MapEnv> env, Math::Coord2DDbl center, Double scale) : UI::GUIForm(parent, 640, 200, ui)
{
	Text::StringBuilderUTF8 sb;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->env = env;
	this->center = center;
	this->scale = scale;
	this->doc = nullptr;

	this->SetText(CSTR("Export with Template"));
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
	this->lblTemplate = ui->NewLabel(*this, CSTR("Template"));
	this->lblTemplate->SetRect(4, 100, 100, 23, false);
	this->txtTemplate = ui->NewTextBox(*this, CSTR(""));
	this->txtTemplate->SetRect(104, 100, 500, 23, false);
	this->txtTemplate->SetReadOnly(true);
	this->lblId = ui->NewLabel(*this, CSTR("Rect ID"));
	this->lblId->SetRect(4, 124, 100, 23, false);
	this->cboId = ui->NewComboBox(*this, false);
	this->cboId->SetRect(104, 124, 150, 23, false);
	this->cboId->HandleSelectionChange(OnIdSelChg, this);
	this->lblDimension = ui->NewLabel(*this, CSTR("Dimension"));
	this->lblDimension->SetRect(4, 148, 100, 23, false);
	this->txtDimensionW = ui->NewTextBox(*this, CSTR("1024"));
	this->txtDimensionW->SetRect(104, 148, 100, 23, false);
	this->txtDimensionH = ui->NewTextBox(*this, CSTR("768"));
	this->txtDimensionH->SetRect(204, 148, 100, 23, false);
	this->btnExport = ui->NewButton(*this, CSTR("Export"));
	this->btnExport->SetRect(4, 172, 100, 23, false);
	this->btnExport->HandleButtonClick(OnExportClicked, this);

	this->HandleDropFiles(OnFiles, this);
}

SSWR::AVIRead::AVIRGISExportTemplateForm::~AVIRGISExportTemplateForm()
{
	this->doc.Delete();
}

void SSWR::AVIRead::AVIRGISExportTemplateForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
