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
		UOSInt ptNearInd = 0;
		Double ptNearDist = 0;
		Int64 ptNearId = -1;
		UOSInt pgNearInd = 0;
		Int64 pgNearId = -1;
		Math::Coord2DDbl nearPos;
		NN<Map::GetObjectSess> sess;
		Int64 id;
		UOSInt i;
		UOSInt j;
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		Data::ArrayListNN<Map::MapDrawLayer> layers;
		NN<Map::MapDrawLayer> lyr;
		NN<Math::CoordinateSystem> csysEnv = me->navi->GetCoordinateSystem();
		NN<Math::CoordinateSystem> csysLyr;
		mapEnvPos = me->navi->ScnXY2MapXY(scnPos);
		me->env->GetLayersInGroup(me->group, layers);
		i = layers.GetCount();
		while (i-- > 0)
		{
			if (layers.GetItem(i).SetTo(lyr))
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
					Map::DrawLayerType lyrType = lyr->GetLayerType();
					if (lyrType == Map::DRAW_LAYER_POINT3D || lyrType == Map::DRAW_LAYER_POINT || lyrType == Map::DRAW_LAYER_POLYLINE || lyrType == Map::DRAW_LAYER_POLYLINE3D)
					{
						Double dist = nearPos.CalcLengTo(mapLyrPos);
						if (ptNearId == -1 || ptNearDist > dist)
						{
							ptNearId = id;
							ptNearInd = i;
							ptNearDist = dist;
							if (!csysEnv->Equals(csysLyr))
							{
								ptNear = Math::CoordinateSystem::Convert(csysLyr, csysEnv, nearPos);
							}
							else
							{
								ptNear = nearPos;
							}
						}
					}
					else if (mapLyrPos == nearPos)
					{
						if (pgNearId == -1)
						{
							pgNearInd = i;
							pgNearId = id;
						}
					}
				}
				lyr->EndGetObject(sess);
			}
		}

		Optional<Map::MapDrawLayer> optLyr;
		optLyr = 0;
		id = -1;
		if (pgNearId != -1)
		{
			id = pgNearId;
			optLyr = layers.GetItem(pgNearInd);
		}
		if (ptNearId != -1)
		{
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
				id = ptNearId;
				optLyr = layers.GetItem(ptNearInd);
			}
		}

		me->lvInfo->ClearItems();
		me->cboItem->ClearItems();
		if (id == -1 || !optLyr.SetTo(lyr))
		{
			me->navi->SetSelectedVector(0);
		}
		else
		{
			Data::ArrayListInt64 arr;
			Optional<Map::NameArray> nameArr;
			me->cboItem->AddItem(lyr->GetName()->ToCString(), 0);
			csysLyr = lyr->GetCoordinateSystem();
			if (!csysEnv->Equals(csysLyr))
			{
				mapLyrPos = Math::CoordinateSystem::Convert(csysEnv, csysLyr, mapEnvPos);
			}
			else
			{
				mapLyrPos = mapEnvPos;
			}
			Text::StringBuilderUTF8 sb;
			sess = lyr->BeginGetObject();
			lyr->GetObjectIdsMapXY(arr, nameArr, Math::RectAreaDbl(mapLyrPos, mapLyrPos), true);
			i = 0;
			j = lyr->GetColumnCnt();
			while (i < j)
			{
				sbuff[0] = 0;
				sptr = lyr->GetColumnName(sbuff, i).Or(sbuff);
				me->lvInfo->AddItem(CSTRP(sbuff, sptr), 0);
				sb.ClearStr();
				lyr->GetString(sb, nameArr, id, i);
				me->lvInfo->SetSubItem(i, 1, sb.ToCString());
				i++;
			}
			NN<Math::Geometry::Vector2D> vec;
			if (lyr->GetNewVectorById(sess, id).SetTo(vec))
			{
				if (!csysEnv->Equals(csysLyr))
				{
					Math::CoordinateSystemConverter converter(csysLyr, csysEnv);
					vec->Convert(converter);
				}
				me->navi->SetSelectedVector(vec);
			}
			else
			{
				me->navi->SetSelectedVector(0);
			}
			lyr->ReleaseNameArr(nameArr);
			lyr->EndGetObject(sess);
		}
	}
	return false;
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
}

void SSWR::AVIRead::AVIRGISGroupQueryForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
