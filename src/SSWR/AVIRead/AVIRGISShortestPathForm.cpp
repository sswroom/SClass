#include "Stdafx.h"
#include "DB/TableDef.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/CoordinateSystemConverter.h"
#include "SSWR/AVIRead/AVIRGISShortestPathForm.h"

void __stdcall SSWR::AVIRead::AVIRGISShortestPathForm::OnNetworkClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISShortestPathForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISShortestPathForm>();
	UOSInt i = me->cboNetwork->GetSelectedItem().GetUOSInt();
	if (i > 0)
	{
		Data::ArrayListNN<Math::Geometry::LineString> lineList;
		me->spath.GetNetworkLines(lineList, (UInt32)i);
		Data::ArrayListNN<Math::Geometry::Vector2D> vecList;
		UOSInt j = lineList.GetCount();
		if (j > 0)
		{
			Math::RectAreaDbl rect;
			NN<Math::Geometry::Vector2D> vec = lineList.GetItemNoCheck(0)->Clone();
			rect = vec->GetBounds();
			vecList.Add(vec);
			i = 1;
			while (i < j)
			{
				vec = lineList.GetItemNoCheck(i)->Clone();
				rect = rect.MergeArea(vec->GetBounds());
				vecList.Add(vec);
				i++;
			}
			me->navi->SetSelectedVectors(vecList);
			Math::Coord2DDbl center = rect.GetCenter();
			if (vec->GetSRID() != me->navi->GetSRID())
			{
				NN<Math::CoordinateSystem> srcCsys = Math::CoordinateSystemManager::SRCreateCSysOrDef(vec->GetSRID());
				NN<Math::CoordinateSystem> mapCsys = me->navi->GetCoordinateSystem();
				center = Math::CoordinateSystem::Convert(srcCsys, mapCsys, center);
				Math::CoordinateSystemConverter converter(srcCsys, mapCsys);
				i = 0;
				while (i < j)
				{
					vecList.GetItemNoCheck(i)->Convert(converter);
					i++;
				}
				srcCsys.Delete();
			}
			me->navi->PanToMap(center);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGISShortestPathForm::OnStartPosClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISShortestPathForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISShortestPathForm>();
	me->mode = 1;
}

void __stdcall SSWR::AVIRead::AVIRGISShortestPathForm::OnEndPosClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISShortestPathForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISShortestPathForm>();
	me->mode = 2;
}

void __stdcall SSWR::AVIRead::AVIRGISShortestPathForm::OnSearchClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISShortestPathForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISShortestPathForm>();
	if (!me->startPos.IsZero() && !me->endPos.IsZero())
	{
		NN<Math::CoordinateSystem> mapCsys = me->navi->GetCoordinateSystem();
		NN<Math::CoordinateSystem> pathCsys = me->spath.GetCoordinateSystem();
		Math::Coord2DDbl startPos = me->startPos;
		Math::Coord2DDbl endPos = me->endPos;
		Data::ArrayListNN<Math::Geometry::LineString> lineList;
		Data::ArrayListNN<Math::Geometry::Vector2D> vecList;
		Data::ArrayListT<Data::DataArray<Optional<Text::String>>> propList;
		NN<Math::Geometry::Vector2D> vec;
		UOSInt i;
		UOSInt j;
		if (!pathCsys->Equals(mapCsys))
		{
			startPos = Math::CoordinateSystem::Convert(mapCsys, pathCsys, startPos);
			endPos = Math::CoordinateSystem::Convert(mapCsys, pathCsys, endPos);
			if (me->spath.GetShortestPathDetail(me->sess, startPos, endPos, lineList, propList))
			{
				Math::CoordinateSystemConverter converter(pathCsys, mapCsys);
				i = 0;
				j = lineList.GetCount();
				while (i < j)
				{
					vec = lineList.GetItemNoCheck(i)->Clone();
					vec->Convert(converter);
					vecList.Add(vec);
					i++;
				}
				me->navi->SetSelectedVectors(vecList);
				me->UpdatePaths(lineList, propList);
				me->spathLineList.Clear();
				me->spathLineList.AddAll(lineList);
			}
			else
			{
				me->lvPaths->ClearItems();
				me->spathLineList.Clear();
			}
		}
		else
		{
			if (me->spath.GetShortestPathDetail(me->sess, startPos, endPos, lineList, propList))
			{
				i = 0;
				j = lineList.GetCount();
				while (i < j)
				{
					vecList.Add(lineList.GetItemNoCheck(i)->Clone());
					i++;
				}
				me->navi->SetSelectedVectors(vecList);
				me->UpdatePaths(lineList, propList);
				me->spathLineList.Clear();
				me->spathLineList.AddAll(lineList);
			}
			else
			{
				me->lvPaths->ClearItems();
				me->spathLineList.Clear();
			}
		}
	}
}

