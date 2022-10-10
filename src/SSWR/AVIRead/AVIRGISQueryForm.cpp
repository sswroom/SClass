#include "Stdafx.h"
#include "Math/GeometryTool.h"
#include "Math/Geometry/Ellipse.h"
#include "SSWR/AVIRead/AVIRGISQueryForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Bool __stdcall SSWR::AVIRead::AVIRGISQueryForm::OnMouseDown(void *userObj, Math::Coord2D<OSInt> scnPos)
{
	SSWR::AVIRead::AVIRGISQueryForm *me = (SSWR::AVIRead::AVIRGISQueryForm*)userObj;
	me->downPos = scnPos;
	return false;
}

Bool __stdcall SSWR::AVIRead::AVIRGISQueryForm::OnMouseUp(void *userObj, Math::Coord2D<OSInt> scnPos)
{
	SSWR::AVIRead::AVIRGISQueryForm *me = (SSWR::AVIRead::AVIRGISQueryForm*)userObj;
	if (me->downPos == scnPos)
	{
		void *sess;
		Int64 id;
		UOSInt i;
		UOSInt j;
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		Math::Coord2DDbl mapPt = me->navi->ScnXY2MapXY(scnPos);
		Math::CoordinateSystem *csys = me->navi->GetCoordinateSystem();
		Math::CoordinateSystem *lyrCSys = me->lyr->GetCoordinateSystem();
		Double tmp;
		if (csys && lyrCSys && !csys->Equals(lyrCSys))
		{
			Math::CoordinateSystem::ConvertXYZ(csys, lyrCSys, mapPt.x, mapPt.y, 0, &mapPt.x, &mapPt.y, &tmp);
		}
		if (me->lyr->CanQuery())
		{
			Data::ArrayList<Text::String*> nameList;
			Data::ArrayList<Text::String*> valueList;
			Math::Geometry::Vector2D *vec = me->lyr->QueryInfo(mapPt, &nameList, &valueList);
			if (vec)
			{
				Text::String *name;
				Text::String *value;
				me->lvInfo->ClearItems();
				i = 0;
				j = nameList.GetCount();
				while (i < j)
				{
					name = nameList.GetItem(i);
					value = valueList.GetItem(i);
					me->lvInfo->AddItem(name, 0);
					if (value)
					{
						me->lvInfo->SetSubItem(i, 1, value);
						value->Release();
					}
					name->Release();
					i++;
				}

				Math::RectAreaDbl bounds;
				if (csys && lyrCSys && !csys->Equals(lyrCSys))
				{
					Double z;
					vec->GetBounds(&bounds);
					Math::CoordinateSystem::ConvertXYZ(lyrCSys, csys, bounds.tl.x, bounds.tl.y, 0, &bounds.tl.x, &bounds.tl.y, &z);
					Math::CoordinateSystem::ConvertXYZ(lyrCSys, csys, bounds.br.x, bounds.br.y, 0, &bounds.br.x, &bounds.br.y, &z);
					vec->ConvCSys(lyrCSys, csys);
				}
				else
				{
					vec->GetBounds(&bounds);
				}

				Math::VectorTextWriter *writer = (Math::VectorTextWriter*)me->cboShapeFmt->GetSelectedItem();
				Text::StringBuilderUTF8 sb;
				writer->ToText(&sb, vec);
				me->txtShape->SetText(sb.ToCString());

				sptr = Text::StrDouble(sbuff, bounds.tl.x);
				me->txtMinX->SetText(CSTRP(sbuff, sptr));
				sptr = Text::StrDouble(sbuff, bounds.tl.y);
				me->txtMinY->SetText(CSTRP(sbuff, sptr));
				sptr = Text::StrDouble(sbuff, bounds.br.x);
				me->txtMaxX->SetText(CSTRP(sbuff, sptr));
				sptr = Text::StrDouble(sbuff, bounds.br.y);
				me->txtMaxY->SetText(CSTRP(sbuff, sptr));

				SDEL_CLASS(me->currVec);
/*				if (lyrCSys)
				{
					Math::Coord2DDbl center = vec->GetDistanceCenter();
					Double dist = Math::GeometryTool::CalcMaxDistanceFromPoint(center, vec, Math::Unit::Distance::DU_METER);
					if (lyrCSys->IsProjected())
					{
						NEW_CLASS(me->currVec, Math::Geometry::Ellipse(lyrCSys->GetSRID(), center.x - dist, center.y - dist, dist * 2, dist * 2));
					}
					else
					{
						Math::EarthEllipsoid *ellipsoid = ((Math::GeographicCoordinateSystem*)lyrCSys)->GetEllipsoid();
						Double x1 = ellipsoid->CalLonByDist(center.lat, center.lon, -dist);
						Double y1 = ellipsoid->CalLatByDist(center.lat, -dist);
						NEW_CLASS(me->currVec, Math::Geometry::Ellipse(lyrCSys->GetSRID(), x1, y1, (center.x - x1) * 2, (center.y - y1) * 2));
					}
					DEL_CLASS(vec);
					vec = me->currVec->Clone();
				}
				else
				{*/
					me->currVec = vec->Clone();
//				}

				me->navi->SetSelectedVector(vec);
				me->layerNames = false;
				return false;
			}
		}
		sess = me->lyr->BeginGetObject();
		id = me->lyr->GetNearestObjectId(sess, mapPt, &mapPt);
		if (!me->layerNames)
		{
			me->layerNames = true;
			me->ShowLayerNames();
		}
		if (id == -1)
		{
			i = me->lyr->GetColumnCnt();
			while (i-- > 0)
			{
				me->lvInfo->SetSubItem(i, 1, CSTR(""));
			}
			SDEL_CLASS(me->currVec);
			me->navi->SetSelectedVector(0);
		}
		else
		{
			Data::ArrayListInt64 arr;
			void *nameArr;
			me->lyr->GetObjectIdsMapXY(&arr, &nameArr, Math::RectAreaDbl(mapPt, mapPt), true);
			i = me->lyr->GetColumnCnt();
			while (i-- > 0)
			{
				sbuff[0] = 0;
				sptr = me->lyr->GetString(sbuff, sizeof(sbuff), nameArr, id, i);
				me->lvInfo->SetSubItem(i, 1, CSTRP(sbuff, sptr));
			}
			Math::Geometry::Vector2D *vec = me->lyr->GetNewVectorById(sess, id);
			Math::RectAreaDbl bounds;
			if (vec && csys && lyrCSys && !csys->Equals(lyrCSys))
			{
				Double z;
				vec->GetBounds(&bounds);
				Math::CoordinateSystem::ConvertXYZ(lyrCSys, csys, bounds.tl.x, bounds.tl.y, 0, &bounds.tl.x, &bounds.tl.y, &z);
				Math::CoordinateSystem::ConvertXYZ(lyrCSys, csys, bounds.br.x, bounds.br.y, 0, &bounds.br.x, &bounds.br.y, &z);
				vec->ConvCSys(lyrCSys, csys);
			}
			else if (vec)
			{
				vec->GetBounds(&bounds);
			}
			SDEL_CLASS(me->currVec);
			if (vec)
			{
				me->currVec = vec->Clone();

				Math::VectorTextWriter *writer = (Math::VectorTextWriter*)me->cboShapeFmt->GetSelectedItem();
				Text::StringBuilderUTF8 sb;
				writer->ToText(&sb, me->currVec);
				me->txtShape->SetText(sb.ToCString());

				sptr = Text::StrDouble(sbuff, bounds.tl.x);
				me->txtMinX->SetText(CSTRP(sbuff, sptr));
				sptr = Text::StrDouble(sbuff, bounds.tl.y);
				me->txtMinY->SetText(CSTRP(sbuff, sptr));
				sptr = Text::StrDouble(sbuff, bounds.br.x);
				me->txtMaxX->SetText(CSTRP(sbuff, sptr));
				sptr = Text::StrDouble(sbuff, bounds.br.y);
				me->txtMaxY->SetText(CSTRP(sbuff, sptr));
			}
			me->navi->SetSelectedVector(vec);
			me->lyr->ReleaseNameArr(nameArr);
		}
		me->lyr->EndGetObject(sess);
	}
	return false;
}

