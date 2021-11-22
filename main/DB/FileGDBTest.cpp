#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/NamedClass.h"
#include "DB/CSVFile.h"
#include "IO/ConsoleWriter.h"
#include "IO/DirectoryPackage.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Manage/HiResClock.h"
#include "Map/ESRI/FileGDBDir.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/StringTool.h"

class Lamppost
{
private:
	const UTF8Char* accLocati;
	const UTF8Char* antiBurgl;
	Int16 bulbQty;
	Data::DateTime* cmsDate;
	const UTF8Char* cmsType;
	Int16 colClass;
	Single columnHt;
	Single concD;
	Single concL;
	Single concW;
	const UTF8Char* contrNo;
	const UTF8Char* czone;
	const UTF8Char* deleted;
	Data::DateTime* disconDate;
	const UTF8Char* district;
	const UTF8Char* fixOPole;
	Int16 iVersion;
	Data::DateTime* installDate;
	Single kerbOs;
	const UTF8Char* lampNo;
	const UTF8Char* lampType;
	const UTF8Char* landmark;
	const UTF8Char* ltgRegion;
	const UTF8Char* mounting;
	const UTF8Char* nOPoint;
	const UTF8Char* nemaSoc;
	Int32 objectid;
	Int32 objectidP;
	const UTF8Char* pUserFid;
	const UTF8Char* paintMate;
	Single plDepth;
	const UTF8Char* poleManu;
	const UTF8Char* poleType;
	const UTF8Char* polecolor;
	const UTF8Char* primarySt;
	Data::DateTime* regDate;
	const UTF8Char* remark1;
	const UTF8Char* remark2;
	const UTF8Char* remark3;
	Int16 rotation;
	const UTF8Char* routeno;
	const UTF8Char* secondSt;
	Math::Vector2D* shape;
	const UTF8Char* shielded;
	const UTF8Char* solarLtg;
	const UTF8Char* specMate;
	Data::DateTime* subDate;
	const UTF8Char* subtype;
	const UTF8Char* updbyhyd;
	const UTF8Char* updbymom;
	Data::DateTime* upddatehyd;
	Data::DateTime* upddatemom;
	const UTF8Char* utilNo;
	const UTF8Char* village;
	Single zCoor;

public:
	Lamppost();
	~Lamppost();

