#include "Stdafx.h"
#include "Core/Core.h"
#include "Data/DateTime.h"
#include "Data/NamedClass.h"
#include "IO/ConsoleWriter.h"
#include "Net/MySQLTCPClient.h"
#include "Net/OSSocketFactory.h"
#include "Text/String.h"

class Userfile
{
private:
	Int32 id;
	Int32 filetype;
	Text::String* orifilename;
	Data::DateTime* filetime;
	Double lat;
	Double lon;
	Int32 webuserId;
	Int32 speciesId;
	Data::DateTime* capturetime;
	Text::String* datafilename;
	Int32 crcval;
	Int32 rottype;
	Text::String* camera;
	Text::String* descript;
	Int32 loctype;
	Text::String* camerasn;
	Int32 prevupdated;
	Double cropleft;
	Double croptop;
	Double cropright;
	Double cropbottom;
	Text::String* location;

public:
	Userfile();
	~Userfile();

	Int32 GetId();
	void SetId(Int32 id);
	Int32 GetFiletype();
	void SetFiletype(Int32 filetype);
	Text::String* GetOrifilename();
	void SetOrifilename(Text::String* orifilename);
	Data::DateTime* GetFiletime();
	void SetFiletime(Data::DateTime* filetime);
	Double GetLat();
	void SetLat(Double lat);
	Double GetLon();
	void SetLon(Double lon);
	Int32 GetWebuserId();
	void SetWebuserId(Int32 webuserId);
	Int32 GetSpeciesId();
	void SetSpeciesId(Int32 speciesId);
	Data::DateTime* GetCapturetime();
	void SetCapturetime(Data::DateTime* capturetime);
	Text::String* GetDatafilename();
	void SetDatafilename(Text::String* datafilename);
	Int32 GetCrcval();
	void SetCrcval(Int32 crcval);
	Int32 GetRottype();
	void SetRottype(Int32 rottype);
	Text::String* GetCamera();
	void SetCamera(Text::String* camera);
	Text::String* GetDescript();
	void SetDescript(Text::String* descript);
	Int32 GetLoctype();
	void SetLoctype(Int32 loctype);
	Text::String* GetCamerasn();
	void SetCamerasn(Text::String* camerasn);
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
	Text::String* GetLocation();
	void SetLocation(Text::String* location);

	Data::NamedClass<Userfile> *CreateClass();
};

Userfile::Userfile()
{
	this->id = 0;
	this->filetype = 0;
	this->orifilename = 0;
	this->filetime = 0;
	this->lat = 0;
	this->lon = 0;
	this->webuserId = 0;
	this->speciesId = 0;
	this->capturetime = 0;
	this->datafilename = 0;
	this->crcval = 0;
	this->rottype = 0;
	this->camera = 0;
	this->descript = 0;
	this->loctype = 0;
	this->camerasn = 0;
	this->prevupdated = 0;
	this->cropleft = 0;
	this->croptop = 0;
	this->cropright = 0;
	this->cropbottom = 0;
	this->location = 0;
}

