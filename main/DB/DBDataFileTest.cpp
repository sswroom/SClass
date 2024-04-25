#include "Stdafx.h"
#include "Core/Core.h"
#include "Crypto/Encrypt/JasyptEncryptor.h"
#include "Data/DateTime.h"
#include "Data/NamedClass.h"
#include "DB/DBClassReader.h"
#include "DB/DBDataFile.h"
#include "DB/DBReader.h"
#include "DB/JavaDBUtil.h"
#include "IO/ConsoleWriter.h"
#include "IO/JavaProperties.h"
#include "IO/Path.h"
#include "Net/OSSocketFactory.h"
#include "Text/String.h"

class LamppostData
{
private:
	Int32 id;
	Data::Timestamp dtRecv;
	Data::Timestamp dtData;
	Text::String* lampno;
	Int32 lightStatus;
	Int32 faultStatus;
	Int32 fault1Status;
	Int32 fault2Status;
	Int32 fault3Status;
	Int32 fault4Status;
	Int32 fault5Status;
	Int32 fault6Status;
	Int32 fault7Status;
	Int32 fault8Status;
	Int32 fault9Status;
	Int32 fault10Status;
	Text::String* fault1Descr;
	Text::String* fault2Descr;
	Text::String* fault3Descr;
	Text::String* fault4Descr;
	Text::String* fault5Descr;
	Text::String* fault6Descr;
	Text::String* fault7Descr;
	Text::String* fault8Descr;
	Text::String* fault9Descr;
	Text::String* fault10Descr;
	Double val1;
	Double val2;
	Double val3;
	Double val4;
	Double val5;
	Double val6;
	Double val7;
	Double val8;
	Double val9;
	Double val10;
	Int32 light1Status;
	Int32 light2Status;
	Int32 light3Status;
	Int32 light4Status;
	Int32 light5Status;
	Int32 light6Status;
	Int32 light7Status;
	Int32 light8Status;
	Int32 light9Status;
	Int32 light10Status;

public:
	LamppostData();
	~LamppostData();

	Int32 GetId();
	void SetId(Int32 id);
	Data::Timestamp GetDtRecv();
	void SetDtRecv(const Data::Timestamp &dtRecv);
	Data::Timestamp GetDtData();
	void SetDtData(const Data::Timestamp &dtData);
	Text::String* GetLampno();
	void SetLampno(Text::String* lampno);
	Int32 GetLightStatus();
	void SetLightStatus(Int32 lightStatus);
	Int32 GetFaultStatus();
	void SetFaultStatus(Int32 faultStatus);
	Int32 GetFault1Status();
	void SetFault1Status(Int32 fault1Status);
	Int32 GetFault2Status();
	void SetFault2Status(Int32 fault2Status);
	Int32 GetFault3Status();
	void SetFault3Status(Int32 fault3Status);
	Int32 GetFault4Status();
	void SetFault4Status(Int32 fault4Status);
	Int32 GetFault5Status();
	void SetFault5Status(Int32 fault5Status);
	Int32 GetFault6Status();
	void SetFault6Status(Int32 fault6Status);
	Int32 GetFault7Status();
	void SetFault7Status(Int32 fault7Status);
	Int32 GetFault8Status();
	void SetFault8Status(Int32 fault8Status);
	Int32 GetFault9Status();
	void SetFault9Status(Int32 fault9Status);
	Int32 GetFault10Status();
	void SetFault10Status(Int32 fault10Status);
	Text::String* GetFault1Descr();
	void SetFault1Descr(Text::String* fault1Descr);
	Text::String* GetFault2Descr();
	void SetFault2Descr(Text::String* fault2Descr);
	Text::String* GetFault3Descr();
	void SetFault3Descr(Text::String* fault3Descr);
	Text::String* GetFault4Descr();
	void SetFault4Descr(Text::String* fault4Descr);
	Text::String* GetFault5Descr();
	void SetFault5Descr(Text::String* fault5Descr);
	Text::String* GetFault6Descr();
	void SetFault6Descr(Text::String* fault6Descr);
	Text::String* GetFault7Descr();
	void SetFault7Descr(Text::String* fault7Descr);
	Text::String* GetFault8Descr();
	void SetFault8Descr(Text::String* fault8Descr);
	Text::String* GetFault9Descr();
	void SetFault9Descr(Text::String* fault9Descr);
	Text::String* GetFault10Descr();
	void SetFault10Descr(Text::String* fault10Descr);
	Double GetVal1();
	void SetVal1(Double val1);
	Double GetVal2();
	void SetVal2(Double val2);
	Double GetVal3();
	void SetVal3(Double val3);
	Double GetVal4();
	void SetVal4(Double val4);
	Double GetVal5();
	void SetVal5(Double val5);
	Double GetVal6();
	void SetVal6(Double val6);
	Double GetVal7();
	void SetVal7(Double val7);
	Double GetVal8();
	void SetVal8(Double val8);
	Double GetVal9();
	void SetVal9(Double val9);
	Double GetVal10();
	void SetVal10(Double val10);
	Int32 GetLight1Status();
	void SetLight1Status(Int32 light1Status);
	Int32 GetLight2Status();
	void SetLight2Status(Int32 light2Status);
	Int32 GetLight3Status();
	void SetLight3Status(Int32 light3Status);
	Int32 GetLight4Status();
	void SetLight4Status(Int32 light4Status);
	Int32 GetLight5Status();
	void SetLight5Status(Int32 light5Status);
	Int32 GetLight6Status();
	void SetLight6Status(Int32 light6Status);
	Int32 GetLight7Status();
	void SetLight7Status(Int32 light7Status);
	Int32 GetLight8Status();
	void SetLight8Status(Int32 light8Status);
	Int32 GetLight9Status();
	void SetLight9Status(Int32 light9Status);
	Int32 GetLight10Status();
	void SetLight10Status(Int32 light10Status);

