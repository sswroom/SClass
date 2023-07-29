#include "Stdafx.h"
#include "DB/DBTool.h"
#include "DB/MDBFileConn.h"
#include "IO/StmData/FileData.h"
#include "Map/HKTDVehRestrict.h"
#include "SSWR/AVIRead/AVIRGISHKTDTonnesForm.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRGISHKTDTonnesForm::OnRoadRouteClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISHKTDTonnesForm *me = (SSWR::AVIRead::AVIRGISHKTDTonnesForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtRoadRoute->GetText(sb);
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"HKTDTonnesRoadRoute", false);
	if (sb.GetLength() > 0)
	{
		dlg.SetFileName(sb.ToCString());
	}
	dlg.SetAllowMultiSel(false);
	me->core->GetParserList()->PrepareSelector(&dlg, IO::ParserType::MapLayer);
	if (dlg.ShowDialog(me->GetHandle()))
	{
		me->txtRoadRoute->SetText(dlg.GetFileName()->ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRGISHKTDTonnesForm::OnVehicleResClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISHKTDTonnesForm *me = (SSWR::AVIRead::AVIRGISHKTDTonnesForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtVehicleRes->GetText(sb);
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"HKTDTonnesVehicleRes", false);
	if (sb.GetLength() > 0)
	{
		dlg.SetFileName(sb.ToCString());
	}
	dlg.SetAllowMultiSel(false);
	me->core->GetParserList()->PrepareSelector(&dlg, IO::ParserType::MapLayer);
	if (dlg.ShowDialog(me->GetHandle()))
	{
		me->txtVehicleRes->SetText(dlg.GetFileName()->ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRGISHKTDTonnesForm::OnOKClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISHKTDTonnesForm *me = (SSWR::AVIRead::AVIRGISHKTDTonnesForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	me->txtRoadRoute->GetText(sb);
	me->txtVehicleRes->GetText(sb2);
	if (sb.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please input Road Route"), CSTR("HK Tonnes Sign"), me);
	}
	else if (sb2.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please input Vehicle Restriction"), CSTR("HK Tonnes Sign"), me);
	}
	else
	{
		Map::MapDrawLayer *lyr;
		{
			IO::StmData::FileData fd(sb.ToCString(), false);
			lyr = (Map::MapDrawLayer*)me->core->GetParserList()->ParseFileType(fd, IO::ParserType::MapLayer);
		}
		DB::DBTool *db = DB::MDBFileConn::CreateDBTool(sb2.ToCString(), 0, CSTR_NULL);
		if (lyr && db)
		{
			Map::DrawLayerType lyrType = lyr->GetLayerType();
			if (lyrType == Map::DRAW_LAYER_POLYLINE || lyrType == Map::DRAW_LAYER_POLYLINE3D)
			{
				Map::HKTDVehRestrict *vehRestrict;
				NEW_CLASS(vehRestrict, Map::HKTDVehRestrict(lyr, db));
				me->lyr = vehRestrict->CreateTonnesSignLayer();
				if (me->lyr)
				{
					me->SetDialogResult(UI::GUIForm::DR_OK);
				}
				else
				{
					UI::MessageDialog::ShowDialog(CSTR("Error in creating layer"), CSTR("HK Tonnes Sign"), me);
				}
				DEL_CLASS(vehRestrict);
			}
			else
			{
				DEL_CLASS(db);
				DEL_CLASS(lyr);
				UI::MessageDialog::ShowDialog(CSTR("The file is not a polyline layer"), CSTR("HK Tonnes Sign"), me);
			}
		}
		else if (lyr)
		{
			DEL_CLASS(lyr);
			UI::MessageDialog::ShowDialog(CSTR("Error in opening file"), CSTR("HK Tonnes Sign"), me);
		}
		else
		{
			if (db)
			{
				DEL_CLASS(db);
			}
			UI::MessageDialog::ShowDialog(CSTR("Error in parsing the file"), CSTR("HK Tonnes Sign"), me);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGISHKTDTonnesForm::OnCancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISHKTDTonnesForm *me = (SSWR::AVIRead::AVIRGISHKTDTonnesForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRGISHKTDTonnesForm::AVIRGISHKTDTonnesForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 144, ui)
{
	this->SetText(CSTR("Hong Kong Tonnes Sign"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->lyr = 0;

	NEW_CLASS(this->lblRoadRoute, UI::GUILabel(ui, this, CSTR("Road Route")));
	this->lblRoadRoute->SetRect(4, 4, 150, 23, false);
	NEW_CLASS(this->txtRoadRoute, UI::GUITextBox(ui, this, CSTR("20180628 Road Traffic\\ROADROUTE.mdb")));
	this->txtRoadRoute->SetRect(154, 4, 750, 23, false);
	NEW_CLASS(this->btnRoadRoute, UI::GUIButton(ui, this, CSTR("Browse")));
	this->btnRoadRoute->SetRect(904, 4, 75, 23, false);
	this->btnRoadRoute->HandleButtonClick(OnRoadRouteClicked, this);
	NEW_CLASS(this->lblVehicleRes, UI::GUILabel(ui, this, CSTR("Vehicle Restriction")));
	this->lblVehicleRes->SetRect(4, 28, 150, 23, false);
	NEW_CLASS(this->txtVehicleRes, UI::GUITextBox(ui, this, CSTR("20180628 Road Traffic\\VEHICLE_RESTRICTION.mdb")));
	this->txtVehicleRes->SetRect(154, 28, 750, 23, false);
	NEW_CLASS(this->btnVehicleRes, UI::GUIButton(ui, this, CSTR("Browse")));
	this->btnVehicleRes->SetRect(904, 28, 75, 23, false);
	this->btnVehicleRes->HandleButtonClick(OnVehicleResClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, CSTR("Cancel")));
	this->btnCancel->SetRect(154, 52, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, CSTR("OK")));
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

Map::MapDrawLayer *SSWR::AVIRead::AVIRGISHKTDTonnesForm::GetMapLayer()
{
	return this->lyr;
}