Userfile::~Userfile()
{
	SDEL_STRING(this->orifilename);
	SDEL_CLASS(this->filetime);
	SDEL_CLASS(this->capturetime);
	SDEL_STRING(this->datafilename);
	SDEL_STRING(this->camera);
	SDEL_STRING(this->descript);
	SDEL_STRING(this->camerasn);
	SDEL_STRING(this->location);
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

Text::String* Userfile::GetOrifilename()
{
	return this->orifilename;
}

void Userfile::SetOrifilename(Text::String* orifilename)
{
	SDEL_STRING(this->orifilename);
	this->orifilename = orifilename?orifilename->Clone():0;
}

Data::DateTime* Userfile::GetFiletime()
{
	return this->filetime;
}

void Userfile::SetFiletime(Data::DateTime* filetime)
{
	SDEL_CLASS(this->filetime);
	this->filetime = filetime?(NEW_CLASS_D(Data::DateTime(filetime))):0;
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

Data::DateTime* Userfile::GetCapturetime()
{
	return this->capturetime;
}

void Userfile::SetCapturetime(Data::DateTime* capturetime)
{
	SDEL_CLASS(this->capturetime);
	this->capturetime = capturetime?(NEW_CLASS_D(Data::DateTime(capturetime))):0;
}

Text::String* Userfile::GetDatafilename()
{
	return this->datafilename;
}

void Userfile::SetDatafilename(Text::String* datafilename)
{
	SDEL_STRING(this->datafilename);
	this->datafilename = datafilename?datafilename->Clone():0;
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

Text::String* Userfile::GetCamera()
{
	return this->camera;
}

void Userfile::SetCamera(Text::String* camera)
{
	SDEL_STRING(this->camera);
	this->camera = camera?camera->Clone():0;
}

Text::String* Userfile::GetDescript()
{
	return this->descript;
}

void Userfile::SetDescript(Text::String* descript)
{
	SDEL_STRING(this->descript);
	this->descript = descript?descript->Clone():0;
}

Int32 Userfile::GetLoctype()
{
	return this->loctype;
}

void Userfile::SetLoctype(Int32 loctype)
{
	this->loctype = loctype;
}

Text::String* Userfile::GetCamerasn()
{
	return this->camerasn;
}

void Userfile::SetCamerasn(Text::String* camerasn)
{
	SDEL_STRING(this->camerasn);
	this->camerasn = camerasn?camerasn->Clone():0;
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

Text::String* Userfile::GetLocation()
{
	return this->location;
}

void Userfile::SetLocation(Text::String* location)
{
	SDEL_STRING(this->location);
	this->location = location?location->Clone():0;
}

Data::NamedClass<Userfile> *Userfile::CreateClass()
{
	Data::NamedClass<Userfile> *cls;
	NEW_CLASS(cls, Data::NamedClass<Userfile>(this));
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

void TextReadAll(DB::DBTool *db)
{
	Data::ArrayList<Userfile*> dataList;
	DB::DBReader *r = db->GetTableData((const UTF8Char*)"userfile", 0, 0, 0, 0, 0);
	if (r)
	{
		Data::NamedClass<Userfile> *cls = Userfile().CreateClass();
		r->ReadAll(&dataList, cls);
		db->CloseReader(r);
		DEL_CLASS(cls);

		Userfile *userfile;
		UOSInt i = dataList.GetCount();
		while (i-- > 0)
		{
			userfile = dataList.GetItem(i);
			DEL_CLASS(userfile);
		}

	}
	else
	{
		console->WriteLineC(UTF8STRC("Error in loading reading table"));
	}
}

void TestBinaryRead(DB::DBTool *db)
{
	Data::ArrayList<Userfile*> dataList;
	Net::MySQLTCPClient *conn = (Net::MySQLTCPClient*)db->GetDBConn();
	DB::DBReader *r = conn->ExecuteReaderBinaryC(UTF8STRC("select * from userfile"));
	if (r)
	{
		Data::NamedClass<Userfile> *cls = Userfile().CreateClass();
		r->ReadAll(&dataList, cls);
		db->CloseReader(r);
		DEL_CLASS(cls);

		Userfile *userfile;
		UOSInt i = dataList.GetCount();
		while (i-- > 0)
		{
			userfile = dataList.GetItem(i);
			DEL_CLASS(userfile);
		}
/*
		UOSInt rowCnt = 0;
		UTF8Char sbuff[64];
		UTF8Char *sptr;
		while (r->ReadNext())
		{
			rowCnt++;
			sptr = Text::StrConcatC(Text::StrUOSInt(sbuff, rowCnt), UTF8STRC(" rows read"));
			console->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
		}
		conn->CloseReader(r);*/
	}
	else
	{
		console->WriteLineC(UTF8STRC("Error in loading reading table"));
	}
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Text::CString mysqlServer;
	Text::CString mysqlDB;
	Text::CString mysqlUID;
	Text::CString mysqlPWD;
	mysqlServer = CSTR("192.168.0.15");
	mysqlDB = CSTR("organism");
	mysqlUID = CSTR("organ");
	mysqlPWD = CSTR("organ");

	IO::LogTool log;
	Net::SocketFactory *sockf;
	DB::DBTool *db;
	NEW_CLASS(console, IO::ConsoleWriter());
	NEW_CLASS(sockf, Net::OSSocketFactory(false));
	db = Net::MySQLTCPClient::CreateDBTool(sockf, mysqlServer, mysqlDB, mysqlUID, mysqlPWD, &log, CSTR("DB: "));
	if (db)
	{
		TestBinaryRead(db);
		TextReadAll(db);

		DEL_CLASS(db);
	}
	else
	{
		console->WriteLineC(UTF8STRC("Error in opening database"));
	}
	DEL_CLASS(sockf);
	DEL_CLASS(console);
	return 0;
}