	NN<Data::NamedClass<LamppostData>> CreateClass();
};

LamppostData::LamppostData()
{
	this->id = 0;
	this->dtRecv = 0;
	this->dtData = 0;
	this->lampno = 0;
	this->lightStatus = 0;
	this->faultStatus = 0;
	this->fault1Status = 0;
	this->fault2Status = 0;
	this->fault3Status = 0;
	this->fault4Status = 0;
	this->fault5Status = 0;
	this->fault6Status = 0;
	this->fault7Status = 0;
	this->fault8Status = 0;
	this->fault9Status = 0;
	this->fault10Status = 0;
	this->fault1Descr = 0;
	this->fault2Descr = 0;
	this->fault3Descr = 0;
	this->fault4Descr = 0;
	this->fault5Descr = 0;
	this->fault6Descr = 0;
	this->fault7Descr = 0;
	this->fault8Descr = 0;
	this->fault9Descr = 0;
	this->fault10Descr = 0;
	this->val1 = 0;
	this->val2 = 0;
	this->val3 = 0;
	this->val4 = 0;
	this->val5 = 0;
	this->val6 = 0;
	this->val7 = 0;
	this->val8 = 0;
	this->val9 = 0;
	this->val10 = 0;
	this->light1Status = 0;
	this->light2Status = 0;
	this->light3Status = 0;
	this->light4Status = 0;
	this->light5Status = 0;
	this->light6Status = 0;
	this->light7Status = 0;
	this->light8Status = 0;
	this->light9Status = 0;
	this->light10Status = 0;
}

LamppostData::~LamppostData()
{
	SDEL_STRING(this->lampno);
	SDEL_STRING(this->fault1Descr);
	SDEL_STRING(this->fault2Descr);
	SDEL_STRING(this->fault3Descr);
	SDEL_STRING(this->fault4Descr);
	SDEL_STRING(this->fault5Descr);
	SDEL_STRING(this->fault6Descr);
	SDEL_STRING(this->fault7Descr);
	SDEL_STRING(this->fault8Descr);
	SDEL_STRING(this->fault9Descr);
	SDEL_STRING(this->fault10Descr);
}

Int32 LamppostData::GetId()
{
	return this->id;
}