	const UTF8Char* GetAccLocati();
	void SetAccLocati(const UTF8Char* accLocati);
	const UTF8Char* GetAntiBurgl();
	void SetAntiBurgl(const UTF8Char* antiBurgl);
	Int16 GetBulbQty();
	void SetBulbQty(Int16 bulbQty);
	Data::DateTime* GetCmsDate();
	void SetCmsDate(Data::DateTime* cmsDate);
	const UTF8Char* GetCmsType();
	void SetCmsType(const UTF8Char* cmsType);
	Int16 GetColClass();
	void SetColClass(Int16 colClass);
	Single GetColumnHt();
	void SetColumnHt(Single columnHt);
	Single GetConcD();
	void SetConcD(Single concD);
	Single GetConcL();
	void SetConcL(Single concL);
	Single GetConcW();
	void SetConcW(Single concW);
	const UTF8Char* GetContrNo();
	void SetContrNo(const UTF8Char* contrNo);
	const UTF8Char* GetCzone();
	void SetCzone(const UTF8Char* czone);
	const UTF8Char* GetDeleted();
	void SetDeleted(const UTF8Char* deleted);
	Data::DateTime* GetDisconDate();
	void SetDisconDate(Data::DateTime* disconDate);
	const UTF8Char* GetDistrict();
	void SetDistrict(const UTF8Char* district);
	const UTF8Char* GetFixOPole();
	void SetFixOPole(const UTF8Char* fixOPole);
	Int16 GetIVersion();
	void SetIVersion(Int16 iVersion);
	Data::DateTime* GetInstallDate();
	void SetInstallDate(Data::DateTime* installDate);
	Single GetKerbOs();
	void SetKerbOs(Single kerbOs);
	const UTF8Char* GetLampNo();
	void SetLampNo(const UTF8Char* lampNo);
	const UTF8Char* GetLampType();
	void SetLampType(const UTF8Char* lampType);
	const UTF8Char* GetLandmark();
	void SetLandmark(const UTF8Char* landmark);
	const UTF8Char* GetLtgRegion();
	void SetLtgRegion(const UTF8Char* ltgRegion);
	const UTF8Char* GetMounting();
	void SetMounting(const UTF8Char* mounting);
	const UTF8Char* GetNOPoint();
	void SetNOPoint(const UTF8Char* nOPoint);
	const UTF8Char* GetNemaSoc();
	void SetNemaSoc(const UTF8Char* nemaSoc);
	Int32 GetObjectid();
	void SetObjectid(Int32 objectid);
	Int32 GetObjectidP();
	void SetObjectidP(Int32 objectidP);
	const UTF8Char* GetPUserFid();
	void SetPUserFid(const UTF8Char* pUserFid);
	const UTF8Char* GetPaintMate();
	void SetPaintMate(const UTF8Char* paintMate);
	Single GetPlDepth();
	void SetPlDepth(Single plDepth);
	const UTF8Char* GetPoleManu();
	void SetPoleManu(const UTF8Char* poleManu);
	const UTF8Char* GetPoleType();
	void SetPoleType(const UTF8Char* poleType);
	const UTF8Char* GetPolecolor();
	void SetPolecolor(const UTF8Char* polecolor);
	const UTF8Char* GetPrimarySt();
	void SetPrimarySt(const UTF8Char* primarySt);
	Data::DateTime* GetRegDate();
	void SetRegDate(Data::DateTime* regDate);
	const UTF8Char* GetRemark1();
	void SetRemark1(const UTF8Char* remark1);
	const UTF8Char* GetRemark2();
	void SetRemark2(const UTF8Char* remark2);
	const UTF8Char* GetRemark3();
	void SetRemark3(const UTF8Char* remark3);
	Int16 GetRotation();
	void SetRotation(Int16 rotation);
	const UTF8Char* GetRouteno();
	void SetRouteno(const UTF8Char* routeno);
	const UTF8Char* GetSecondSt();
	void SetSecondSt(const UTF8Char* secondSt);
	Math::Vector2D* GetShape();
	void SetShape(Math::Vector2D* shape);
	const UTF8Char* GetShielded();
	void SetShielded(const UTF8Char* shielded);
	const UTF8Char* GetSolarLtg();
	void SetSolarLtg(const UTF8Char* solarLtg);
	const UTF8Char* GetSpecMate();
	void SetSpecMate(const UTF8Char* specMate);
	Data::DateTime* GetSubDate();
	void SetSubDate(Data::DateTime* subDate);
	const UTF8Char* GetSubtype();
	void SetSubtype(const UTF8Char* subtype);
	const UTF8Char* GetUpdbyhyd();
	void SetUpdbyhyd(const UTF8Char* updbyhyd);
	const UTF8Char* GetUpdbymom();
	void SetUpdbymom(const UTF8Char* updbymom);
	Data::DateTime* GetUpddatehyd();
	void SetUpddatehyd(Data::DateTime* upddatehyd);
	Data::DateTime* GetUpddatemom();
	void SetUpddatemom(Data::DateTime* upddatemom);
	const UTF8Char* GetUtilNo();
	void SetUtilNo(const UTF8Char* utilNo);
	const UTF8Char* GetVillage();
	void SetVillage(const UTF8Char* village);
	Single GetZCoor();
	void SetZCoor(Single zCoor);

	Data::NamedClass<Lamppost> *CreateClass();
};