Bool __stdcall SSWR::AVIRead::AVIRGISQueryForm::OnMouseMove(void *userObj, Math::Coord2D<OSInt> scnPos)
{
	SSWR::AVIRead::AVIRGISQueryForm *me = (SSWR::AVIRead::AVIRGISQueryForm*)userObj;
	if (me->currVec)
	{
		Math::CoordinateSystem *csys = me->navi->GetCoordinateSystem();
		Math::Coord2DDbl mapPos = me->navi->ScnXY2MapXY(scnPos);
		Math::Coord2DDbl nearPos = mapPos;
		me->currVec->CalBoundarySqrDistance(mapPos, &nearPos);
		Double d = csys->CalSurfaceDistanceXY(mapPos, nearPos, Math::Unit::Distance::DU_METER);
		UTF8Char sbuff[64];
		UTF8Char *sptr;
		sptr = Text::StrDouble(sbuff, d);
		me->txtDist->SetText(CSTRP(sbuff, sptr));
	}
	return false;
}


void __stdcall SSWR::AVIRead::AVIRGISQueryForm::OnShapeFmtChanged(void *userObj)
{
	SSWR::AVIRead::AVIRGISQueryForm *me = (SSWR::AVIRead::AVIRGISQueryForm*)userObj;
	if (me->currVec)
	{
		Math::VectorTextWriter *writer = (Math::VectorTextWriter*)me->cboShapeFmt->GetSelectedItem();
		Text::StringBuilderUTF8 sb;
		writer->ToText(&sb, me->currVec);
		me->txtShape->SetText(sb.ToCString());
	}
}

