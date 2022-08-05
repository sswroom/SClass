#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRGISSearchForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRGISSearchForm::OnTextChg(void *userObj)
{
	UTF8Char sbuff[512];
	SSWR::AVIRead::AVIRGISSearchForm *me = (SSWR::AVIRead::AVIRGISSearchForm*)userObj;
	me->txtSearchStr->GetText(sbuff);

	me->layer->ReleaseSearchStr(me->dispList);
	me->layer->SearchString(me->dispList, me->searching, me->nameArr, sbuff, 100, me->strIndex);

	me->UpdateResults();
}

void __stdcall SSWR::AVIRead::AVIRGISSearchForm::OnResultSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRGISSearchForm *me = (SSWR::AVIRead::AVIRGISSearchForm*)userObj;
	UOSInt i = me->lbResults->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		Math::Coord2DDbl center;
		void *sess;
		Text::String *s = me->lbResults->GetItemTextNew(i);

		sess = me->layer->BeginGetObject();
		Math::Geometry::Vector2D *vec = me->layer->GetVectorByStr(me->searching, me->nameArr, sess, s->v, me->strIndex);
		me->layer->EndGetObject(sess);

		if (vec)
		{
			Math::CoordinateSystem *csys1 = me->navi->GetCoordinateSystem();
			Math::CoordinateSystem *csys2 = me->layer->GetCoordinateSystem();
			if (!csys1->Equals(csys2))
			{
				vec->ConvCSys(csys2, csys1);
			}

			center = vec->GetCenter();
			me->navi->SetSelectedVector(vec);
			me->navi->PanToMap(center);
		}
		s->Release();
	}
}

void SSWR::AVIRead::AVIRGISSearchForm::UpdateResults()
{
	UOSInt i;
	UOSInt j = this->dispList->GetCount();
	Text::StringBuilderUTF8 sb;
	if (j > 100)
	{
		j = 100;
	}
	this->lbResults->ClearItems();
	i = 0;
	while (i < j)
	{
		sb.ClearStr();
		sb.Append(this->dispList->GetItem(i));
		if (this->flags & Map::MapEnv::SFLG_TRIM)
		{
			sb.Trim();
		}
		if (this->flags & Map::MapEnv::SFLG_CAPITAL)
		{
			sb.ToCapital();
		}
		this->lbResults->AddItem(sb.ToCString(), 0);

		i++;
	}
}

SSWR::AVIRead::AVIRGISSearchForm::AVIRGISSearchForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IMapNavigator *navi, Map::IMapDrawLayer *layer, Text::SearchIndexer *searching, UOSInt strIndex, Int32 flags) : UI::GUIForm(parent, 320, 360, ui)
{
	this->core = core;
	this->navi = navi;
	this->layer = layer;
	this->searching = searching;
	this->strIndex = strIndex;
	this->flags = flags;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetText(CSTR("GIS Search"));
	this->SetFont(0, 0, 8.25, false);

	NEW_CLASS(this->txtSearchStr, UI::GUITextBox(ui, this, CSTR("")));
	this->txtSearchStr->SetRect(0, 0, 100, 21, false);
	this->txtSearchStr->SetDockType(UI::GUIControl::DOCK_TOP);
	this->txtSearchStr->HandleTextChanged(OnTextChg, this);
	NEW_CLASS(this->lbResults, UI::GUIListBox(ui, this, false));
	this->lbResults->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbResults->HandleSelectionChange(OnResultSelChg, this);

	void *nameArr;
	NEW_CLASS(this->dispList, Data::ArrayListString());
	NEW_CLASS(this->objIds, Data::ArrayListInt64());
	this->layer->GetAllObjectIds(this->objIds, &nameArr);
	this->nameArr = nameArr;
	this->UpdateResults();
}

SSWR::AVIRead::AVIRGISSearchForm::~AVIRGISSearchForm()
{
	DEL_CLASS(this->searching);
	//this->layer->ReleaseSearchStr(this->dispList);
	this->layer->ReleaseNameArr(this->nameArr);
	DEL_CLASS(this->dispList);
	DEL_CLASS(this->objIds);
	this->navi->SetSelectedVector(0);
}

void SSWR::AVIRead::AVIRGISSearchForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
