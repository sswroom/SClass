#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/DateTime.h"
#include "Data/NamedClass.hpp"
#include "DB/DBClassReader.hpp"
#include "IO/ConsoleWriter.h"
#include "Net/MySQLTCPClient.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Text/String.h"

class Userfile
{
private:
	Int32 id;
	Int32 filetype;
	Optional<Text::String> orifilename;
	Data::Timestamp filetime;
	Double lat;
	Double lon;
	Int32 webuserId;
	Int32 speciesId;
	Data::Timestamp capturetime;
	Optional<Text::String> datafilename;
	Int32 crcval;
	Int32 rottype;
	Optional<Text::String> camera;
	Optional<Text::String> descript;
	Int32 loctype;
	Optional<Text::String> camerasn;
	Int32 prevupdated;
	Double cropleft;
	Double croptop;
	Double cropright;
	Double cropbottom;
	Optional<Text::String> location;

public:
	Userfile();
	~Userfile();

	Int32 GetId();
	void SetId(Int32 id);
	Int32 GetFiletype();
	void SetFiletype(Int32 filetype);
	Optional<Text::String> GetOrifilename();
	void SetOrifilename(Optional<Text::String> orifilename);
	Data::Timestamp GetFiletime();
	void SetFiletime(const Data::Timestamp &filetime);
	Double GetLat();
	void SetLat(Double lat);
	Double GetLon();
	void SetLon(Double lon);
	Int32 GetWebuserId();
	void SetWebuserId(Int32 webuserId);
	Int32 GetSpeciesId();
	void SetSpeciesId(Int32 speciesId);
	Data::Timestamp GetCapturetime();
	void SetCapturetime(const Data::Timestamp &capturetime);
	Optional<Text::String> GetDatafilename();
	void SetDatafilename(Optional<Text::String> datafilename);
	Int32 GetCrcval();
	void SetCrcval(Int32 crcval);
	Int32 GetRottype();
	void SetRottype(Int32 rottype);
	Optional<Text::String> GetCamera();
	void SetCamera(Optional<Text::String> camera);
	Optional<Text::String> GetDescript();
	void SetDescript(Optional<Text::String> descript);
	Int32 GetLoctype();
	void SetLoctype(Int32 loctype);
	Optional<Text::String> GetCamerasn();
	void SetCamerasn(Optional<Text::String> camerasn);
	Int32 GetPrevupdated();
	void SetPrevupdated(Int32 prevupdated);
	Double GetCropleft();
	void SetCropleft(Double cropleft);
	Double GetCroptop();
	void SetCroptop(Double croptop);
	Double GetCropright();
	void SetCropright(Double cropright);
	Double GetCropbottom();
	void SetCropbottom(Double cropbottom);
	Optional<Text::String> GetLocation();
	void SetLocation(Optional<Text::String> location);

	NN<Data::NamedClass<Userfile>> CreateClass();
};

Userfile::Userfile()
{
	this->id = 0;
	this->filetype = 0;
	this->orifilename = nullptr;
	this->filetime = 0;
	this->lat = 0;
	this->lon = 0;
	this->webuserId = 0;
	this->speciesId = 0;
	this->capturetime = 0;
	this->datafilename = nullptr;
	this->crcval = 0;
	this->rottype = 0;
	this->camera = nullptr;
	this->descript = nullptr;
	this->loctype = 0;
	this->camerasn = nullptr;
	this->prevupdated = 0;
	this->cropleft = 0;
	this->croptop = 0;
	this->cropright = 0;
	this->cropbottom = 0;
	this->location = nullptr;
}

Userfile::~Userfile()
{
	OPTSTR_DEL(this->orifilename);
	OPTSTR_DEL(this->datafilename);
	OPTSTR_DEL(this->camera);
	OPTSTR_DEL(this->descript);
	OPTSTR_DEL(this->camerasn);
	OPTSTR_DEL(this->location);
}

Int32 Userfile::GetId()
{
	return this->id;
}

void Userfile::SetId(Int32 id)
{
	this->id = id;
}

Int32 Userfile::GetFiletype()
{
	return this->filetype;
}

void Userfile::SetFiletype(Int32 filetype)
{
	this->filetype = filetype;
}

Optional<Text::String> Userfile::GetOrifilename()
{
	return this->orifilename;
}

void Userfile::SetOrifilename(Optional<Text::String> orifilename)
{
	OPTSTR_DEL(this->orifilename);
	this->orifilename = Text::String::CopyOrNull(orifilename);
}

Data::Timestamp Userfile::GetFiletime()
{
	return this->filetime;
}

void Userfile::SetFiletime(const Data::Timestamp &filetime)
{
	this->filetime = filetime;
}

