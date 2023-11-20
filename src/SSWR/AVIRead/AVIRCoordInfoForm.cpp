#include "Stdafx.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/SROGCWKTWriter.h"
#include "SSWR/AVIRead/AVIRCoordInfoForm.h"

void __stdcall SSWR::AVIRead::AVIRCoordInfoForm::OnSRIDClicked(void *userObj)
{
	SSWR::AVIRead::AVIRCoordInfoForm *me = (SSWR::AVIRead::AVIRCoordInfoForm *)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtSRID->GetText(sb);
	UInt32 srid;
	const Math::CoordinateSystemManager::SpatialRefInfo *srinfo = 0;
	if (sb.ToUInt32(srid))
	{
		srinfo = Math::CoordinateSystemManager::SRGetSpatialRef(srid);
	}
	me->ShowInfo(srinfo);
}

void __stdcall SSWR::AVIRead::AVIRCoordInfoForm::OnSRIDPrevClicked(void *userObj)
{
	SSWR::AVIRead::AVIRCoordInfoForm *me = (SSWR::AVIRead::AVIRCoordInfoForm *)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtSRID->GetText(sb);
	UInt32 srid;
	const Math::CoordinateSystemManager::SpatialRefInfo *srinfo = 0;
	if (sb.ToUInt32(srid))
	{
		srinfo = Math::CoordinateSystemManager::SRGetSpatialRefPrev(srid);
		if (srinfo)
		{
			sb.ClearStr();
			sb.AppendU32(srinfo->srid);
			me->txtSRID->SetText(sb.ToCString());
		}
	}
	me->ShowInfo(srinfo);
}

void __stdcall SSWR::AVIRead::AVIRCoordInfoForm::OnSRIDNextClicked(void *userObj)
{
	SSWR::AVIRead::AVIRCoordInfoForm *me = (SSWR::AVIRead::AVIRCoordInfoForm *)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtSRID->GetText(sb);
	UInt32 srid;
	const Math::CoordinateSystemManager::SpatialRefInfo *srinfo = 0;
	if (sb.ToUInt32(srid))
	{
		srinfo = Math::CoordinateSystemManager::SRGetSpatialRefNext(srid);
		if (srinfo)
		{
			sb.ClearStr();
			sb.AppendU32(srinfo->srid);
			me->txtSRID->SetText(sb.ToCString());
		}
	}
	me->ShowInfo(srinfo);
}

