#include "Stdafx.h"
#include "Math/Math_C.h"
#include "SSWR/AVIRead/AVIRGISEditVectorForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Bool __stdcall SSWR::AVIRead::AVIRGISEditVectorForm::OnMouseDown(AnyType userObj, Math::Coord2D<OSInt> scnPos)
{
	NN<SSWR::AVIRead::AVIRGISEditVectorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISEditVectorForm>();
	me->downPos = scnPos;
	return false;
}

Bool __stdcall SSWR::AVIRead::AVIRGISEditVectorForm::OnMouseUp(AnyType userObj, Math::Coord2D<OSInt> scnPos)
{
	NN<SSWR::AVIRead::AVIRGISEditVectorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISEditVectorForm>();
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	if (me->status == 1)
	{
		if (me->downPos == scnPos)
		{
			Math::Coord2DDbl mapXY = me->navi->ScnXY2MapXY(scnPos);
			NN<Math::CoordinateSystem> lyrCSys = me->lyr->GetCoordinateSystem();
			NN<Math::CoordinateSystem> envCSys = me->navi->GetCoordinateSystem();
			if (!lyrCSys->Equals(envCSys))
			{
				mapXY = Math::CoordinateSystem::Convert(envCSys, lyrCSys, mapXY);
			}
			me->points.Add(mapXY);
			sptr = Text::StrUOSInt(sbuff, me->points.GetCount());
			me->txtNPoints->SetText(CSTRP(sbuff, sptr));
		}
	}
	return false;
}

