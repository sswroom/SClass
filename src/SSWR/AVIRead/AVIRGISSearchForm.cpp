#include "Stdafx.h"
#include "Math/CoordinateSystemConverter.h"
#include "SSWR/AVIRead/AVIRGISSearchForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRGISSearchForm::OnTextChg(AnyType userObj)
{
	UTF8Char sbuff[512];
	NotNullPtr<SSWR::AVIRead::AVIRGISSearchForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISSearchForm>();
	me->txtSearchStr->GetText(sbuff);

	me->layer->ReleaseSearchStr(me->dispList);
	me->layer->SearchString(me->dispList, me->searching, me->nameArr, sbuff, 100, me->strIndex);

	me->UpdateResults();
}

void __stdcall SSWR::AVIRead::AVIRGISSearchForm::OnResultSelChg(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRGISSearchForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISSearchForm>();
	NotNullPtr<Text::String> s;
	UOSInt i = me->lbResults->GetSelectedIndex();
	if (i != INVALID_INDEX && me->lbResults->GetItemTextNew(i).SetTo(s))
	{
		Math::Coord2DDbl center;
		Map::GetObjectSess *sess;

		sess = me->layer->BeginGetObject();
		Math::Geometry::Vector2D *vec = me->layer->GetVectorByStr(me->searching, me->nameArr, sess, s->ToCString(), me->strIndex);
		me->layer->EndGetObject(sess);

		if (vec)
		{
			NotNullPtr<Math::CoordinateSystem> csys1 = me->navi->GetCoordinateSystem();
			NotNullPtr<Math::CoordinateSystem> csys2 = me->layer->GetCoordinateSystem();
			if (!csys1->Equals(csys2))
			{
				Math::CoordinateSystemConverter converter(csys2, csys1);
				vec->Convert(converter);
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
	UOSInt j = this->dispList.GetCount();
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
		sb.Append(this->dispList.GetItem(i));
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

SSWR::AVIRead::AVIRGISSearchForm::AVIRGISSearchForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IMapNavigator *navi, NotNullPtr<Map::MapDrawLayer> layer, Text::SearchIndexer *searching, UOSInt strIndex, Int32 flags) : UI::GUIForm(parent, 320, 360, ui)
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

	this->txtSearchStr = ui->NewTextBox(*this, CSTR(""));
	this->txtSearchStr->SetRect(0, 0, 100, 21, false);
	this->txtSearchStr->SetDockType(UI::GUIControl::DOCK_TOP);
	this->txtSearchStr->HandleTextChanged(OnTextChg, this);
	this->lbResults = ui->NewListBox(*this, false);
	this->lbResults->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbResults->HandleSelectionChange(OnResultSelChg, this);

	Map::NameArray *nameArr;
	this->layer->GetAllObjectIds(this->objIds, &nameArr);
	this->nameArr = nameArr;
	this->UpdateResults();
}

SSWR::AVIRead::AVIRGISSearchForm::~AVIRGISSearchForm()
{
	DEL_CLASS(this->searching);
	this->layer->ReleaseNameArr(this->nameArr);
}

void SSWR::AVIRead::AVIRGISSearchForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