void LamppostData::SetId(Int32 id)
{
	this->id = id;
}

Data::Timestamp LamppostData::GetDtRecv()
{
	return this->dtRecv;
}

void LamppostData::SetDtRecv(const Data::Timestamp &dtRecv)
{
	this->dtRecv = dtRecv;
}

Data::Timestamp LamppostData::GetDtData()
{
	return this->dtData;
}

void LamppostData::SetDtData(const Data::Timestamp &dtData)
{
	this->dtData = dtData;
}

Text::String* LamppostData::GetLampno()
{
	return this->lampno;
}

void LamppostData::SetLampno(Text::String* lampno)
{
	SDEL_STRING(this->lampno);
	this->lampno = lampno?lampno->Clone().Ptr():0;
}

Int32 LamppostData::GetLightStatus()
{
	return this->lightStatus;
}

void LamppostData::SetLightStatus(Int32 lightStatus)
{
	this->lightStatus = lightStatus;
}

Int32 LamppostData::GetFaultStatus()
{
	return this->faultStatus;
}

void LamppostData::SetFaultStatus(Int32 faultStatus)
{
	this->faultStatus = faultStatus;
}

Int32 LamppostData::GetFault1Status()
{
	return this->fault1Status;
}

void LamppostData::SetFault1Status(Int32 fault1Status)
{
	this->fault1Status = fault1Status;
}

Int32 LamppostData::GetFault2Status()
{
	return this->fault2Status;
}

void LamppostData::SetFault2Status(Int32 fault2Status)
{
	this->fault2Status = fault2Status;
}

Int32 LamppostData::GetFault3Status()
{
	return this->fault3Status;
}

void LamppostData::SetFault3Status(Int32 fault3Status)
{
	this->fault3Status = fault3Status;
}

Int32 LamppostData::GetFault4Status()
{
	return this->fault4Status;
}

void LamppostData::SetFault4Status(Int32 fault4Status)
{
	this->fault4Status = fault4Status;
}

Int32 LamppostData::GetFault5Status()
{
	return this->fault5Status;
}

void LamppostData::SetFault5Status(Int32 fault5Status)
{
	this->fault5Status = fault5Status;
}

Int32 LamppostData::GetFault6Status()
{
	return this->fault6Status;
}

void LamppostData::SetFault6Status(Int32 fault6Status)
{
	this->fault6Status = fault6Status;
}

Int32 LamppostData::GetFault7Status()
{
	return this->fault7Status;
}

void LamppostData::SetFault7Status(Int32 fault7Status)
{
	this->fault7Status = fault7Status;
}

Int32 LamppostData::GetFault8Status()
{
	return this->fault8Status;
}

void LamppostData::SetFault8Status(Int32 fault8Status)
{
	this->fault8Status = fault8Status;
}

Int32 LamppostData::GetFault9Status()
{
	return this->fault9Status;
}

void LamppostData::SetFault9Status(Int32 fault9Status)
{
	this->fault9Status = fault9Status;
}

Int32 LamppostData::GetFault10Status()
{
	return this->fault10Status;
}

void LamppostData::SetFault10Status(Int32 fault10Status)
{
	this->fault10Status = fault10Status;
}

Text::String* LamppostData::GetFault1Descr()
{
	return this->fault1Descr;
}

void LamppostData::SetFault1Descr(Text::String* fault1Descr)
{
	SDEL_STRING(this->fault1Descr);
	this->fault1Descr = fault1Descr?fault1Descr->Clone().Ptr():0;
}

Text::String* LamppostData::GetFault2Descr()
{
	return this->fault2Descr;
}

void LamppostData::SetFault2Descr(Text::String* fault2Descr)
{
	SDEL_STRING(this->fault2Descr);
	this->fault2Descr = fault2Descr?fault2Descr->Clone().Ptr():0;
}

Text::String* LamppostData::GetFault3Descr()
{
	return this->fault3Descr;
}

void LamppostData::SetFault3Descr(Text::String* fault3Descr)
{
	SDEL_STRING(this->fault3Descr);
	this->fault3Descr = fault3Descr?fault3Descr->Clone().Ptr():0;
}

