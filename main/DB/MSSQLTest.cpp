#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/NamedClass.h"
#include "DB/DBClassReader.h"
#include "DB/MSSQLConn.h"
#include "IO/ConsoleLogHandler.h"
#include "IO/ConsoleWriter.h"
#include "Manage/HiResClock.h"

class AdsbMovementstatistics
{
private:
	Text::String* uploadId;
	Text::String* finalUploadId;
	Data::Timestamp adsbDatetime;
	Text::String* callsign;
	Double latitude;
	Double longitude;
	Math::Geometry::Vector2D* geolocation;
	Bool insideHkfir;
	Bool insideAerodrome;
	Bool insideApp;
	Bool insideCtr;
	Bool insideTme;
	Bool insideTms;
	Bool insideTma;
	Double geometricHeight;
	Double flightLevel;
	Double groundSpeed;
	Double verticalRate;
	Int32 category;
	Int32 subgroup;
	Text::String* aircraftType;
	Data::Timestamp ataAtd;
	Text::String* departure;
	Text::String* destination;
	Text::String* inOutbound;
	Text::String* runway;
	Text::String* serviceType;
	Text::String* ioFlight;

public:
	AdsbMovementstatistics();
	~AdsbMovementstatistics();

	Text::String* GetUploadId() const;
	void SetUploadId(Text::String* uploadId);
	Text::String* GetFinalUploadId() const;
	void SetFinalUploadId(Text::String* finalUploadId);
	Data::Timestamp GetAdsbDatetime() const;
	void SetAdsbDatetime(Data::Timestamp adsbDatetime);
	Text::String* GetCallsign() const;
	void SetCallsign(Text::String* callsign);
	Double GetLatitude() const;
	void SetLatitude(Double latitude);
	Double GetLongitude() const;
	void SetLongitude(Double longitude);
	Math::Geometry::Vector2D* GetGeolocation() const;
	void SetGeolocation(Math::Geometry::Vector2D* geolocation);
	Bool GetInsideHkfir() const;
	void SetInsideHkfir(Bool insideHkfir);
	Bool GetInsideAerodrome() const;
	void SetInsideAerodrome(Bool insideAerodrome);
	Bool GetInsideApp() const;
	void SetInsideApp(Bool insideApp);
	Bool GetInsideCtr() const;
	void SetInsideCtr(Bool insideCtr);
	Bool GetInsideTme() const;
	void SetInsideTme(Bool insideTme);
	Bool GetInsideTms() const;
	void SetInsideTms(Bool insideTms);
	Bool GetInsideTma() const;
	void SetInsideTma(Bool insideTma);
	Double GetGeometricHeight() const;
	void SetGeometricHeight(Double geometricHeight);
	Double GetFlightLevel() const;
	void SetFlightLevel(Double flightLevel);
	Double GetGroundSpeed() const;
	void SetGroundSpeed(Double groundSpeed);
	Double GetVerticalRate() const;
	void SetVerticalRate(Double verticalRate);
	Int32 GetCategory() const;
	void SetCategory(Int32 category);
	Int32 GetSubgroup() const;
	void SetSubgroup(Int32 subgroup);
	Text::String* GetAircraftType() const;
	void SetAircraftType(Text::String* aircraftType);
	Data::Timestamp GetAtaAtd() const;
	void SetAtaAtd(Data::Timestamp ataAtd);
	Text::String* GetDeparture() const;
	void SetDeparture(Text::String* departure);
	Text::String* GetDestination() const;
	void SetDestination(Text::String* destination);
	Text::String* GetInOutbound() const;
	void SetInOutbound(Text::String* inOutbound);
	Text::String* GetRunway() const;
	void SetRunway(Text::String* runway);
	Text::String* GetServiceType() const;
	void SetServiceType(Text::String* serviceType);
	Text::String* GetIoFlight() const;
	void SetIoFlight(Text::String* ioFlight);

	Data::NamedClass<AdsbMovementstatistics> *CreateClass() const;
};

