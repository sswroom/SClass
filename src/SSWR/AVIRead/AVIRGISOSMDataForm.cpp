#include "Stdafx.h"
#include "Map/OSM/OSMData.h"
#include "SSWR/AVIRead/AVIRGISOSMDataForm.h"

void __stdcall SSWR::AVIRead::AVIRGISOSMDataForm::OnRelationSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISOSMDataForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISOSMDataForm>();
	me->lvRelationTags->ClearItems();
	NN<Map::OSM::RelationInfo> rel;
	if (!me->lbRelation->GetSelectedItem().GetOpt<Map::OSM::RelationInfo>().SetTo(rel))
	{
		return;
	}
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> s;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	NN<Data::ArrayListNN<Map::OSM::TagInfo>> tags;
	if (rel->tags.SetTo(tags))
	{
		NN<Map::OSM::TagInfo> tag;
		i = 0;
		j = tags->GetCount();
		while (i < j)
		{
			tag = tags->GetItemNoCheck(i);
			k = me->lvRelationTags->AddItem(tag->k, tag);
			me->lvRelationTags->SetSubItem(k, 1, tag->v);
			i++;
		}
	}
	Data::ArrayListNN<Math::Geometry::Vector2D> vecList;
	NN<Math::Geometry::Vector2D> vec;
	NN<Map::GetObjectSess> sess = me->osmData->BeginGetObject();
	Math::RectAreaDbl bounds = Math::RectAreaDbl(Math::Coord2DDbl(0, 0), Math::Coord2DDbl(0, 0));
	me->lvRelationMember->ClearItems();
	i = 0;
	j = rel->members.GetCount();
	while (i < j)
	{
		NN<Map::OSM::RelationMember> member = rel->members.GetItemNoCheck(i);
		k = me->lvRelationMember->AddItem(Map::OSM::ElementTypeGetName(member->type), member);
		sptr = Text::StrInt64(sbuff, member->refId);
		me->lvRelationMember->SetSubItem(k, 1, CSTRP(sbuff, sptr));
		if (member->role.SetTo(s))
		{
			me->lvRelationMember->SetSubItem(k, 2, s);
		}
		if (me->osmData->GetNewVectorById(sess, member->refId << 2 | (Int64)member->type).SetTo(vec))
		{
			vecList.Add(vec);
			if (bounds.IsZero())
			{
				bounds = vec->GetBounds();
			}
			else
			{
				bounds = bounds.MergeArea(vec->GetBounds());
			}
		}
		i++;
	}
	me->osmData->EndGetObject(sess);
	me->nav->SetSelectedVectors(vecList);
	if (!bounds.IsZero())
	{
		Math::Coord2DDbl center = bounds.GetCenter();
		NN<Math::CoordinateSystem> csys = me->nav->GetCoordinateSystem();
		if (csys != me->osmData->GetCoordinateSystem())
		{
			center = Math::CoordinateSystem::Convert(me->osmData->GetCoordinateSystem(), csys, center);
		}
		if (!me->nav->InMap(center))
		{
			me->nav->PanToMap(center);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGISOSMDataForm::OnRelationTypeSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISOSMDataForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISOSMDataForm>();
	me->UpdateRelationList();
}

void __stdcall SSWR::AVIRead::AVIRGISOSMDataForm::OnQueryResultSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISOSMDataForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISOSMDataForm>();
	me->lvQueryTags->ClearItems();
	NN<Map::OSM::ElementInfo> elem;
	if (!me->lbQueryResult->GetSelectedItem().GetOpt<Map::OSM::ElementInfo>().SetTo(elem))
	{
		me->txtQueryType->SetText(CSTR(""));
		return;
	}
	UOSInt i;
	UOSInt j;
	NN<Data::ArrayListNN<Map::OSM::TagInfo>> tags;
	if (elem->tags.SetTo(tags))
	{
		NN<Map::OSM::TagInfo> tag;
		i = 0;
		j = tags->GetCount();
		while (i < j)
		{
			tag = tags->GetItemNoCheck(i);
			UOSInt k = me->lvQueryTags->AddItem(tag->k, tag);
			me->lvQueryTags->SetSubItem(k, 1, tag->v);
			i++;
		}
	}
	me->txtQueryType->SetText(Map::OSM::LayerTypeGetName(elem->layerType));
	me->nav->SetSelectedVector(me->osmData->CreateVector(elem));
}