Text::String* LamppostData::GetFault4Descr()
{
	return this->fault4Descr;
}

void LamppostData::SetFault4Descr(Text::String* fault4Descr)
{
	SDEL_STRING(this->fault4Descr);
	this->fault4Descr = fault4Descr?fault4Descr->Clone().Ptr():0;
}

Text::String* LamppostData::GetFault5Descr()
{
	return this->fault5Descr;
}

void LamppostData::SetFault5Descr(Text::String* fault5Descr)
{
	SDEL_STRING(this->fault5Descr);
	this->fault5Descr = fault5Descr?fault5Descr->Clone().Ptr():0;
}

Text::String* LamppostData::GetFault6Descr()
{
	return this->fault6Descr;
}

void LamppostData::SetFault6Descr(Text::String* fault6Descr)
{
	SDEL_STRING(this->fault6Descr);
	this->fault6Descr = fault6Descr?fault6Descr->Clone().Ptr():0;
}

Text::String* LamppostData::GetFault7Descr()
{
	return this->fault7Descr;
}

void LamppostData::SetFault7Descr(Text::String* fault7Descr)
{
	SDEL_STRING(this->fault7Descr);
	this->fault7Descr = fault7Descr?fault7Descr->Clone().Ptr():0;
}

Text::String* LamppostData::GetFault8Descr()
{
	return this->fault8Descr;
}

void LamppostData::SetFault8Descr(Text::String* fault8Descr)
{
	SDEL_STRING(this->fault8Descr);
	this->fault8Descr = fault8Descr?fault8Descr->Clone().Ptr():0;
}

Text::String* LamppostData::GetFault9Descr()
{
	return this->fault9Descr;
}

void LamppostData::SetFault9Descr(Text::String* fault9Descr)
{
	SDEL_STRING(this->fault9Descr);
	this->fault9Descr = fault9Descr?fault9Descr->Clone().Ptr():0;
}

Text::String* LamppostData::GetFault10Descr()
{
	return this->fault10Descr;
}

void LamppostData::SetFault10Descr(Text::String* fault10Descr)
{
	SDEL_STRING(this->fault10Descr);
	this->fault10Descr = fault10Descr?fault10Descr->Clone().Ptr():0;
}

Double LamppostData::GetVal1()
{
	return this->val1;
}

void LamppostData::SetVal1(Double val1)
{
	this->val1 = val1;
}

Double LamppostData::GetVal2()
{
	return this->val2;
}

void LamppostData::SetVal2(Double val2)
{
	this->val2 = val2;
}

Double LamppostData::GetVal3()
{
	return this->val3;
}

void LamppostData::SetVal3(Double val3)
{
	this->val3 = val3;
}

Double LamppostData::GetVal4()
{
	return this->val4;
}

void LamppostData::SetVal4(Double val4)
{
	this->val4 = val4;
}

Double LamppostData::GetVal5()
{
	return this->val5;
}

void LamppostData::SetVal5(Double val5)
{
	this->val5 = val5;
}

Double LamppostData::GetVal6()
{
	return this->val6;
}

void LamppostData::SetVal6(Double val6)
{
	this->val6 = val6;
}

Double LamppostData::GetVal7()
{
	return this->val7;
}

void LamppostData::SetVal7(Double val7)
{
	this->val7 = val7;
}

Double LamppostData::GetVal8()
{
	return this->val8;
}

void LamppostData::SetVal8(Double val8)
{
	this->val8 = val8;
}

Double LamppostData::GetVal9()
{
	return this->val9;
}

void LamppostData::SetVal9(Double val9)
{
	this->val9 = val9;
}

Double LamppostData::GetVal10()
{
	return this->val10;
}

void LamppostData::SetVal10(Double val10)
{
	this->val10 = val10;
}

Int32 LamppostData::GetLight1Status()
{
	return this->light1Status;
}

void LamppostData::SetLight1Status(Int32 light1Status)
{
	this->light1Status = light1Status;
}

Int32 LamppostData::GetLight2Status()
{
	return this->light2Status;
}