class FlightHoldingsPeriod
{
private:
	Text::String* finalUploadId;
	Data::Timestamp beginTime;
	Data::Timestamp endTime;
	Text::String* visitedwaypoints;
	Int32 seqno;

public:
	FlightHoldingsPeriod();
	~FlightHoldingsPeriod();

	Text::String* GetFinalUploadId() const;
	void SetFinalUploadId(Text::String* finalUploadId);
	Data::Timestamp GetBeginTime() const;
	void SetBeginTime(Data::Timestamp beginTime);
	Data::Timestamp GetEndTime() const;
	void SetEndTime(Data::Timestamp endTime);
	Text::String* GetVisitedwaypoints() const;
	void SetVisitedwaypoints(Text::String* visitedwaypoints);
	Int32 GetSeqno() const;
	void SetSeqno(Int32 seqno);

	Data::NamedClass<FlightHoldingsPeriod> *CreateClass() const;
};

AdsbMovementstatistics::AdsbMovementstatistics()
{
	this->uploadId = 0;
	this->finalUploadId = 0;
	this->adsbDatetime = 0;
	this->callsign = 0;
	this->latitude = 0;
	this->longitude = 0;
	this->geolocation = 0;
	this->insideHkfir = 0;
	this->insideAerodrome = 0;
	this->insideApp = 0;
	this->insideCtr = 0;
	this->insideTme = 0;
	this->insideTms = 0;
	this->insideTma = 0;
	this->geometricHeight = 0;
	this->flightLevel = 0;
	this->groundSpeed = 0;
	this->verticalRate = 0;
	this->category = 0;
	this->subgroup = 0;
	this->aircraftType = 0;
	this->ataAtd = 0;
	this->departure = 0;
	this->destination = 0;
	this->inOutbound = 0;
	this->runway = 0;
	this->serviceType = 0;
	this->ioFlight = 0;
}

AdsbMovementstatistics::~AdsbMovementstatistics()
{
	SDEL_STRING(this->uploadId);
	SDEL_STRING(this->finalUploadId);
	SDEL_STRING(this->callsign);
	SDEL_CLASS(this->geolocation);
	SDEL_STRING(this->aircraftType);
	SDEL_STRING(this->departure);
	SDEL_STRING(this->destination);
	SDEL_STRING(this->inOutbound);
	SDEL_STRING(this->runway);
	SDEL_STRING(this->serviceType);
	SDEL_STRING(this->ioFlight);
}

Text::String* AdsbMovementstatistics::GetUploadId() const
{
	return this->uploadId;
}

void AdsbMovementstatistics::SetUploadId(Text::String* uploadId)
{
	SDEL_STRING(this->uploadId);
	this->uploadId = uploadId?uploadId->Clone().Ptr():0;
}

Text::String* AdsbMovementstatistics::GetFinalUploadId() const
{
	return this->finalUploadId;
}

void AdsbMovementstatistics::SetFinalUploadId(Text::String* finalUploadId)
{
	SDEL_STRING(this->finalUploadId);
	this->finalUploadId = finalUploadId?finalUploadId->Clone().Ptr():0;
}

Data::Timestamp AdsbMovementstatistics::GetAdsbDatetime() const
{
	return this->adsbDatetime;
}

void AdsbMovementstatistics::SetAdsbDatetime(Data::Timestamp adsbDatetime)
{
	this->adsbDatetime = adsbDatetime;
}

Text::String* AdsbMovementstatistics::GetCallsign() const
{
	return this->callsign;
}

void AdsbMovementstatistics::SetCallsign(Text::String* callsign)
{
	SDEL_STRING(this->callsign);
	this->callsign = callsign?callsign->Clone().Ptr():0;
}

Double AdsbMovementstatistics::GetLatitude() const
{
	return this->latitude;
}

void AdsbMovementstatistics::SetLatitude(Double latitude)
{
	this->latitude = latitude;
}

