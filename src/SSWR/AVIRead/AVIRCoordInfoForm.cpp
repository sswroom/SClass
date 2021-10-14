#include "Stdafx.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/SROGCWKTWriter.h"
#include "SSWR/AVIRead/AVIRCoordInfoForm.h"

void __stdcall SSWR::AVIRead::AVIRCoordInfoForm::OnSRIDClicked(void *userObj)
{
	SSWR::AVIRead::AVIRCoordInfoForm *me = (SSWR::AVIRead::AVIRCoordInfoForm *)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtSRID->GetText(&sb);
	UInt32 srid;
	const Math::CoordinateSystemManager::SpatialRefInfo *srinfo = 0;
	if (sb.ToUInt32(&srid))
	{
		srinfo = Math::CoordinateSystemManager::SRGetSpatialRef(srid);
	}
	me->ShowInfo(srinfo);
}

void __stdcall SSWR::AVIRead::AVIRCoordInfoForm::OnSRIDPrevClicked(void *userObj)
{
	SSWR::AVIRead::AVIRCoordInfoForm *me = (SSWR::AVIRead::AVIRCoordInfoForm *)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtSRID->GetText(&sb);
	UInt32 srid;
	const Math::CoordinateSystemManager::SpatialRefInfo *srinfo = 0;
	if (sb.ToUInt32(&srid))
	{
		srinfo = Math::CoordinateSystemManager::SRGetSpatialRefPrev(srid);
		if (srinfo)
		{
			sb.ClearStr();
			sb.AppendU32(srinfo->srid);
			me->txtSRID->SetText(sb.ToString());
		}
	}
	me->ShowInfo(srinfo);
}

void __stdcall SSWR::AVIRead::AVIRCoordInfoForm::OnSRIDNextClicked(void *userObj)
{
	SSWR::AVIRead::AVIRCoordInfoForm *me = (SSWR::AVIRead::AVIRCoordInfoForm *)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtSRID->GetText(&sb);
	UInt32 srid;
	const Math::CoordinateSystemManager::SpatialRefInfo *srinfo = 0;
	if (sb.ToUInt32(&srid))
	{
		srinfo = Math::CoordinateSystemManager::SRGetSpatialRefNext(srid);
		if (srinfo)
		{
			sb.ClearStr();
			sb.AppendU32(srinfo->srid);
			me->txtSRID->SetText(sb.ToString());
		}
	}
	me->ShowInfo(srinfo);
}

void SSWR::AVIRead::AVIRCoordInfoForm::ShowInfo(const Math::CoordinateSystemManager::SpatialRefInfo *srinfo)
{
	Char sbuff[1024];
	if (srinfo == 0)
	{

	}
	else if (srinfo->srType == Math::CoordinateSystemManager::SRT_GEOGCS || srinfo->srType == Math::CoordinateSystemManager::SRT_PROJCS)
	{
		Math::CoordinateSystem *csys = Math::CoordinateSystemManager::SRCreateCSys(srinfo->srid);
		if (csys)
		{
			Text::StringBuilderUTF8 sb;
			Math::SROGCWKTWriter wkt;
			wkt.WriteCSys(csys, sbuff, 0, Text::LineBreakType::CRLF);
			this->txtWKT->SetText((const UTF8Char*)sbuff);
			csys->ToString(&sb);
			this->txtDisp->SetText(sb.ToString());
			DEL_CLASS(csys);
			return;
		}
	}
	this->txtWKT->SetText((const UTF8Char*)"");
	this->txtDisp->SetText((const UTF8Char*)"");
}

SSWR::AVIRead::AVIRCoordInfoForm::AVIRCoordInfoForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText((const UTF8Char*)"Coordinate Info");
	this->SetFont(0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlCoord, UI::GUIPanel(ui, this));
	this->pnlCoord->SetRect(0, 0, 100, 31, false);
	this->pnlCoord->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblSRID, UI::GUILabel(ui, this->pnlCoord, (const UTF8Char*)"SRID"));
	this->lblSRID->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtSRID, UI::GUITextBox(ui, this->pnlCoord, (const UTF8Char*)"4326"));
	this->txtSRID->SetRect(104, 4, 100, 23, false);
	NEW_CLASS(this->btnSRID, UI::GUIButton(ui, this->pnlCoord, (const UTF8Char*)"View"));
	this->btnSRID->SetRect(204, 4, 75, 23, false);
	this->btnSRID->HandleButtonClick(OnSRIDClicked, this);
	NEW_CLASS(this->btnSRIDPrev, UI::GUIButton(ui, this->pnlCoord, (const UTF8Char*)"Prev"));
	this->btnSRIDPrev->SetRect(284, 4, 75, 23, false);
	this->btnSRIDPrev->HandleButtonClick(OnSRIDPrevClicked, this);
	NEW_CLASS(this->btnSRIDNext, UI::GUIButton(ui, this->pnlCoord, (const UTF8Char*)"Next"));
	this->btnSRIDNext->SetRect(364, 4, 75, 23, false);
	this->btnSRIDNext->HandleButtonClick(OnSRIDNextClicked, this);
	NEW_CLASS(this->txtWKT, UI::GUITextBox(ui, this, (const UTF8Char*)"", true));
	this->txtWKT->SetRect(0, 0, 300, 23, false);
	this->txtWKT->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->txtWKT->SetReadOnly(true);
	NEW_CLASS(this->hspWKT, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->txtDisp, UI::GUITextBox(ui, this, (const UTF8Char*)"", true));
	this->txtDisp->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtDisp->SetReadOnly(true);

	OnSRIDClicked(this);
}

SSWR::AVIRead::AVIRCoordInfoForm::~AVIRCoordInfoForm()
{
}

void SSWR::AVIRead::AVIRCoordInfoForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
