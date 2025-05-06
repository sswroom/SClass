#include "Stdafx.h"
#include "Math/Math.h"
#include "Math/Geometry/PointZ.h"
#include "SSWR/AVIRead/AVIRGISEditPointForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Bool __stdcall SSWR::AVIRead::AVIRGISEditPointForm::OnMouseDown(AnyType userObj, Math::Coord2D<OSInt> scnPos)
{
	NN<SSWR::AVIRead::AVIRGISEditPointForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISEditPointForm>();
	if (me->status == 1)
	{
		Math::Coord2DDbl mapXY = me->navi->ScnXY2MapXY(scnPos);
		NN<Math::CoordinateSystem> lyrCSys = me->lyr->GetCoordinateSystem();
		NN<Math::CoordinateSystem> envCSys = me->navi->GetCoordinateSystem();
		if (!lyrCSys->Equals(envCSys))
		{
			mapXY = Math::CoordinateSystem::Convert(envCSys, lyrCSys, mapXY);
		}
		lyrCSys->GetSRID();
		NN<Math::Geometry::Point> pt;
		if (me->lyr->GetLayerType() == Map::DRAW_LAYER_POINT)
		{
			NEW_CLASSNN(pt, Math::Geometry::Point(lyrCSys->GetSRID(), mapXY));
		}
		else if (me->lyr->GetLayerType() == Map::DRAW_LAYER_POINT3D)
		{
			NEW_CLASSNN(pt, Math::Geometry::PointZ(lyrCSys->GetSRID(), mapXY.x, mapXY.y, 0));
		}
		else
		{
			return false;
		}
		UnsafeArray<UnsafeArrayOpt<const UTF8Char>> strs;
		UOSInt cnt = me->lyr->GetColumnCnt();
		strs = MemAllocArr(UnsafeArrayOpt<const UTF8Char>, cnt);
		UOSInt i;
		i = cnt;
		while (i-- > 0)
		{
			strs[i] = 0;
		}
		Text::StringBuilderUTF8 sb;
		UOSInt nameCol = me->lyr->GetNameCol();
		if (nameCol < cnt)
		{
			me->txtName->GetText(sb);
			strs[nameCol] = UnsafeArray<const UTF8Char>(sb.v);
		}
		Int64 id = me->lyr->AddVector2(pt, strs);
		MemFreeArr(strs);
		if (sb.leng == 0)
		{
			sb.Append(CSTR("Object "));
			sb.AppendI64(id);
		}
		me->lbObjects->AddItem(sb.ToCString(), (void*)(OSInt)id);
		me->status = 0;
		me->txtStatus->SetText(CSTR("View"));
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRGISEditPointForm::OnObjectsDblClk(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISEditPointForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISEditPointForm>();
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

void __stdcall SSWR::AVIRead::AVIRGISEditPointForm::OnObjectsSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISEditPointForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISEditPointForm>();
	if (me->status != 0)
		return;
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
			me->navi->SetSelectedVector(vec);
		}
		else
		{
			me->navi->SetSelectedVector(0);
		}
		me->lyr->EndGetObject(sess);
	}
}

void __stdcall SSWR::AVIRead::AVIRGISEditPointForm::OnNewClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISEditPointForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISEditPointForm>();
	if (me->status == 0)
	{
		if (me->lyr->GetLayerType() == Map::DRAW_LAYER_POINT3D || me->lyr->GetLayerType() == Map::DRAW_LAYER_POINT)
		{
			me->status = 1;
			me->txtStatus->SetText(CSTR("New"));
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGISEditPointForm::OnDeleteClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISEditPointForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISEditPointForm>();
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

void SSWR::AVIRead::AVIRGISEditPointForm::UpdateList()
{
	this->lbObjects->ClearItems();
	UOSInt nameCol = this->lyr->GetNameCol();
	Int64 objId;
	Data::ArrayListInt64 objIds;
	Optional<Map::NameArray> nameArr;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UOSInt cnt = this->lyr->GetColumnCnt();
	UOSInt i;
	UOSInt j;
	this->lyr->GetAllObjectIds(objIds, nameArr);
	if (cnt > nameCol)
	{
		Text::StringBuilderUTF8 sb;
		i = 0;
		j = objIds.GetCount();
		while (i < j)
		{
			objId = objIds.GetItem(i);
			sb.ClearStr();
			this->lyr->GetString(sb, nameArr, objId, nameCol);
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
	this->lyr->ReleaseNameArr(nameArr);
}

SSWR::AVIRead::AVIRGISEditPointForm::AVIRGISEditPointForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::VectorLayer> lyr, NN<AVIRMapNavigator> navi) : UI::GUIForm(parent, 416, 408, ui)
{
	Text::StringBuilderUTF8 sb;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->lyr = lyr;
	this->navi = navi;
	this->status = 0;
	sb.AppendC(UTF8STRC("Edit Point - "));
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
	this->lblName = ui->NewLabel(this->pnlObjects, CSTR("Name"));
	this->lblName->SetRect(4, 28, 100, 23, false);
	this->txtName = ui->NewTextBox(this->pnlObjects, CSTR(""));
	this->txtName->SetRect(104, 28, 150, 23, false);
	this->btnNew = ui->NewButton(this->pnlObjects, CSTR("New"));
	this->btnNew->SetRect(4, 52, 75, 23, false);
	this->btnNew->HandleButtonClick(OnNewClicked, this);
	this->btnDelete = ui->NewButton(this->pnlObjects, CSTR("Delete"));
	this->btnDelete->SetRect(84, 52, 75, 23, false);
	this->btnDelete->HandleButtonClick(OnDeleteClicked, this);

	this->navi->HandleMapMouseLDown(OnMouseDown, this);
	this->UpdateList();
}

SSWR::AVIRead::AVIRGISEditPointForm::~AVIRGISEditPointForm()
{
	this->navi->UnhandleMapMouse(this);
}

void SSWR::AVIRead::AVIRGISEditPointForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
