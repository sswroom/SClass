#include "Stdafx.h"
#include "Math/CoordinateSystemConverter.h"
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
		Map::GetObjectSess *sess;
		UOSInt i;
		UOSInt i2;
		UOSInt j;
		UOSInt k;
		Math::Coord2DDbl mapPt = me->navi->ScnXY2MapXY(scnPos);
		NotNullPtr<Math::CoordinateSystem> csys = me->navi->GetCoordinateSystem();
		NotNullPtr<Math::CoordinateSystem> lyrCSys = me->lyr->GetCoordinateSystem();
		if (!csys->Equals(lyrCSys))
		{
			mapPt = Math::CoordinateSystem::Convert(csys, lyrCSys, mapPt);
		}
		me->ClearQueryResults();
		if (me->lyr->CanQuery())
		{
			if (me->lyr->QueryInfos(mapPt, &me->queryVecList, &me->queryValueOfstList, &me->queryNameList, &me->queryValueList) && me->queryVecList.GetCount() > 0)
			{
				Math::Geometry::Vector2D *vec;
				me->cboObj->ClearItems();
				i = 0;
				j = me->queryVecList.GetCount();
				while (i < j)
				{
					vec = me->queryVecList.GetItem(i);
					me->cboObj->AddItem(Math::Geometry::Vector2D::VectorTypeGetName(vec->GetVectorType()), 0);
					i++;
				}
				me->cboObj->SetSelectedIndex(0);
				me->SetQueryItem(0);

				if (!csys->Equals(lyrCSys))
				{
					Math::CoordinateSystemConverter converter(lyrCSys, csys);
					i = me->queryVecList.GetCount();
					while (i-- > 0)
					{
						me->queryVecList.GetItem(i)->Convert(converter);
					}
				}

				me->navi->SetSelectedVectors(me->queryVecList);
				me->layerNames = false;
				return false;
			}
		}
		scnPos.x += 5;
		Math::Coord2DDbl mapPt2 = me->navi->ScnXY2MapXY(scnPos);
		if (!csys->Equals(lyrCSys))
		{
			mapPt2 = Math::CoordinateSystem::Convert(csys, lyrCSys, mapPt2);
		}
		sess = me->lyr->BeginGetObject();
		Data::ArrayList<Map::MapDrawLayer::ObjectInfo*> objList;
		me->lyr->GetNearObjects(sess, objList, mapPt, mapPt2.x - mapPt.x);
		if (!me->layerNames)
		{
			me->layerNames = true;
			me->ShowLayerNames();
		}
		me->cboObj->ClearItems();
		if (objList.GetCount() == 0)
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
			Map::MapDrawLayer::ObjectInfo *obj;
			Math::Geometry::Vector2D *vec = 0;
			Data::ArrayListInt64 arr;
			Map::NameArray *nameArr;
			Text::StringBuilderUTF8 sb;
			me->lyr->GetObjectIdsMapXY(arr, &nameArr, Math::RectAreaDbl(mapPt, mapPt), true);
			j = 0;
			k = objList.GetCount();
			while (j < k)
			{
				obj = objList.GetItem(j);

				vec = me->lyr->GetNewVectorById(sess, obj->objId);
				if (vec && !csys->Equals(lyrCSys))
				{
					Math::CoordinateSystemConverter converter(lyrCSys, csys);
					vec->Convert(converter);
				}
				if (vec)
				{
					sb.ClearStr();
					if (me->lyr->GetString(sb, nameArr, obj->objId, me->lyr->GetNameCol()))
					{
						sb.AppendC(UTF8STRC(" - "));
						sb.Append(Math::Geometry::Vector2D::VectorTypeGetName(vec->GetVectorType()));
						me->cboObj->AddItem(sb.ToCString(), 0);
					}
					else
					{
						me->cboObj->AddItem(Math::Geometry::Vector2D::VectorTypeGetName(vec->GetVectorType()), 0);
					}
					me->queryVecList.Add(vec);
					i = 0;
					i2 = me->lyr->GetColumnCnt();
					while (i < i2)
					{
						sb.ClearStr();
						me->lyr->GetString(sb, nameArr, obj->objId, i);
						me->queryValueList.Add(Text::String::New(sb.ToCString()).Ptr());
						i++;
					}
				}

				j++;
			}
			me->cboObj->SetSelectedIndex(0);
			me->SetQueryItem(0);
			me->lyr->ReleaseNameArr(nameArr);
			me->lyr->FreeObjects(objList);
			me->navi->SetSelectedVectors(me->queryVecList);
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
		NotNullPtr<Math::CoordinateSystem> csys = me->navi->GetCoordinateSystem();
		Math::Coord2DDbl mapPos = me->navi->ScnXY2MapXY(scnPos);
		Math::Coord2DDbl nearPos = mapPos;
		me->currVec->CalBoundarySqrDistance(mapPos, nearPos);
		Double d = csys->CalSurfaceDistance(mapPos, nearPos, Math::Unit::Distance::DU_METER);
		UTF8Char sbuff[64];
		UTF8Char *sptr;
		sptr = Text::StrDouble(sbuff, d);
		me->txtDist->SetText(CSTRP(sbuff, sptr));
		if (me->currVec->InsideOrTouch(mapPos))
		{
			me->txtInside->SetText(CSTR("Inside"));
		}
		else
		{
			me->txtInside->SetText(CSTR("Outside"));
		}
	}
	return false;
}


