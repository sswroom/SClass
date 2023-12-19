#include "Stdafx.h"
#include "Math/CoordinateSystemConverter.h"
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
		Data::ArrayListNN<Map::MapDrawLayer> layers;
		NotNullPtr<Map::MapDrawLayer> lyr;
		NotNullPtr<Math::CoordinateSystem> csysEnv = me->navi->GetCoordinateSystem();
		NotNullPtr<Math::CoordinateSystem> csysLyr;
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
				optLyr = layers.GetItem(ptNearInd);
			}
		}

		me->lvInfo->ClearItems();
		if (id == -1 || !optLyr.SetTo(lyr))
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
			lyr->GetObjectIdsMapXY(arr, &nameArr, Math::RectAreaDbl(mapLyrPos, mapLyrPos), true);
			i = 0;
			j = lyr->GetColumnCnt();
			while (i < j)
			{
				sbuff[0] = 0;
				sptr = lyr->GetColumnName(sbuff, i);
				me->lvInfo->AddItem(CSTRP(sbuff, sptr), 0);
				sb.ClearStr();
				lyr->GetString(sb, nameArr, id, i);
				me->lvInfo->SetSubItem(i, 1, sb.ToCString());
				i++;
			}
			Math::Geometry::Vector2D *vec = lyr->GetNewVectorById(sess, id);
			if (!csysEnv->Equals(csysLyr))
			{
				Math::CoordinateSystemConverter converter(csysLyr, csysEnv);
				vec->Convert(converter);
			}
			me->navi->SetSelectedVector(vec);
			lyr->ReleaseNameArr(nameArr);
			lyr->EndGetObject(sess);
		}
	}
	return false;
}

SSWR::AVIRead::AVIRGISGroupQueryForm::AVIRGISGroupQueryForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IMapNavigator *navi, NotNullPtr<Map::MapEnv> env, Map::MapEnv::GroupItem *group) : UI::GUIForm(parent, 416, 408, ui)
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

	this->txtLayer = ui->NewTextBox(*this, CSTR(""));
	this->txtLayer->SetRect(0, 0, 100, 23, false);
	this->txtLayer->SetReadOnly(true);
	this->txtLayer->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lvInfo, UI::GUIListView(ui, *this, UI::GUIListView::LVSTYLE_TABLE, 2));
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