Bool __stdcall SSWR::AVIRead::AVIRGISShortestPathForm::OnMouseDown(AnyType userObj, Math::Coord2D<OSInt> scnPos)
{
	NN<SSWR::AVIRead::AVIRGISShortestPathForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISShortestPathForm>();
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	if (me->mode == 1)
	{
		me->startPos = me->navi->ScnXY2MapXY(scnPos);
		sptr = me->Coord2DDblToString(sbuff, me->startPos);
		me->txtStartPos->SetText(CSTRP(sbuff, sptr));
		me->mode = 0;
		return true;
	}
	else if (me->mode == 2)
	{
		me->endPos = me->navi->ScnXY2MapXY(scnPos);
		sptr = me->Coord2DDblToString(sbuff, me->endPos);
		me->txtEndPos->SetText(CSTRP(sbuff, sptr));
		me->mode = 0;
		return true;
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRGISShortestPathForm::OnPathsSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISShortestPathForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISShortestPathForm>();
	NN<Math::Geometry::LineString> line;
	NN<Math::Geometry::Vector2D> vec;
	if (me->lvPaths->GetSelectedItem().GetOpt<Math::Geometry::LineString>().SetTo(line))
	{
		NN<Math::CoordinateSystem> mapCsys = me->navi->GetCoordinateSystem();
		NN<Math::CoordinateSystem> pathCsys = me->spath.GetCoordinateSystem();
		if (!pathCsys->Equals(mapCsys))
		{
			Math::CoordinateSystemConverter converter(pathCsys, mapCsys);
			vec = line->Clone();
			vec->Convert(converter);
			me->navi->SetSelectedVector(vec);
		}
		else
		{
			me->navi->SetSelectedVector(line->Clone());
		}
	}
}

UnsafeArray<UTF8Char> SSWR::AVIRead::AVIRGISShortestPathForm::Coord2DDblToString(UnsafeArray<UTF8Char> sbuff, Math::Coord2DDbl coord)
{
	return Text::StrDouble(Text::StrConcatC(Text::StrDouble(sbuff, coord.GetLat()), UTF8STRC(", ")), coord.GetLon());
}

void SSWR::AVIRead::AVIRGISShortestPathForm::UpdatePaths(NN<Data::ArrayListNN<Math::Geometry::LineString>> lineList, NN<Data::ArrayListT<Data::DataArray<Optional<Text::String>>>> propList)
{
	Data::ArrayList<Double> dirList;
	Data::ArrayList<Bool> reverseList;
	Map::ShortestPath3D::CalcDirReverse(lineList, dirList, reverseList);
	UOSInt eName = INVALID_INDEX;
	UOSInt cName = INVALID_INDEX;
	UOSInt type = INVALID_INDEX;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	NN<DB::TableDef> tableDef;
	NN<DB::ColDef> colDef;
	if (this->spath.GetPropDef().SetTo(tableDef))
	{
		i = tableDef->GetColCnt();
		while (i-- > 0)
		{
			if (tableDef->GetCol(i).SetTo(colDef))
			{
				if (colDef->GetColName()->StartsWith(CSTR("AliasNameE")))
				{
					eName = i;
				}
				else if (colDef->GetColName()->StartsWith(CSTR("AliasNameT")))
				{
					cName = i;
				}
				else if (colDef->GetColName()->StartsWith(CSTR("FeatureTyp")))
				{
					type = i;
				}
			}
		}
	}
	Data::DataArray<Optional<Text::String>> prop;
	NN<Math::Geometry::LineString> line;
	NN<Text::String> s;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	Double dist;
	this->lvPaths->ClearItems();
	i = 0;
	j = propList->GetCount();
	while (i < j)
	{
		line = lineList->GetItemNoCheck(i);
		dist = line->Calc3DLength();
		prop = propList->GetItem(i);
		sptr = Text::StrDouble(sbuff, dist);
		k = this->lvPaths->AddItem(CSTRP(sbuff, sptr), line);
		if (i < dirList.GetCount())
		{
			sptr = Text::StrDouble(sbuff, dirList.GetItem(i));
			this->lvPaths->SetSubItem(k, 1, CSTRP(sbuff, sptr));
		}
		if (eName != INVALID_INDEX && prop[eName].SetTo(s))
		{
			this->lvPaths->SetSubItem(k, 2, s->ToCString());
		}
		if (cName != INVALID_INDEX && prop[cName].SetTo(s))
		{
			this->lvPaths->SetSubItem(k, 3, s->ToCString());
		}
		if (type != INVALID_INDEX && prop[type].SetTo(s))
		{
			this->lvPaths->SetSubItem(k, 4, s->ToCString());
		}
		i++;
	}
}

SSWR::AVIRead::AVIRGISShortestPathForm::AVIRGISShortestPathForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<AVIRMapNavigator> navi, NN<Map::MapDrawLayer> layer) : UI::GUIForm(parent, 320, 360, ui), spath(layer, 100.0)
{
	this->core = core;
	this->navi = navi;
	this->layer = layer;
	this->sess = this->spath.CreateSession();
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetText(CSTR("Shortest Path"));
	this->SetFont(0, 0, 8.25, false);
	this->mode = 0;
	this->startPos = Math::Coord2DDbl(0, 0);
	this->endPos = Math::Coord2DDbl(0, 0);

	this->grpNetwork = ui->NewGroupBox(*this, CSTR("Network"));
	this->grpNetwork->SetRect(4, 4, 100, 31, false);
	this->grpNetwork->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblNetwork = ui->NewLabel(this->grpNetwork, CSTR("Network"));
	this->lblNetwork->SetRect(4, 4, 100, 23, false);
	this->cboNetwork = ui->NewComboBox(this->grpNetwork, false);
	this->cboNetwork->SetRect(104, 4, 150, 23, false);
	this->btnNetwork = ui->NewButton(this->grpNetwork, CSTR("Show"));
	this->btnNetwork->SetRect(254, 4, 75, 23, false);
	this->btnNetwork->HandleButtonClick(OnNetworkClicked, this);
	this->grpPath = ui->NewGroupBox(*this, CSTR("Path"));
	this->grpPath->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pnlPath = ui->NewPanel(this->grpPath);
	this->pnlPath->SetRect(0, 0, 100, 83, false);
	this->pnlPath->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblStartPos = ui->NewLabel(this->pnlPath, CSTR("Start Pos"));
	this->lblStartPos->SetRect(4, 4, 100, 23, false);
	this->txtStartPos = ui->NewTextBox(this->pnlPath, CSTR(""), false);
	this->txtStartPos->SetRect(104, 4, 150, 23, false);
	this->txtStartPos->SetReadOnly(true);
	this->btnStartPos = ui->NewButton(this->pnlPath, CSTR("Select"));
	this->btnStartPos->SetRect(254, 4, 75, 23, false);
	this->btnStartPos->HandleButtonClick(OnStartPosClicked, this);
	this->lblEndPos = ui->NewLabel(this->pnlPath, CSTR("End Pos"));
	this->lblEndPos->SetRect(4, 28, 100, 23, false);
	this->txtEndPos = ui->NewTextBox(this->pnlPath, CSTR(""), false);
	this->txtEndPos->SetRect(104, 28, 150, 23, false);
	this->txtEndPos->SetReadOnly(true);
	this->btnEndPos = ui->NewButton(this->pnlPath, CSTR("Select"));
	this->btnEndPos->SetRect(254, 28, 75, 23, false);
	this->btnEndPos->HandleButtonClick(OnEndPosClicked, this);
	this->btnSearch = ui->NewButton(this->pnlPath, CSTR("Search"));
	this->btnSearch->SetRect(104, 52, 75, 23, false);
	this->btnSearch->HandleButtonClick(OnSearchClicked, this);
	this->lvPaths = ui->NewListView(this->grpPath, UI::ListViewStyle::Table, 5);
	this->lvPaths->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvPaths->AddColumn(CSTR("Dist"), 100);
	this->lvPaths->AddColumn(CSTR("Dir"), 100);
	this->lvPaths->AddColumn(CSTR("CName"), 60);
	this->lvPaths->AddColumn(CSTR("EName"), 100);
	this->lvPaths->AddColumn(CSTR("Type"), 100);
	this->lvPaths->HandleSelChg(OnPathsSelChg, this);

	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr;
	UOSInt networkCnt = this->spath.GetNetworkCnt();
	UOSInt i = 1;
	while (i <= networkCnt)
	{
		sptr = Text::StrUOSInt(sbuff, i);
		this->cboNetwork->AddItem(CSTRP(sbuff, sptr), AnyType((void*)i));
		i++;
	}
	if (networkCnt > 0)
	{
		this->cboNetwork->SetSelectedIndex(0);
	}
	this->navi->HandleMapMouseLDown(OnMouseDown, this);
}

SSWR::AVIRead::AVIRGISShortestPathForm::~AVIRGISShortestPathForm()
{
	this->spath.FreeSession(this->sess);
}

void SSWR::AVIRead::AVIRGISShortestPathForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