void __stdcall SSWR::AVIRead::AVIRGISQueryForm::OnShapeFmtChanged(void *userObj)
{
	SSWR::AVIRead::AVIRGISQueryForm *me = (SSWR::AVIRead::AVIRGISQueryForm*)userObj;
	NotNullPtr<Math::Geometry::Vector2D> vec;
	if (vec.Set(me->currVec))
	{
		Math::VectorTextWriter *writer = (Math::VectorTextWriter*)me->cboShapeFmt->GetSelectedItem();
		Text::StringBuilderUTF8 sb;
		writer->ToText(sb, vec);
		me->txtShape->SetText(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRGISQueryForm::OnObjSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRGISQueryForm *me = (SSWR::AVIRead::AVIRGISQueryForm*)userObj;
	UOSInt index = me->cboObj->GetSelectedIndex();
	if (index != INVALID_INDEX)
	{
		me->SetQueryItem(index);
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

void SSWR::AVIRead::AVIRGISQueryForm::ClearQueryResults()
{
	Text::String *value;
	UOSInt i = this->queryNameList.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(this->queryNameList.GetItem(i));
	}
	i = this->queryValueList.GetCount();
	while (i-- > 0)
	{
		value = this->queryValueList.GetItem(i);
		SDEL_STRING(value);
	}
	this->queryNameList.Clear();
	this->queryValueList.Clear();
	this->queryValueOfstList.Clear();
	this->queryVecList.Clear();
}

void SSWR::AVIRead::AVIRGISQueryForm::SetQueryItem(UOSInt index)
{
	NotNullPtr<Math::Geometry::Vector2D> vec = this->queryVecList.GetItem(index)->Clone();
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Optional<Text::String> name;
	Text::String *value;
	if (this->layerNames)
	{
		i = 0;
		j = this->lyr->GetColumnCnt();
		k = index * j;
		while (i < j)
		{
			value = this->queryValueList.GetItem(i + k);
			this->lvInfo->SetSubItem(i, 1, Text::String::OrEmpty(value));
			i++;
		}
	}
	else
	{
		this->lvInfo->ClearItems();
		i = this->queryValueOfstList.GetItem(index);
		j = this->queryValueOfstList.GetItem(index + 1);
		if (j == 0)
			j = this->queryNameList.GetCount();
		NotNullPtr<Text::String> valueStr;
		while (i < j)
		{
			name = this->queryNameList.GetItem(i);
			value = this->queryValueList.GetItem(i);
			k = this->lvInfo->AddItem(Text::String::OrEmpty(name), 0);
			if (valueStr.Set(value))
			{
				this->lvInfo->SetSubItem(k, 1, valueStr);
			}
			i++;
		}
	}
	Math::RectAreaDbl bounds = vec->GetBounds();

	Math::VectorTextWriter *writer = (Math::VectorTextWriter*)this->cboShapeFmt->GetSelectedItem();
	Text::StringBuilderUTF8 sb;
	writer->ToText(sb, vec);
	this->txtShape->SetText(sb.ToCString());

	sptr = Text::StrDouble(sbuff, bounds.tl.x);
	this->txtMinX->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, bounds.tl.y);
	this->txtMinY->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, bounds.br.x);
	this->txtMaxX->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, bounds.br.y);
	this->txtMaxY->SetText(CSTRP(sbuff, sptr));

	SDEL_CLASS(this->currVec);
	this->currVec = vec.Ptr();
}