Double Userfile::GetLat()
{
	return this->lat;
}

void Userfile::SetLat(Double lat)
{
	this->lat = lat;
}

Double Userfile::GetLon()
{
	return this->lon;
}

void Userfile::SetLon(Double lon)
{
	this->lon = lon;
}

Int32 Userfile::GetWebuserId()
{
	return this->webuserId;
}

void Userfile::SetWebuserId(Int32 webuserId)
{
	this->webuserId = webuserId;
}

Int32 Userfile::GetSpeciesId()
{
	return this->speciesId;
}

void Userfile::SetSpeciesId(Int32 speciesId)
{
	this->speciesId = speciesId;
}

Data::Timestamp Userfile::GetCapturetime()
{
	return this->capturetime;
}

void Userfile::SetCapturetime(const Data::Timestamp &capturetime)
{
	this->capturetime = capturetime;
}

Optional<Text::String> Userfile::GetDatafilename()
{
	return this->datafilename;
}

void Userfile::SetDatafilename(Optional<Text::String> datafilename)
{
	OPTSTR_DEL(this->datafilename);
	this->datafilename = Text::String::CopyOrNull(datafilename);
}

Int32 Userfile::GetCrcval()
{
	return this->crcval;
}

void Userfile::SetCrcval(Int32 crcval)
{
	this->crcval = crcval;
}

Int32 Userfile::GetRottype()
{
	return this->rottype;
}

void Userfile::SetRottype(Int32 rottype)
{
	this->rottype = rottype;
}

Optional<Text::String> Userfile::GetCamera()
{
	return this->camera;
}

void Userfile::SetCamera(Optional<Text::String> camera)
{
	OPTSTR_DEL(this->camera);
	this->camera = Text::String::CopyOrNull(camera);
}

Optional<Text::String> Userfile::GetDescript()
{
	return this->descript;
}

void Userfile::SetDescript(Optional<Text::String> descript)
{
	OPTSTR_DEL(this->descript);
	this->descript = Text::String::CopyOrNull(descript);
}

Int32 Userfile::GetLoctype()
{
	return this->loctype;
}

void Userfile::SetLoctype(Int32 loctype)
{
	this->loctype = loctype;
}

Optional<Text::String> Userfile::GetCamerasn()
{
	return this->camerasn;
}

void Userfile::SetCamerasn(Optional<Text::String> camerasn)
{
	OPTSTR_DEL(this->camerasn);
	this->camerasn = Text::String::CopyOrNull(camerasn);
}

Int32 Userfile::GetPrevupdated()
{
	return this->prevupdated;
}

void Userfile::SetPrevupdated(Int32 prevupdated)
{
	this->prevupdated = prevupdated;
}

Double Userfile::GetCropleft()
{
	return this->cropleft;
}

void Userfile::SetCropleft(Double cropleft)
{
	this->cropleft = cropleft;
}

Double Userfile::GetCroptop()
{
	return this->croptop;
}

void Userfile::SetCroptop(Double croptop)
{
	this->croptop = croptop;
}

Double Userfile::GetCropright()
{
	return this->cropright;
}

void Userfile::SetCropright(Double cropright)
{
	this->cropright = cropright;
}

Double Userfile::GetCropbottom()
{
	return this->cropbottom;
}

void Userfile::SetCropbottom(Double cropbottom)
{
	this->cropbottom = cropbottom;
}

Optional<Text::String> Userfile::GetLocation()
{
	return this->location;
}

void Userfile::SetLocation(Optional<Text::String> location)
{
	OPTSTR_DEL(this->location);
	this->location = Text::String::CopyOrNull(location);
}

NN<Data::NamedClass<Userfile>> Userfile::CreateClass()
{
	NN<Data::NamedClass<Userfile>> cls;
	NEW_CLASSNN(cls, Data::NamedClass<Userfile>(this));
	CLASS_ADD(cls, id);
	CLASS_ADD(cls, filetype);
	CLASS_ADD(cls, orifilename);
	CLASS_ADD(cls, filetime);
	CLASS_ADD(cls, lat);
	CLASS_ADD(cls, lon);
	CLASS_ADD(cls, webuserId);
	CLASS_ADD(cls, speciesId);
	CLASS_ADD(cls, capturetime);
	CLASS_ADD(cls, datafilename);
	CLASS_ADD(cls, crcval);
	CLASS_ADD(cls, rottype);
	CLASS_ADD(cls, camera);
	CLASS_ADD(cls, descript);
	CLASS_ADD(cls, loctype);
	CLASS_ADD(cls, camerasn);
	CLASS_ADD(cls, prevupdated);
	CLASS_ADD(cls, cropleft);
	CLASS_ADD(cls, croptop);
	CLASS_ADD(cls, cropright);
	CLASS_ADD(cls, cropbottom);
	CLASS_ADD(cls, location);
	return cls;
}