Double AdsbMovementstatistics::GetLongitude() const
{
	return this->longitude;
}

void AdsbMovementstatistics::SetLongitude(Double longitude)
{
	this->longitude = longitude;
}

Math::Geometry::Vector2D* AdsbMovementstatistics::GetGeolocation() const
{
	return this->geolocation;
}

void AdsbMovementstatistics::SetGeolocation(Math::Geometry::Vector2D* geolocation)
{
	SDEL_CLASS(this->geolocation);
	this->geolocation = geolocation?geolocation->Clone():0;
}

Bool AdsbMovementstatistics::GetInsideHkfir() const
{
	return this->insideHkfir;
}

void AdsbMovementstatistics::SetInsideHkfir(Bool insideHkfir)
{
	this->insideHkfir = insideHkfir;
}

Bool AdsbMovementstatistics::GetInsideAerodrome() const
{
	return this->insideAerodrome;
}

void AdsbMovementstatistics::SetInsideAerodrome(Bool insideAerodrome)
{
	this->insideAerodrome = insideAerodrome;
}

Bool AdsbMovementstatistics::GetInsideApp() const
{
	return this->insideApp;
}

void AdsbMovementstatistics::SetInsideApp(Bool insideApp)
{
	this->insideApp = insideApp;
}

Bool AdsbMovementstatistics::GetInsideCtr() const
{
	return this->insideCtr;
}

void AdsbMovementstatistics::SetInsideCtr(Bool insideCtr)
{
	this->insideCtr = insideCtr;
}

Bool AdsbMovementstatistics::GetInsideTme() const
{
	return this->insideTme;
}

void AdsbMovementstatistics::SetInsideTme(Bool insideTme)
{
	this->insideTme = insideTme;
}

Bool AdsbMovementstatistics::GetInsideTms() const
{
	return this->insideTms;
}

void AdsbMovementstatistics::SetInsideTms(Bool insideTms)
{
	this->insideTms = insideTms;
}

Bool AdsbMovementstatistics::GetInsideTma() const
{
	return this->insideTma;
}

void AdsbMovementstatistics::SetInsideTma(Bool insideTma)
{
	this->insideTma = insideTma;
}

Double AdsbMovementstatistics::GetGeometricHeight() const
{
	return this->geometricHeight;
}

void AdsbMovementstatistics::SetGeometricHeight(Double geometricHeight)
{
	this->geometricHeight = geometricHeight;
}

Double AdsbMovementstatistics::GetFlightLevel() const
{
	return this->flightLevel;
}

void AdsbMovementstatistics::SetFlightLevel(Double flightLevel)
{
	this->flightLevel = flightLevel;
}

Double AdsbMovementstatistics::GetGroundSpeed() const
{
	return this->groundSpeed;
}

void AdsbMovementstatistics::SetGroundSpeed(Double groundSpeed)
{
	this->groundSpeed = groundSpeed;
}

Double AdsbMovementstatistics::GetVerticalRate() const
{
	return this->verticalRate;
}

void AdsbMovementstatistics::SetVerticalRate(Double verticalRate)
{
	this->verticalRate = verticalRate;
}

Int32 AdsbMovementstatistics::GetCategory() const
{
	return this->category;
}

void AdsbMovementstatistics::SetCategory(Int32 category)
{
	this->category = category;
}

Int32 AdsbMovementstatistics::GetSubgroup() const
{
	return this->subgroup;
}

void AdsbMovementstatistics::SetSubgroup(Int32 subgroup)
{
	this->subgroup = subgroup;
}

Text::String* AdsbMovementstatistics::GetAircraftType() const
{
	return this->aircraftType;
}

void AdsbMovementstatistics::SetAircraftType(Text::String* aircraftType)
{
	SDEL_STRING(this->aircraftType);
	this->aircraftType = aircraftType?aircraftType->Clone().Ptr():0;
}

Data::Timestamp AdsbMovementstatistics::GetAtaAtd() const
{
	return this->ataAtd;
}