Bool __stdcall SSWR::AVIRead::AVIRGISEditVectorForm::OnMouseMove(AnyType userObj, Math::Coord2D<OSInt> scnPos)
{
	NN<SSWR::AVIRead::AVIRGISEditVectorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISEditVectorForm>();
	if (me->status == 1)
	{
		if (me->lyr->GetLayerType() == Map::DRAW_LAYER_POLYLINE)
		{
			if (me->points.GetCount() > 0)
			{
				UOSInt nPoints;
				NN<Math::Geometry::LineString> ls;
				NEW_CLASSNN(ls, Math::Geometry::LineString(me->lyr->GetCoordinateSystem()->GetSRID(), me->points.GetCount() + 1, false, false));
				UnsafeArray<Math::Coord2DDbl> ptList = ls->GetPointList(nPoints);
				UOSInt i = 0;
				while (i < nPoints - 1)
				{
					ptList[i] = me->points.GetItem(i);
					i++;
				}
				Math::Coord2DDbl mapXY = me->navi->ScnXY2MapXY(scnPos);
				NN<Math::CoordinateSystem> lyrCSys = me->lyr->GetCoordinateSystem();
				NN<Math::CoordinateSystem> envCSys = me->navi->GetCoordinateSystem();
				if (!lyrCSys->Equals(envCSys))
				{
					mapXY = Math::CoordinateSystem::Convert(envCSys, lyrCSys, mapXY);
				}
				ptList[i] = mapXY;
				me->navi->SetSelectedVector(ls);
			}
		}
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRGISEditVectorForm::OnObjectsDblClk(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISEditVectorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISEditVectorForm>();
	UOSInt selInd = me->lbObjects->GetSelectedIndex();
	if (selInd != INVALID_INDEX)
	{
		Int64 objId;

#if _OSINT_SIZE == 64
		objId = (Int64)(OSInt)me->lbObjects->GetItem(selInd).p;
#else
		UTF8Char sbuff[32];
		UnsafeArray<UTF8Char> sptr;
		sbuff[0] = 0;
		sptr = me->lbObjects->GetItemText(sbuff, selInd).Or(sbuff);
		objId = Text::StrToInt64(sbuff);
#endif

		NN<Map::GetObjectSess> sess = me->lyr->BeginGetObject();
		NN<Math::Geometry::Vector2D> vec;
		if (me->lyr->GetNewVectorById(sess, objId).SetTo(vec))
		{
			me->navi->PanToMap(vec->GetCenter());
			vec.Delete();
		}
		me->lyr->EndGetObject(sess);
	}
}

void __stdcall SSWR::AVIRead::AVIRGISEditVectorForm::OnObjectsSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISEditVectorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISEditVectorForm>();
	if (me->status != 0)
		return;
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	UOSInt selInd = me->lbObjects->GetSelectedIndex();
	if (selInd != INVALID_INDEX)
	{
		Int64 objId;

#if _OSINT_SIZE == 64
		objId = (Int64)(OSInt)me->lbObjects->GetItem(selInd).p;
#else
		sbuff[0] = 0;
		sptr = me->lbObjects->GetItemText(sbuff, selInd).Or(sbuff);
		objId = Text::StrToInt64(sbuff);
#endif

		NN<Map::GetObjectSess> sess = me->lyr->BeginGetObject();
		NN<Math::Geometry::Vector2D> vec;
		if (me->lyr->GetNewVectorById(sess, objId).SetTo(vec))
		{
			sptr = Text::StrUOSInt(sbuff, vec->GetPointCount());
			me->lblNPoints->SetText(CSTRP(sbuff, sptr));
			me->navi->SetSelectedVector(vec);
		}
		else
		{
			me->navi->SetSelectedVector(0);
		}
		me->lyr->EndGetObject(sess);
	}
}

void __stdcall SSWR::AVIRead::AVIRGISEditVectorForm::OnNewClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISEditVectorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISEditVectorForm>();
	if (me->status == 0)
	{
		if (me->lyr->GetLayerType() == Map::DRAW_LAYER_POLYLINE)
		{
			me->status = 1;
			me->txtStatus->SetText(CSTR("New"));
			me->txtNPoints->SetText(CSTR("0"));
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGISEditVectorForm::OnDeleteClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISEditVectorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISEditVectorForm>();
	UOSInt selInd = me->lbObjects->GetSelectedIndex();
	if (selInd != INVALID_INDEX)
	{
		Int64 objId;

#if _OSINT_SIZE == 64
		objId = (Int64)(OSInt)me->lbObjects->GetItem(selInd).p;
#else
		UTF8Char sbuff[32];
		UnsafeArray<UTF8Char> sptr;
		sbuff[0] = 0;
		sptr = me->lbObjects->GetItemText(sbuff, selInd).Or(sbuff);
		objId = Text::StrToInt64(sbuff);
#endif
		me->lyr->DeleteVector(objId);
		if (me->status == 0)
		{
			me->navi->SetSelectedVector(0);
		}
		me->UpdateList();
	}
}

void __stdcall SSWR::AVIRead::AVIRGISEditVectorForm::OnEndClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISEditVectorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISEditVectorForm>();
	if (me->lyr->GetLayerType() == Map::DRAW_LAYER_POLYLINE)
	{
		if (me->status == 1)
		{
			if (me->points.GetCount() == 0)
			{
			}
			else if (me->points.GetCount() == 1)
			{
				me->ui->ShowMsgOK(CSTR("Please add at least 2 points"), CSTR("Edit Layer"), me);
				return;
			}
			else
			{
				Data::ArrayListStringNN names;
				NN<Math::Geometry::Polyline> pl;
				NN<Math::Geometry::LineString> ls;
				NEW_CLASSNN(ls, Math::Geometry::LineString(me->lyr->GetCoordinateSystem()->GetSRID(), me->points.GetCount(), false, false));
				UOSInt nPoints;
				UnsafeArray<Math::Coord2DDbl> ptList = ls->GetPointList(nPoints);
				UOSInt i = 0;
				while (i < nPoints)
				{
					ptList[i] = me->points.GetItem(i);
					i++;
				}
				NEW_CLASSNN(pl, Math::Geometry::Polyline(me->lyr->GetCoordinateSystem()->GetSRID()));
				pl->AddGeometry(ls);
				me->lyr->AddVector2(pl, names);
				me->points.Clear();
				me->navi->RedrawMap();
				me->UpdateList();
			}
			me->navi->SetSelectedVector(0);
			me->status = 0;
			me->txtStatus->SetText(CSTR("View"));
			me->txtNPoints->SetText(CSTR("0"));
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGISEditVectorForm::OnBackClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISEditVectorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISEditVectorForm>();
	if (me->status == 1 && me->points.GetCount() > 0)
	{
		me->points.Pop();
	}
}

void SSWR::AVIRead::AVIRGISEditVectorForm::UpdateList()
{
	this->lbObjects->ClearItems();
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Data::ArrayListInt64 idList;
	Int64 id;
	this->lyr->GetAllObjectIds(idList, 0);
	UOSInt i = 0;
	UOSInt j = idList.GetCount();
	while (i < j)
	{
		id = idList.GetItem(i);
		sptr = Text::StrInt64(sbuff, id);
		this->lbObjects->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)id);
		i++;
	}
}

SSWR::AVIRead::AVIRGISEditVectorForm::AVIRGISEditVectorForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::VectorLayer> lyr, NN<AVIRMapNavigator> navi) : UI::GUIForm(parent, 416, 408, ui)
{
	Text::StringBuilderUTF8 sb;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->lyr = lyr;
	this->navi = navi;
	this->status = 0;
	sb.AppendC(UTF8STRC("Edit Vector - "));
	sb.Append(lyr->GetSourceNameObj());
	this->SetText(sb.ToCString());
	this->SetFont(0, 0, 8.25, false);

	this->lbObjects = ui->NewListBox(*this, false);
	this->lbObjects->SetRect(0, 0, 150, 23, false);
	this->lbObjects->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbObjects->HandleSelectionChange(OnObjectsSelChg, this);
	this->lbObjects->HandleDoubleClicked(OnObjectsDblClk, this);
	this->hspObjects = ui->NewHSplitter(*this, 3, false);
	this->pnlObjects = ui->NewPanel(*this);
	this->pnlObjects->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblStatus = ui->NewLabel(this->pnlObjects, CSTR("Status"));
	this->lblStatus->SetRect(4, 4, 100, 23, false);
	this->txtStatus = ui->NewTextBox(this->pnlObjects, CSTR("View"));
	this->txtStatus->SetReadOnly(true);
	this->txtStatus->SetRect(104, 4, 150, 23, false);
	this->lblNPoints = ui->NewLabel(this->pnlObjects, CSTR("Num Points"));
	this->lblNPoints->SetRect(4, 28, 100, 23, false);
	this->txtNPoints = ui->NewTextBox(this->pnlObjects, CSTR("0"));
	this->txtNPoints->SetReadOnly(true);
	this->txtNPoints->SetRect(104, 28, 150, 23, false);
	this->btnNew = ui->NewButton(this->pnlObjects, CSTR("New"));
	this->btnNew->SetRect(4, 52, 75, 23, false);
	this->btnNew->HandleButtonClick(OnNewClicked, this);
	this->btnDelete = ui->NewButton(this->pnlObjects, CSTR("Delete"));
	this->btnDelete->SetRect(84, 52, 75, 23, false);
	this->btnDelete->HandleButtonClick(OnDeleteClicked, this);
	this->btnEnd = ui->NewButton(this->pnlObjects, CSTR("End"));
	this->btnEnd->SetRect(4, 76, 75, 23, false);
	this->btnEnd->HandleButtonClick(OnEndClicked, this);
	this->btnBack = ui->NewButton(this->pnlObjects, CSTR("Back"));
	this->btnBack->SetRect(84, 76, 75, 23, false);
	this->btnBack->HandleButtonClick(OnBackClicked, this);

	UOSInt nameCol = lyr->GetNameCol();
	Int64 objId;
	Data::ArrayListInt64 objIds;
	Optional<Map::NameArray> nameArr;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UOSInt cnt = lyr->GetColumnCnt();
	UOSInt i;
	UOSInt j;
	lyr->GetAllObjectIds(objIds, nameArr);
	if (cnt > nameCol)
	{
		i = 0;
		j = objIds.GetCount();
		while (i < j)
		{
			objId = objIds.GetItem(i);
			sb.ClearStr();
			lyr->GetString(sb, nameArr, objId, nameCol);
			this->lbObjects->AddItem(sb.ToCString(), (void*)(OSInt)objId);
			i++;
		}
	}
	else
	{
		i = 0;
		j = objIds.GetCount();
		while (i < j)
		{
			objId = objIds.GetItem(i);
			sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Object ")), i);
			this->lbObjects->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)objId);
			i++;
		}
	}
	lyr->ReleaseNameArr(nameArr);
	this->navi->HandleMapMouseLDown(OnMouseDown, this);
	this->navi->HandleMapMouseLUp(OnMouseUp, this);
	this->navi->HandleMapMouseMove(OnMouseMove, this);
	this->UpdateList();
}

SSWR::AVIRead::AVIRGISEditVectorForm::~AVIRGISEditVectorForm()
{
	this->navi->UnhandleMapMouse(this);
}

void SSWR::AVIRead::AVIRGISEditVectorForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