IO::ConsoleWriter *console;

void TextReadAll(NN<DB::DBTool> db)
{
	Data::ArrayListNN<Userfile> dataList;
	NN<DB::DBReader> r;
	if (db->QueryTableData(nullptr, CSTR("userfile"), nullptr, 0, 0, nullptr, nullptr).SetTo(r))
	{
		NN<Data::NamedClass<Userfile>> cls = Userfile().CreateClass();
		{
			DB::DBClassReader<Userfile> reader(r, cls);
			reader.ReadAll(dataList);
		}
		db->CloseReader(r);
		cls.Delete();
		dataList.DeleteAll();
	}
	else
	{
		console->WriteLine(CSTR("Error in loading reading table"));
	}
}

void TestBinaryRead(NN<DB::DBTool> db)
{
	Data::ArrayListNN<Userfile> dataList;
	NN<Net::MySQLTCPClient> conn = NN<Net::MySQLTCPClient>::ConvertFrom(db->GetDBConn());
	NN<DB::DBReader> r;
	if (conn->ExecuteReaderBinary(CSTR("select * from userfile")).SetTo(r))
	{
		NN<Data::NamedClass<Userfile>> cls = Userfile().CreateClass();
		{
			DB::DBClassReader<Userfile> reader(r, cls);
			reader.ReadAll(dataList);
		}
		db->CloseReader(r);
		cls.Delete();
		dataList.DeleteAll();
/*
		UIntOS rowCnt = 0;
		UTF8Char sbuff[64];
		UnsafeArray<UTF8Char> sptr;
		while (r->ReadNext())
		{
			rowCnt++;
			sptr = Text::StrConcatC(Text::StrUIntOS(sbuff, rowCnt), UTF8STRC(" rows read"));
			console->WriteLine(CSTRP(sbuff, sptr));
		}
		conn->CloseReader(r);*/
	}
	else
	{
		console->WriteLine(CSTR("Error in loading reading table"));
	}
}

void TempTest(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, IO::Writer *console)
{
	Text::CStringNN mysqlServer;
	Text::CStringNN mysqlDB;
	Text::CStringNN mysqlUID;
	Text::CStringNN mysqlPWD;
	NN<DB::DBTool> db;
	IO::LogTool log;
	mysqlServer = CSTR("192.168.0.15");
	mysqlDB = CSTR("organism");
	mysqlUID = CSTR("organ");
	mysqlPWD = CSTR("organ");
	if (Net::MySQLTCPClient::CreateDBTool(clif, ssl, mysqlServer, mysqlDB, mysqlUID, mysqlPWD, log, CSTR("DB: ")).SetTo(db))
	{
		NN<DB::DBReader> r;
		if (db->ExecuteReader(CSTR("select id, time1, time2 from test")).SetTo(r))
		{
			UTF8Char sbuff[64];
			UnsafeArray<UTF8Char> sptr;
			while (r->ReadNext())
			{
				sptr = r->GetTimestamp(1).ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
				console->WriteLine(CSTRP(sbuff, sptr));
				sptr = r->GetTimestamp(2).ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
				console->WriteLine(CSTRP(sbuff, sptr));
			}
			db->CloseReader(r);
		}
		db.Delete();
	}
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Text::CStringNN mysqlServer;
	Text::CStringNN mysqlDB;
	Text::CStringNN mysqlUID;
	Text::CStringNN mysqlPWD;
	mysqlServer = CSTR("192.168.0.15");
	mysqlDB = CSTR("organism");
	mysqlUID = CSTR("organ");
	mysqlPWD = CSTR("organ");

	IO::LogTool log;
	NN<DB::DBTool> db;
	NEW_CLASS(console, IO::ConsoleWriter());
	Net::OSSocketFactory sockf(false);
	Net::TCPClientFactory clif(sockf);
	Optional<Net::SSLEngine> ssl = Net::SSLEngineFactory::Create(clif, false);
	TempTest(clif, ssl, console);
	if (Net::MySQLTCPClient::CreateDBTool(clif, ssl, mysqlServer, mysqlDB, mysqlUID, mysqlPWD, log, CSTR("DB: ")).SetTo(db))
	{
		TestBinaryRead(db);
		TextReadAll(db);

		db.Delete();
	}
	else
	{
		console->WriteLine(CSTR("Error in opening database"));
	}
	DEL_CLASS(console);
	return 0;
}
