#include "Stdafx.h"
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
		sess = me->lyr->BeginGetObject();
		id = me->lyr->GetNearestObjectId(sess, mapPt, &mapPt);
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
			Math::Vector2D *vec = me->lyr->GetNewVectorById(sess, id);
			if (vec && csys && lyrCSys && !csys->Equals(lyrCSys))
			{
				vec->ConvCSys(lyrCSys, csys);
			}
			SDEL_CLASS(me->currVec);
			if (vec)
			{
				me->currVec = vec->Clone();

				Math::VectorTextWriter *writer = (Math::VectorTextWriter*)me->cboShapeFmt->GetSelectedItem();
				Text::StringBuilderUTF8 sb;
				writer->ToText(&sb, me->currVec);
				me->txtShape->SetText(sb.ToCString());
			}
			me->navi->SetSelectedVector(vec);
			me->lyr->ReleaseNameArr(nameArr);
		}
		me->lyr->EndGetObject(sess);
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

SSWR::AVIRead::AVIRGISQueryForm::AVIRGISQueryForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Map::IMapDrawLayer *lyr, IMapNavigator *navi) : UI::GUIForm(parent, 416, 408, ui)
{
	Text::StringBuilderUTF8 sb;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->lyr = lyr;
	this->navi = navi;
	this->currVec = 0;
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

	i = 0;
	j = this->writerList.GetCount();
	while (i < j)
	{
		Math::VectorTextWriter *writer = this->writerList.GetItem(i);
		this->cboShapeFmt->AddItem(writer->GetWriterName(), writer);
		i++;
	}
	this->cboShapeFmt->SetSelectedIndex(0);
	this->navi->HandleMapMouseDown(OnMouseDown, this);
	this->navi->HandleMapMouseUp(OnMouseUp, this);
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