void LamppostData::SetLight2Status(Int32 light2Status)
{
	this->light2Status = light2Status;
}

Int32 LamppostData::GetLight3Status()
{
	return this->light3Status;
}

void LamppostData::SetLight3Status(Int32 light3Status)
{
	this->light3Status = light3Status;
}

Int32 LamppostData::GetLight4Status()
{
	return this->light4Status;
}

void LamppostData::SetLight4Status(Int32 light4Status)
{
	this->light4Status = light4Status;
}

Int32 LamppostData::GetLight5Status()
{
	return this->light5Status;
}

void LamppostData::SetLight5Status(Int32 light5Status)
{
	this->light5Status = light5Status;
}

Int32 LamppostData::GetLight6Status()
{
	return this->light6Status;
}

void LamppostData::SetLight6Status(Int32 light6Status)
{
	this->light6Status = light6Status;
}

Int32 LamppostData::GetLight7Status()
{
	return this->light7Status;
}

void LamppostData::SetLight7Status(Int32 light7Status)
{
	this->light7Status = light7Status;
}

Int32 LamppostData::GetLight8Status()
{
	return this->light8Status;
}

void LamppostData::SetLight8Status(Int32 light8Status)
{
	this->light8Status = light8Status;
}

Int32 LamppostData::GetLight9Status()
{
	return this->light9Status;
}

void LamppostData::SetLight9Status(Int32 light9Status)
{
	this->light9Status = light9Status;
}

Int32 LamppostData::GetLight10Status()
{
	return this->light10Status;
}

void LamppostData::SetLight10Status(Int32 light10Status)
{
	this->light10Status = light10Status;
}

