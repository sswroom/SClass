#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRGISQueryForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Bool __stdcall SSWR::AVIRead::AVIRGISQueryForm::OnMouseDown(void *userObj, OSInt x, OSInt y)
{
	SSWR::AVIRead::AVIRGISQueryForm *me = (SSWR::AVIRead::AVIRGISQueryForm*)userObj;
	me->downX = x;
	me->downY = y;
	return false;
}

Bool __stdcall SSWR::AVIRead::AVIRGISQueryForm::OnMouseUp(void *userObj, OSInt x, OSInt y)
{
	SSWR::AVIRead::AVIRGISQueryForm *me = (SSWR::AVIRead::AVIRGISQueryForm*)userObj;
	if (me->downX == x && me->downY == y)
	{
		Double mapX;
		Double mapY;
		void *sess;
		Int64 id;
		UOSInt i;
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		me->navi->ScnXY2MapXY(x, y, &mapX, &mapY);
		sess = me->lyr->BeginGetObject();
		id = me->lyr->GetNearestObjectId(sess, mapX, mapY, &mapX, &mapY);
		if (id == -1)
		{
			i = me->lyr->GetColumnCnt();
			while (i-- > 0)
			{
				me->lvInfo->SetSubItem(i, 1, CSTR(""));
			}
			me->navi->SetSelectedVector(0);
		}
		else
		{
			Data::ArrayListInt64 arr;
			void *nameArr;
			me->lyr->GetObjectIdsMapXY(&arr, &nameArr, mapX, mapY, mapX, mapY, true);
			i = me->lyr->GetColumnCnt();
			while (i-- > 0)
			{
				sbuff[0] = 0;
				sptr = me->lyr->GetString(sbuff, sizeof(sbuff), nameArr, id, i);
				me->lvInfo->SetSubItem(i, 1, CSTRP(sbuff, sptr));
			}
			me->navi->SetSelectedVector(me->lyr->GetVectorById(sess, id));
			me->lyr->ReleaseNameArr(nameArr);
		}
		me->lyr->EndGetObject(sess);
	}
	return false;
}

SSWR::AVIRead::AVIRGISQueryForm::AVIRGISQueryForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Map::IMapDrawLayer *lyr, IMapNavigator *navi) : UI::GUIForm(parent, 416, 408, ui)
{
	Text::StringBuilderUTF8 sb;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->lyr = lyr;
	this->navi = navi;
	sb.AppendC(UTF8STRC("Query - "));
	sb.Append(lyr->GetSourceNameObj());
	this->SetText(sb.ToCString());
	this->SetFont(0, 0, 8.25, false);

	NEW_CLASS(this->lvInfo, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvInfo->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvInfo->AddColumn(CSTR("Name"), 100);
	this->lvInfo->AddColumn(CSTR("Value"), 300);
	this->lvInfo->SetShowGrid(true);
	this->lvInfo->SetFullRowSelect(true);

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

	this->navi->HandleMapMouseDown(OnMouseDown, this);
	this->navi->HandleMapMouseUp(OnMouseUp, this);
}

SSWR::AVIRead::AVIRGISQueryForm::~AVIRGISQueryForm()
{
	this->navi->UnhandleMapMouse(this);
}

void SSWR::AVIRead::AVIRGISQueryForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