void SSWR::AVIRead::AVIRGISQueryForm::ShowLayerNames()
{
	this->lvInfo->ClearItems();
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UOSInt i = 0;
	UOSInt j = this->lyr->GetColumnCnt();
	while (i < j)
	{
		sbuff[0] = 0;
		sptr = this->lyr->GetColumnName(sbuff, i);
		if (sptr == 0)
		{
			sptr = sbuff;
		}
		this->lvInfo->AddItem(CSTRP(sbuff, sptr), 0);
		i++;
	}
}

SSWR::AVIRead::AVIRGISQueryForm::AVIRGISQueryForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Map::IMapDrawLayer *lyr, IMapNavigator *navi) : UI::GUIForm(parent, 416, 408, ui)
{
	Text::StringBuilderUTF8 sb;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->lyr = lyr;
	this->navi = navi;
	this->currVec = 0;
	this->layerNames = true;
	sb.AppendC(UTF8STRC("Query - "));
	sb.Append(lyr->GetSourceNameObj());
	this->SetText(sb.ToCString());
	this->SetFont(0, 0, 8.25, false);

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpInfo = this->tcMain->AddTabPage(CSTR("Info"));
	NEW_CLASS(this->lvInfo, UI::GUIListView(ui, this->tpInfo, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvInfo->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvInfo->AddColumn(CSTR("Name"), 100);
	this->lvInfo->AddColumn(CSTR("Value"), 300);
	this->lvInfo->SetShowGrid(true);
	this->lvInfo->SetFullRowSelect(true);

	this->tpShape = this->tcMain->AddTabPage(CSTR("Shape"));
	NEW_CLASS(this->pnlShape, UI::GUIPanel(ui, this->tpShape));
	this->pnlShape->SetRect(0, 0, 100, 31, false);
	this->pnlShape->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblShapeFmt, UI::GUILabel(ui, this->pnlShape, CSTR("Format")));
	this->lblShapeFmt->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboShapeFmt, UI::GUIComboBox(ui, this->pnlShape, false));
	this->cboShapeFmt->SetRect(104, 4, 200, 23, false);
	this->cboShapeFmt->HandleSelectionChange(OnShapeFmtChanged, this);
	NEW_CLASS(this->txtShape, UI::GUITextBox(ui, this->tpShape, CSTR(""), true));
	this->txtShape->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtShape->SetReadOnly(true);

	this->tpBounds = this->tcMain->AddTabPage(CSTR("Bounds"));
	NEW_CLASS(this->lblMinX, UI::GUILabel(ui, this->tpBounds, CSTR("Min X")));
	this->lblMinX->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtMinX, UI::GUITextBox(ui, this->tpBounds, CSTR("")));
	this->txtMinX->SetRect(104, 4, 150, 23, false);
	this->txtMinX->SetReadOnly(true);
	NEW_CLASS(this->lblMinY, UI::GUILabel(ui, this->tpBounds, CSTR("Min Y")));
	this->lblMinY->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtMinY, UI::GUITextBox(ui, this->tpBounds, CSTR("")));
	this->txtMinY->SetRect(104, 28, 150, 23, false);
	this->txtMinY->SetReadOnly(true);
	NEW_CLASS(this->lblMaxX, UI::GUILabel(ui, this->tpBounds, CSTR("Max X")));
	this->lblMaxX->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtMaxX, UI::GUITextBox(ui, this->tpBounds, CSTR("")));
	this->txtMaxX->SetRect(104, 52, 150, 23, false);
	this->txtMaxX->SetReadOnly(true);
	NEW_CLASS(this->lblMaxY, UI::GUILabel(ui, this->tpBounds, CSTR("Max Y")));
	this->lblMaxY->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtMaxY, UI::GUITextBox(ui, this->tpBounds, CSTR("")));
	this->txtMaxY->SetRect(104, 76, 150, 23, false);
	this->txtMaxY->SetReadOnly(true);

	this->tpDist = this->tcMain->AddTabPage(CSTR("Distance"));
	NEW_CLASS(this->lblDist, UI::GUILabel(ui, this->tpDist, CSTR("Dist to boundaries")));
	this->lblDist->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtDist, UI::GUITextBox(ui, this->tpDist, CSTR("")));
	this->txtDist->SetReadOnly(true);
	this->txtDist->SetRect(104, 4, 150, 23, false);

	this->ShowLayerNames();

	UOSInt i = 0;
	UOSInt j = this->writerList.GetCount();
	while (i < j)
	{
		Math::VectorTextWriter *writer = this->writerList.GetItem(i);
		this->cboShapeFmt->AddItem(writer->GetWriterName(), writer);
		i++;
	}
	this->cboShapeFmt->SetSelectedIndex(0);
	this->navi->HandleMapMouseDown(OnMouseDown, this);
	this->navi->HandleMapMouseUp(OnMouseUp, this);
	this->navi->HandleMapMouseMove(OnMouseMove, this);
}

SSWR::AVIRead::AVIRGISQueryForm::~AVIRGISQueryForm()
{
	this->navi->UnhandleMapMouse(this);
	SDEL_CLASS(this->currVec);
}

void SSWR::AVIRead::AVIRGISQueryForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