Lamppost::Lamppost()
{
	this->accLocati = 0;
	this->antiBurgl = 0;
	this->bulbQty = 0;
	this->cmsDate = 0;
	this->cmsType = 0;
	this->colClass = 0;
	this->columnHt = 0;
	this->concD = 0;
	this->concL = 0;
	this->concW = 0;
	this->contrNo = 0;
	this->czone = 0;
	this->deleted = 0;
	this->disconDate = 0;
	this->district = 0;
	this->fixOPole = 0;
	this->iVersion = 0;
	this->installDate = 0;
	this->kerbOs = 0;
	this->lampNo = 0;
	this->lampType = 0;
	this->landmark = 0;
	this->ltgRegion = 0;
	this->mounting = 0;
	this->nOPoint = 0;
	this->nemaSoc = 0;
	this->objectid = 0;
	this->objectidP = 0;
	this->pUserFid = 0;
	this->paintMate = 0;
	this->plDepth = 0;
	this->poleManu = 0;
	this->poleType = 0;
	this->polecolor = 0;
	this->primarySt = 0;
	this->regDate = 0;
	this->remark1 = 0;
	this->remark2 = 0;
	this->remark3 = 0;
	this->rotation = 0;
	this->routeno = 0;
	this->secondSt = 0;
	this->shape = 0;
	this->shielded = 0;
	this->solarLtg = 0;
	this->specMate = 0;
	this->subDate = 0;
	this->subtype = 0;
	this->updbyhyd = 0;
	this->updbymom = 0;
	this->upddatehyd = 0;
	this->upddatemom = 0;
	this->utilNo = 0;
	this->village = 0;
	this->zCoor = 0;
}

Lamppost::~Lamppost()
{
	SDEL_TEXT(this->accLocati);
	SDEL_TEXT(this->antiBurgl);
	SDEL_CLASS(this->cmsDate);
	SDEL_TEXT(this->cmsType);
	SDEL_TEXT(this->contrNo);
	SDEL_TEXT(this->czone);
	SDEL_TEXT(this->deleted);
	SDEL_CLASS(this->disconDate);
	SDEL_TEXT(this->district);
	SDEL_TEXT(this->fixOPole);
	SDEL_CLASS(this->installDate);
	SDEL_TEXT(this->lampNo);
	SDEL_TEXT(this->lampType);
	SDEL_TEXT(this->landmark);
	SDEL_TEXT(this->ltgRegion);
	SDEL_TEXT(this->mounting);
	SDEL_TEXT(this->nOPoint);
	SDEL_TEXT(this->nemaSoc);
	SDEL_TEXT(this->pUserFid);
	SDEL_TEXT(this->paintMate);
	SDEL_TEXT(this->poleManu);
	SDEL_TEXT(this->poleType);
	SDEL_TEXT(this->polecolor);
	SDEL_TEXT(this->primarySt);
	SDEL_CLASS(this->regDate);
	SDEL_TEXT(this->remark1);
	SDEL_TEXT(this->remark2);
	SDEL_TEXT(this->remark3);
	SDEL_TEXT(this->routeno);
	SDEL_TEXT(this->secondSt);
	SDEL_CLASS(this->shape);
	SDEL_TEXT(this->shielded);
	SDEL_TEXT(this->solarLtg);
	SDEL_TEXT(this->specMate);
	SDEL_CLASS(this->subDate);
	SDEL_TEXT(this->subtype);
	SDEL_TEXT(this->updbyhyd);
	SDEL_TEXT(this->updbymom);
	SDEL_CLASS(this->upddatehyd);
	SDEL_CLASS(this->upddatemom);
	SDEL_TEXT(this->utilNo);
	SDEL_TEXT(this->village);
}

const UTF8Char* Lamppost::GetAccLocati()
{
	return this->accLocati;
}

void Lamppost::SetAccLocati(const UTF8Char* accLocati)
{
	this->accLocati = SCOPY_TEXT(accLocati);
}

const UTF8Char* Lamppost::GetAntiBurgl()
{
	return this->antiBurgl;
}

void Lamppost::SetAntiBurgl(const UTF8Char* antiBurgl)
{
	this->antiBurgl = SCOPY_TEXT(antiBurgl);
}

Int16 Lamppost::GetBulbQty()
{
	return this->bulbQty;
}

