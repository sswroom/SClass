#include "Stdafx.h"
#include "DB/DBTool.h"
#include "DB/MDBFileConn.h"
#include "IO/StmData/FileData.h"
#include "Map/HKTDVehRestrict.h"
#include "SSWR/AVIRead/AVIRGISHKTDTonnesForm.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIRGISHKTDTonnesForm::OnRoadRouteClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISHKTDTonnesForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISHKTDTonnesForm>();
	Text::StringBuilderUTF8 sb;
	me->txtRoadRoute->GetText(sb);
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"HKTDTonnesRoadRoute", false);
	if (sb.GetLength() > 0)
	{
		dlg->SetFileName(sb.ToCString());
	}
	dlg->SetAllowMultiSel(false);
	me->core->GetParserList()->PrepareSelector(dlg, IO::ParserType::MapLayer);
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtRoadRoute->SetText(dlg->GetFileName()->ToCString());
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRGISHKTDTonnesForm::OnVehicleResClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISHKTDTonnesForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISHKTDTonnesForm>();
	Text::StringBuilderUTF8 sb;
	me->txtVehicleRes->GetText(sb);
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"HKTDTonnesVehicleRes", false);
	if (sb.GetLength() > 0)
	{
		dlg->SetFileName(sb.ToCString());
	}
	dlg->SetAllowMultiSel(false);
	me->core->GetParserList()->PrepareSelector(dlg, IO::ParserType::MapLayer);
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtVehicleRes->SetText(dlg->GetFileName()->ToCString());
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRGISHKTDTonnesForm::OnOKClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISHKTDTonnesForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISHKTDTonnesForm>();
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	me->txtRoadRoute->GetText(sb);
	me->txtVehicleRes->GetText(sb2);
	if (sb.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please input Road Route"), CSTR("HK Tonnes Sign"), me);
	}
	else if (sb2.GetLength() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please input Vehicle Restriction"), CSTR("HK Tonnes Sign"), me);
	}
	else
	{
		Optional<Map::MapDrawLayer> lyr;
		{
			IO::StmData::FileData fd(sb.ToCString(), false);
			lyr = Optional<Map::MapDrawLayer>::ConvertFrom(me->core->GetParserList()->ParseFileType(fd, IO::ParserType::MapLayer));
		}
		Optional<DB::DBTool> db = DB::MDBFileConn::CreateDBTool(sb2.ToCString(), me->core->GetLog(), CSTR_NULL);
		NN<DB::DBTool> nndb;
		NN<Map::MapDrawLayer> nnlyr;
		if (lyr.SetTo(nnlyr) && db.SetTo(nndb))
		{
			Map::DrawLayerType lyrType = nnlyr->GetLayerType();
			if (lyrType == Map::DRAW_LAYER_POLYLINE || lyrType == Map::DRAW_LAYER_POLYLINE3D)
			{
				Map::HKTDVehRestrict *vehRestrict;
				NEW_CLASS(vehRestrict, Map::HKTDVehRestrict(nnlyr, nndb));
				me->lyr = vehRestrict->CreateTonnesSignLayer();
				if (me->lyr.NotNull())
				{
					me->SetDialogResult(UI::GUIForm::DR_OK);
				}
				else
				{
					me->ui->ShowMsgOK(CSTR("Error in creating layer"), CSTR("HK Tonnes Sign"), me);
				}
				DEL_CLASS(vehRestrict);
			}
			else
			{
				db.Delete();
				lyr.Delete();
				me->ui->ShowMsgOK(CSTR("The file is not a polyline layer"), CSTR("HK Tonnes Sign"), me);
			}
		}
		else if (lyr.NotNull())
		{
			lyr.Delete();
			me->ui->ShowMsgOK(CSTR("Error in opening file"), CSTR("HK Tonnes Sign"), me);
		}
		else
		{
			db.Delete();
			me->ui->ShowMsgOK(CSTR("Error in parsing the file"), CSTR("HK Tonnes Sign"), me);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGISHKTDTonnesForm::OnCancelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISHKTDTonnesForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISHKTDTonnesForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRGISHKTDTonnesForm::AVIRGISHKTDTonnesForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 144, ui)
{
	this->SetText(CSTR("Hong Kong Tonnes Sign"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->lyr = 0;

	this->lblRoadRoute = ui->NewLabel(*this, CSTR("Road Route"));
	this->lblRoadRoute->SetRect(4, 4, 150, 23, false);
	this->txtRoadRoute = ui->NewTextBox(*this, CSTR("20180628 Road Traffic\\ROADROUTE.mdb"));
	this->txtRoadRoute->SetRect(154, 4, 750, 23, false);
	this->btnRoadRoute = ui->NewButton(*this, CSTR("Browse"));
	this->btnRoadRoute->SetRect(904, 4, 75, 23, false);
	this->btnRoadRoute->HandleButtonClick(OnRoadRouteClicked, this);
	this->lblVehicleRes = ui->NewLabel(*this, CSTR("Vehicle Restriction"));
	this->lblVehicleRes->SetRect(4, 28, 150, 23, false);
	this->txtVehicleRes = ui->NewTextBox(*this, CSTR("20180628 Road Traffic\\VEHICLE_RESTRICTION.mdb"));
	this->txtVehicleRes->SetRect(154, 28, 750, 23, false);
	this->btnVehicleRes = ui->NewButton(*this, CSTR("Browse"));
	this->btnVehicleRes->SetRect(904, 28, 75, 23, false);
	this->btnVehicleRes->HandleButtonClick(OnVehicleResClicked, this);
	this->btnCancel = ui->NewButton(*this, CSTR("Cancel"));
	this->btnCancel->SetRect(154, 52, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->btnOK = ui->NewButton(*this, CSTR("OK"));
	this->btnOK->SetRect(234, 52, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
}

SSWR::AVIRead::AVIRGISHKTDTonnesForm::~AVIRGISHKTDTonnesForm()
{
}

void SSWR::AVIRead::AVIRGISHKTDTonnesForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Optional<Map::MapDrawLayer> SSWR::AVIRead::AVIRGISHKTDTonnesForm::GetMapLayer()
{
	return this->lyr;
}
