#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRGISGroupQueryForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Bool __stdcall SSWR::AVIRead::AVIRGISGroupQueryForm::OnMouseDown(void *userObj, Math::Coord2D<OSInt> scnPos)
{
	SSWR::AVIRead::AVIRGISGroupQueryForm *me = (SSWR::AVIRead::AVIRGISGroupQueryForm*)userObj;
	me->downPos = scnPos;
	return false;
}

Bool __stdcall SSWR::AVIRead::AVIRGISGroupQueryForm::OnMouseUp(void *userObj, Math::Coord2D<OSInt> scnPos)
{
	SSWR::AVIRead::AVIRGISGroupQueryForm *me = (SSWR::AVIRead::AVIRGISGroupQueryForm*)userObj;
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
		Map::GetObjectSess *sess;
		Int64 id;
		UOSInt i;
		UOSInt j;
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		Data::ArrayList<Map::MapDrawLayer*> layers;
		Map::MapDrawLayer *lyr;
		Math::CoordinateSystem *csysEnv = me->navi->GetCoordinateSystem();
		Math::CoordinateSystem *csysLyr;
		Double zTemp;
		mapEnvPos = me->navi->ScnXY2MapXY(scnPos);
		me->env->GetLayersInGroup(me->group, &layers);
		i = layers.GetCount();
		while (i-- > 0)
		{
			lyr = layers.GetItem(i);
			sess = lyr->BeginGetObject();
			nearPos = {0, 0};
			csysLyr = lyr->GetCoordinateSystem();
			if (csysEnv != 0 && csysLyr != 0 && !csysEnv->Equals(csysLyr))
			{
				Math::CoordinateSystem::ConvertXYZ(csysEnv, csysLyr, mapEnvPos.x, mapEnvPos.y, 0, &mapLyrPos.x, &mapLyrPos.y, &zTemp);
			}
			else
			{
				mapLyrPos = mapEnvPos;
			}
			id = lyr->GetNearestObjectId(sess, mapLyrPos, &nearPos);
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
						ptNear = nearPos;
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

		lyr = 0;
		id = -1;
		if (pgNearId != -1)
		{
			id = pgNearId;
			lyr = layers.GetItem(pgNearInd);
		}
		if (ptNearId != -1)
		{
			Math::Coord2D<OSInt> nearScn = me->navi->MapXY2ScnXY(ptNear);
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
				lyr = layers.GetItem(ptNearInd);
			}
		}

		me->lvInfo->ClearItems();
		if (id == -1)
		{
			me->txtLayer->SetText(CSTR(""));
			me->navi->SetSelectedVector(0);
		}
		else
		{
			Data::ArrayListInt64 arr;
			Map::NameArray *nameArr;
			me->txtLayer->SetText(lyr->GetName()->ToCString());
			csysLyr = lyr->GetCoordinateSystem();
			if (csysEnv != 0 && csysLyr != 0 && !csysEnv->Equals(csysLyr))
			{
				Math::CoordinateSystem::ConvertXYZ(csysEnv, csysLyr, mapEnvPos.x, mapEnvPos.y, 0, &mapLyrPos.x, &mapLyrPos.y, &zTemp);
			}
			else
			{
				mapLyrPos = mapEnvPos;
			}
			sess = lyr->BeginGetObject();
			lyr->GetObjectIdsMapXY(arr, &nameArr, Math::RectAreaDbl(mapLyrPos, mapLyrPos), true);
			i = 0;
			j = lyr->GetColumnCnt();
			while (i < j)
			{
				sbuff[0] = 0;
				sptr = lyr->GetColumnName(sbuff, i);
				me->lvInfo->AddItem(CSTRP(sbuff, sptr), 0);
				sptr = lyr->GetString(sbuff, sizeof(sbuff), nameArr, id, i);
				me->lvInfo->SetSubItem(i, 1, CSTRP(sbuff, sptr));
				i++;
			}
			Math::Geometry::Vector2D *vec = lyr->GetNewVectorById(sess, id);
			if (csysEnv != 0 && csysLyr != 0 && !csysEnv->Equals(csysLyr))
			{
				vec->ConvCSys(csysLyr, csysEnv);
			}
			me->navi->SetSelectedVector(vec);
			lyr->ReleaseNameArr(nameArr);
			lyr->EndGetObject(sess);
		}
	}
	return false;
}

SSWR::AVIRead::AVIRGISGroupQueryForm::AVIRGISGroupQueryForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IMapNavigator *navi, Map::MapEnv *env, Map::MapEnv::GroupItem *group) : UI::GUIForm(parent, 416, 408, ui)
{
	Text::StringBuilderUTF8 sb;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->navi = navi;
	this->env = env;
	this->group = group;
	sb.AppendC(UTF8STRC("Group Query - "));
	if (group == 0)
	{
		sb.AppendC(UTF8STRC("ROOT"));
	}
	else
	{
		NotNullPtr<Text::String> name = env->GetGroupName(group);
		sb.Append(name);
	}
	this->SetText(sb.ToCString());
	this->SetFont(0, 0, 8.25, false);

	NEW_CLASS(this->txtLayer, UI::GUITextBox(ui, this, CSTR("")));
	this->txtLayer->SetRect(0, 0, 100, 23, false);
	this->txtLayer->SetReadOnly(true);
	this->txtLayer->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lvInfo, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvInfo->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvInfo->AddColumn(CSTR("Name"), 100);
	this->lvInfo->AddColumn(CSTR("Value"), 300);
	this->lvInfo->SetShowGrid(true);
	this->lvInfo->SetFullRowSelect(true);

	this->navi->HandleMapMouseDown(OnMouseDown, this);
	this->navi->HandleMapMouseUp(OnMouseUp, this);
}

SSWR::AVIRead::AVIRGISGroupQueryForm::~AVIRGISGroupQueryForm()
{
	this->navi->UnhandleMapMouse(this);
}

void SSWR::AVIRead::AVIRGISGroupQueryForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