void Lamppost::SetBulbQty(Int16 bulbQty)
{
	this->bulbQty = bulbQty;
}

Data::DateTime* Lamppost::GetCmsDate()
{
	return this->cmsDate;
}

void Lamppost::SetCmsDate(Data::DateTime* cmsDate)
{
	this->cmsDate = cmsDate?(NEW_CLASS_D(Data::DateTime(cmsDate))):0;
}

const UTF8Char* Lamppost::GetCmsType()
{
	return this->cmsType;
}

void Lamppost::SetCmsType(const UTF8Char* cmsType)
{
	this->cmsType = SCOPY_TEXT(cmsType);
}

Int16 Lamppost::GetColClass()
{
	return this->colClass;
}

void Lamppost::SetColClass(Int16 colClass)
{
	this->colClass = colClass;
}

Single Lamppost::GetColumnHt()
{
	return this->columnHt;
}

void Lamppost::SetColumnHt(Single columnHt)
{
	this->columnHt = columnHt;
}

Single Lamppost::GetConcD()
{
	return this->concD;
}

void Lamppost::SetConcD(Single concD)
{
	this->concD = concD;
}

Single Lamppost::GetConcL()
{
	return this->concL;
}

void Lamppost::SetConcL(Single concL)
{
	this->concL = concL;
}

Single Lamppost::GetConcW()
{
	return this->concW;
}

void Lamppost::SetConcW(Single concW)
{
	this->concW = concW;
}

const UTF8Char* Lamppost::GetContrNo()
{
	return this->contrNo;
}

void Lamppost::SetContrNo(const UTF8Char* contrNo)
{
	this->contrNo = SCOPY_TEXT(contrNo);
}

const UTF8Char* Lamppost::GetCzone()
{
	return this->czone;
}

void Lamppost::SetCzone(const UTF8Char* czone)
{
	this->czone = SCOPY_TEXT(czone);
}

const UTF8Char* Lamppost::GetDeleted()
{
	return this->deleted;
}

void Lamppost::SetDeleted(const UTF8Char* deleted)
{
	this->deleted = SCOPY_TEXT(deleted);
}

Data::DateTime* Lamppost::GetDisconDate()
{
	return this->disconDate;
}

void Lamppost::SetDisconDate(Data::DateTime* disconDate)
{
	this->disconDate = disconDate?(NEW_CLASS_D(Data::DateTime(disconDate))):0;
}

const UTF8Char* Lamppost::GetDistrict()
{
	return this->district;
}

void Lamppost::SetDistrict(const UTF8Char* district)
{
	this->district = SCOPY_TEXT(district);
}

const UTF8Char* Lamppost::GetFixOPole()
{
	return this->fixOPole;
}

void Lamppost::SetFixOPole(const UTF8Char* fixOPole)
{
	this->fixOPole = SCOPY_TEXT(fixOPole);
}

Int16 Lamppost::GetIVersion()
{
	return this->iVersion;
}

void Lamppost::SetIVersion(Int16 iVersion)
{
	this->iVersion = iVersion;
}

Data::DateTime* Lamppost::GetInstallDate()
{
	return this->installDate;
}

void Lamppost::SetInstallDate(Data::DateTime* installDate)
{
	this->installDate = installDate?(NEW_CLASS_D(Data::DateTime(installDate))):0;
}

Single Lamppost::GetKerbOs()
{
	return this->kerbOs;
}

void Lamppost::SetKerbOs(Single kerbOs)
{
	this->kerbOs = kerbOs;
}

const UTF8Char* Lamppost::GetLampNo()
{
	return this->lampNo;
}

void Lamppost::SetLampNo(const UTF8Char* lampNo)
{
	this->lampNo = SCOPY_TEXT(lampNo);
}

const UTF8Char* Lamppost::GetLampType()
{
	return this->lampType;
}

void Lamppost::SetLampType(const UTF8Char* lampType)
{
	this->lampType = SCOPY_TEXT(lampType);
}

const UTF8Char* Lamppost::GetLandmark()
{
	return this->landmark;
}

