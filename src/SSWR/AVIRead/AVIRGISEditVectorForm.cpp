#include "Stdafx.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRGISEditVectorForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Bool __stdcall SSWR::AVIRead::AVIRGISEditVectorForm::OnMouseDown(void *userObj, Math::Coord2D<OSInt> scnPos)
{
//	SSWR::AVIRead::AVIRGISEditVectorForm *me = (SSWR::AVIRead::AVIRGISEditVectorForm*)userObj;
	return false;
}

Bool __stdcall SSWR::AVIRead::AVIRGISEditVectorForm::OnMouseUp(void *userObj, Math::Coord2D<OSInt> scnPos)
{
//	SSWR::AVIRead::AVIRGISEditVectorForm *me = (SSWR::AVIRead::AVIRGISEditVectorForm*)userObj;
	return false;
}

Bool __stdcall SSWR::AVIRead::AVIRGISEditVectorForm::OnMouseMove(void *userObj, Math::Coord2D<OSInt> scnPos)
{
//	SSWR::AVIRead::AVIRGISEditVectorForm *me = (SSWR::AVIRead::AVIRGISEditVectorForm*)userObj;
	return false;
}

void __stdcall SSWR::AVIRead::AVIRGISEditVectorForm::OnObjectsDblClk(void *userObj)
{
	SSWR::AVIRead::AVIRGISEditVectorForm *me = (SSWR::AVIRead::AVIRGISEditVectorForm*)userObj;
	UOSInt selInd = me->lbObjects->GetSelectedIndex();
	if (selInd != INVALID_INDEX)
	{
		Int64 objId = (Int64)(OSInt)me->lbObjects->GetItem(selInd);

		Map::GetObjectSess *sess = me->lyr->BeginGetObject();
		Math::Geometry::Vector2D *vec = me->lyr->GetNewVectorById(sess, objId);
		me->lyr->EndGetObject(sess);
		if (vec)
		{
			me->navi->PanToMap(vec->GetCenter());
			DEL_CLASS(vec);
		}
	}
}

SSWR::AVIRead::AVIRGISEditVectorForm::AVIRGISEditVectorForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<Map::VectorLayer> lyr, IMapNavigator *navi) : UI::GUIForm(parent, 416, 408, ui)
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
	Map::NameArray *nameArr;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UOSInt cnt = lyr->GetColumnCnt();
	UOSInt i;
	UOSInt j;
	lyr->GetAllObjectIds(objIds, &nameArr);
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