void SSWR::AVIRead::AVIRCoordInfoForm::ShowInfo(const Math::CoordinateSystemManager::SpatialRefInfo *srinfo)
{
	UTF8Char sbuff[1024];
	UTF8Char *sptr;
	if (srinfo == 0)
	{

	}
	else if (srinfo->srType == Math::CoordinateSystemManager::SRT_GEOGCS || srinfo->srType == Math::CoordinateSystemManager::SRT_PROJCS)
	{
		NotNullPtr<Math::CoordinateSystem> csys;
		if (csys.Set(Math::CoordinateSystemManager::SRCreateCSys(srinfo->srid)))
		{
			Text::StringBuilderUTF8 sb;
			Math::SROGCWKTWriter wkt;
			sptr = wkt.WriteCSys(csys, sbuff, 0, Text::LineBreakType::CRLF);
			this->txtWKT->SetText(CSTRP(sbuff, sptr));
			csys->ToString(sb);
			this->txtDisp->SetText(sb.ToCString());
			csys.Delete();
			return;
		}
	}
	else if (srinfo->srType == Math::CoordinateSystemManager::SRT_DATUM)
	{
		const Math::CoordinateSystemManager::DatumInfo *datumInfo = Math::CoordinateSystemManager::SRGetDatum(srinfo->srid);
		if (datumInfo)
		{
			const Math::CoordinateSystemManager::SpheroidInfo *spheroid = Math::CoordinateSystemManager::SRGetSpheroid(datumInfo->spheroid);
			if (spheroid)
			{
				Text::StringBuilderUTF8 sb;
				Math::CoordinateSystem::DatumData1 datum;
				Math::EarthEllipsoid ee(spheroid->eet);
				Math::CoordinateSystemManager::FillDatumData(datum, datumInfo, Text::CString::FromPtr((const UTF8Char*)srinfo->name), ee, spheroid);
				Math::SROGCWKTWriter wkt;
				sptr = wkt.WriteDatum(datum, sbuff, 0, Text::LineBreakType::CRLF);
				this->txtWKT->SetText(CSTRP(sbuff, sptr));
				Math::CoordinateSystem::DatumData1ToString(datum, sb);
				this->txtDisp->SetText(sb.ToCString());
				return;
			}
		}
	}
	else if (srinfo->srType == Math::CoordinateSystemManager::SRT_SPHERO)
	{
		const Math::CoordinateSystemManager::SpheroidInfo *spheroid = Math::CoordinateSystemManager::SRGetSpheroid(srinfo->srid);
		if (spheroid)
		{
			Text::StringBuilderUTF8 sb;
			Math::CoordinateSystem::SpheroidData data;
			Math::EarthEllipsoid ee(spheroid->eet);
			data.name = spheroid->name;
			data.nameLen = spheroid->nameLen;
			data.srid = spheroid->srid;
			data.ellipsoid = ee;
			Math::SROGCWKTWriter wkt;
			sptr = wkt.WriteSpheroid(&data, sbuff, 0, Text::LineBreakType::CRLF);
			this->txtWKT->SetText(CSTRP(sbuff, sptr));
			ee.ToString(sb);
			this->txtDisp->SetText(sb.ToCString());
			return;
		}
	}
	else if (srinfo->srType == Math::CoordinateSystemManager::SRT_PRIMEM)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("PRIMEM[\""));
		sb.AppendSlow((const UTF8Char*)srinfo->name);
		sb.AppendC(UTF8STRC("\",0,AUTHORITY[\"EPSG\",\""));
		sb.AppendU32(srinfo->srid);
		sb.AppendC(UTF8STRC("\"]]"));
		this->txtWKT->SetText(sb.ToCString());
		this->txtDisp->SetText(sb.ToCString());
		return;
	}
	else if (srinfo->srType == Math::CoordinateSystemManager::SRT_UNIT)
	{
		switch (srinfo->srid)
		{
		case 9001:
			this->txtWKT->SetText(CSTR("UNIT[\"metre\",1,AUTHORITY[\"EPSG\",\"9001\"]]"));
			this->txtDisp->SetText(CSTR("Unit: metre\r\nMeter Ratio: 1"));
			break;
		case 9122:
			this->txtWKT->SetText(CSTR("UNIT[\"degree\",0.01745329251994328,AUTHORITY[\"EPSG\",\"9122\"]]"));
			this->txtDisp->SetText(CSTR("Unit: degree\r\nMeter Ratio: 0.01745329251994328"));
			break;
		default:
			this->txtWKT->SetText(CSTR(""));
			this->txtDisp->SetText(CSTR(""));
			break;
		}
		return;
	}
	this->txtWKT->SetText(CSTR(""));
	this->txtDisp->SetText(CSTR(""));
}

SSWR::AVIRead::AVIRCoordInfoForm::AVIRCoordInfoForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Coordinate Info"));
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASSNN(this->pnlCoord, UI::GUIPanel(ui, *this));
	this->pnlCoord->SetRect(0, 0, 100, 31, false);
	this->pnlCoord->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblSRID, UI::GUILabel(ui, this->pnlCoord, CSTR("SRID")));
	this->lblSRID->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtSRID, UI::GUITextBox(ui, this->pnlCoord, CSTR("4326")));
	this->txtSRID->SetRect(104, 4, 100, 23, false);
	NEW_CLASS(this->btnSRID, UI::GUIButton(ui, this->pnlCoord, CSTR("View")));
	this->btnSRID->SetRect(204, 4, 75, 23, false);
	this->btnSRID->HandleButtonClick(OnSRIDClicked, this);
	NEW_CLASS(this->btnSRIDPrev, UI::GUIButton(ui, this->pnlCoord, CSTR("Prev")));
	this->btnSRIDPrev->SetRect(284, 4, 75, 23, false);
	this->btnSRIDPrev->HandleButtonClick(OnSRIDPrevClicked, this);
	NEW_CLASS(this->btnSRIDNext, UI::GUIButton(ui, this->pnlCoord, CSTR("Next")));
	this->btnSRIDNext->SetRect(364, 4, 75, 23, false);
	this->btnSRIDNext->HandleButtonClick(OnSRIDNextClicked, this);
	NEW_CLASS(this->txtWKT, UI::GUITextBox(ui, *this, CSTR(""), true));
	this->txtWKT->SetRect(0, 0, 300, 23, false);
	this->txtWKT->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->txtWKT->SetReadOnly(true);
	NEW_CLASS(this->hspWKT, UI::GUIHSplitter(ui, *this, 3, false));
	NEW_CLASS(this->txtDisp, UI::GUITextBox(ui, *this, CSTR(""), true));
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