void Lamppost::SetLandmark(const UTF8Char* landmark)
{
	this->landmark = SCOPY_TEXT(landmark);
}

const UTF8Char* Lamppost::GetLtgRegion()
{
	return this->ltgRegion;
}

void Lamppost::SetLtgRegion(const UTF8Char* ltgRegion)
{
	this->ltgRegion = SCOPY_TEXT(ltgRegion);
}

const UTF8Char* Lamppost::GetMounting()
{
	return this->mounting;
}

void Lamppost::SetMounting(const UTF8Char* mounting)
{
	this->mounting = SCOPY_TEXT(mounting);
}

const UTF8Char* Lamppost::GetNOPoint()
{
	return this->nOPoint;
}

void Lamppost::SetNOPoint(const UTF8Char* nOPoint)
{
	this->nOPoint = SCOPY_TEXT(nOPoint);
}

const UTF8Char* Lamppost::GetNemaSoc()
{
	return this->nemaSoc;
}

void Lamppost::SetNemaSoc(const UTF8Char* nemaSoc)
{
	this->nemaSoc = SCOPY_TEXT(nemaSoc);
}

Int32 Lamppost::GetObjectid()
{
	return this->objectid;
}

void Lamppost::SetObjectid(Int32 objectid)
{
	this->objectid = objectid;
}

Int32 Lamppost::GetObjectidP()
{
	return this->objectidP;
}

void Lamppost::SetObjectidP(Int32 objectidP)
{
	this->objectidP = objectidP;
}

const UTF8Char* Lamppost::GetPUserFid()
{
	return this->pUserFid;
}

void Lamppost::SetPUserFid(const UTF8Char* pUserFid)
{
	this->pUserFid = SCOPY_TEXT(pUserFid);
}

const UTF8Char* Lamppost::GetPaintMate()
{
	return this->paintMate;
}

void Lamppost::SetPaintMate(const UTF8Char* paintMate)
{
	this->paintMate = SCOPY_TEXT(paintMate);
}

Single Lamppost::GetPlDepth()
{
	return this->plDepth;
}

void Lamppost::SetPlDepth(Single plDepth)
{
	this->plDepth = plDepth;
}

const UTF8Char* Lamppost::GetPoleManu()
{
	return this->poleManu;
}

void Lamppost::SetPoleManu(const UTF8Char* poleManu)
{
	this->poleManu = SCOPY_TEXT(poleManu);
}

const UTF8Char* Lamppost::GetPoleType()
{
	return this->poleType;
}

void Lamppost::SetPoleType(const UTF8Char* poleType)
{
	this->poleType = SCOPY_TEXT(poleType);
}

const UTF8Char* Lamppost::GetPolecolor()
{
	return this->polecolor;
}

void Lamppost::SetPolecolor(const UTF8Char* polecolor)
{
	this->polecolor = SCOPY_TEXT(polecolor);
}

const UTF8Char* Lamppost::GetPrimarySt()
{
	return this->primarySt;
}

void Lamppost::SetPrimarySt(const UTF8Char* primarySt)
{
	this->primarySt = SCOPY_TEXT(primarySt);
}

Data::DateTime* Lamppost::GetRegDate()
{
	return this->regDate;
}

void Lamppost::SetRegDate(Data::DateTime* regDate)
{
	this->regDate = regDate?(NEW_CLASS_D(Data::DateTime(regDate))):0;
}

const UTF8Char* Lamppost::GetRemark1()
{
	return this->remark1;
}

void Lamppost::SetRemark1(const UTF8Char* remark1)
{
	this->remark1 = SCOPY_TEXT(remark1);
}

const UTF8Char* Lamppost::GetRemark2()
{
	return this->remark2;
}

void Lamppost::SetRemark2(const UTF8Char* remark2)
{
	this->remark2 = SCOPY_TEXT(remark2);
}

const UTF8Char* Lamppost::GetRemark3()
{
	return this->remark3;
}

void Lamppost::SetRemark3(const UTF8Char* remark3)
{
	this->remark3 = SCOPY_TEXT(remark3);
}

