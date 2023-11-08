#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/NamedClass.h"
#include "DB/CSVFile.h"
#include "DB/DBClassReader.h"
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
	Text::String* accLocati;
	Text::String* antiBurgl;
	Int16 bulbQty;
	Data::Timestamp cmsDate;
	Text::String* cmsType;
	Int16 colClass;
	Single columnHt;
	Single concD;
	Single concL;
	Single concW;
	Text::String* contrNo;
	Text::String* czone;
	Text::String* deleted;
	Data::Timestamp disconDate;
	Text::String* district;
	Text::String* fixOPole;
	Int16 iVersion;
	Data::Timestamp installDate;
	Single kerbOs;
	Text::String* lampNo;
	Text::String* lampType;
	Text::String* landmark;
	Text::String* ltgRegion;
	Text::String* mounting;
	Text::String* nOPoint;
	Text::String* nemaSoc;
	Int32 objectid;
	Int32 objectidP;
	Text::String* pUserFid;
	Text::String* paintMate;
	Single plDepth;
	Text::String* poleManu;
	Text::String* poleType;
	Text::String* polecolor;
	Text::String* primarySt;
	Data::Timestamp regDate;
	Text::String* remark1;
	Text::String* remark2;
	Text::String* remark3;
	Int16 rotation;
	Text::String* routeno;
	Text::String* secondSt;
	Math::Geometry::Vector2D* shape;
	Text::String* shielded;
	Text::String* solarLtg;
	Text::String* specMate;
	Data::Timestamp subDate;
	Text::String* subtype;
	Text::String* updbyhyd;
	Text::String* updbymom;
	Data::Timestamp upddatehyd;
	Data::Timestamp upddatemom;
	Text::String* utilNo;
	Text::String* village;
	Single zCoor;

