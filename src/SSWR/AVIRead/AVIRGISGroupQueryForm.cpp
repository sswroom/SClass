#include "Stdafx.h"
#include "Math/CoordinateSystemConverter.h"
#include "SSWR/AVIRead/AVIRGISGroupQueryForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Bool __stdcall SSWR::AVIRead::AVIRGISGroupQueryForm::OnMouseDown(AnyType userObj, Math::Coord2D<OSInt> scnPos)
{
	NN<SSWR::AVIRead::AVIRGISGroupQueryForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISGroupQueryForm>();
	me->downPos = scnPos;
	return false;
}

Bool __stdcall SSWR::AVIRead::AVIRGISGroupQueryForm::OnMouseUp(AnyType userObj, Math::Coord2D<OSInt> scnPos)
{
	NN<SSWR::AVIRead::AVIRGISGroupQueryForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISGroupQueryForm>();
	if (me->downPos == scnPos)
	{
		Math::Coord2DDbl mapEnvPos;
		Math::Coord2DDbl mapLyrPos;
		Math::Coord2DDbl ptNear = {0, 0};
		Int64 nearId = -1;
		Math::Coord2DDbl nearPos;
		NN<Map::GetObjectSess> sess;
		Int64 id;
		UOSInt i;
		UOSInt j;
		UOSInt k;
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		Data::ArrayListNN<Map::MapDrawLayer> layers;
		NN<Map::MapDrawLayer> lyr;
		NN<Math::CoordinateSystem> csysEnv = me->navi->GetCoordinateSystem();
		NN<Math::CoordinateSystem> csysLyr;
		Text::StringBuilderUTF8 sb;
		mapEnvPos = me->navi->ScnXY2MapXY(scnPos);
		me->env->GetLayersInGroup(me->group, layers);
		me->lvInfo->ClearItems();
		me->cboItem->ClearItems();
		me->ClearQueryResults();
		k = layers.GetCount();
		while (k-- > 0)
		{
			if (layers.GetItem(k).SetTo(lyr))
			{
				sess = lyr->BeginGetObject();
				nearPos = {0, 0};
				csysLyr = lyr->GetCoordinateSystem();
				if (!csysEnv->Equals(csysLyr))
				{
					mapLyrPos = Math::CoordinateSystem::Convert(csysEnv, csysLyr, mapEnvPos);
				}
				else
				{
					mapLyrPos = mapEnvPos;
				}
				id = lyr->GetNearestObjectId(sess, mapLyrPos, nearPos);
				if (id != -1)
				{
					nearId = -1;
					Map::DrawLayerType lyrType = lyr->GetLayerType();
					if (lyrType == Map::DRAW_LAYER_POINT3D || lyrType == Map::DRAW_LAYER_POINT || lyrType == Map::DRAW_LAYER_POLYLINE || lyrType == Map::DRAW_LAYER_POLYLINE3D)
					{
						if (!csysEnv->Equals(csysLyr))
						{
							ptNear = Math::CoordinateSystem::Convert(csysLyr, csysEnv, nearPos);
						}
						else
						{
							ptNear = nearPos;
						}
						Math::Coord2D<OSInt> nearScn;
						nearScn = me->navi->MapXY2ScnXY(ptNear);
						if (nearScn.x < scnPos.x)
						{
							nearScn.x = scnPos.x - nearScn.x;
						}
						else
						{
							nearScn.x = nearScn.x - scnPos.x;
						}
						if (nearScn.y < scnPos.y)
						{
							nearScn.y = scnPos.y - nearScn.y;
						}
						else
						{
							nearScn.y = nearScn.y - scnPos.y;
						}
						if (nearScn.x < 10 && nearScn.x < 10)
						{
							nearId = id;
						}
					}
					else if (mapLyrPos == nearPos)
					{
						if (nearId == -1)
						{
							nearId = id;
						}
					}
					NN<Math::Geometry::Vector2D> vec;
					if (nearId != -1 && lyr->GetNewVectorById(sess, id).SetTo(vec))
					{
						Data::ArrayListInt64 arr;
						Optional<Map::NameArray> nameArr;
						sb.ClearStr();
						sb.Append(lyr->GetName());
						sb.Append(CSTR(" - "));
						sb.AppendI64(nearId);
						me->cboItem->AddItem(sb.ToCString(), 0);
						me->queryValueOfstList.Add(me->queryValueList.GetCount());
						csysLyr = lyr->GetCoordinateSystem();
						if (!csysEnv->Equals(csysLyr))
						{
							mapLyrPos = Math::CoordinateSystem::Convert(csysEnv, csysLyr, mapEnvPos);
						}
						else
						{
							mapLyrPos = mapEnvPos;
						}
						lyr->GetObjectIdsMapXY(arr, nameArr, Math::RectAreaDbl(mapLyrPos, mapLyrPos), true);
						i = 0;
						j = lyr->GetColumnCnt();
						while (i < j)
						{
							sbuff[0] = 0;
							sptr = lyr->GetColumnName(sbuff, i).Or(sbuff);
							me->queryNameList.Add(Text::String::NewP(sbuff, sptr));
							sb.ClearStr();
							lyr->GetString(sb, nameArr, id, i);
							me->queryValueList.Add(Text::String::New(sb.ToCString()));
							i++;
						}
						me->queryVecOriList.Add(vec->Clone());
						if (!csysEnv->Equals(csysLyr))
						{
							Math::CoordinateSystemConverter converter(csysLyr, csysEnv);
							vec->Convert(converter);
						}
						me->queryVecList.Add(vec);
						lyr->ReleaseNameArr(nameArr);
					}
				}
				lyr->EndGetObject(sess);
			}
		}
		me->navi->SetSelectedVectors(me->queryVecList);
		if (me->queryVecList.GetCount() > 0)
		{
			me->cboItem->SetSelectedIndex(0);
			me->SetQueryItem(0);
		}
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRGISGroupQueryForm::OnItemSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISGroupQueryForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISGroupQueryForm>();
	me->SetQueryItem(me->cboItem->GetSelectedIndex());
}

void SSWR::AVIRead::AVIRGISGroupQueryForm::ClearQueryResults()
{
	NN<Text::String> value;
	this->queryNameList.FreeAll();
	UOSInt i = this->queryValueList.GetCount();
	while (i-- > 0)
	{
		value = this->queryValueList.GetItemNoCheck(i);
		value->Release();
	}
	this->queryNameList.Clear();
	this->queryValueList.Clear();
	this->queryValueOfstList.Clear();
	this->queryVecList.Clear();
	this->queryVecOriList.DeleteAll();
}

void SSWR::AVIRead::AVIRGISGroupQueryForm::SetQueryItem(UOSInt index)
{
//	NN<Math::VectorTextWriter> writer;
	NN<Math::Geometry::Vector2D> vec;
//	if (!this->queryVecList.GetItem(index).SetTo(vec) || !this->cboShapeFmt->GetSelectedItem().GetOpt<Math::VectorTextWriter>().SetTo(writer))
	if (!this->queryVecList.GetItem(index).SetTo(vec))
		return;
//	this->currVec.Delete();
//	this->currVec = vec->Clone();

	this->queryVecOriList.GetItem(index).SetTo(vec);
//	UTF8Char sbuff[64];
//	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Optional<Text::String> name;
	Optional<Text::String> value;
	this->lvInfo->ClearItems();
	i = this->queryValueOfstList.GetItem(index);
	j = this->queryValueOfstList.GetItem(index + 1);
	if (j == 0)
		j = this->queryNameList.GetCount();
	NN<Text::String> valueStr;
	while (i < j)
	{
		name = this->queryNameList.GetItem(i);
		value = this->queryValueList.GetItem(i);
		k = this->lvInfo->AddItem(Text::String::OrEmpty(name), 0);
		if (value.SetTo(valueStr))
		{
			this->lvInfo->SetSubItem(k, 1, valueStr);
		}
		i++;
	}
/*	Math::RectAreaDbl bounds = vec->GetBounds();

	Text::StringBuilderUTF8 sb;
	writer->ToText(sb, vec);
	this->txtShape->SetText(sb.ToCString());
	NN<Math::CoordinateSystem> csys = this->lyr->GetCoordinateSystem();
	sb.ClearStr();
	sb.AppendDouble(csys->CalDistance(vec, false, Math::Unit::Distance::DU_METER));
	this->txtShapeHLength->SetText(sb.ToCString());
	sb.ClearStr();
	sb.AppendDouble(csys->CalDistance(vec, true, Math::Unit::Distance::DU_METER));
	this->txtShapeLength->SetText(sb.ToCString());
	sb.ClearStr();
	if (csys->IsProjected())
	{
		sb.AppendDouble(vec->CalArea());
	}
	else
	{
		sb.AppendDouble(vec->CalArea() / 0.0174532925199433 / 0.0174532925199433);
	}
	this->txtShapeArea->SetText(sb.ToCString());

	sptr = Text::StrDouble(sbuff, bounds.min.x);
	this->txtMinX->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, bounds.min.y);
	this->txtMinY->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, bounds.max.x);
	this->txtMaxX->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, bounds.max.y);
	this->txtMaxY->SetText(CSTRP(sbuff, sptr));*/
}

SSWR::AVIRead::AVIRGISGroupQueryForm::AVIRGISGroupQueryForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<AVIRMapNavigator> navi, NN<Map::MapEnv> env, Optional<Map::MapEnv::GroupItem> group) : UI::GUIForm(parent, 416, 408, ui)
{
	Text::StringBuilderUTF8 sb;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->navi = navi;
	this->env = env;
	this->group = group;
	sb.AppendC(UTF8STRC("Group Query - "));
	NN<Map::MapEnv::GroupItem> nngroup;
	if (!group.SetTo(nngroup))
	{
		sb.AppendC(UTF8STRC("ROOT"));
	}
	else
	{
		NN<Text::String> name = env->GetGroupName(nngroup);
		sb.Append(name);
	}
	this->SetText(sb.ToCString());
	this->SetFont(0, 0, 8.25, false);

	this->cboItem = ui->NewComboBox(*this, false);
	this->cboItem->SetRect(0, 0, 100, 23, false);
	this->cboItem->SetDockType(UI::GUIControl::DOCK_TOP);
	this->cboItem->HandleSelectionChange(OnItemSelChg, this);
	this->lvInfo = ui->NewListView(*this, UI::ListViewStyle::Table, 2);
	this->lvInfo->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvInfo->AddColumn(CSTR("Name"), 100);
	this->lvInfo->AddColumn(CSTR("Value"), 300);
	this->lvInfo->SetShowGrid(true);
	this->lvInfo->SetFullRowSelect(true);

	this->navi->HandleMapMouseLDown(OnMouseDown, this);
	this->navi->HandleMapMouseLUp(OnMouseUp, this);
}

SSWR::AVIRead::AVIRGISGroupQueryForm::~AVIRGISGroupQueryForm()
{
	this->navi->UnhandleMapMouse(this);
	this->ClearQueryResults();
}

void SSWR::AVIRead::AVIRGISGroupQueryForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