Int16 Lamppost::GetRotation()
{
	return this->rotation;
}

void Lamppost::SetRotation(Int16 rotation)
{
	this->rotation = rotation;
}

const UTF8Char* Lamppost::GetRouteno()
{
	return this->routeno;
}

void Lamppost::SetRouteno(const UTF8Char* routeno)
{
	this->routeno = SCOPY_TEXT(routeno);
}

const UTF8Char* Lamppost::GetSecondSt()
{
	return this->secondSt;
}

void Lamppost::SetSecondSt(const UTF8Char* secondSt)
{
	this->secondSt = SCOPY_TEXT(secondSt);
}

Math::Vector2D* Lamppost::GetShape()
{
	return this->shape;
}

void Lamppost::SetShape(Math::Vector2D* shape)
{
	this->shape = shape?shape->Clone():0;
}

const UTF8Char* Lamppost::GetShielded()
{
	return this->shielded;
}

void Lamppost::SetShielded(const UTF8Char* shielded)
{
	this->shielded = SCOPY_TEXT(shielded);
}

const UTF8Char* Lamppost::GetSolarLtg()
{
	return this->solarLtg;
}

void Lamppost::SetSolarLtg(const UTF8Char* solarLtg)
{
	this->solarLtg = SCOPY_TEXT(solarLtg);
}

const UTF8Char* Lamppost::GetSpecMate()
{
	return this->specMate;
}

void Lamppost::SetSpecMate(const UTF8Char* specMate)
{
	this->specMate = SCOPY_TEXT(specMate);
}

Data::DateTime* Lamppost::GetSubDate()
{
	return this->subDate;
}

void Lamppost::SetSubDate(Data::DateTime* subDate)
{
	this->subDate = subDate?(NEW_CLASS_D(Data::DateTime(subDate))):0;
}

const UTF8Char* Lamppost::GetSubtype()
{
	return this->subtype;
}

void Lamppost::SetSubtype(const UTF8Char* subtype)
{
	this->subtype = SCOPY_TEXT(subtype);
}

const UTF8Char* Lamppost::GetUpdbyhyd()
{
	return this->updbyhyd;
}

void Lamppost::SetUpdbyhyd(const UTF8Char* updbyhyd)
{
	this->updbyhyd = SCOPY_TEXT(updbyhyd);
}

const UTF8Char* Lamppost::GetUpdbymom()
{
	return this->updbymom;
}

void Lamppost::SetUpdbymom(const UTF8Char* updbymom)
{
	this->updbymom = SCOPY_TEXT(updbymom);
}

Data::DateTime* Lamppost::GetUpddatehyd()
{
	return this->upddatehyd;
}

void Lamppost::SetUpddatehyd(Data::DateTime* upddatehyd)
{
	this->upddatehyd = upddatehyd?(NEW_CLASS_D(Data::DateTime(upddatehyd))):0;
}

Data::DateTime* Lamppost::GetUpddatemom()
{
	return this->upddatemom;
}

void Lamppost::SetUpddatemom(Data::DateTime* upddatemom)
{
	this->upddatemom = upddatemom?(NEW_CLASS_D(Data::DateTime(upddatemom))):0;
}

const UTF8Char* Lamppost::GetUtilNo()
{
	return this->utilNo;
}

void Lamppost::SetUtilNo(const UTF8Char* utilNo)
{
	this->utilNo = SCOPY_TEXT(utilNo);
}

const UTF8Char* Lamppost::GetVillage()
{
	return this->village;
}

void Lamppost::SetVillage(const UTF8Char* village)
{
	this->village = SCOPY_TEXT(village);
}

Single Lamppost::GetZCoor()
{
	return this->zCoor;
}

void Lamppost::SetZCoor(Single zCoor)
{
	this->zCoor = zCoor;
}