void AdsbMovementstatistics::SetAtaAtd(Data::Timestamp ataAtd)
{
	this->ataAtd = ataAtd;
}

Text::String* AdsbMovementstatistics::GetDeparture() const
{
	return this->departure;
}

void AdsbMovementstatistics::SetDeparture(Text::String* departure)
{
	SDEL_STRING(this->departure);
	this->departure = departure?departure->Clone().Ptr():0;
}

Text::String* AdsbMovementstatistics::GetDestination() const
{
	return this->destination;
}

void AdsbMovementstatistics::SetDestination(Text::String* destination)
{
	SDEL_STRING(this->destination);
	this->destination = destination?destination->Clone().Ptr():0;
}

Text::String* AdsbMovementstatistics::GetInOutbound() const
{
	return this->inOutbound;
}

void AdsbMovementstatistics::SetInOutbound(Text::String* inOutbound)
{
	SDEL_STRING(this->inOutbound);
	this->inOutbound = inOutbound?inOutbound->Clone().Ptr():0;
}

Text::String* AdsbMovementstatistics::GetRunway() const
{
	return this->runway;
}

void AdsbMovementstatistics::SetRunway(Text::String* runway)
{
	SDEL_STRING(this->runway);
	this->runway = runway?runway->Clone().Ptr():0;
}

Text::String* AdsbMovementstatistics::GetServiceType() const
{
	return this->serviceType;
}

void AdsbMovementstatistics::SetServiceType(Text::String* serviceType)
{
	SDEL_STRING(this->serviceType);
	this->serviceType = serviceType?serviceType->Clone().Ptr():0;
}

Text::String* AdsbMovementstatistics::GetIoFlight() const
{
	return this->ioFlight;
}

void AdsbMovementstatistics::SetIoFlight(Text::String* ioFlight)
{
	SDEL_STRING(this->ioFlight);
	this->ioFlight = ioFlight?ioFlight->Clone().Ptr():0;
}

Data::NamedClass<AdsbMovementstatistics> *AdsbMovementstatistics::CreateClass() const
{
	Data::NamedClass<AdsbMovementstatistics> *cls;
	NEW_CLASS(cls, Data::NamedClass<AdsbMovementstatistics>(this));
	CLASS_ADD(cls, uploadId);
	CLASS_ADD(cls, finalUploadId);
	CLASS_ADD(cls, adsbDatetime);
	CLASS_ADD(cls, callsign);
	CLASS_ADD(cls, latitude);
	CLASS_ADD(cls, longitude);
	CLASS_ADD(cls, geolocation);
	CLASS_ADD(cls, insideHkfir);
	CLASS_ADD(cls, insideAerodrome);
	CLASS_ADD(cls, insideApp);
	CLASS_ADD(cls, insideCtr);
	CLASS_ADD(cls, insideTme);
	CLASS_ADD(cls, insideTms);
	CLASS_ADD(cls, insideTma);
	CLASS_ADD(cls, geometricHeight);
	CLASS_ADD(cls, flightLevel);
	CLASS_ADD(cls, groundSpeed);
	CLASS_ADD(cls, verticalRate);
	CLASS_ADD(cls, category);
	CLASS_ADD(cls, subgroup);
	CLASS_ADD(cls, aircraftType);
	CLASS_ADD(cls, ataAtd);
	CLASS_ADD(cls, departure);
	CLASS_ADD(cls, destination);
	CLASS_ADD(cls, inOutbound);
	CLASS_ADD(cls, runway);
	CLASS_ADD(cls, serviceType);
	CLASS_ADD(cls, ioFlight);
	return cls;
}

FlightHoldingsPeriod::FlightHoldingsPeriod()
{
	this->finalUploadId = 0;
	this->beginTime = 0;
	this->endTime = 0;
	this->visitedwaypoints = 0;
	this->seqno = 0;
}

FlightHoldingsPeriod::~FlightHoldingsPeriod()
{
	SDEL_STRING(this->finalUploadId);
	SDEL_STRING(this->visitedwaypoints);
}

