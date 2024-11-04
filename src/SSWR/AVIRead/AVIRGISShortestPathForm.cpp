#include "Stdafx.h"
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

SSWR::AVIRead::AVIRGISShortestPathForm::AVIRGISShortestPathForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IMapNavigator> navi, NN<Map::MapDrawLayer> layer) : UI::GUIForm(parent, 320, 360, ui), spath(layer, 100.0)
{
	this->core = core;
	this->navi = navi;
	this->layer = layer;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetText(CSTR("Shortest Path"));
	this->SetFont(0, 0, 8.25, false);

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
	this->lvPaths->AddColumn(CSTR("CName"), 60);
	this->lvPaths->AddColumn(CSTR("EName"), 100);
	this->lvPaths->AddColumn(CSTR("Type"), 100);
	this->lvPaths->AddColumn(CSTR("Dist"), 60);
	this->lvPaths->AddColumn(CSTR("Dir"), 60);

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
}

SSWR::AVIRead::AVIRGISShortestPathForm::~AVIRGISShortestPathForm()
{
}

void SSWR::AVIRead::AVIRGISShortestPathForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
