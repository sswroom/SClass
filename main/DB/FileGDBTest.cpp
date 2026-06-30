#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/NamedClass.hpp"
#include "DB/CSVFile.h"
#include "DB/DBClassReader.hpp"
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
	Optional<Text::String> accLocati;
	Optional<Text::String> antiBurgl;
	Int16 bulbQty;
	Data::Timestamp cmsDate;
	Optional<Text::String> cmsType;
	Int16 colClass;
	Single columnHt;
	Single concD;
	Single concL;
	Single concW;
	Optional<Text::String> contrNo;
	Optional<Text::String> czone;
	Optional<Text::String> deleted;
	Data::Timestamp disconDate;
	Optional<Text::String> district;
	Optional<Text::String> fixOPole;
	Int16 iVersion;
	Data::Timestamp installDate;
	Single kerbOs;
	Optional<Text::String> lampNo;
	Optional<Text::String> lampType;
	Optional<Text::String> landmark;
	Optional<Text::String> ltgRegion;
	Optional<Text::String> mounting;
	Optional<Text::String> nOPoint;
	Optional<Text::String> nemaSoc;
	Int32 objectid;
	Int32 objectidP;
	Optional<Text::String> pUserFid;
	Optional<Text::String> paintMate;
	Single plDepth;
	Optional<Text::String> poleManu;
	Optional<Text::String> poleType;
	Optional<Text::String> polecolor;
	Optional<Text::String> primarySt;
	Data::Timestamp regDate;
	Optional<Text::String> remark1;
	Optional<Text::String> remark2;
	Optional<Text::String> remark3;
	Int16 rotation;
	Optional<Text::String> routeno;
	Optional<Text::String> secondSt;
	Optional<Math::Geometry::Vector2D> shape;
	Optional<Text::String> shielded;
	Optional<Text::String> solarLtg;
	Optional<Text::String> specMate;
	Data::Timestamp subDate;
	Optional<Text::String> subtype;
	Optional<Text::String> updbyhyd;
	Optional<Text::String> updbymom;
	Data::Timestamp upddatehyd;
	Data::Timestamp upddatemom;
	Optional<Text::String> utilNo;
	Optional<Text::String> village;
	Single zCoor;