NN<Data::NamedClass<LamppostData>> LamppostData::CreateClass()
{
	NN<Data::NamedClass<LamppostData>> cls;
	NEW_CLASSNN(cls, Data::NamedClass<LamppostData>(this));
	CLASS_ADD(cls, id);
	CLASS_ADD(cls, dtRecv);
	CLASS_ADD(cls, dtData);
	CLASS_ADD(cls, lampno);
	CLASS_ADD(cls, lightStatus);
	CLASS_ADD(cls, faultStatus);
	CLASS_ADD(cls, fault1Status);
	CLASS_ADD(cls, fault2Status);
	CLASS_ADD(cls, fault3Status);
	CLASS_ADD(cls, fault4Status);
	CLASS_ADD(cls, fault5Status);
	CLASS_ADD(cls, fault6Status);
	CLASS_ADD(cls, fault7Status);
	CLASS_ADD(cls, fault8Status);
	CLASS_ADD(cls, fault9Status);
	CLASS_ADD(cls, fault10Status);
	CLASS_ADD(cls, fault1Descr);
	CLASS_ADD(cls, fault2Descr);
	CLASS_ADD(cls, fault3Descr);
	CLASS_ADD(cls, fault4Descr);
	CLASS_ADD(cls, fault5Descr);
	CLASS_ADD(cls, fault6Descr);
	CLASS_ADD(cls, fault7Descr);
	CLASS_ADD(cls, fault8Descr);
	CLASS_ADD(cls, fault9Descr);
	CLASS_ADD(cls, fault10Descr);
	CLASS_ADD(cls, val1);
	CLASS_ADD(cls, val2);
	CLASS_ADD(cls, val3);
	CLASS_ADD(cls, val4);
	CLASS_ADD(cls, val5);
	CLASS_ADD(cls, val6);
	CLASS_ADD(cls, val7);
	CLASS_ADD(cls, val8);
	CLASS_ADD(cls, val9);
	CLASS_ADD(cls, val10);
	CLASS_ADD(cls, light1Status);
	CLASS_ADD(cls, light2Status);
	CLASS_ADD(cls, light3Status);
	CLASS_ADD(cls, light4Status);
	CLASS_ADD(cls, light5Status);
	CLASS_ADD(cls, light6Status);
	CLASS_ADD(cls, light7Status);
	CLASS_ADD(cls, light8Status);
	CLASS_ADD(cls, light9Status);
	CLASS_ADD(cls, light10Status);
	return cls;
}

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	Text::CString key = CSTR("WEBnAPI");
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	IO::ConsoleWriter console;
	IO::LogTool log;
	Net::OSSocketFactory sockf(false);
	NN<IO::ConfigFile> cfg;
	Text::StringBuilderUTF8 sb;
	if (IO::JavaProperties::ParseAppProp().SetTo(cfg))
	{
		Crypto::Encrypt::JasyptEncryptor jasypt(Crypto::Encrypt::JasyptEncryptor::KA_PBEWITHHMACSHA512, Crypto::Encrypt::JasyptEncryptor::CA_AES256, key.ToByteArray());
		jasypt.Decrypt(cfg);
		NN<DB::DBTool> db;
//		console.WriteLine(Text::String::OrEmpty(cfg->GetValue(UTF8STRC("spring.datasource.url")))->v);
//		console.WriteLine(Text::String::OrEmpty(cfg->GetValue(UTF8STRC("spring.datasource.username")))->v);
//		console.WriteLine(Text::String::OrEmpty(cfg->GetValue(UTF8STRC("spring.datasource.password")))->v);
		if (DB::JavaDBUtil::OpenJDBC(cfg->GetValue(CSTR("spring.datasource.url")).OrNull(),
			cfg->GetValue(CSTR("spring.datasource.username")).OrNull(),
			cfg->GetValue(CSTR("spring.datasource.password")).OrNull(), log, sockf).SetTo(db))
		{
			UOSInt i;
			Data::ArrayListNN<LamppostData> dataList;
			Data::ArrayListNN<LamppostData> dataList2;
			NN<Data::NamedClass<LamppostData>> cls = LamppostData().CreateClass();
			NN<DB::DBReader> r;
			if (db->QueryTableData(CSTR("dbo"), CSTR("lamppost_data"), 0, 0, 0, CSTR_NULL, 0).SetTo(r))
			{
				{
					DB::DBClassReader<LamppostData> reader(r, cls);
					reader.ReadAll(dataList);
				}
				db->CloseReader(r);

				sb.ClearStr();
				sb.AppendUOSInt(dataList.GetCount());
				sb.AppendC(UTF8STRC(" rows of records loaded"));
				console.WriteLineC(sb.ToString(), sb.GetLength());

				sptr = IO::Path::GetRealPath(sbuff, UTF8STRC("~/Progs/Temp/LamppostData.ddf"));
				if (DB::DBDataFile<LamppostData>::SaveFile(CSTRP(sbuff, sptr), dataList, cls))
				{
					console.WriteLineC(UTF8STRC("File saved"));
				}
				if (DB::DBDataFile<LamppostData>::LoadFile(CSTRP(sbuff, sptr), cls, dataList2))
				{
					sb.ClearStr();
					sb.AppendUOSInt(dataList2.GetCount());
					sb.AppendC(UTF8STRC(" rows of records loaded from file"));
					console.WriteLineC(sb.ToString(), sb.GetLength());
				}
				if (dataList.GetCount() == dataList2.GetCount())
				{
					Bool succ = true;
					console.WriteLineC(UTF8STRC("Comparing two sets of data"));
					i = dataList.GetCount();
					while (i-- > 0)
					{
						if (!cls->Equals(dataList.GetItemNoCheck(i), dataList2.GetItemNoCheck(i)))
						{
							console.WriteLineC(UTF8STRC("Data not match"));
							succ = false;
							break;
						}
					}
					if (succ)
					{
						console.WriteLineC(UTF8STRC("Data matched"));
					}
				}
			}

			dataList.DeleteAll();
			dataList2.DeleteAll();
			cls.Delete();
			db.Delete();
		}
		else
		{
			console.WriteLineC(UTF8STRC("Error in opening DB connection"));
		}
		cfg.Delete();
	}
	else
	{
		console.WriteLineC(UTF8STRC("Error in loading application.properties"));
	}
	return 0;
}