SSWR::AVIRead::AVIRGISQueryForm::AVIRGISQueryForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Map::MapDrawLayer> lyr, IMapNavigator *navi) : UI::GUIForm(parent, 416, 408, ui)
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

	NEW_CLASSNN(this->pnlObj, UI::GUIPanel(ui, *this));
	this->pnlObj->SetRect(0, 0, 100, 31, false);
	this->pnlObj->SetDockType(UI::GUIControl::DOCK_TOP);
	this->cboObj = ui->NewComboBox(this->pnlObj, false);
	this->cboObj->SetRect(4, 4, 200, 23, false);
	this->cboObj->HandleSelectionChange(OnObjSelChg, this);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpInfo = this->tcMain->AddTabPage(CSTR("Info"));
	NEW_CLASS(this->lvInfo, UI::GUIListView(ui, this->tpInfo, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvInfo->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvInfo->AddColumn(CSTR("Name"), 100);
	this->lvInfo->AddColumn(CSTR("Value"), 300);
	this->lvInfo->SetShowGrid(true);
	this->lvInfo->SetFullRowSelect(true);

	this->tpShape = this->tcMain->AddTabPage(CSTR("Shape"));
	NEW_CLASSNN(this->pnlShape, UI::GUIPanel(ui, this->tpShape));
	this->pnlShape->SetRect(0, 0, 100, 31, false);
	this->pnlShape->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblShapeFmt = ui->NewLabel(this->pnlShape, CSTR("Format"));
	this->lblShapeFmt->SetRect(4, 4, 100, 23, false);
	this->cboShapeFmt = ui->NewComboBox(this->pnlShape, false);
	this->cboShapeFmt->SetRect(104, 4, 200, 23, false);
	this->cboShapeFmt->HandleSelectionChange(OnShapeFmtChanged, this);
	this->txtShape = ui->NewTextBox(this->tpShape, CSTR(""), true);
	this->txtShape->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtShape->SetReadOnly(true);

	this->tpBounds = this->tcMain->AddTabPage(CSTR("Bounds"));
	this->lblMinX = ui->NewLabel(this->tpBounds, CSTR("Min X"));
	this->lblMinX->SetRect(4, 4, 100, 23, false);
	this->txtMinX = ui->NewTextBox(this->tpBounds, CSTR(""));
	this->txtMinX->SetRect(104, 4, 150, 23, false);
	this->txtMinX->SetReadOnly(true);
	this->lblMinY = ui->NewLabel(this->tpBounds, CSTR("Min Y"));
	this->lblMinY->SetRect(4, 28, 100, 23, false);
	this->txtMinY = ui->NewTextBox(this->tpBounds, CSTR(""));
	this->txtMinY->SetRect(104, 28, 150, 23, false);
	this->txtMinY->SetReadOnly(true);
	this->lblMaxX = ui->NewLabel(this->tpBounds, CSTR("Max X"));
	this->lblMaxX->SetRect(4, 52, 100, 23, false);
	this->txtMaxX = ui->NewTextBox(this->tpBounds, CSTR(""));
	this->txtMaxX->SetRect(104, 52, 150, 23, false);
	this->txtMaxX->SetReadOnly(true);
	this->lblMaxY = ui->NewLabel(this->tpBounds, CSTR("Max Y"));
	this->lblMaxY->SetRect(4, 76, 100, 23, false);
	this->txtMaxY = ui->NewTextBox(this->tpBounds, CSTR(""));
	this->txtMaxY->SetRect(104, 76, 150, 23, false);
	this->txtMaxY->SetReadOnly(true);

	this->tpDist = this->tcMain->AddTabPage(CSTR("Distance"));
	this->lblDist = ui->NewLabel(this->tpDist, CSTR("Dist to boundaries"));
	this->lblDist->SetRect(4, 4, 100, 23, false);
	this->txtDist = ui->NewTextBox(this->tpDist, CSTR(""));
	this->txtDist->SetReadOnly(true);
	this->txtDist->SetRect(104, 4, 150, 23, false);
	this->lblInside = ui->NewLabel(this->tpDist, CSTR("Inside Vector"));
	this->lblInside->SetRect(4, 28, 100, 23, false);
	this->txtInside = ui->NewTextBox(this->tpDist, CSTR(""));
	this->txtInside->SetReadOnly(true);
	this->txtInside->SetRect(104, 28, 150, 23, false);

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
	this->ClearQueryResults();
	this->navi->UnhandleMapMouse(this);
	SDEL_CLASS(this->currVec);
}

void SSWR::AVIRead::AVIRGISQueryForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