public:
	Lamppost();
	~Lamppost();

	Optional<Text::String> GetAccLocati();
	void SetAccLocati(Optional<Text::String> accLocati);
	Optional<Text::String> GetAntiBurgl();
	void SetAntiBurgl(Optional<Text::String> antiBurgl);
	Int16 GetBulbQty();
	void SetBulbQty(Int16 bulbQty);
	Data::Timestamp GetCmsDate();
	void SetCmsDate(const Data::Timestamp &cmsDate);
	Optional<Text::String> GetCmsType();
	void SetCmsType(Optional<Text::String> cmsType);
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
	Optional<Text::String> GetContrNo();
	void SetContrNo(Optional<Text::String> contrNo);
	Optional<Text::String> GetCzone();
	void SetCzone(Optional<Text::String> czone);
	Optional<Text::String> GetDeleted();
	void SetDeleted(Optional<Text::String> deleted);
	Data::Timestamp GetDisconDate();
	void SetDisconDate(const Data::Timestamp &disconDate);
	Optional<Text::String> GetDistrict();
	void SetDistrict(Optional<Text::String> district);
	Optional<Text::String> GetFixOPole();
	void SetFixOPole(Optional<Text::String> fixOPole);
	Int16 GetIVersion();
	void SetIVersion(Int16 iVersion);
	Data::Timestamp GetInstallDate();
	void SetInstallDate(const Data::Timestamp &installDate);
	Single GetKerbOs();
	void SetKerbOs(Single kerbOs);
	Optional<Text::String> GetLampNo();
	void SetLampNo(Optional<Text::String> lampNo);
	Optional<Text::String> GetLampType();
	void SetLampType(Optional<Text::String> lampType);
	Optional<Text::String> GetLandmark();
	void SetLandmark(Optional<Text::String> landmark);
	Optional<Text::String> GetLtgRegion();
	void SetLtgRegion(Optional<Text::String> ltgRegion);
	Optional<Text::String> GetMounting();
	void SetMounting(Optional<Text::String> mounting);
	Optional<Text::String> GetNOPoint();
	void SetNOPoint(Optional<Text::String> nOPoint);
	Optional<Text::String> GetNemaSoc();
	void SetNemaSoc(Optional<Text::String> nemaSoc);
	Int32 GetObjectid();
	void SetObjectid(Int32 objectid);
	Int32 GetObjectidP();
	void SetObjectidP(Int32 objectidP);
	Optional<Text::String> GetPUserFid();
	void SetPUserFid(Optional<Text::String> pUserFid);
	Optional<Text::String> GetPaintMate();
	void SetPaintMate(Optional<Text::String> paintMate);
	Single GetPlDepth();
	void SetPlDepth(Single plDepth);
	Optional<Text::String> GetPoleManu();
	void SetPoleManu(Optional<Text::String> poleManu);
	Optional<Text::String> GetPoleType();
	void SetPoleType(Optional<Text::String> poleType);
	Optional<Text::String> GetPolecolor();
	void SetPolecolor(Optional<Text::String> polecolor);
	Optional<Text::String> GetPrimarySt();
	void SetPrimarySt(Optional<Text::String> primarySt);
	Data::Timestamp GetRegDate();
	void SetRegDate(const Data::Timestamp &regDate);
	Optional<Text::String> GetRemark1();
	void SetRemark1(Optional<Text::String> remark1);
	Optional<Text::String> GetRemark2();
	void SetRemark2(Optional<Text::String> remark2);
	Optional<Text::String> GetRemark3();
	void SetRemark3(Optional<Text::String> remark3);
	Int16 GetRotation();
	void SetRotation(Int16 rotation);
	Optional<Text::String> GetRouteno();
	void SetRouteno(Optional<Text::String> routeno);
	Optional<Text::String> GetSecondSt();
	void SetSecondSt(Optional<Text::String> secondSt);
	Optional<Math::Geometry::Vector2D> GetShape();
	void SetShape(Optional<Math::Geometry::Vector2D> shape);
	Optional<Text::String> GetShielded();
	void SetShielded(Optional<Text::String> shielded);
	Optional<Text::String> GetSolarLtg();
	void SetSolarLtg(Optional<Text::String> solarLtg);
	Optional<Text::String> GetSpecMate();
	void SetSpecMate(Optional<Text::String> specMate);
	Data::Timestamp GetSubDate();
	void SetSubDate(const Data::Timestamp &subDate);
	Optional<Text::String> GetSubtype();
	void SetSubtype(Optional<Text::String> subtype);
	Optional<Text::String> GetUpdbyhyd();
	void SetUpdbyhyd(Optional<Text::String> updbyhyd);
	Optional<Text::String> GetUpdbymom();
	void SetUpdbymom(Optional<Text::String> updbymom);
	Data::Timestamp GetUpddatehyd();
	void SetUpddatehyd(const Data::Timestamp &upddatehyd);
	Data::Timestamp GetUpddatemom();
	void SetUpddatemom(const Data::Timestamp &upddatemom);
	Optional<Text::String> GetUtilNo();
	void SetUtilNo(Optional<Text::String> utilNo);
	Optional<Text::String> GetVillage();
	void SetVillage(Optional<Text::String> village);
	Single GetZCoor();
	void SetZCoor(Single zCoor);

	NN<Data::NamedClass<Lamppost>> CreateClass();
};