Text::String* FlightHoldingsPeriod::GetFinalUploadId() const
{
	return this->finalUploadId;
}

void FlightHoldingsPeriod::SetFinalUploadId(Text::String* finalUploadId)
{
	SDEL_STRING(this->finalUploadId);
	this->finalUploadId = finalUploadId?finalUploadId->Clone().Ptr():0;
}

Data::Timestamp FlightHoldingsPeriod::GetBeginTime() const
{
	return this->beginTime;
}

void FlightHoldingsPeriod::SetBeginTime(Data::Timestamp beginTime)
{
	this->beginTime = beginTime;
}

Data::Timestamp FlightHoldingsPeriod::GetEndTime() const
{
	return this->endTime;
}

void FlightHoldingsPeriod::SetEndTime(Data::Timestamp endTime)
{
	this->endTime = endTime;
}

Text::String* FlightHoldingsPeriod::GetVisitedwaypoints() const
{
	return this->visitedwaypoints;
}

void FlightHoldingsPeriod::SetVisitedwaypoints(Text::String* visitedwaypoints)
{
	SDEL_STRING(this->visitedwaypoints);
	this->visitedwaypoints = visitedwaypoints?visitedwaypoints->Clone().Ptr():0;
}

Int32 FlightHoldingsPeriod::GetSeqno() const
{
	return this->seqno;
}

void FlightHoldingsPeriod::SetSeqno(Int32 seqno)
{
	this->seqno = seqno;
}

Data::NamedClass<FlightHoldingsPeriod> *FlightHoldingsPeriod::CreateClass() const
{
	Data::NamedClass<FlightHoldingsPeriod> *cls;
	NEW_CLASS(cls, Data::NamedClass<FlightHoldingsPeriod>(this));
	CLASS_ADD(cls, finalUploadId);
	CLASS_ADD(cls, beginTime);
	CLASS_ADD(cls, endTime);
	CLASS_ADD(cls, visitedwaypoints);
	CLASS_ADD(cls, seqno);
	return cls;
}

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	Text::CString serverHost;
	Text::CString database;
	Text::CString uid;
	Text::CString pwd;
	serverHost = CSTR("192.168.1.138");
	database = CSTR("ATMVAD_DEV");
	uid = CSTR("sa");
	pwd = CSTR("sa");

	IO::ConsoleWriter console;
	IO::LogTool log;
	IO::ConsoleLogHandler logHdlr(&console);
	log.AddLogHandler(&logHdlr, IO::LogHandler::LogLevel::Raw);
	DB::DBTool *db;
	db = DB::MSSQLConn::CreateDBToolTCP(serverHost, 1433, false, database, uid, pwd, &log, CSTR("DB: "));
	if (db)
	{
		DB::DBReader *r = db->QueryTableData(CSTR("dbo"), CSTR("Flight_Holdings_Period"), 0, 0, 0, CSTR_NULL, 0);
		if (r)
		{
			Manage::HiResClock clk;
			Data::NamedClass<FlightHoldingsPeriod> *cls;
			{
				FlightHoldingsPeriod ams;
				cls = ams.CreateClass();
			}
			UOSInt cnt = 0;
			{
				DB::DBClassReader<FlightHoldingsPeriod> reader(r, cls);
				while (true)
				{
					FlightHoldingsPeriod ams;
					if (!reader.ReadNext(&ams))
						break;
					cnt++;
				}
			}
			db->CloseReader(r);
			Double t = clk.GetTimeDiff();
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Time used = "));
			sb.AppendDouble(t);
			sb.AppendC(UTF8STRC(", rows read = "));
			sb.AppendUOSInt(cnt);
			console.WriteLineCStr(sb.ToCString());
			DEL_CLASS(cls);
		}
		DEL_CLASS(db);
	}
	else
	{
		console.WriteLineC(UTF8STRC("Error in connecting to database"));
	}
	return 0;
}
