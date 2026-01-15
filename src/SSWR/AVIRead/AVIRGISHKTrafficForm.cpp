#include "Stdafx.h"
#include "IO/StmData/FileData.h"
#include "Map/HKTrafficLayer.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRGISHKTrafficForm.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIRGISHKTrafficForm::OnRoadCenterlineClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISHKTrafficForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISHKTrafficForm>();
	Text::StringBuilderUTF8 sb;
	me->txtRoadCenterline->GetText(sb);
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"HKTraffic", false);
	if (sb.GetLength() > 0)
	{
		dlg->SetFileName(sb.ToCString());
	}
	dlg->SetAllowMultiSel(false);
	me->core->GetParserList()->PrepareSelector(dlg, IO::ParserType::MapLayer);
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtRoadCenterline->SetText(dlg->GetFileName()->ToCString());
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRGISHKTrafficForm::OnOKClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISHKTrafficForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISHKTrafficForm>();
	Text::StringBuilderUTF8 sb;
	me->txtRoadCenterline->GetText(sb);
	NN<Map::MapDrawLayer> lyr;
	{
		IO::StmData::FileData fd(sb.ToCString(), false);
		if (!Optional<Map::MapDrawLayer>::ConvertFrom(me->core->GetParserList()->ParseFileType(fd, IO::ParserType::MapLayer)).SetTo(lyr))
		{
			me->ui->ShowMsgOK(CSTR("Error in parsing the file"), CSTR("HK Traffic"), me);
			return;
		}
	}
	Map::DrawLayerType lyrType = lyr->GetLayerType();
	if (lyrType == Map::DRAW_LAYER_POLYLINE || lyrType == Map::DRAW_LAYER_POLYLINE3D)
	{
		Map::HKTrafficLayer *traffic;
		NEW_CLASS(traffic, Map::HKTrafficLayer(me->core->GetTCPClientFactory(), me->ssl, me->core->GetEncFactory()));
		traffic->AddRoadLayer(lyr);
		traffic->EndInit();
		me->lyr = traffic;
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
	else
	{
		lyr.Delete();
		me->ui->ShowMsgOK(CSTR("The file is not a polyline layer"), CSTR("HK Traffic"), me);
	}
}

void __stdcall SSWR::AVIRead::AVIRGISHKTrafficForm::OnCancelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRGISHKTrafficForm> me = userObj.GetNN<SSWR::AVIRead::AVIRGISHKTrafficForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRGISHKTrafficForm::AVIRGISHKTrafficForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 120, ui)
{
	this->SetText(CSTR("Hong Kong Traffic"));
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);

	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetTCPClientFactory(), true);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->lyr = nullptr;

	this->lblRoadCenterline = ui->NewLabel(*this, CSTR("Centerline"));
	this->lblRoadCenterline->SetRect(4, 4, 100, 23, false);
	this->txtRoadCenterline = ui->NewTextBox(*this, CSTR("20180628 Road Traffic\\CENTRELINE.mdb"));
	this->txtRoadCenterline->SetRect(104, 4, 800, 23, false);
	this->btnRoadCenterline = ui->NewButton(*this, CSTR("Browse"));
	this->btnRoadCenterline->SetRect(904, 4, 75, 23, false);
	this->btnRoadCenterline->HandleButtonClick(OnRoadCenterlineClicked, this);
	this->btnCancel = ui->NewButton(*this, CSTR("Cancel"));
	this->btnCancel->SetRect(104, 28, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->btnOK = ui->NewButton(*this, CSTR("OK"));
	this->btnOK->SetRect(184, 28, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
}

SSWR::AVIRead::AVIRGISHKTrafficForm::~AVIRGISHKTrafficForm()
{
}

void SSWR::AVIRead::AVIRGISHKTrafficForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Optional<Map::MapDrawLayer> SSWR::AVIRead::AVIRGISHKTrafficForm::GetMapLayer()
{
	return this->lyr;
}
