#include "Stdafx.h"
#include "DB/DBTool.h"
#include "DB/MDBFile.h"
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
	UI::FileDialog *dlg;
	me->txtRoadRoute->GetText(&sb);
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"HKTDTonnesRoadRoute", false));
	if (sb.GetLength() > 0)
	{
		dlg->SetFileName(sb.ToString());
	}
	dlg->SetAllowMultiSel(false);
	me->core->GetParserList()->PrepareSelector(dlg, IO::ParsedObject::PT_MAP_LAYER_PARSER);
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtRoadRoute->SetText(dlg->GetFileName());
	}
	DEL_CLASS(dlg);
}

void __stdcall SSWR::AVIRead::AVIRGISHKTDTonnesForm::OnVehicleResClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISHKTDTonnesForm *me = (SSWR::AVIRead::AVIRGISHKTDTonnesForm*)userObj;
	Text::StringBuilderUTF8 sb;
	UI::FileDialog *dlg;
	me->txtVehicleRes->GetText(&sb);
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"HKTDTonnesVehicleRes", false));
	if (sb.GetLength() > 0)
	{
		dlg->SetFileName(sb.ToString());
	}
	dlg->SetAllowMultiSel(false);
	me->core->GetParserList()->PrepareSelector(dlg, IO::ParsedObject::PT_MAP_LAYER_PARSER);
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtVehicleRes->SetText(dlg->GetFileName());
	}
	DEL_CLASS(dlg);
}

void __stdcall SSWR::AVIRead::AVIRGISHKTDTonnesForm::OnOKClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISHKTDTonnesForm *me = (SSWR::AVIRead::AVIRGISHKTDTonnesForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	me->txtRoadRoute->GetText(&sb);
	me->txtVehicleRes->GetText(&sb2);
	IO::StmData::FileData *fd;
	if (sb.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please input Road Route", (const UTF8Char*)"HK Tonnes Sign", me);
	}
	else if (sb2.GetLength() == 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please input Vehicle Restriction", (const UTF8Char*)"HK Tonnes Sign", me);
	}
	else
	{
		NEW_CLASS(fd, IO::StmData::FileData(sb.ToString(), false));
		Map::IMapDrawLayer *lyr = (Map::IMapDrawLayer*)me->core->GetParserList()->ParseFileType(fd, IO::ParsedObject::PT_MAP_LAYER_PARSER);
		DEL_CLASS(fd);
		DB::DBTool *db = DB::MDBFile::CreateDBTool(sb2.ToString(), 0, false, 0);
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
					UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in creating layer", (const UTF8Char*)"HK Tonnes Sign", me);
				}
				DEL_CLASS(vehRestrict);
			}
			else
			{
				DEL_CLASS(db);
				DEL_CLASS(lyr);
				UI::MessageDialog::ShowDialog((const UTF8Char*)"The file is not a polyline layer", (const UTF8Char*)"HK Tonnes Sign", me);
			}
		}
		else if (lyr)
		{
			DEL_CLASS(lyr);
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in opening file", (const UTF8Char*)"HK Tonnes Sign", me);
		}
		else
		{
			if (db)
			{
				DEL_CLASS(db);
			}
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in parsing the file", (const UTF8Char*)"HK Tonnes Sign", me);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRGISHKTDTonnesForm::OnCancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIRGISHKTDTonnesForm *me = (SSWR::AVIRead::AVIRGISHKTDTonnesForm*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRGISHKTDTonnesForm::AVIRGISHKTDTonnesForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 144, ui)
{
	this->SetText((const UTF8Char*)"Hong Kong Tonnes Sign");
	this->SetFont(0, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->lyr = 0;

	NEW_CLASS(this->lblRoadRoute, UI::GUILabel(ui, this, (const UTF8Char*)"Road Route"));
	this->lblRoadRoute->SetRect(4, 4, 150, 23, false);
	NEW_CLASS(this->txtRoadRoute, UI::GUITextBox(ui, this, (const UTF8Char*)"L:\\Backup\\PROGS_20101124\\PROGS\\Skypower\\0_req\\20180628 Road Traffic\\ROADROUTE.mdb"));
	this->txtRoadRoute->SetRect(154, 4, 750, 23, false);
	NEW_CLASS(this->btnRoadRoute, UI::GUIButton(ui, this, (const UTF8Char*)"Browse"));
	this->btnRoadRoute->SetRect(904, 4, 75, 23, false);
	this->btnRoadRoute->HandleButtonClick(OnRoadRouteClicked, this);
	NEW_CLASS(this->lblVehicleRes, UI::GUILabel(ui, this, (const UTF8Char*)"Vehicle Restriction"));
	this->lblVehicleRes->SetRect(4, 28, 150, 23, false);
	NEW_CLASS(this->txtVehicleRes, UI::GUITextBox(ui, this, (const UTF8Char*)"L:\\Backup\\PROGS_20101124\\PROGS\\Skypower\\0_req\\20180628 Road Traffic\\VEHICLE_RESTRICTION.mdb"));
	this->txtVehicleRes->SetRect(154, 28, 750, 23, false);
	NEW_CLASS(this->btnVehicleRes, UI::GUIButton(ui, this, (const UTF8Char*)"Browse"));
	this->btnVehicleRes->SetRect(904, 28, 75, 23, false);
	this->btnVehicleRes->HandleButtonClick(OnVehicleResClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this, (const UTF8Char*)"Cancel"));
	this->btnCancel->SetRect(154, 52, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this, (const UTF8Char*)"OK"));
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

Map::IMapDrawLayer *SSWR::AVIRead::AVIRGISHKTDTonnesForm::GetMapLayer()
{
	return this->lyr;
}