public:
	Lamppost();
	~Lamppost();

	Text::String* GetAccLocati();
	void SetAccLocati(Text::String* accLocati);
	Text::String* GetAntiBurgl();
	void SetAntiBurgl(Text::String* antiBurgl);
	Int16 GetBulbQty();
	void SetBulbQty(Int16 bulbQty);
	Data::Timestamp GetCmsDate();
	void SetCmsDate(const Data::Timestamp &cmsDate);
	Text::String* GetCmsType();
	void SetCmsType(Text::String* cmsType);
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
	Text::String* GetContrNo();
	void SetContrNo(Text::String* contrNo);
	Text::String* GetCzone();
	void SetCzone(Text::String* czone);
	Text::String* GetDeleted();
	void SetDeleted(Text::String* deleted);
	Data::Timestamp GetDisconDate();
	void SetDisconDate(const Data::Timestamp &disconDate);
	Text::String* GetDistrict();
	void SetDistrict(Text::String* district);
	Text::String* GetFixOPole();
	void SetFixOPole(Text::String* fixOPole);
	Int16 GetIVersion();
	void SetIVersion(Int16 iVersion);
	Data::Timestamp GetInstallDate();
	void SetInstallDate(const Data::Timestamp &installDate);
	Single GetKerbOs();
	void SetKerbOs(Single kerbOs);
	Text::String* GetLampNo();
	void SetLampNo(Text::String* lampNo);
	Text::String* GetLampType();
	void SetLampType(Text::String* lampType);
	Text::String* GetLandmark();
	void SetLandmark(Text::String* landmark);
	Text::String* GetLtgRegion();
	void SetLtgRegion(Text::String* ltgRegion);
	Text::String* GetMounting();
	void SetMounting(Text::String* mounting);
	Text::String* GetNOPoint();
	void SetNOPoint(Text::String* nOPoint);
	Text::String* GetNemaSoc();
	void SetNemaSoc(Text::String* nemaSoc);
	Int32 GetObjectid();
	void SetObjectid(Int32 objectid);
	Int32 GetObjectidP();
	void SetObjectidP(Int32 objectidP);
	Text::String* GetPUserFid();
	void SetPUserFid(Text::String* pUserFid);
	Text::String* GetPaintMate();
	void SetPaintMate(Text::String* paintMate);
	Single GetPlDepth();
	void SetPlDepth(Single plDepth);
	Text::String* GetPoleManu();
	void SetPoleManu(Text::String* poleManu);
	Text::String* GetPoleType();
	void SetPoleType(Text::String* poleType);
	Text::String* GetPolecolor();
	void SetPolecolor(Text::String* polecolor);
	Text::String* GetPrimarySt();
	void SetPrimarySt(Text::String* primarySt);
	Data::Timestamp GetRegDate();
	void SetRegDate(const Data::Timestamp &regDate);
	Text::String* GetRemark1();
	void SetRemark1(Text::String* remark1);
	Text::String* GetRemark2();
	void SetRemark2(Text::String* remark2);
	Text::String* GetRemark3();
	void SetRemark3(Text::String* remark3);
	Int16 GetRotation();
	void SetRotation(Int16 rotation);
	Text::String* GetRouteno();
	void SetRouteno(Text::String* routeno);
	Text::String* GetSecondSt();
	void SetSecondSt(Text::String* secondSt);
	Math::Geometry::Vector2D* GetShape();
	void SetShape(Math::Geometry::Vector2D* shape);
	Text::String* GetShielded();
	void SetShielded(Text::String* shielded);
	Text::String* GetSolarLtg();
	void SetSolarLtg(Text::String* solarLtg);
	Text::String* GetSpecMate();
	void SetSpecMate(Text::String* specMate);
	Data::Timestamp GetSubDate();
	void SetSubDate(const Data::Timestamp &subDate);
	Text::String* GetSubtype();
	void SetSubtype(Text::String* subtype);
	Text::String* GetUpdbyhyd();
	void SetUpdbyhyd(Text::String* updbyhyd);
	Text::String* GetUpdbymom();
	void SetUpdbymom(Text::String* updbymom);
	Data::Timestamp GetUpddatehyd();
	void SetUpddatehyd(const Data::Timestamp &upddatehyd);
	Data::Timestamp GetUpddatemom();
	void SetUpddatemom(const Data::Timestamp &upddatemom);
	Text::String* GetUtilNo();
	void SetUtilNo(Text::String* utilNo);
	Text::String* GetVillage();
	void SetVillage(Text::String* village);
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
	SDEL_STRING(this->accLocati);
	SDEL_STRING(this->antiBurgl);
	SDEL_STRING(this->cmsType);
	SDEL_STRING(this->contrNo);
	SDEL_STRING(this->czone);
	SDEL_STRING(this->deleted);
	SDEL_STRING(this->district);
	SDEL_STRING(this->fixOPole);
	SDEL_STRING(this->lampNo);
	SDEL_STRING(this->lampType);
	SDEL_STRING(this->landmark);
	SDEL_STRING(this->ltgRegion);
	SDEL_STRING(this->mounting);
	SDEL_STRING(this->nOPoint);
	SDEL_STRING(this->nemaSoc);
	SDEL_STRING(this->pUserFid);
	SDEL_STRING(this->paintMate);
	SDEL_STRING(this->poleManu);
	SDEL_STRING(this->poleType);
	SDEL_STRING(this->polecolor);
	SDEL_STRING(this->primarySt);
	SDEL_STRING(this->remark1);
	SDEL_STRING(this->remark2);
	SDEL_STRING(this->remark3);
	SDEL_STRING(this->routeno);
	SDEL_STRING(this->secondSt);
	SDEL_CLASS(this->shape);
	SDEL_STRING(this->shielded);
	SDEL_STRING(this->solarLtg);
	SDEL_STRING(this->specMate);
	SDEL_STRING(this->subtype);
	SDEL_STRING(this->updbyhyd);
	SDEL_STRING(this->updbymom);
	SDEL_STRING(this->utilNo);
	SDEL_STRING(this->village);
}

Text::String* Lamppost::GetAccLocati()
{
	return this->accLocati;
}

void Lamppost::SetAccLocati(Text::String* accLocati)
{
	SDEL_STRING(this->accLocati);
	this->accLocati = accLocati?accLocati->Clone().Ptr():0;
}

Text::String* Lamppost::GetAntiBurgl()
{
	return this->antiBurgl;
}

void Lamppost::SetAntiBurgl(Text::String* antiBurgl)
{
	SDEL_STRING(this->antiBurgl);
	this->antiBurgl = antiBurgl?antiBurgl->Clone().Ptr():0;
}