Data::NamedClass<Lamppost> *Lamppost::CreateClass()
{
	Data::NamedClass<Lamppost> *cls;
	NEW_CLASS(cls, Data::NamedClass<Lamppost>(this));
	CLASS_ADD(cls, accLocati);
	CLASS_ADD(cls, antiBurgl);
	CLASS_ADD(cls, bulbQty);
	CLASS_ADD(cls, cmsDate);
	CLASS_ADD(cls, cmsType);
	CLASS_ADD(cls, colClass);
	CLASS_ADD(cls, columnHt);
	CLASS_ADD(cls, concD);
	CLASS_ADD(cls, concL);
	CLASS_ADD(cls, concW);
	CLASS_ADD(cls, contrNo);
	CLASS_ADD(cls, czone);
	CLASS_ADD(cls, deleted);
	CLASS_ADD(cls, disconDate);
	CLASS_ADD(cls, district);
	CLASS_ADD(cls, fixOPole);
	CLASS_ADD(cls, iVersion);
	CLASS_ADD(cls, installDate);
	CLASS_ADD(cls, kerbOs);
	CLASS_ADD(cls, lampNo);
	CLASS_ADD(cls, lampType);
	CLASS_ADD(cls, landmark);
	CLASS_ADD(cls, ltgRegion);
	CLASS_ADD(cls, mounting);
	CLASS_ADD(cls, nOPoint);
	CLASS_ADD(cls, nemaSoc);
	CLASS_ADD(cls, objectid);
	CLASS_ADD(cls, objectidP);
	CLASS_ADD(cls, pUserFid);
	CLASS_ADD(cls, paintMate);
	CLASS_ADD(cls, plDepth);
	CLASS_ADD(cls, poleManu);
	CLASS_ADD(cls, poleType);
	CLASS_ADD(cls, polecolor);
	CLASS_ADD(cls, primarySt);
	CLASS_ADD(cls, regDate);
	CLASS_ADD(cls, remark1);
	CLASS_ADD(cls, remark2);
	CLASS_ADD(cls, remark3);
	CLASS_ADD(cls, rotation);
	CLASS_ADD(cls, routeno);
	CLASS_ADD(cls, secondSt);
	CLASS_ADD(cls, shape);
	CLASS_ADD(cls, shielded);
	CLASS_ADD(cls, solarLtg);
	CLASS_ADD(cls, specMate);
	CLASS_ADD(cls, subDate);
	CLASS_ADD(cls, subtype);
	CLASS_ADD(cls, updbyhyd);
	CLASS_ADD(cls, updbymom);
	CLASS_ADD(cls, upddatehyd);
	CLASS_ADD(cls, upddatemom);
	CLASS_ADD(cls, utilNo);
	CLASS_ADD(cls, village);
	CLASS_ADD(cls, zCoor);
	return cls;
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	UTF8Char sbuff[512];
	IO::ConsoleWriter console;
	Manage::HiResClock clk;
	IO::DirectoryPackage *dir;
	NEW_CLASS(dir, IO::DirectoryPackage((const UTF8Char*)"~/Progs/Temp/E20210522_PLIS.gdb"));
	Map::ESRI::FileGDBDir *fileGDB;
	fileGDB = Map::ESRI::FileGDBDir::OpenDir(dir);
	DEL_CLASS(dir);
	if (fileGDB)
	{
		Text::StringBuilderUTF8 sb;
		Data::ArrayList<const UTF8Char*> colNames;
		colNames.Add((const UTF8Char*)"OBJECTID");
		colNames.Add((const UTF8Char*)"Shape");
		Data::QueryConditions cond;
		cond.Int32Equals((const UTF8Char*)"OBJECTID", 40);
		DB::DBReader *r = fileGDB->GetTableData((const UTF8Char*)"LAMPPOST", &colNames, 0, 10, (const UTF8Char*)"OBJECTID desc", 0);//&cond);
		if (r)
		{
			while (r->ReadNext())
			{
				Data::VariObject *obj = r->CreateVariObject();
				sb.ClearStr();
				obj->ToString(&sb);
				console.WriteLine(sb.ToString());
				DEL_CLASS(obj);
			}
			fileGDB->CloseReader(r);
		}

		Data::NamedClass<Lamppost> *cls = Lamppost().CreateClass();
		Lamppost *lamppost;
		Lamppost *lamppost2;
		UOSInt i;
		UOSInt j;

		r = fileGDB->GetTableData((const UTF8Char*)"LAMPPOST", 0, 0, 0, 0, 0);
		if (r)
		{
			Double t1;
			Double t2;
			Double t3;
			Double t4 = 0;
			Data::ArrayList<Lamppost*> lamppostList;
			Data::ArrayList<Lamppost*> lamppostListCSV;
			clk.Start();
			r->ReadAll(&lamppostList, cls);
			t1 = clk.GetTimeDiff();
			fileGDB->CloseReader(r);

			IO::Path::GetRealPath(sbuff, (const UTF8Char*)"~/Progs/Temp/Lamppost.csv");
			IO::FileStream *fs;
			NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
			clk.Start();
			DB::DBUtil::SaveCSV(fs, &lamppostList, cls);
			t2 = clk.GetTimeDiff();
			DEL_CLASS(fs);
			

			DB::CSVFile *csv;
			NEW_CLASS(csv, DB::CSVFile(sbuff, 65001));
			csv->SetNullIfEmpty(true);
			r = csv->GetTableData((const UTF8Char*)"Lamppost", 0, 0, 0, 0, 0);
			clk.Start();
			r->ReadAll(&lamppostListCSV, cls);
			t3 = clk.GetTimeDiff();
			csv->CloseReader(r);
			DEL_CLASS(csv);
			
			sb.ClearStr();
			sb.Append((const UTF8Char*)"FileGDB count = ");
			sb.AppendUOSInt(lamppostList.GetCount());
			sb.Append((const UTF8Char*)", CSV count = ");
			sb.AppendUOSInt(lamppostListCSV.GetCount());
			console.WriteLine(sb.ToString());
			
			if (lamppostList.GetCount() == lamppostListCSV.GetCount())
			{
				clk.Start();	
				i = 0;
				j = lamppostList.GetCount();
				while (i < j)
				{
					lamppost = lamppostList.GetItem(i);
					lamppost2 = lamppostListCSV.GetItem(i);
					if (!cls->Equals(lamppost, lamppost2))
					{
						console.WriteLine((const UTF8Char*)"Not equals:");
						console.WriteLine((const UTF8Char*)"FileGDB:");
						sb.ClearStr();
						Text::StringTool::BuildString(&sb, lamppost, cls);
						console.WriteLine(sb.ToString());
						console.WriteLine((const UTF8Char*)"CSV:");
						sb.ClearStr();
						Text::StringTool::BuildString(&sb, lamppost2, cls);
						console.WriteLine(sb.ToString());
						break;
					}
					i++;
				}
				t4 = clk.GetTimeDiff();
			}
/*			sb.ClearStr();
			Text::StringTool::BuildString(&sb, &lamppostList, cls, (const UTF8Char*)"Lamppost");
			console.WriteLine(sb.ToString());*/

			i = lamppostList.GetCount();
			while (i-- > 0)
			{
				lamppost = lamppostList.GetItem(i);
				DEL_CLASS(lamppost);
			}

			i = lamppostListCSV.GetCount();
			while (i-- > 0)
			{
				lamppost = lamppostListCSV.GetItem(i);
				DEL_CLASS(lamppost);
			}

			sb.ClearStr();
			sb.Append((const UTF8Char*)"t1 = ");
			Text::SBAppendF64(&sb, t1);
			sb.Append((const UTF8Char*)", t2 = ");
			Text::SBAppendF64(&sb, t2);
			sb.Append((const UTF8Char*)", t3 = ");
			Text::SBAppendF64(&sb, t3);
			sb.Append((const UTF8Char*)", t4 = ");
			Text::SBAppendF64(&sb, t4);
			console.WriteLine(sb.ToString());
		}
		DEL_CLASS(cls);

		DEL_CLASS(fileGDB);
	}
	return 0;
}