UI::EventState __stdcall SSWR::AVIRead::AVIRGISOSMDataForm::OnMouseLDown(AnyType userObj, Math::Coord2D<OSInt> scnPos)
{
	NN<SSWR::AVIRead::AVIRGISOSMDataForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISOSMDataForm>();
	me->mouseDown = true;
	me->mouseDownPos = scnPos;
	return UI::EventState::ContinueEvent;
}
UI::EventState __stdcall SSWR::AVIRead::AVIRGISOSMDataForm::OnMouseLUp(AnyType userObj, Math::Coord2D<OSInt> scnPos)
{
	NN<SSWR::AVIRead::AVIRGISOSMDataForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISOSMDataForm>();
	if (!me->mouseDown)
	{
		return UI::EventState::ContinueEvent;
	}
	me->mouseDown = false;
	if (me->mouseDownPos == scnPos)
	{
		Math::Coord2DDbl mapPos = me->nav->ScnXY2MapXY(scnPos);
		Math::Coord2DDbl mapPos2 = me->nav->ScnXY2MapXY(Math::Coord2D<OSInt>(scnPos.x + 10, scnPos.y));
		Double maxDist = mapPos.CalcLengTo(mapPos2);
		NN<Math::CoordinateSystem> csys = me->nav->GetCoordinateSystem();
		if (csys != me->osmData->GetCoordinateSystem())
		{
			mapPos = Math::CoordinateSystem::Convert(csys, me->osmData->GetCoordinateSystem(), mapPos);
		}
		NN<Map::GetObjectSess> sess = me->osmData->BeginGetObject();
		Data::ArrayListNN<Map::MapDrawLayer::ObjectInfo> objs;
		NN<Map::MapDrawLayer::ObjectInfo> obj;
		me->osmData->GetNearObjects(sess, objs, mapPos, maxDist);
		me->lbQueryResult->ClearItems();
		if (me->tcMain->GetSelectedPage() != me->tpQuery)
		{
			me->tcMain->SetSelectedPage(me->tpQuery);
		}
		NN<Map::OSM::ElementInfo> elem;
		NN<Data::ArrayListNN<Map::OSM::TagInfo>> tags;
		NN<Map::OSM::TagInfo> tag;
		Int64 mapId;
		UOSInt i = 0;
		UOSInt j = objs.GetCount();
		UOSInt k;
		UOSInt l;
		if (j > 0)
		{
			Text::StringBuilderUTF8 sb;
			while (i < j)
			{
				obj = objs.GetItemNoCheck(i);
				mapId = obj->objId;
				if (me->osmData->GetElementById(mapId >> 2, (Map::OSM::ElementType)(mapId & 3)).SetTo(elem))
				{
					sb.ClearStr();
					sb.AppendI64(elem->id);
					if (elem->tags.SetTo(tags))
					{
						k = 0;
						l = tags->GetCount();
						while (k < l)
						{
							tag = tags->GetItemNoCheck(k);
							if (tag->k->Equals(UTF8STRC("name")))
							{
								sb.AppendC(UTF8STRC(" "));
								sb.Append(tag->v);
								break;
							}
							k++;
						}
					}
					me->lbQueryResult->AddItem(sb.ToCString(), elem);
				}
				i++;
			}
			me->lbQueryResult->SetSelectedIndex(0);
		}
		me->osmData->FreeObjects(objs);
		me->osmData->EndGetObject(sess);
		return UI::EventState::ContinueEvent;
	}
	return UI::EventState::ContinueEvent;
}

void __stdcall SSWR::AVIRead::AVIRGISOSMDataForm::OnShowUnknownChg(AnyType userObj, Bool newState)
{
	NN<SSWR::AVIRead::AVIRGISOSMDataForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISOSMDataForm>();
	me->osmData->SetShowUnknown(newState);
	me->nav->RedrawMap();
}

void __stdcall SSWR::AVIRead::AVIRGISOSMDataForm::OnDefaultStyleClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISOSMDataForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISOSMDataForm>();
	me->osmData->SetStyleDefault();
	me->nav->RedrawMap();
}