Int16 Lamppost::GetBulbQty()
{
	return this->bulbQty;
}

void Lamppost::SetBulbQty(Int16 bulbQty)
{
	this->bulbQty = bulbQty;
}

Data::Timestamp Lamppost::GetCmsDate()
{
	return this->cmsDate;
}

void Lamppost::SetCmsDate(const Data::Timestamp &cmsDate)
{
	this->cmsDate = cmsDate;
}

Text::String* Lamppost::GetCmsType()
{
	return this->cmsType;
}

void Lamppost::SetCmsType(Text::String* cmsType)
{
	SDEL_STRING(this->cmsType);
	this->cmsType = cmsType?cmsType->Clone().Ptr():0;
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

Text::String* Lamppost::GetContrNo()
{
	return this->contrNo;
}

void Lamppost::SetContrNo(Text::String* contrNo)
{
	SDEL_STRING(this->contrNo);
	this->contrNo = contrNo?contrNo->Clone().Ptr():0;
}

Text::String* Lamppost::GetCzone()
{
	return this->czone;
}

void Lamppost::SetCzone(Text::String* czone)
{
	SDEL_STRING(this->czone);
	this->czone = czone?czone->Clone().Ptr():0;
}

Text::String* Lamppost::GetDeleted()
{
	return this->deleted;
}

void Lamppost::SetDeleted(Text::String* deleted)
{
	SDEL_STRING(this->deleted);
	this->deleted = deleted?deleted->Clone().Ptr():0;
}

Data::Timestamp Lamppost::GetDisconDate()
{
	return this->disconDate;
}

void Lamppost::SetDisconDate(const Data::Timestamp &disconDate)
{
	this->disconDate = disconDate;
}

Text::String* Lamppost::GetDistrict()
{
	return this->district;
}

void Lamppost::SetDistrict(Text::String* district)
{
	SDEL_STRING(this->district);
	this->district = district?district->Clone().Ptr():0;
}

Text::String* Lamppost::GetFixOPole()
{
	return this->fixOPole;
}

void Lamppost::SetFixOPole(Text::String* fixOPole)
{
	SDEL_STRING(this->fixOPole);
	this->fixOPole = fixOPole?fixOPole->Clone().Ptr():0;
}

Int16 Lamppost::GetIVersion()
{
	return this->iVersion;
}

void Lamppost::SetIVersion(Int16 iVersion)
{
	this->iVersion = iVersion;
}

Data::Timestamp Lamppost::GetInstallDate()
{
	return this->installDate;
}

void Lamppost::SetInstallDate(const Data::Timestamp &installDate)
{
	this->installDate = installDate;
}

Single Lamppost::GetKerbOs()
{
	return this->kerbOs;
}

void Lamppost::SetKerbOs(Single kerbOs)
{
	this->kerbOs = kerbOs;
}

Text::String* Lamppost::GetLampNo()
{
	return this->lampNo;
}

void Lamppost::SetLampNo(Text::String* lampNo)
{
	SDEL_STRING(this->lampNo);
	this->lampNo = lampNo?lampNo->Clone().Ptr():0;
}

Text::String* Lamppost::GetLampType()
{
	return this->lampType;
}

void Lamppost::SetLampType(Text::String* lampType)
{
	SDEL_STRING(this->lampType);
	this->lampType = lampType?lampType->Clone().Ptr():0;
}

Text::String* Lamppost::GetLandmark()
{
	return this->landmark;
}

void Lamppost::SetLandmark(Text::String* landmark)
{
	SDEL_STRING(this->landmark);
	this->landmark = landmark?landmark->Clone().Ptr():0;
}

Text::String* Lamppost::GetLtgRegion()
{
	return this->ltgRegion;
}

void Lamppost::SetLtgRegion(Text::String* ltgRegion)
{
	SDEL_STRING(this->ltgRegion);
	this->ltgRegion = ltgRegion?ltgRegion->Clone().Ptr():0;
}

Text::String* Lamppost::GetMounting()
{
	return this->mounting;
}

void Lamppost::SetMounting(Text::String* mounting)
{
	SDEL_STRING(this->mounting);
	this->mounting = mounting?mounting->Clone().Ptr():0;
}

Text::String* Lamppost::GetNOPoint()
{
	return this->nOPoint;
}

void Lamppost::SetNOPoint(Text::String* nOPoint)
{
	SDEL_STRING(this->nOPoint);
	this->nOPoint = nOPoint?nOPoint->Clone().Ptr():0;
}

Text::String* Lamppost::GetNemaSoc()
{
	return this->nemaSoc;
}

void Lamppost::SetNemaSoc(Text::String* nemaSoc)
{
	SDEL_STRING(this->nemaSoc);
	this->nemaSoc = nemaSoc?nemaSoc->Clone().Ptr():0;
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

Text::String* Lamppost::GetPUserFid()
{
	return this->pUserFid;
}

void Lamppost::SetPUserFid(Text::String* pUserFid)
{
	SDEL_STRING(this->pUserFid);
	this->pUserFid = pUserFid?pUserFid->Clone().Ptr():0;
}

Text::String* Lamppost::GetPaintMate()
{
	return this->paintMate;
}

void Lamppost::SetPaintMate(Text::String* paintMate)
{
	SDEL_STRING(this->paintMate);
	this->paintMate = paintMate?paintMate->Clone().Ptr():0;
}

Single Lamppost::GetPlDepth()
{
	return this->plDepth;
}

void Lamppost::SetPlDepth(Single plDepth)
{
	this->plDepth = plDepth;
}

Text::String* Lamppost::GetPoleManu()
{
	return this->poleManu;
}

void Lamppost::SetPoleManu(Text::String* poleManu)
{
	SDEL_STRING(this->poleManu);
	this->poleManu = poleManu?poleManu->Clone().Ptr():0;
}

Text::String* Lamppost::GetPoleType()
{
	return this->poleType;
}

void Lamppost::SetPoleType(Text::String* poleType)
{
	SDEL_STRING(this->poleType);
	this->poleType = poleType?poleType->Clone().Ptr():0;
}

Text::String* Lamppost::GetPolecolor()
{
	return this->polecolor;
}

void Lamppost::SetPolecolor(Text::String* polecolor)
{
	SDEL_STRING(this->polecolor);
	this->polecolor = polecolor?polecolor->Clone().Ptr():0;
}

Text::String* Lamppost::GetPrimarySt()
{
	return this->primarySt;
}

void Lamppost::SetPrimarySt(Text::String* primarySt)
{
	SDEL_STRING(this->primarySt);
	this->primarySt = primarySt?primarySt->Clone().Ptr():0;
}

Data::Timestamp Lamppost::GetRegDate()
{
	return this->regDate;
}

void Lamppost::SetRegDate(const Data::Timestamp &regDate)
{
	this->regDate = regDate;
}

Text::String* Lamppost::GetRemark1()
{
	return this->remark1;
}

void Lamppost::SetRemark1(Text::String* remark1)
{
	SDEL_STRING(this->remark1);
	this->remark1 = remark1?remark1->Clone().Ptr():0;
}

Text::String* Lamppost::GetRemark2()
{
	return this->remark2;
}

void Lamppost::SetRemark2(Text::String* remark2)
{
	SDEL_STRING(this->remark2);
	this->remark2 = remark2?remark2->Clone().Ptr():0;
}

Text::String* Lamppost::GetRemark3()
{
	return this->remark3;
}

void Lamppost::SetRemark3(Text::String* remark3)
{
	SDEL_STRING(this->remark3);
	this->remark3 = remark3?remark3->Clone().Ptr():0;
}

Int16 Lamppost::GetRotation()
{
	return this->rotation;
}

void Lamppost::SetRotation(Int16 rotation)
{
	this->rotation = rotation;
}

Text::String* Lamppost::GetRouteno()
{
	return this->routeno;
}

void Lamppost::SetRouteno(Text::String* routeno)
{
	SDEL_STRING(this->routeno);
	this->routeno = routeno?routeno->Clone().Ptr():0;
}

Text::String* Lamppost::GetSecondSt()
{
	return this->secondSt;
}

void Lamppost::SetSecondSt(Text::String* secondSt)
{
	SDEL_STRING(this->secondSt);
	this->secondSt = secondSt?secondSt->Clone().Ptr():0;
}

Math::Geometry::Vector2D* Lamppost::GetShape()
{
	return this->shape;
}

void Lamppost::SetShape(Math::Geometry::Vector2D* shape)
{
	SDEL_CLASS(this->shape);
	this->shape = shape?shape->Clone().Ptr():0;
}

Text::String* Lamppost::GetShielded()
{
	return this->shielded;
}

void Lamppost::SetShielded(Text::String* shielded)
{
	SDEL_STRING(this->shielded);
	this->shielded = shielded?shielded->Clone().Ptr():0;
}

Text::String* Lamppost::GetSolarLtg()
{
	return this->solarLtg;
}

void Lamppost::SetSolarLtg(Text::String* solarLtg)
{
	SDEL_STRING(this->solarLtg);
	this->solarLtg = solarLtg?solarLtg->Clone().Ptr():0;
}

Text::String* Lamppost::GetSpecMate()
{
	return this->specMate;
}

void Lamppost::SetSpecMate(Text::String* specMate)
{
	SDEL_STRING(this->specMate);
	this->specMate = specMate?specMate->Clone().Ptr():0;
}

Data::Timestamp Lamppost::GetSubDate()
{
	return this->subDate;
}

void Lamppost::SetSubDate(const Data::Timestamp &subDate)
{
	this->subDate = subDate;
}

Text::String* Lamppost::GetSubtype()
{
	return this->subtype;
}

void Lamppost::SetSubtype(Text::String* subtype)
{
	SDEL_STRING(this->subtype);
	this->subtype = subtype?subtype->Clone().Ptr():0;
}

Text::String* Lamppost::GetUpdbyhyd()
{
	return this->updbyhyd;
}

void Lamppost::SetUpdbyhyd(Text::String* updbyhyd)
{
	SDEL_STRING(this->updbyhyd);
	this->updbyhyd = updbyhyd?updbyhyd->Clone().Ptr():0;
}

Text::String* Lamppost::GetUpdbymom()
{
	return this->updbymom;
}

void Lamppost::SetUpdbymom(Text::String* updbymom)
{
	SDEL_STRING(this->updbymom);
	this->updbymom = updbymom?updbymom->Clone().Ptr():0;
}

Data::Timestamp Lamppost::GetUpddatehyd()
{
	return this->upddatehyd;
}

void Lamppost::SetUpddatehyd(const Data::Timestamp &upddatehyd)
{
	this->upddatehyd = upddatehyd;
}

Data::Timestamp Lamppost::GetUpddatemom()
{
	return this->upddatemom;
}

void Lamppost::SetUpddatemom(const Data::Timestamp &upddatemom)
{
	this->upddatemom = upddatemom;
}

Text::String* Lamppost::GetUtilNo()
{
	return this->utilNo;
}

void Lamppost::SetUtilNo(Text::String* utilNo)
{
	SDEL_STRING(this->utilNo);
	this->utilNo = utilNo?utilNo->Clone().Ptr():0;
}

Text::String* Lamppost::GetVillage()
{
	return this->village;
}

void Lamppost::SetVillage(Text::String* village)
{
	SDEL_STRING(this->village);
	this->village = village?village->Clone().Ptr():0;
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

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	IO::ConsoleWriter console;
	Manage::HiResClock clk;
	NotNullPtr<IO::DirectoryPackage> dir;
	NEW_CLASSNN(dir, IO::DirectoryPackage(CSTR("~/Progs/Temp/E20210522_PLIS.gdb")));
	Map::ESRI::FileGDBDir *fileGDB;
	Math::ArcGISPRJParser prjParser;
	fileGDB = Map::ESRI::FileGDBDir::OpenDir(dir, prjParser);
	dir.Delete();
	if (fileGDB)
	{
		Text::StringBuilderUTF8 sb;
		Data::ArrayList<const UTF8Char*> colNames;
		colNames.Add((const UTF8Char*)"OBJECTID");
		colNames.Add((const UTF8Char*)"Shape");
		Data::QueryConditions cond;
		cond.Int32Equals(CSTR("OBJECTID"), 40);
		NotNullPtr<DB::DBReader> r;
/*		if (r.Set(fileGDB->QueryTableData((const UTF8Char*)"LAMPPOST", &colNames, 0, 10, (const UTF8Char*)"OBJECTID desc", 0)))//&cond);
		{
			while (r->ReadNext())
			{
				Data::VariObject *obj = r->CreateVariObject();
				sb.ClearStr();
				obj->ToString(sb);
				console.WriteLineC(sb.ToString(), sb.GetLength());
				DEL_CLASS(obj);
			}
			fileGDB->CloseReader(r);
		}*/

		Data::NamedClass<Lamppost> *cls = Lamppost().CreateClass();

		if (r.Set(fileGDB->QueryTableData(CSTR_NULL, CSTR("LAMPPOST"), 0, 0, 0, CSTR_NULL, 0)))
		{
			Lamppost *lamppost;
			UOSInt i;
			Double t1;
			Double t2;
			Double t3;
//			Double t4 = 0;
			Data::ArrayList<Lamppost*> lamppostList;
			clk.Start();
			{
				DB::DBClassReader<Lamppost> reader(r, cls);
				reader.ReadAll(&lamppostList);
			}
			t1 = clk.GetTimeDiff();
			fileGDB->CloseReader(r);

			sptr = IO::Path::GetRealPath(sbuff, UTF8STRC("~/Progs/Temp/Lamppost.csv"));
			{
				IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				clk.Start();
				DB::DBUtil::SaveCSV(&fs, &lamppostList, cls);
				t2 = clk.GetTimeDiff();
			}
			

			Data::ArrayList<Lamppost*> lamppostListCSV;
			DB::CSVFile *csv;
			NEW_CLASS(csv, DB::CSVFile(CSTRP(sbuff, sptr), 65001));
			csv->SetNullIfEmpty(true);
			if (r.Set(csv->QueryTableData(CSTR_NULL, CSTR("Lamppost"), 0, 0, 0, CSTR_NULL, 0)))
			{
				clk.Start();
				{
					DB::DBClassReader<Lamppost> reader(r, cls);
					reader.ReadAll(&lamppostListCSV);
				}
				t3 = clk.GetTimeDiff();
				csv->CloseReader(r);
			}
			else
			{
				t3 = -1;
			}
			DEL_CLASS(csv);
			
			sb.ClearStr();
			sb.AppendC(UTF8STRC("FileGDB count = "));
			sb.AppendUOSInt(lamppostList.GetCount());
			sb.AppendC(UTF8STRC(", CSV count = "));
			sb.AppendUOSInt(lamppostListCSV.GetCount());
			console.WriteLineC(sb.ToString(), sb.GetLength());
			
/*			if (lamppostList.GetCount() == lamppostListCSV.GetCount())
			{
				Lamppost *lamppost2;
				UOSInt j;
				clk.Start();	
				i = 0;
				j = lamppostList.GetCount();
				while (i < j)
				{
					lamppost = lamppostList.GetItem(i);
					lamppost2 = lamppostListCSV.GetItem(i);
					if (!cls->Equals(lamppost, lamppost2))
					{
						console.WriteLineC(UTF8STRC("Not equals:");
						console.WriteLineC(UTF8STRC("FileGDB:");
						sb.ClearStr();
						Text::StringTool::BuildString(sb, lamppost, cls);
						console.WriteLineC(sb.ToString(), sb.GetLength());
						console.WriteLineC(UTF8STRC("CSV:");
						sb.ClearStr();
						Text::StringTool::BuildString(sb, lamppost2, cls);
						console.WriteLineC(sb.ToString(), sb.GetLength());
						break;
					}
					i++;
				}
				t4 = clk.GetTimeDiff();
			}*/
/*			sb.ClearStr();
			Text::StringTool::BuildString(sb, &lamppostList, cls, (const UTF8Char*)"Lamppost");
			console.WriteLineC(sb.ToString(), sb.GetLength());*/

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
			sb.AppendC(UTF8STRC("t1 = "));
			sb.AppendDouble(t1);
			sb.AppendC(UTF8STRC(", t2 = "));
			sb.AppendDouble(t2);
			sb.AppendC(UTF8STRC(", t3 = "));
			sb.AppendDouble(t3);
/*			sb.AppendC(UTF8STRC(", t4 = "));
			sb.AppendDouble(t4);*/
			console.WriteLineC(sb.ToString(), sb.GetLength());
		}
		DEL_CLASS(cls);

		DEL_CLASS(fileGDB);
	}
	return 0;
}
