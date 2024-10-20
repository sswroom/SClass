#include "Stdafx.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRGISEditVectorForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Bool __stdcall SSWR::AVIRead::AVIRGISEditVectorForm::OnMouseDown(AnyType userObj, Math::Coord2D<OSInt> scnPos)
{
//	NN<SSWR::AVIRead::AVIRGISEditVectorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISEditVectorForm>();
	return false;
}

Bool __stdcall SSWR::AVIRead::AVIRGISEditVectorForm::OnMouseUp(AnyType userObj, Math::Coord2D<OSInt> scnPos)
{
//	NN<SSWR::AVIRead::AVIRGISEditVectorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISEditVectorForm>();
	return false;
}

Bool __stdcall SSWR::AVIRead::AVIRGISEditVectorForm::OnMouseMove(AnyType userObj, Math::Coord2D<OSInt> scnPos)
{
//	NN<SSWR::AVIRead::AVIRGISEditVectorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISEditVectorForm>();
	return false;
}

void __stdcall SSWR::AVIRead::AVIRGISEditVectorForm::OnObjectsDblClk(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISEditVectorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISEditVectorForm>();
	UOSInt selInd = me->lbObjects->GetSelectedIndex();
	if (selInd != INVALID_INDEX)
	{
		Int64 objId = (Int64)(OSInt)me->lbObjects->GetItem(selInd).p;

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

SSWR::AVIRead::AVIRGISEditVectorForm::AVIRGISEditVectorForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::VectorLayer> lyr, IMapNavigator *navi) : UI::GUIForm(parent, 416, 408, ui)
{
	Text::StringBuilderUTF8 sb;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->lyr = lyr;
	this->navi = navi;
	sb.AppendC(UTF8STRC("Edit Vector - "));
	sb.Append(lyr->GetSourceNameObj());
	this->SetText(sb.ToCString());
	this->SetFont(0, 0, 8.25, false);

	this->lbObjects = ui->NewListBox(*this, false);
	this->lbObjects->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbObjects->HandleDoubleClicked(OnObjectsDblClk, this);

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
	this->navi->HandleMapMouseDown(OnMouseDown, this);
	this->navi->HandleMapMouseUp(OnMouseUp, this);
	this->navi->HandleMapMouseMove(OnMouseMove, this);
}

SSWR::AVIRead::AVIRGISEditVectorForm::~AVIRGISEditVectorForm()
{
	this->navi->UnhandleMapMouse(this);
}

void SSWR::AVIRead::AVIRGISEditVectorForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