class CadRunway
{
private:
	Int32 id;
	Text::String* code;
	Text::String* displayName;
	Text::String* descr;
	Data::Timestamp dtCreate;
	Data::Timestamp dtModify;
	Int32 uidCreate;
	Int32 uidModify;
	Int32 status;

public:
	CadRunway();
	~CadRunway();

	Int32 GetId() const;
	void SetId(Int32 id);
	Text::String* GetCode() const;
	void SetCode(Text::String* code);
	Text::String* GetDisplayName() const;
	void SetDisplayName(Text::String* displayName);
	Text::String* GetDescr() const;
	void SetDescr(Text::String* descr);
	Data::Timestamp GetDtCreate() const;
	void SetDtCreate(const Data::Timestamp &dtCreate);
	Data::Timestamp GetDtModify() const;
	void SetDtModify(const Data::Timestamp &dtModify);
	Int32 GetUidCreate() const;
	void SetUidCreate(Int32 uidCreate);
	Int32 GetUidModify() const;
	void SetUidModify(Int32 uidModify);
	Int32 GetStatus() const;
	void SetStatus(Int32 status);

	Data::NamedClass<CadRunway> *CreateClass() const;
};

CadRunway::CadRunway()
{
	this->id = 0;
	this->code = 0;
	this->displayName = 0;
	this->descr = 0;
	this->dtCreate = 0;
	this->dtModify = 0;
	this->uidCreate = 0;
	this->uidModify = 0;
	this->status = 0;
}

CadRunway::~CadRunway()
{
	SDEL_STRING(this->code);
	SDEL_STRING(this->displayName);
	SDEL_STRING(this->descr);
}

Int32 CadRunway::GetId() const
{
	return this->id;
}

void CadRunway::SetId(Int32 id)
{
	this->id = id;
}

Text::String* CadRunway::GetCode() const
{
	return this->code;
}

void CadRunway::SetCode(Text::String* code)
{
	SDEL_STRING(this->code);
	this->code = code?code->Clone().Ptr():0;
}

Text::String* CadRunway::GetDisplayName() const
{
	return this->displayName;
}

void CadRunway::SetDisplayName(Text::String* displayName)
{
	SDEL_STRING(this->displayName);
	this->displayName = displayName?displayName->Clone().Ptr():0;
}

Text::String* CadRunway::GetDescr() const
{
	return this->descr;
}

void CadRunway::SetDescr(Text::String* descr)
{
	SDEL_STRING(this->descr);
	this->descr = descr?descr->Clone().Ptr():0;
}

Data::Timestamp CadRunway::GetDtCreate() const
{
	return this->dtCreate;
}

void CadRunway::SetDtCreate(const Data::Timestamp &dtCreate)
{
	this->dtCreate = dtCreate;
}

Data::Timestamp CadRunway::GetDtModify() const
{
	return this->dtModify;
}

void CadRunway::SetDtModify(const Data::Timestamp &dtModify)
{
	this->dtModify = dtModify;
}

Int32 CadRunway::GetUidCreate() const
{
	return this->uidCreate;
}

void CadRunway::SetUidCreate(Int32 uidCreate)
{
	this->uidCreate = uidCreate;
}

Int32 CadRunway::GetUidModify() const
{
	return this->uidModify;
}

void CadRunway::SetUidModify(Int32 uidModify)
{
	this->uidModify = uidModify;
}

Int32 CadRunway::GetStatus() const
{
	return this->status;
}

void CadRunway::SetStatus(Int32 status)
{
	this->status = status;
}

Data::NamedClass<CadRunway> *CadRunway::CreateClass() const
{
	Data::NamedClass<CadRunway> *cls;
	NEW_CLASS(cls, Data::NamedClass<CadRunway>(this));
	CLASS_ADD(cls, id);
	CLASS_ADD(cls, code);
	CLASS_ADD(cls, displayName);
	CLASS_ADD(cls, descr);
	CLASS_ADD(cls, dtCreate);
	CLASS_ADD(cls, dtModify);
	CLASS_ADD(cls, uidCreate);
	CLASS_ADD(cls, uidModify);
	CLASS_ADD(cls, status);
	return cls;
}
