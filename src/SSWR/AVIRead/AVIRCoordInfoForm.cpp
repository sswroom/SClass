#include "Stdafx.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/SROGCWKTWriter.h"
#include "SSWR/AVIRead/AVIRCoordInfoForm.h"

void __stdcall SSWR::AVIRead::AVIRCoordInfoForm::OnSRIDClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCoordInfoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCoordInfoForm>();
	Text::StringBuilderUTF8 sb;
	me->txtSRID->GetText(sb);
	UInt32 srid;
	Optional<const Math::CoordinateSystemManager::SpatialRefInfo> srinfo = nullptr;
	if (sb.ToUInt32(srid))
	{
		srinfo = Math::CoordinateSystemManager::SRGetSpatialRef(srid);
	}
	me->ShowInfo(srinfo);
}

void __stdcall SSWR::AVIRead::AVIRCoordInfoForm::OnSRIDPrevClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCoordInfoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCoordInfoForm>();
	Text::StringBuilderUTF8 sb;
	me->txtSRID->GetText(sb);
	UInt32 srid;
	Optional<const Math::CoordinateSystemManager::SpatialRefInfo> srinfo = nullptr;
	NN<const Math::CoordinateSystemManager::SpatialRefInfo> nnsrinfo;
	if (sb.ToUInt32(srid))
	{
		srinfo = Math::CoordinateSystemManager::SRGetSpatialRefPrev(srid);
		if (srinfo.SetTo(nnsrinfo))
		{
			sb.ClearStr();
			sb.AppendU32(nnsrinfo->srid);
			me->txtSRID->SetText(sb.ToCString());
		}
	}
	me->ShowInfo(srinfo);
}

void __stdcall SSWR::AVIRead::AVIRCoordInfoForm::OnSRIDNextClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCoordInfoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCoordInfoForm>();
	Text::StringBuilderUTF8 sb;
	me->txtSRID->GetText(sb);
	UInt32 srid;
	Optional<const Math::CoordinateSystemManager::SpatialRefInfo> srinfo = nullptr;
	NN<const Math::CoordinateSystemManager::SpatialRefInfo> nnsrinfo;
	if (sb.ToUInt32(srid))
	{
		srinfo = Math::CoordinateSystemManager::SRGetSpatialRefNext(srid);
		if (srinfo.SetTo(nnsrinfo))
		{
			sb.ClearStr();
			sb.AppendU32(nnsrinfo->srid);
			me->txtSRID->SetText(sb.ToCString());
		}
	}
	me->ShowInfo(srinfo);
}

void SSWR::AVIRead::AVIRCoordInfoForm::ShowInfo(Optional<const Math::CoordinateSystemManager::SpatialRefInfo> srinfo)
{
	UTF8Char sbuff[1024];
	UnsafeArray<UTF8Char> sptr;
	NN<const Math::CoordinateSystemManager::SpatialRefInfo> nnsrinfo;
	if (!srinfo.SetTo(nnsrinfo))
	{

	}
	else if (nnsrinfo->srType == Math::CoordinateSystemManager::SRT_GEOGCS || nnsrinfo->srType == Math::CoordinateSystemManager::SRT_PROJCS)
	{
		NN<Math::CoordinateSystem> csys;
		if (Math::CoordinateSystemManager::SRCreateCSys(nnsrinfo->srid).SetTo(csys))
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
	else if (nnsrinfo->srType == Math::CoordinateSystemManager::SRT_DATUM)
	{
		NN<const Math::CoordinateSystemManager::DatumInfo> datumInfo;
		if (Math::CoordinateSystemManager::SRGetDatum(nnsrinfo->srid).SetTo(datumInfo))
		{
			NN<const Math::CoordinateSystemManager::SpheroidInfo> spheroid;
			if (Math::CoordinateSystemManager::SRGetSpheroid(datumInfo->spheroid).SetTo(spheroid))
			{
				Text::StringBuilderUTF8 sb;
				Math::CoordinateSystem::DatumData1 datum;
				Math::EarthEllipsoid ee(spheroid->eet);
				Math::CoordinateSystemManager::FillDatumData(datum, datumInfo, Text::CStringNN::FromPtr((const UTF8Char*)nnsrinfo->name), ee, spheroid);
				Math::SROGCWKTWriter wkt;
				sptr = wkt.WriteDatum(datum, sbuff, 0, Text::LineBreakType::CRLF);
				this->txtWKT->SetText(CSTRP(sbuff, sptr));
				Math::CoordinateSystem::DatumData1ToString(datum, sb);
				this->txtDisp->SetText(sb.ToCString());
				return;
			}
		}
	}
	else if (nnsrinfo->srType == Math::CoordinateSystemManager::SRT_SPHERO)
	{
		NN<const Math::CoordinateSystemManager::SpheroidInfo> spheroid;
		if (Math::CoordinateSystemManager::SRGetSpheroid(nnsrinfo->srid).SetTo(spheroid))
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
	else if (nnsrinfo->srType == Math::CoordinateSystemManager::SRT_PRIMEM)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("PRIMEM[\""));
		sb.AppendSlow((const UTF8Char*)nnsrinfo->name);
		sb.AppendC(UTF8STRC("\",0,AUTHORITY[\"EPSG\",\""));
		sb.AppendU32(nnsrinfo->srid);
		sb.AppendC(UTF8STRC("\"]]"));
		this->txtWKT->SetText(sb.ToCString());
		this->txtDisp->SetText(sb.ToCString());
		return;
	}
	else if (nnsrinfo->srType == Math::CoordinateSystemManager::SRT_UNIT)
	{
		switch (nnsrinfo->srid)
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

SSWR::AVIRead::AVIRCoordInfoForm::AVIRCoordInfoForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Coordinate Info"));
	this->SetFont(nullptr, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlCoord = ui->NewPanel(*this);
	this->pnlCoord->SetRect(0, 0, 100, 31, false);
	this->pnlCoord->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblSRID = ui->NewLabel(this->pnlCoord, CSTR("SRID"));
	this->lblSRID->SetRect(4, 4, 100, 23, false);
	this->txtSRID = ui->NewTextBox(this->pnlCoord, CSTR("4326"));
	this->txtSRID->SetRect(104, 4, 100, 23, false);
	this->btnSRID = ui->NewButton(this->pnlCoord, CSTR("View"));
	this->btnSRID->SetRect(204, 4, 75, 23, false);
	this->btnSRID->HandleButtonClick(OnSRIDClicked, this);
	this->btnSRIDPrev = ui->NewButton(this->pnlCoord, CSTR("Prev"));
	this->btnSRIDPrev->SetRect(284, 4, 75, 23, false);
	this->btnSRIDPrev->HandleButtonClick(OnSRIDPrevClicked, this);
	this->btnSRIDNext = ui->NewButton(this->pnlCoord, CSTR("Next"));
	this->btnSRIDNext->SetRect(364, 4, 75, 23, false);
	this->btnSRIDNext->HandleButtonClick(OnSRIDNextClicked, this);
	this->txtWKT = ui->NewTextBox(*this, CSTR(""), true);
	this->txtWKT->SetRect(0, 0, 300, 23, false);
	this->txtWKT->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->txtWKT->SetReadOnly(true);
	this->hspWKT = ui->NewHSplitter(*this, 3, false);
	this->txtDisp = ui->NewTextBox(*this, CSTR(""), true);
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