void __stdcall SSWR::AVIRead::AVIRGISOSMDataForm::OnCenterlineClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISOSMDataForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISOSMDataForm>();
	me->osmData->SetStyleCenterline();
	me->nav->RedrawMap();
}

void SSWR::AVIRead::AVIRGISOSMDataForm::UpdateRelationList()
{
	Text::StringBuilderUTF8 sbType;
	Data::ArrayListNN<Map::OSM::RelationInfo> relList;
	this->osmData->GetRelations(relList);
	this->cboRelationType->GetText(sbType);
	this->lbRelation->ClearItems();
	NN<Map::OSM::RelationInfo> rel;
	NN<Data::ArrayListNN<Map::OSM::TagInfo>> tags;
	Optional<Text::String> type;
	NN<Text::String> nntype;
	Text::StringBuilderUTF8 sb;
	UOSInt i = 0;
	UOSInt j = relList.GetCount();
	while (i < j)
	{
		rel = relList.GetItemNoCheck(i);
		sb.ClearStr();
		sb.AppendI64(rel->id);
		type = 0;
		if (rel->tags.SetTo(tags))
		{
			UOSInt k = 0;
			UOSInt l = tags->GetCount();
			while (k < l)
			{
				NN<Map::OSM::TagInfo> tag = tags->GetItemNoCheck(k);
				if (tag->k->Equals(UTF8STRC("name")))
				{
					sb.AppendC(UTF8STRC(" "));
					sb.Append(tag->v);
				}
				else if (tag->k->Equals(UTF8STRC("type")))
				{
					type = tag->v;
				}
				k++;
			}
		}
		if (sbType.leng == 0)
		{
			if (!type.SetTo(nntype) || nntype->leng == 0)
			{
				this->lbRelation->AddItem(sb.ToCString(), rel);
			}
		}
		else if (type.SetTo(nntype) && nntype->Equals(sbType.ToCString()))
		{
			this->lbRelation->AddItem(sb.ToCString(), rel);
		}
		i++;
	}
}