Lamppost::Lamppost()
{
	this->accLocati = nullptr;
	this->antiBurgl = nullptr;
	this->bulbQty = 0;
	this->cmsDate = 0;
	this->cmsType = nullptr;
	this->colClass = 0;
	this->columnHt = 0;
	this->concD = 0;
	this->concL = 0;
	this->concW = 0;
	this->contrNo = nullptr;
	this->czone = nullptr;
	this->deleted = nullptr;
	this->disconDate = 0;
	this->district = nullptr;
	this->fixOPole = nullptr;
	this->iVersion = 0;
	this->installDate = 0;
	this->kerbOs = 0;
	this->lampNo = nullptr;
	this->lampType = nullptr;
	this->landmark = nullptr;
	this->ltgRegion = nullptr;
	this->mounting = nullptr;
	this->nOPoint = nullptr;
	this->nemaSoc = nullptr;
	this->objectid = 0;
	this->objectidP = 0;
	this->pUserFid = nullptr;
	this->paintMate = nullptr;
	this->plDepth = 0;
	this->poleManu = nullptr;
	this->poleType = nullptr;
	this->polecolor = nullptr;
	this->primarySt = nullptr;
	this->regDate = 0;
	this->remark1 = nullptr;
	this->remark2 = nullptr;
	this->remark3 = nullptr;
	this->rotation = 0;
	this->routeno = nullptr;
	this->secondSt = nullptr;
	this->shape = nullptr;
	this->shielded = nullptr;
	this->solarLtg = nullptr;
	this->specMate = nullptr;
	this->subDate = 0;
	this->subtype = nullptr;
	this->updbyhyd = nullptr;
	this->updbymom = nullptr;
	this->upddatehyd = 0;
	this->upddatemom = 0;
	this->utilNo = nullptr;
	this->village = nullptr;
	this->zCoor = 0;
}

Lamppost::~Lamppost()
{
	OPTSTR_DEL(this->accLocati);
	OPTSTR_DEL(this->antiBurgl);
	OPTSTR_DEL(this->cmsType);
	OPTSTR_DEL(this->contrNo);
	OPTSTR_DEL(this->czone);
	OPTSTR_DEL(this->deleted);
	OPTSTR_DEL(this->district);
	OPTSTR_DEL(this->fixOPole);
	OPTSTR_DEL(this->lampNo);
	OPTSTR_DEL(this->lampType);
	OPTSTR_DEL(this->landmark);
	OPTSTR_DEL(this->ltgRegion);
	OPTSTR_DEL(this->mounting);
	OPTSTR_DEL(this->nOPoint);
	OPTSTR_DEL(this->nemaSoc);
	OPTSTR_DEL(this->pUserFid);
	OPTSTR_DEL(this->paintMate);
	OPTSTR_DEL(this->poleManu);
	OPTSTR_DEL(this->poleType);
	OPTSTR_DEL(this->polecolor);
	OPTSTR_DEL(this->primarySt);
	OPTSTR_DEL(this->remark1);
	OPTSTR_DEL(this->remark2);
	OPTSTR_DEL(this->remark3);
	OPTSTR_DEL(this->routeno);
	OPTSTR_DEL(this->secondSt);
	this->shape.Delete();
	OPTSTR_DEL(this->shielded);
	OPTSTR_DEL(this->solarLtg);
	OPTSTR_DEL(this->specMate);
	OPTSTR_DEL(this->subtype);
	OPTSTR_DEL(this->updbyhyd);
	OPTSTR_DEL(this->updbymom);
	OPTSTR_DEL(this->utilNo);
	OPTSTR_DEL(this->village);
}

Optional<Text::String> Lamppost::GetAccLocati()
{
	return this->accLocati;
}

void Lamppost::SetAccLocati(Optional<Text::String> accLocati)
{
	OPTSTR_DEL(this->accLocati);
	this->accLocati = Text::String::CopyOrNull(accLocati);
}

Optional<Text::String> Lamppost::GetAntiBurgl()
{
	return this->antiBurgl;
}