SSWR::AVIRead::AVIRGISOSMDataForm::AVIRGISOSMDataForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Map::OSM::OSMData> osmData, NN<SSWR::AVIRead::AVIRMapNavigator> nav) : UI::GUIForm(parent, 416, 408, ui)
{
	this->core = core;
	this->osmData = osmData;
	this->nav = nav;
	this->mouseDown = false;
	this->mouseDownPos = Math::Coord2D<OSInt>(0, 0);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("OSM Data - "));
	sb.Append(osmData->GetSourceNameObj());
	this->SetText(sb.ToCString());
	this->SetFont(nullptr, 8.25, false);

	this->pnlControl = this->ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->chkShowUnknown = this->ui->NewCheckBox(this->pnlControl, CSTR("Show Unknown"), osmData->IsShowUnknown());
	this->chkShowUnknown->SetRect(4, 4, 150, 23, false);
	this->chkShowUnknown->HandleCheckedChange(OnShowUnknownChg, this);
	this->btnDefaultStyle = this->ui->NewButton(this->pnlControl, CSTR("Default Style"));
	this->btnDefaultStyle->SetRect(160, 4, 100, 23, false);
	this->btnDefaultStyle->HandleButtonClick(OnDefaultStyleClicked, this);
	this->btnCenterline = this->ui->NewButton(this->pnlControl, CSTR("Centerline"));
	this->btnCenterline->SetRect(264, 4, 100, 23, false);
	this->btnCenterline->HandleButtonClick(OnCenterlineClicked, this);
	this->tcMain = this->ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpRelation = this->tcMain->AddTabPage(CSTR("Relations"));
	this->pnlRelation = this->ui->NewPanel(this->tpRelation);
	this->pnlRelation->SetRect(0, 0, 100, 31, false);
	this->pnlRelation->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblRelationType = this->ui->NewLabel(this->pnlRelation, CSTR("Type"));
	this->lblRelationType->SetRect(4, 4, 100, 23, false);
	this->cboRelationType = this->ui->NewComboBox(this->pnlRelation, false);
	this->cboRelationType->SetRect(104, 4, 150, 23, false);
	this->cboRelationType->HandleSelectionChange(OnRelationTypeSelChg, this);
	this->lbRelation = this->ui->NewListBox(this->tpRelation, false);
	this->lbRelation->SetRect(0, 0, 100, 100, false);
	this->lbRelation->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbRelation->HandleSelectionChange(OnRelationSelChg, this);
	this->lvRelationMember = this->ui->NewListView(this->tpRelation, UI::ListViewStyle::Table, 3);
	this->lvRelationMember->SetRect(0, 0, 100, 100, false);
	this->lvRelationMember->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lvRelationMember->AddColumn(CSTR("Type"), 100);
	this->lvRelationMember->AddColumn(CSTR("RefId"), 100);
	this->lvRelationMember->AddColumn(CSTR("Role"), 100);
	this->lvRelationTags = this->ui->NewListView(this->tpRelation, UI::ListViewStyle::Table, 2);
	this->lvRelationTags->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvRelationTags->AddColumn(CSTR("k"), 100);
	this->lvRelationTags->AddColumn(CSTR("v"), 200);

	this->tpQuery = this->tcMain->AddTabPage(CSTR("Query"));
	this->lbQueryResult = this->ui->NewListBox(this->tpQuery, false);
	this->lbQueryResult->SetRect(0, 0, 100, 100, false);
	this->lbQueryResult->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbQueryResult->HandleSelectionChange(OnQueryResultSelChg, this);
	this->pnlQuery = this->ui->NewPanel(this->tpQuery);
	this->pnlQuery->SetRect(0, 0, 100, 31, false);
	this->pnlQuery->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblQueryType = this->ui->NewLabel(this->pnlQuery, CSTR("Type"));
	this->lblQueryType->SetRect(4, 4, 100, 23, false);
	this->txtQueryType = this->ui->NewTextBox(this->pnlQuery, CSTR(""));
	this->txtQueryType->SetRect(104, 4, 150, 23, false);
	this->txtQueryType->SetReadOnly(true);
	this->lvQueryTags = this->ui->NewListView(this->tpQuery, UI::ListViewStyle::Table, 2);
	this->lvQueryTags->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvQueryTags->AddColumn(CSTR("k"), 100);
	this->lvQueryTags->AddColumn(CSTR("v"), 200);

	Optional<Text::String> unkType = 0;
	Data::ArrayListNN<Map::OSM::RelationInfo> relList;
	this->osmData->GetRelations(relList);
	NN<Data::ArrayListNN<Map::OSM::TagInfo>> tags;
	Data::ArrayListStringNN typeList;
	NN<Map::OSM::RelationInfo> rel;
	Optional<Text::String> type;
	NN<Text::String> nntype;
	UOSInt i = 0;
	UOSInt j = relList.GetCount();
	while (i < j)
	{
		rel = relList.GetItemNoCheck(i);
		type = 0;
		if (rel->tags.SetTo(tags))
		{
			UOSInt k = 0;
			UOSInt l = tags->GetCount();
			while (k < l)
			{
				NN<Map::OSM::TagInfo> tag = tags->GetItemNoCheck(k);
				if (tag->k->Equals(UTF8STRC("type")))
				{
					type = tag->v;
				}
				k++;
			}
		}
		if (type.SetTo(nntype))
		{
			if (typeList.SortedIndexOf(nntype) < 0)
			{
				typeList.SortedInsert(nntype);
			}
		}
		else if (unkType.IsNull())
		{
			unkType = nntype = Text::String::New(CSTR("unknown"));
			typeList.SortedInsert(nntype);
		}
		i++;
	}
	i = 0;
	j = typeList.GetCount();
	while (i < j)
	{
		this->cboRelationType->AddItem(typeList.GetItemNoCheck(i), 0);
		i++;
	}
	OPTSTR_DEL(unkType);
	if (j > 0)
	{
		this->cboRelationType->SetSelectedIndex(0);
	}

	this->nav->HandleMapMouseLDown(OnMouseLDown, this);
	this->nav->HandleMapMouseLUp(OnMouseLUp, this);
}

SSWR::AVIRead::AVIRGISOSMDataForm::~AVIRGISOSMDataForm()
{
	this->nav->UnhandleMapMouse(this);
}

void SSWR::AVIRead::AVIRGISOSMDataForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