void Lamppost::SetAntiBurgl(Optional<Text::String> antiBurgl)
{
	OPTSTR_DEL(this->antiBurgl);
	this->antiBurgl = Text::String::CopyOrNull(antiBurgl);
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

Optional<Text::String> Lamppost::GetCmsType()
{
	return this->cmsType;
}

void Lamppost::SetCmsType(Optional<Text::String> cmsType)
{
	OPTSTR_DEL(this->cmsType);
	this->cmsType = Text::String::CopyOrNull(cmsType);
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

Optional<Text::String> Lamppost::GetContrNo()
{
	return this->contrNo;
}

void Lamppost::SetContrNo(Optional<Text::String> contrNo)
{
	OPTSTR_DEL(this->contrNo);
	this->contrNo = Text::String::CopyOrNull(contrNo);
}

Optional<Text::String> Lamppost::GetCzone()
{
	return this->czone;
}

void Lamppost::SetCzone(Optional<Text::String> czone)
{
	OPTSTR_DEL(this->czone);
	this->czone = Text::String::CopyOrNull(czone);
}

Optional<Text::String> Lamppost::GetDeleted()
{
	return this->deleted;
}

void Lamppost::SetDeleted(Optional<Text::String> deleted)
{
	OPTSTR_DEL(this->deleted);
	this->deleted = Text::String::CopyOrNull(deleted);
}

Data::Timestamp Lamppost::GetDisconDate()
{
	return this->disconDate;
}

void Lamppost::SetDisconDate(const Data::Timestamp &disconDate)
{
	this->disconDate = disconDate;
}

Optional<Text::String> Lamppost::GetDistrict()
{
	return this->district;
}

void Lamppost::SetDistrict(Optional<Text::String> district)
{
	OPTSTR_DEL(this->district);
	this->district = Text::String::CopyOrNull(district);
}

Optional<Text::String> Lamppost::GetFixOPole()
{
	return this->fixOPole;
}

void Lamppost::SetFixOPole(Optional<Text::String> fixOPole)
{
	OPTSTR_DEL(this->fixOPole);
	this->fixOPole = Text::String::CopyOrNull(fixOPole);
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

Optional<Text::String> Lamppost::GetLampNo()
{
	return this->lampNo;
}

void Lamppost::SetLampNo(Optional<Text::String> lampNo)
{
	OPTSTR_DEL(this->lampNo);
	this->lampNo = Text::String::CopyOrNull(lampNo);
}

Optional<Text::String> Lamppost::GetLampType()
{
	return this->lampType;
}

void Lamppost::SetLampType(Optional<Text::String> lampType)
{
	OPTSTR_DEL(this->lampType);
	this->lampType = Text::String::CopyOrNull(lampType);
}

Optional<Text::String> Lamppost::GetLandmark()
{
	return this->landmark;
}

void Lamppost::SetLandmark(Optional<Text::String> landmark)
{
	OPTSTR_DEL(this->landmark);
	this->landmark = Text::String::CopyOrNull(landmark);
}

Optional<Text::String> Lamppost::GetLtgRegion()
{
	return this->ltgRegion;
}

void Lamppost::SetLtgRegion(Optional<Text::String> ltgRegion)
{
	OPTSTR_DEL(this->ltgRegion);
	this->ltgRegion = Text::String::CopyOrNull(ltgRegion);
}

Optional<Text::String> Lamppost::GetMounting()
{
	return this->mounting;
}

void Lamppost::SetMounting(Optional<Text::String> mounting)
{
	OPTSTR_DEL(this->mounting);
	this->mounting = Text::String::CopyOrNull(mounting);
}

Optional<Text::String> Lamppost::GetNOPoint()
{
	return this->nOPoint;
}

void Lamppost::SetNOPoint(Optional<Text::String> nOPoint)
{
	OPTSTR_DEL(this->nOPoint);
	this->nOPoint = Text::String::CopyOrNull(nOPoint);
}

Optional<Text::String> Lamppost::GetNemaSoc()
{
	return this->nemaSoc;
}

void Lamppost::SetNemaSoc(Optional<Text::String> nemaSoc)
{
	OPTSTR_DEL(this->nemaSoc);
	this->nemaSoc = Text::String::CopyOrNull(nemaSoc);
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

Optional<Text::String> Lamppost::GetPUserFid()
{
	return this->pUserFid;
}

void Lamppost::SetPUserFid(Optional<Text::String> pUserFid)
{
	OPTSTR_DEL(this->pUserFid);
	this->pUserFid = Text::String::CopyOrNull(pUserFid);
}

Optional<Text::String> Lamppost::GetPaintMate()
{
	return this->paintMate;
}

void Lamppost::SetPaintMate(Optional<Text::String> paintMate)
{
	OPTSTR_DEL(this->paintMate);
	this->paintMate = Text::String::CopyOrNull(paintMate);
}

Single Lamppost::GetPlDepth()
{
	return this->plDepth;
}

void Lamppost::SetPlDepth(Single plDepth)
{
	this->plDepth = plDepth;
}

Optional<Text::String> Lamppost::GetPoleManu()
{
	return this->poleManu;
}

void Lamppost::SetPoleManu(Optional<Text::String> poleManu)
{
	OPTSTR_DEL(this->poleManu);
	this->poleManu = Text::String::CopyOrNull(poleManu);
}

Optional<Text::String> Lamppost::GetPoleType()
{
	return this->poleType;
}

void Lamppost::SetPoleType(Optional<Text::String> poleType)
{
	OPTSTR_DEL(this->poleType);
	this->poleType = Text::String::CopyOrNull(poleType);
}

Optional<Text::String> Lamppost::GetPolecolor()
{
	return this->polecolor;
}

void Lamppost::SetPolecolor(Optional<Text::String> polecolor)
{
	OPTSTR_DEL(this->polecolor);
	this->polecolor = Text::String::CopyOrNull(polecolor);
}

Optional<Text::String> Lamppost::GetPrimarySt()
{
	return this->primarySt;
}

void Lamppost::SetPrimarySt(Optional<Text::String> primarySt)
{
	OPTSTR_DEL(this->primarySt);
	this->primarySt = Text::String::CopyOrNull(primarySt);
}

Data::Timestamp Lamppost::GetRegDate()
{
	return this->regDate;
}

void Lamppost::SetRegDate(const Data::Timestamp &regDate)
{
	this->regDate = regDate;
}

Optional<Text::String> Lamppost::GetRemark1()
{
	return this->remark1;
}

void Lamppost::SetRemark1(Optional<Text::String> remark1)
{
	OPTSTR_DEL(this->remark1);
	this->remark1 = Text::String::CopyOrNull(remark1);
}

Optional<Text::String> Lamppost::GetRemark2()
{
	return this->remark2;
}

void Lamppost::SetRemark2(Optional<Text::String> remark2)
{
	OPTSTR_DEL(this->remark2);
	this->remark2 = Text::String::CopyOrNull(remark2);
}

Optional<Text::String> Lamppost::GetRemark3()
{
	return this->remark3;
}

void Lamppost::SetRemark3(Optional<Text::String> remark3)
{
	OPTSTR_DEL(this->remark3);
	this->remark3 = Text::String::CopyOrNull(remark3);
}

Int16 Lamppost::GetRotation()
{
	return this->rotation;
}

void Lamppost::SetRotation(Int16 rotation)
{
	this->rotation = rotation;
}

Optional<Text::String> Lamppost::GetRouteno()
{
	return this->routeno;
}

void Lamppost::SetRouteno(Optional<Text::String> routeno)
{
	OPTSTR_DEL(this->routeno);
	this->routeno = Text::String::CopyOrNull(routeno);
}

Optional<Text::String> Lamppost::GetSecondSt()
{
	return this->secondSt;
}

void Lamppost::SetSecondSt(Optional<Text::String> secondSt)
{
	OPTSTR_DEL(this->secondSt);
	this->secondSt = Text::String::CopyOrNull(secondSt);
}

Optional<Math::Geometry::Vector2D> Lamppost::GetShape()
{
	return this->shape;
}

void Lamppost::SetShape(Optional<Math::Geometry::Vector2D> shape)
{
	this->shape.Delete();
	this->shape = shape.IsNull()?Optional<Math::Geometry::Vector2D>(nullptr):shape.OrNull()->Clone();
}

Optional<Text::String> Lamppost::GetShielded()
{
	return this->shielded;
}

void Lamppost::SetShielded(Optional<Text::String> shielded)
{
	OPTSTR_DEL(this->shielded);
	this->shielded = Text::String::CopyOrNull(shielded);
}

Optional<Text::String> Lamppost::GetSolarLtg()
{
	return this->solarLtg;
}

void Lamppost::SetSolarLtg(Optional<Text::String> solarLtg)
{
	OPTSTR_DEL(this->solarLtg);
	this->solarLtg = Text::String::CopyOrNull(solarLtg);
}

Optional<Text::String> Lamppost::GetSpecMate()
{
	return this->specMate;
}

void Lamppost::SetSpecMate(Optional<Text::String> specMate)
{
	OPTSTR_DEL(this->specMate);
	this->specMate = Text::String::CopyOrNull(specMate);
}

Data::Timestamp Lamppost::GetSubDate()
{
	return this->subDate;
}

void Lamppost::SetSubDate(const Data::Timestamp &subDate)
{
	this->subDate = subDate;
}

Optional<Text::String> Lamppost::GetSubtype()
{
	return this->subtype;
}

void Lamppost::SetSubtype(Optional<Text::String> subtype)
{
	OPTSTR_DEL(this->subtype);
	this->subtype = Text::String::CopyOrNull(subtype);
}

Optional<Text::String> Lamppost::GetUpdbyhyd()
{
	return this->updbyhyd;
}

void Lamppost::SetUpdbyhyd(Optional<Text::String> updbyhyd)
{
	OPTSTR_DEL(this->updbyhyd);
	this->updbyhyd = Text::String::CopyOrNull(updbyhyd);
}

Optional<Text::String> Lamppost::GetUpdbymom()
{
	return this->updbymom;
}

void Lamppost::SetUpdbymom(Optional<Text::String> updbymom)
{
	OPTSTR_DEL(this->updbymom);
	this->updbymom = Text::String::CopyOrNull(updbymom);
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

Optional<Text::String> Lamppost::GetUtilNo()
{
	return this->utilNo;
}

void Lamppost::SetUtilNo(Optional<Text::String> utilNo)
{
	OPTSTR_DEL(this->utilNo);
	this->utilNo = Text::String::CopyOrNull(utilNo);
}

Optional<Text::String> Lamppost::GetVillage()
{
	return this->village;
}

void Lamppost::SetVillage(Optional<Text::String> village)
{
	OPTSTR_DEL(this->village);
	this->village = Text::String::CopyOrNull(village);
}

Single Lamppost::GetZCoor()
{
	return this->zCoor;
}

void Lamppost::SetZCoor(Single zCoor)
{
	this->zCoor = zCoor;
}

NN<Data::NamedClass<Lamppost>> Lamppost::CreateClass()
{
	NN<Data::NamedClass<Lamppost>> cls;
	NEW_CLASSNN(cls, Data::NamedClass<Lamppost>(this));
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

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	IO::ConsoleWriter console;
	Manage::HiResClock clk;
	IO::DirectoryPackage dir(CSTR("~/Progs/Temp/E20210522_PLIS.gdb"));
	NN<Map::ESRI::FileGDBDir> fileGDB;
	if (Map::ESRI::FileGDBDir::OpenDir(dir).SetTo(fileGDB))
	{
		Text::StringBuilderUTF8 sb;
		Data::ArrayListObj<const UTF8Char*> colNames;
		colNames.Add((const UTF8Char*)"OBJECTID");
		colNames.Add((const UTF8Char*)"Shape");
		Data::QueryConditions cond;
		cond.Int32Equals(CSTR("OBJECTID"), 40);
		NN<DB::DBReader> r;
/*		if (r.Set(fileGDB->QueryTableData((const UTF8Char*)"LAMPPOST", &colNames, nullptr, 10, (const UTF8Char*)"OBJECTID desc", nullptr)))//&cond);
		{
			while (r->ReadNext())
			{
				Data::VariObject *obj = r->CreateVariObject();
				sb.ClearStr();
				obj->ToString(sb);
				console.WriteLine(sb.ToCString());
				DEL_CLASS(obj);
			}
			fileGDB->CloseReader(r);
		}*/

		NN<Data::NamedClass<Lamppost>> cls = Lamppost().CreateClass();

		if (fileGDB->QueryTableData(nullptr, CSTR("LAMPPOST"), nullptr, 0, 0, nullptr, nullptr).SetTo(r))
		{
			Double t1;
			Double t2;
			Double t3;
//			Double t4 = 0;
			Data::ArrayListNN<Lamppost> lamppostList;
			clk.Start();
			{
				DB::DBClassReader<Lamppost> reader(r, cls);
				reader.ReadAll(lamppostList);
			}
			t1 = clk.GetTimeDiff();
			fileGDB->CloseReader(r);

			sptr = IO::Path::GetRealPath(sbuff, UTF8STRC("~/Progs/Temp/Lamppost.csv"));
			{
				IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				clk.Start();
				DB::DBUtil::SaveCSV<Lamppost>(fs, lamppostList, cls);
				t2 = clk.GetTimeDiff();
			}
			

			Data::ArrayListNN<Lamppost> lamppostListCSV;
			NN<DB::CSVFile> csv;
			NEW_CLASSNN(csv, DB::CSVFile(CSTRP(sbuff, sptr), 65001));
			csv->SetNullIfEmpty(true);
			if (csv->QueryTableData(nullptr, CSTR("Lamppost"), nullptr, 0, 0, nullptr, nullptr).SetTo(r))
			{
				clk.Start();
				{
					DB::DBClassReader<Lamppost> reader(r, cls);
					reader.ReadAll(lamppostListCSV);
				}
				t3 = clk.GetTimeDiff();
				csv->CloseReader(r);
			}
			else
			{
				t3 = -1;
			}
			csv.Delete();
			
			sb.ClearStr();
			sb.AppendC(UTF8STRC("FileGDB count = "));
			sb.AppendUIntOS(lamppostList.GetCount());
			sb.AppendC(UTF8STRC(", CSV count = "));
			sb.AppendUIntOS(lamppostListCSV.GetCount());
			console.WriteLine(sb.ToCString());
			
/*			if (lamppostList.GetCount() == lamppostListCSV.GetCount())
			{
				Lamppost *lamppost2;
				UIntOS j;
				clk.Start();	
				i = 0;
				j = lamppostList.GetCount();
				while (i < j)
				{
					lamppost = lamppostList.GetItem(i);
					lamppost2 = lamppostListCSV.GetItem(i);
					if (!cls->Equals(lamppost, lamppost2))
					{
						console.WriteLine(CSTR("Not equals:");
						console.WriteLine(CSTR("FileGDB:");
						sb.ClearStr();
						Text::StringTool::BuildString(sb, lamppost, cls);
						console.WriteLine(sb.ToCString());
						console.WriteLine(CSTR("CSV:");
						sb.ClearStr();
						Text::StringTool::BuildString(sb, lamppost2, cls);
						console.WriteLine(sb.ToCString());
						break;
					}
					i++;
				}
				t4 = clk.GetTimeDiff();
			}*/
/*			sb.ClearStr();
			Text::StringTool::BuildString(sb, &lamppostList, cls, (const UTF8Char*)"Lamppost");
			console.WriteLine(sb.ToCString());*/

			lamppostList.DeleteAll();
			lamppostListCSV.DeleteAll();

			sb.ClearStr();
			sb.AppendC(UTF8STRC("t1 = "));
			sb.AppendDouble(t1);
			sb.AppendC(UTF8STRC(", t2 = "));
			sb.AppendDouble(t2);
			sb.AppendC(UTF8STRC(", t3 = "));
			sb.AppendDouble(t3);
/*			sb.AppendC(UTF8STRC(", t4 = "));
			sb.AppendDouble(t4);*/
			console.WriteLine(sb.ToCString());
		}
		cls.Delete();

		fileGDB.Delete();
	}
	return 0;
}
