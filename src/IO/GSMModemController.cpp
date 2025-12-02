#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/GSMModemController.h"
#include "Sync/MutexUsage.h"
#include "Text/CString.h"
#include "Text/MyString.h"

Bool IO::GSMModemController::SetSMSFormat(IO::GSMModemController::SMSFormat smsFormat)
{
	if (smsFormat == IO::GSMModemController::SMSF_PDU)
	{
		Bool succ = this->SendBoolCommandC(UTF8STRC("AT+CMGF=0"));
		if (succ)
			this->smsFormat = 0;
		return succ;
	}
	else if (smsFormat == IO::GSMModemController::SMSF_TEXT)
	{
		Bool succ = this->SendBoolCommandC(UTF8STRC("AT+CMGF=1"));
		if (succ)
			this->smsFormat = 1;
		return succ;
	}
	return false;
}

Bool IO::GSMModemController::GetSMSFormat(OutParam<SMSFormat> smsFormat)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	if (!SendStringCommand(sbuff, UTF8STRC("AT+CMGF?"), 3000).SetTo(sptr))
		return false;
	if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("+CMGF: 1")))
	{
		smsFormat.Set(IO::GSMModemController::SMSF_TEXT);
		return true;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("+CMGF: 0")))
	{
		smsFormat.Set(IO::GSMModemController::SMSF_PDU);
		return true;
	}
	else
	{
		return false;
	}
}

void __stdcall IO::GSMModemController::FreeOperator(NN<Operator> oper)
{
	oper->longName->Release();
	oper->shortName->Release();
	MemFreeNN(oper);
}

IO::GSMModemController::GSMModemController(NN<IO::ATCommandChannel> channel, Bool needRelease) : IO::ModemController(channel, needRelease)
{
	this->smsFormat = -1;
}

IO::GSMModemController::~GSMModemController()
{
}

UnsafeArrayOpt<UTF8Char> IO::GSMModemController::GSMGetManufacturer(UnsafeArray<UTF8Char> manu)
{
	return this->SendStringCommand(manu, UTF8STRC("AT+CGMI"), 3000);
}

UnsafeArrayOpt<UTF8Char> IO::GSMModemController::GSMGetModelIdent(UnsafeArray<UTF8Char> model)
{
	return this->SendStringCommand(model, UTF8STRC("AT+CGMM"), 3000);
}

UnsafeArrayOpt<UTF8Char> IO::GSMModemController::GSMGetModemVer(UnsafeArray<UTF8Char> ver)
{
	return this->SendStringCommand(ver, UTF8STRC("AT+CGMR"), 3000);
}

UnsafeArrayOpt<UTF8Char> IO::GSMModemController::GSMGetIMEI(UnsafeArray<UTF8Char> imei)
{
	return this->SendStringCommand(imei, UTF8STRC("AT+CGSN"), 3000);
}

UnsafeArrayOpt<UTF8Char> IO::GSMModemController::GSMGetTECharset(UnsafeArray<UTF8Char> cs)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	if (this->SendStringCommand(sbuff, UTF8STRC("AT+CSCS?"), 3000).SetTo(sptr) && Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("+CSCS: ")))
	{
		if (sbuff[7] == '"' && sptr[-1] == '"')
		{
			return Text::StrConcatC(cs, &sbuff[8], (UOSInt)(sptr - &sbuff[9]));
		}
		else
		{
			return Text::StrConcatC(cs, &sbuff[7], (UOSInt)(sptr - &sbuff[7]));
		}
	}
	return 0;
}

Bool IO::GSMModemController::GSMSetTECharset(const UnsafeArray<UTF8Char> cs)
{
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr = Text::StrConcatC(sbuff, UTF8STRC("AT+CSCS="));
	sptr = Text::StrConcat(sptr, cs);
	return this->SendBoolCommandC(sbuff, (UOSInt)(sptr - sbuff));
}

Bool IO::GSMModemController::GSMGetTECharsetsSupported(NN<Data::ArrayListStringNN> csList)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr2;
	UnsafeArray<UTF8Char> sptr;
	if (this->SendStringCommand(sbuff, UTF8STRC("AT+CSCS=?"), 3000).SetTo(sptr) && Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("+CSCS: ")))
	{
		sptr2 = &sbuff[7];
		if (sptr2[0] == '(' && sptr[-1] == ')')
		{
			sptr2++;
			sptr--;
			*sptr = 0;
		}
		Text::PString sarr[2];
		UOSInt sarrCnt;
		sarr[1] = Text::PString(sptr2, (UOSInt)(sptr - sptr2));
		while (true)
		{
			sarrCnt = Text::StrSplitP(sarr, 2, sarr[1], ',');
			if (sarr[0].v[0] == '"' && sarr[0].v[sarr[0].leng - 1] == '"')
			{
				sarr[0] = sarr[0].Substring(1);
				sarr[0].RemoveChars(1);
			}
			csList->Add(Text::String::New(sarr[0].ToCString()));
			if (sarrCnt != 2)
				break;
		}
		return true;
	}
	return false;
}

UnsafeArrayOpt<UTF8Char> IO::GSMModemController::GSMGetIMSI(UnsafeArray<UTF8Char> imsi)
{
	return this->SendStringCommand(imsi, UTF8STRC("AT+CIMI"), 3000);
}

UnsafeArrayOpt<UTF8Char> IO::GSMModemController::GSMGetCurrOperator(UnsafeArray<UTF8Char> oper)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sbuffs[5];
	UnsafeArray<UTF8Char> sptr;
	this->SendStringCommand(sbuff, UTF8STRC("AT+COPS=3,0"), 3000);
	if (!this->SendStringCommand(sbuff, UTF8STRC("AT+COPS?"), 3000).SetTo(sptr))
		return 0;
	if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("+COPS: ")))
	{
		if (Text::StrCSVSplit(sbuffs, 5, &sbuff[7]) == 4)
		{
			return Text::StrConcat(oper, sbuffs[2]);
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

UnsafeArrayOpt<UTF8Char> IO::GSMModemController::GSMGetCurrPLMN(UnsafeArray<UTF8Char> plmn)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sbuffs[5];
	UnsafeArray<UTF8Char> sptr;
	this->SendStringCommand(sbuff, UTF8STRC("AT+COPS=3,2"), 3000);
	if (!this->SendStringCommand(sbuff, UTF8STRC("AT+COPS?"), 3000).SetTo(sptr))
		return 0;
	if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("+COPS: ")))
	{
		if (Text::StrCSVSplit(sbuffs, 5, &sbuff[7]) == 4)
		{
			return Text::StrConcat(plmn, sbuffs[2]);
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

Bool IO::GSMModemController::GSMConnectPLMN(Int32 plmn)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr = Text::StrConcatC(sbuff, UTF8STRC("AT+COPS=1,2,\""));
	sptr = Text::StrInt32(sptr, plmn);
	*sptr++ = '"';
	*sptr = 0;
	return this->SendBoolCommandC(sbuff, (UOSInt)(sptr - sbuff), 300000);
}

Bool IO::GSMModemController::GSMGetAllowedOperators(NN<Data::ArrayListNN<Operator>> operList)
{
	UTF8Char sbuff[256];
	UTF8Char lbuff[256];
	UTF8Char tmpBuff[256];
	UOSInt sbuffLen = 0;
	UOSInt lbuffLen = 0;
	UnsafeArray<const UTF8Char> sptr;
	UTF8Char c;
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, UTF8STRC("AT+COPS=?"), 60000);
	UOSInt i;
	UOSInt j = this->cmdResults.GetCount();
	UOSInt k = 0;
	UOSInt l = 0;
	Bool operStarted;
	Bool quoteStarted;
	Bool operExist;
	Bool operError = false;
	NN<Text::String> val;
	if (j > 1 && this->cmdResults.GetItem(j - 1).SetTo(val))
	{
		if (val->Equals(UTF8STRC("OK")))
		{
			Int32 status = 0;
			Int32 plmn = 0;
			NN<Operator> newOper;
			j -= 1;
			i = 1;
			while (i < j)
			{
				if (this->cmdResults.GetItem(i).SetTo(val) && val->StartsWith(UTF8STRC("+COPS: ")))
				{
					sptr = &val->v[7];
					operStarted = false;
					quoteStarted = false;
					operExist = false;
					while ((c = *sptr) != 0)
					{
						if (operStarted)
						{
							if (quoteStarted)
							{
								if (c == '\"')
									quoteStarted = false;
								else
								{
									if (k == 1)
									{
										lbuff[l++] = (UTF8Char)c;
									}
									else if (k == 2)
									{
										sbuff[l++] = (UTF8Char)c;
									}
									else
									{
										tmpBuff[l++] = (UTF8Char)c;
									}
								}
							}
							else
							{
								if (c == ')')
								{
									if (k >= 3 && !operError)
									{
										if (k == 3)
										{
											plmn = Text::StrToInt32(tmpBuff);
										}
										tmpBuff[l] = 0;
										newOper = MemAllocNN(Operator);
										newOper->status = (OperStatus)status;
										newOper->plmn = plmn;
										newOper->longName = Text::String::New(lbuff, lbuffLen);
										newOper->shortName = Text::String::New(sbuff, sbuffLen);
										if (k == 4)
										{
											newOper->netact = Text::StrToInt32(tmpBuff);
										}
										else
										{
											newOper->netact = 0;
										}
										operList->Add(newOper);
										operStarted = false;
									}
									else
									{
										operError = true;
										operStarted = false;
									}
								}
								else if (c == ',')
								{
									if (k == 0)
									{
										if (l == 0)
											operError = true;
										else
										{
											tmpBuff[l] = 0;
											status = Text::StrToInt32(tmpBuff);
										}
										k = 1;
										l = 0;
									}
									else if (k == 1)
									{
										lbuff[l] = 0;
										lbuffLen = 0;
										k = 2;
										l = 0;
									}
									else if (k == 2)
									{
										sbuff[l] = 0;
										sbuffLen = l;
										k = 3;
										l = 0;
									}
									else
									{
										if (k == 3)
										{
											if (l == 0)
												operError = true;
											else
											{
												tmpBuff[l] = 0;
												plmn = Text::StrToInt32(tmpBuff);
											}
										}
										k++;
										l = 0;
									}
								}
								else if (c == '\"')
								{
									quoteStarted = true;
								}
								else
								{
									if (!operError)
									{
										if (k == 0)
										{
											tmpBuff[l++] = (UTF8Char)c;
										}
										else if (k == 1)
										{
											lbuff[l++] = (UTF8Char)c;
										}
										else if (k == 2)
										{
											sbuff[l++] = (UTF8Char)c;
										}
										else
										{
											tmpBuff[l++] = (UTF8Char)c;
										}
									}
								}
							}
						}
						else
						{
							if (c == '(')
							{
								operStarted = true;
								operError = false;
								operExist = true;
								k = 0;
								l = 0;
							}
							else if (c == ',')
							{
								if (!operExist)
								{
									break;
								}
								operExist = false;
							}
						}
						sptr++;
					}
					break;
				}
				i++;
			}
			
			ClearCmdResult();
			mutUsage.EndUse();
			return true;
		}
		else
		{
			ClearCmdResult();
			mutUsage.EndUse();
			return false;
		}
	}
	else
	{
		ClearCmdResult();
		mutUsage.EndUse();
		return false;
	}
}

void IO::GSMModemController::GSMFreeOperators(NN<Data::ArrayListNN<Operator>> operList)
{
	operList->FreeAll(FreeOperator);
}

Int32 IO::GSMModemController::GSMSearchOperatorPLMN(Int32 netact)
{
	Data::ArrayListNN<IO::GSMModemController::Operator> operList;
	NN<IO::GSMModemController::Operator> oper;
	Int32 plmn = 0;
	UOSInt i;
	UOSInt j;
	this->GSMGetAllowedOperators(operList);
	i = 0;
	j = operList.GetCount();
	while (i < j)
	{
		oper = operList.GetItemNoCheck(i);
		if (oper->status == IO::GSMModemController::OSTAT_AVAILABLE || oper->status == IO::GSMModemController::OSTAT_CURRENT)
		{
			plmn = oper->plmn;
			break;
		}
		i++;
	}
	this->GSMFreeOperators(operList);
	return plmn;
}

IO::GSMModemController::SIMStatus IO::GSMModemController::GSMGetSIMStatus()
{
	IO::GSMModemController::SIMStatus sims = IO::GSMModemController::SIMS_UNKNOWN;

	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, UTF8STRC("AT+CPIN?"), 3000);
	UOSInt i = this->cmdResults.GetCount();
	NN<Text::String> val;
	while (i-- > 0)
	{
		if (this->cmdResults.GetItem(i).SetTo(val))
		{
			if (val->Equals(UTF8STRC("+CPIN: READY")))
			{
				sims = IO::GSMModemController::SIMS_READY;
				break;
			}
			else if (val->Equals(UTF8STRC("+CME ERROR: SIM not inserted")))
			{
				sims = IO::GSMModemController::SIMS_ABSENT;
				break;
			}
			else if (val->StartsWith(UTF8STRC("+CME ERROR: ")))
			{
				break;
			}
		}
	}
	ClearCmdResult();
	mutUsage.EndUse();
	return sims;
}

Bool IO::GSMModemController::GSMGetSignalQuality(OutParam<RSSI> rssi, OutParam<BER> ber)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sbuffs[3];
	UnsafeArray<UTF8Char> sptr;
	if (!this->SendStringCommand(sbuff, UTF8STRC("AT+CSQ"), 3000).SetTo(sptr))
		return false;
	if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("+CSQ: ")))
	{
		if (Text::StrSplitTrim(sbuffs, 3, &sbuff[6], ',') == 2)
		{
			rssi.Set((RSSI)Text::StrToInt32(sbuffs[0]));
			ber.Set((BER)Text::StrToInt32(sbuffs[1]));
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

Bool IO::GSMModemController::GSMSetFunctionalityMin()
{
	return this->SendBoolCommandC(UTF8STRC("AT+CFUN=0"));
}

Bool IO::GSMModemController::GSMSetFunctionalityFull()
{
	return this->SendBoolCommandC(UTF8STRC("AT+CFUN=1"));
}

Bool IO::GSMModemController::GSMSetFunctionalityReset()
{
	return this->SendBoolCommandC(UTF8STRC("AT+CFUN=6"));
}

Bool IO::GSMModemController::GSMGetModemTime(NN<Data::DateTime> date)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	if (!this->SendStringCommand(sbuff, UTF8STRC("AT+CCLK?"), 3000).SetTo(sptr))
		return false;
	if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("+CCLK: \"")))
	{
		UOSInt i = Text::StrIndexOfChar(&sbuff[8], '\"');
		if (i != INVALID_INDEX)
		{
			sbuff[8 + i] = 0;
			sbuff[6] = '2';
			sbuff[7] = '0';
			if (sbuff[16] == ',')
			{
				sbuff[16] = ' ';
			}
			UInt8 tzSign = sbuff[25];
			sbuff[25] = 0;
			sbuff[28] = 0;
			date->SetValue(Text::CStringNN(&sbuff[6], 25 - 6));
			if (date->GetYear() >= 2080)
			{
				date->SetYear((UInt16)(date->GetYear() - 100));
			}
			if (tzSign == '-')
			{
				date->SetTimeZoneQHR((Int8)(-Text::StrToInt32(&sbuff[26])));
			}
			else
			{
				date->SetTimeZoneQHR((Int8)Text::StrToInt32(&sbuff[26]));
			}
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

Bool IO::GSMModemController::GSMSetModemTime(NN<Data::DateTime> date)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr = Text::StrConcatC(sbuff, UTF8STRC("AT+CCLK=\""));
	sptr = date->ToString(sptr, "yy/MM/dd,HH:mm:ss");
	Int32 tz = date->GetTimeZoneQHR();
	if (tz < 0)
	{
		*sptr++ = '-';
		tz = -tz;
	}
	else
	{
		*sptr++ = '+';
	}
	if (tz < 10)
	{
		*sptr++ = '0';
	}
	sptr = Text::StrInt32(sptr, tz);
	*sptr++ = '"';
	*sptr = 0;	
	return SendBoolCommandC(sbuff, (UOSInt)(sptr - sbuff));
}

Bool IO::GSMModemController::GSMGetRegisterNetwork(OutParam<NetworkResult> n, OutParam<RegisterStatus> stat, OutParam<UInt16> lac, OutParam<UInt32> ci, OutParam<AccessTech> act)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	if (!this->SendStringCommand(sbuff, UTF8STRC("AT+CREG?"), 3000).SetTo(sptr))
		return false;
	if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("+CREG: ")))
	{
		Text::PString sarr[5];
		UOSInt sarrCnt = Text::StrSplitP(sarr, 5, Text::PString(&sbuff[7], (UOSInt)(sptr - &sbuff[7])), ',');
		if (sarrCnt < 2)
			return false;
		NetworkResult nres = (NetworkResult)sarr[0].ToInt32();
		n.Set(nres);
		stat.Set((RegisterStatus)sarr[1].ToInt32());
		if (nres == NetworkResult::Enable_w_Location)
		{
			if (sarrCnt != 5)
			{
				return false;
			}
			if (sarr[2].v[0] == '"' && sarr[2].EndsWith('"'))
			{
				sarr[2].RemoveChars(1);
				if (!sarr[2].Substring(1).Hex2UInt16(lac))
					return false;
			}
			else
			{
				if (!sarr[2].Hex2UInt16(lac))
					return false;
			}
			if (sarr[3].v[0] == '"' && sarr[3].EndsWith('"'))
			{
				sarr[3].RemoveChars(1);
				if (!sarr[3].Substring(1).Hex2UInt32(ci))
					return false;
			}
			else
			{
				if (!sarr[3].Hex2UInt32(ci))
					return false;
			}
			act.Set((AccessTech)sarr[4].ToInt32());
		}
		else
		{
			lac.Set(0);
			ci.Set(0);
			act.Set(AccessTech::GSM);
		}

		//////////////////////////////////
		return true;
	}
	else
	{
		return false;
	}	
}

Int32 IO::GSMModemController::GSMGetSIMPLMN()
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	if (!this->GSMGetIMSI(sbuff).SetTo(sptr))
	{
		return 0;
	}
	Int32 digitCnt = 5;
	if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("405")))
	{
		/////////////////////////
		digitCnt = 6;
	}
	else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("50215")))
	{
		digitCnt = 6;
	}
	else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("3")))
	{
		/////////////////////////
		digitCnt = 6;
	}
	else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("722")))
	{
		digitCnt = 6;
	}
	else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("732")))
	{
		digitCnt = 6;
	}
	sbuff[digitCnt] = 0;
	return Text::StrToInt32(sbuff);
}

Bool IO::GSMModemController::GPRSEPSReg()
{
	return this->SendBoolCommandC(UTF8STRC("AT+CEREG=1"));
}

Bool IO::GSMModemController::GPRSNetworkReg()
{
	return this->SendBoolCommandC(UTF8STRC("AT+CGREG=1"));
}

Bool IO::GSMModemController::GPRSServiceIsAttached(OutParam<Bool> attached)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	if (!this->SendStringCommand(sbuff, UTF8STRC("AT+CGATT?"), 3000).SetTo(sptr) || !Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("+CGATT: ")))
	{
		return false;
	}
	attached.Set(Text::StrToInt32(&sbuff[8]) != 0);
	return true;
}

Bool IO::GSMModemController::GPRSServiceSetAttached(Bool attached)
{
	if (attached)
	{
		return SendBoolCommandC(UTF8STRC("AT+CGATT=1"), 300000);
	}
	else
	{
		return SendBoolCommandC(UTF8STRC("AT+CGATT=0"), 300000);
	}
}

Bool IO::GSMModemController::GPRSSetAPN(Text::CStringNN apn)
{
	return GPRSSetPDPContext(1, CSTR("IP"), apn);
}

Bool IO::GSMModemController::GPRSSetPDPContext(UInt32 cid, Text::CStringNN type, Text::CStringNN apn)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr = Text::StrConcatC(sbuff, UTF8STRC("AT+CGDCONT="));
	sptr = Text::StrUInt32(sptr, cid);
	sptr = Text::StrConcatC(sptr, UTF8STRC(",\""));
	sptr = type.ConcatTo(sptr);
	sptr = Text::StrConcatC(sptr, UTF8STRC("\",\""));
	sptr = apn.ConcatTo(sptr);
	*sptr++ = '"';
	*sptr = 0;
	return SendBoolCommandC(sbuff, (UOSInt)(sptr - sbuff));
}

Bool IO::GSMModemController::GPRSGetPDPContext(NN<Data::ArrayListNN<PDPContext>> ctxList)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Text::PString sarr[5];
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, UTF8STRC("AT+CGDCONT?"), 3000);
	UOSInt i;
	UOSInt j = this->cmdResults.GetCount();
	NN<Text::String> val;
	NN<PDPContext> ctx;

	if (j > 1)
	{
		if (this->cmdResults.GetItem(j - 1).SetTo(val) && val->Equals(UTF8STRC("OK")))
		{
			j -= 1;
			i = 0;
			while (i < j)
			{
				if (this->cmdResults.GetItem(i).SetTo(val) && val->StartsWith(UTF8STRC("+CGDCONT: ")))
				{
					sptr = Text::StrConcatC(sbuff, &val->v[10], val->leng - 10);
					if (Text::StrSplitP(sarr, 4, Text::PString(sbuff, (UOSInt)(sptr - sbuff)), ',') >= 3)
					{
						ctx = MemAllocNN(PDPContext);
						ctx->cid = sarr[0].ToUInt32();
						if (sarr[1].v[0] == '"' && sarr[1].EndsWith('"'))
						{
							sarr[1].RemoveChars(1);
							ctx->type = Text::String::New(sarr[1].Substring(1).ToCString());
						}
						else
						{
							ctx->type = Text::String::New(sarr[1].ToCString());
						}
						if (sarr[2].v[0] == '"' && sarr[2].EndsWith('"'))
						{
							sarr[2].RemoveChars(1);
							ctx->apn = Text::String::New(sarr[2].Substring(1).ToCString());
						}
						else
						{
							ctx->apn = Text::String::New(sarr[2].ToCString());
						}
						ctxList->Add(ctx);
					}
				}
				i++;
			}
			ClearCmdResult();
			return true;
		}
		else
		{
			ClearCmdResult();
			return false;
		}
	}
	else
	{
		ClearCmdResult();
		return false;
	}
}

void IO::GSMModemController::GPRSFreePDPContext(NN<Data::ArrayListNN<PDPContext>> ctxList)
{
	NN<PDPContext> ctx;
	UOSInt i = ctxList->GetCount();
	while (i-- > 0)
	{
		ctx = ctxList->GetItemNoCheck(i);
		ctx->type->Release();
		ctx->apn->Release();
		MemFreeNN(ctx);
	}
	ctxList->Clear();
}

Bool IO::GSMModemController::GPRSSetPDPActive(Bool active)
{
	if (active)
	{
		return SendBoolCommandC(UTF8STRC("AT+CGACT=1"), 3000);
	}
	else
	{
		return SendBoolCommandC(UTF8STRC("AT+CGACT=0"), 3000);
	}
}

Bool IO::GSMModemController::GPRSSetPDPActive(Bool active, UInt32 cid)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	if (active)
	{
		sptr = Text::StrConcatC(sbuff, UTF8STRC("AT+CGACT=1,"));
	}
	else
	{
		sptr = Text::StrConcatC(sbuff, UTF8STRC("AT+CGACT=1,"));
	}
	sptr = Text::StrUInt32(sptr, cid);
	return SendBoolCommandC(sbuff, (UOSInt)(sptr - sbuff), 3000);
}

Bool IO::GSMModemController::GPRSGetPDPActive(NN<Data::ArrayList<ActiveState>> actList)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Text::PString sarr[5];
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, UTF8STRC("AT+CGACT?"), 3000);
	UOSInt i;
	UOSInt j = this->cmdResults.GetCount();
	NN<Text::String> val;
	ActiveState act;

	if (j > 1)
	{
		if (this->cmdResults.GetItem(j - 1).SetTo(val) && val->Equals(UTF8STRC("OK")))
		{
			j -= 1;
			i = 0;
			while (i < j)
			{
				if (this->cmdResults.GetItem(i).SetTo(val) && val->StartsWith(UTF8STRC("+CGACT: ")))
				{
					sptr = Text::StrConcatC(sbuff, &val->v[8], val->leng - 8);
					if (Text::StrSplitP(sarr, 3, Text::PString(sbuff, (UOSInt)(sptr - sbuff)), ',') >= 2)
					{
						act.cid = sarr[0].ToUInt32();
						act.active = sarr[1].ToUInt32() != 0;
						actList->Add(act);
					}
				}
				i++;
			}
			ClearCmdResult();
			return true;
		}
		else
		{
			ClearCmdResult();
			return false;
		}
	}
	else
	{
		ClearCmdResult();
		return false;
	}
}

Bool IO::GSMModemController::SMSListMessages(NN<Data::ArrayListNN<IO::GSMModemController::SMSMessage>> msgList, IO::GSMModemController::SMSStatus status)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sbuffs[5];
	UOSInt strLen;
	if (this->smsFormat != 0)
	{
		this->SetSMSFormat(IO::GSMModemController::SMSF_PDU);
	}
	sptr = Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("AT+CMGL=")), (Int32)status);
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, sbuff, (UOSInt)(sptr - sbuff), 10000);
	UOSInt i;
	UOSInt j = this->cmdResults.GetCount();
	Int32 lastIndex = 0;
	NN<Text::String> val;
	NN<Text::String> val2;
	NN<IO::GSMModemController::SMSMessage> msg;

	if (j > 1)
	{
		if (this->cmdResults.GetItem(j - 1).SetTo(val) && val->Equals(UTF8STRC("OK")))
		{
			j -= 1;
			if (this->cmdResults.GetItem(0).SetTo(val2) && val2->StartsWith(UTF8STRC("+CMGL: ")))
			{
				i = 0;
			}
			else
			{
				i = 1;
			}
			while (i < j)
			{
				if (this->cmdResults.GetItem(i).SetTo(val) && this->cmdResults.GetItem(i + 1).SetTo(val2))
				{
					if (val->StartsWith(UTF8STRC("+CMGL: ")))
					{
						Text::StrConcatC(sbuff, &val->v[7], val->leng - 7);
						if (Text::StrSplit(sbuffs, 5, sbuff, ',') == 4)
						{
							strLen = val2->leng;
							msg = MemAllocNN(IO::GSMModemController::SMSMessage);
							lastIndex = msg->index = Text::StrToInt32(sbuffs[0]);
							msg->status = (SMSStatus)Text::StrToInt32(sbuffs[1]);
							msg->pduLeng = (strLen >> 1);
							msg->pduMessage = MemAllocArr(UInt8, strLen >> 1);
							Text::StrHex2Bytes(val2->v, msg->pduMessage);
							msgList->Add(msg);
						}
					}
					else
					{
						if (Text::StrIndexOfChar(val->v, ',') == INVALID_INDEX)
						{
							strLen = val->leng;
							lastIndex++;
							msg = MemAllocNN(IO::GSMModemController::SMSMessage);
							msg->index = lastIndex + 1;
							msg->status = (SMSStatus)0;
							msg->pduLeng = (strLen >> 1);
							msg->pduMessage = MemAllocArr(UInt8, strLen >> 1);
							Text::StrHex2Bytes(val->v, msg->pduMessage);
							msgList->Add(msg);
						}
					}
				}
				///////////////////////////////////////
				i += 2;
			}
			ClearCmdResult();
			mutUsage.EndUse();
			return true;
		}
		else
		{
			ClearCmdResult();
			mutUsage.EndUse();
			return false;
		}
	}
	else
	{
		ClearCmdResult();
		mutUsage.EndUse();
		return false;
	}
}

void IO::GSMModemController::SMSFreeMessages(NN<Data::ArrayListNN<SMSMessage>> msgList)
{
	msgList->FreeAll(SMSFreeMessage);
}

void __stdcall IO::GSMModemController::SMSFreeMessage(NN<SMSMessage> msg)
{
	MemFreeArr(msg->pduMessage);
	MemFreeNN(msg);
}

Bool IO::GSMModemController::SMSDeleteMessage(Int32 index)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("AT+CMGD=")), index);
	return this->SendBoolCommandC(sbuff, (UOSInt)(sptr - sbuff));
}

Bool IO::GSMModemController::SMSSendMessage(NN<Text::SMSMessage> msg)
{
	UTF8Char sbuff1[32];
	UnsafeArray<UTF8Char> sptr1;
	UTF8Char sbuff2[512];
	UInt8 dataBuff[256];
	Bool isSucc;
	UOSInt buffSize = msg->ToSubmitPDU(dataBuff);
	Text::StrHexBytes(sbuff2, dataBuff, buffSize, 0);
	sptr1 = Text::StrInt32(Text::StrConcatC(sbuff1, UTF8STRC("AT+CMGS=")), (Int32)(buffSize - 1));

	if (this->smsFormat != 0)
	{
		this->SetSMSFormat(IO::GSMModemController::SMSF_PDU);
	}
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommands(this->cmdResults, sbuff1, (UOSInt)(sptr1 - sbuff1), sbuff2, 10000);
	isSucc = this->IsCmdSucceed();
	ClearCmdResult();
	mutUsage.EndUse();
	return isSucc;
}

Bool IO::GSMModemController::SMSSetStorage(SMSStorage reading, SMSStorage writing, SMSStorage store)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;

	sptr = Text::StrConcatC(sbuff, UTF8STRC("AT+CPMS="));
	if (reading == IO::GSMModemController::SMSSTORE_SIM)
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("\"SM\""));
	}
	else if (reading == IO::GSMModemController::SMSSTORE_FLASH)
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("\"ME\""));
	}
	else if (reading == IO::GSMModemController::SMSSTORE_STATUSREPORT)
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("\"SR\""));
	}
	else if (reading == IO::GSMModemController::SMSSTORE_CBMMESSAGE)
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC("\"BM\""));
	}

	if (store != IO::GSMModemController::SMSSTORE_UNCHANGE || writing != IO::GSMModemController::SMSSTORE_UNCHANGE)
	{
		sptr = Text::StrConcatC(sptr, UTF8STRC(","));
		if (writing == IO::GSMModemController::SMSSTORE_SIM)
		{
			sptr = Text::StrConcatC(sptr, UTF8STRC("\"SM\""));
		}
		else if (writing == IO::GSMModemController::SMSSTORE_FLASH)
		{
			sptr = Text::StrConcatC(sptr, UTF8STRC("\"ME\""));
		}
		else if (writing == IO::GSMModemController::SMSSTORE_STATUSREPORT)
		{
			sptr = Text::StrConcatC(sptr, UTF8STRC("\"SR\""));
		}
		else if (writing == IO::GSMModemController::SMSSTORE_CBMMESSAGE)
		{
			sptr = Text::StrConcatC(sptr, UTF8STRC("\"BM\""));
		}
		if (store != IO::GSMModemController::SMSSTORE_UNCHANGE)
		{
			sptr = Text::StrConcatC(sptr, UTF8STRC(","));
			if (store == IO::GSMModemController::SMSSTORE_SIM)
			{
				sptr = Text::StrConcatC(sptr, UTF8STRC("\"SM\""));
			}
			else if (store == IO::GSMModemController::SMSSTORE_FLASH)
			{
				sptr = Text::StrConcatC(sptr, UTF8STRC("\"ME\""));
			}
			else if (store == IO::GSMModemController::SMSSTORE_STATUSREPORT)
			{
				sptr = Text::StrConcatC(sptr, UTF8STRC("\"SR\""));
			}
			else if (store == IO::GSMModemController::SMSSTORE_CBMMESSAGE)
			{
				sptr = Text::StrConcatC(sptr, UTF8STRC("\"BM\""));
			}
		}
	}
	return this->SendBoolCommandC(sbuff, (UOSInt)(sptr - sbuff));
}

Bool IO::GSMModemController::SMSGetStorageInfo(Optional<SMSStorageInfo> reading, Optional<SMSStorageInfo> writing, Optional<SMSStorageInfo> store)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Text::PString buffs[10];
	if (!this->SendStringCommand(sbuff, UTF8STRC("AT+CPMS?"), 3000).SetTo(sptr))
		return false;
	if (!Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("+CPMS: ")))
		return false;
	if (Text::StrSplitP(buffs, 10, {&sbuff[7], (UOSInt)(sptr - &sbuff[7])}, ',') != 9)
		return false;
	NN<SMSStorageInfo> info;
	if (reading.SetTo(info))
	{
		if (Text::StrEqualsC(buffs[0].v, buffs[0].leng, UTF8STRC("\"SM\"")))
			info->storage = IO::GSMModemController::SMSSTORE_SIM;
		else if (Text::StrEqualsC(buffs[0].v, buffs[0].leng, UTF8STRC("\"ME\"")))
			info->storage = IO::GSMModemController::SMSSTORE_FLASH;
		else if (Text::StrEqualsC(buffs[0].v, buffs[0].leng, UTF8STRC("\"SR\"")))
			info->storage = IO::GSMModemController::SMSSTORE_STATUSREPORT;
		else if (Text::StrEqualsC(buffs[0].v, buffs[0].leng, UTF8STRC("\"BM\"")))
			info->storage = IO::GSMModemController::SMSSTORE_CBMMESSAGE;
		else
			info->storage = IO::GSMModemController::SMSSTORE_UNCHANGE;
		info->used = Text::StrToInt32(buffs[1].v);
		info->available = Text::StrToInt32(buffs[2].v);
	}
	if (writing.SetTo(info))
	{
		if (Text::StrEqualsC(buffs[3].v, buffs[3].leng, UTF8STRC("\"SM\"")))
			info->storage = IO::GSMModemController::SMSSTORE_SIM;
		else if (Text::StrEqualsC(buffs[3].v, buffs[3].leng, UTF8STRC("\"ME\"")))
			info->storage = IO::GSMModemController::SMSSTORE_FLASH;
		else if (Text::StrEqualsC(buffs[3].v, buffs[3].leng, UTF8STRC("\"SR\"")))
			info->storage = IO::GSMModemController::SMSSTORE_STATUSREPORT;
		else if (Text::StrEqualsC(buffs[3].v, buffs[3].leng, UTF8STRC("\"BM\"")))
			info->storage = IO::GSMModemController::SMSSTORE_CBMMESSAGE;
		else
			info->storage = IO::GSMModemController::SMSSTORE_UNCHANGE;
		info->used = Text::StrToInt32(buffs[4].v);
		info->available = Text::StrToInt32(buffs[5].v);
	}
	if (store.SetTo(info))
	{
		if (Text::StrEqualsC(buffs[6].v, buffs[6].leng, UTF8STRC("\"SM\"")))
			info->storage = IO::GSMModemController::SMSSTORE_SIM;
		else if (Text::StrEqualsC(buffs[6].v, buffs[6].leng, UTF8STRC("\"ME\"")))
			info->storage = IO::GSMModemController::SMSSTORE_FLASH;
		else if (Text::StrEqualsC(buffs[6].v, buffs[6].leng, UTF8STRC("\"SR\"")))
			info->storage = IO::GSMModemController::SMSSTORE_STATUSREPORT;
		else if (Text::StrEqualsC(buffs[6].v, buffs[6].leng, UTF8STRC("\"BM\"")))
			info->storage = IO::GSMModemController::SMSSTORE_CBMMESSAGE;
		else
			info->storage = IO::GSMModemController::SMSSTORE_UNCHANGE;
		info->used = Text::StrToInt32(buffs[7].v);
		info->available = Text::StrToInt32(buffs[8].v);
	}
	return true;
}

UnsafeArrayOpt<UTF8Char> IO::GSMModemController::SMSGetSMSC(UnsafeArray<UTF8Char> buff)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sarr[3];
	UnsafeArrayOpt<UTF8Char> sptr = 0;
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, UTF8STRC("AT+CSCA?"), 3000);
	UOSInt i = this->cmdResults.GetCount();
	NN<Text::String> val;
	while (i-- > 0)
	{
		if (this->cmdResults.GetItem(i).SetTo(val) && val->StartsWith(UTF8STRC("+CSCA: ")))
		{
			Text::StrConcatC(sbuff, &val->v[7], val->leng - 7);
			Text::StrCSVSplit(sarr, 3, sbuff);
			sptr = Text::StrConcat(buff, sarr[0]);
			break;
		}
	}
	ClearCmdResult();
	mutUsage.EndUse();
	return sptr;
}

UnsafeArrayOpt<UTF8Char> IO::GSMModemController::PBGetCharset(UnsafeArray<UTF8Char> cs)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	if (this->SendStringCommand(sbuff, UTF8STRC("AT+WPCS?"), 3000).SetTo(sptr) && Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("+WPCS: ")))
	{
		return Text::StrConcatC(cs, &sbuff[7], (UOSInt)(sptr - &sbuff[7]));
	}
	return 0;
}


Bool IO::GSMModemController::PBSetCharset(UnsafeArray<const UTF8Char> cs, UOSInt csLen)
{
	UTF8Char sbuff[16];
	UnsafeArray<UTF8Char> sptr = Text::StrConcatC(sbuff, UTF8STRC("AT+WPCS="));
	sptr = Text::StrConcatC(sptr, cs, csLen);
	return this->SendBoolCommandC(sbuff, (UOSInt)(sptr - sbuff));
}

Bool IO::GSMModemController::PBSetStorage(PBStorage storage)
{
	Text::CStringNN cmd;
	switch (storage)
	{
	case PBSTORE_SIM:
		cmd = CSTR("AT+CPBS=\"SM\"");
		break;
	case PBSTORE_SIM_RESTRICTED:
		cmd = CSTR("AT+CPBS=\"FD\"");
		break;
	case PBSTORE_SIM_OWN_NUMBERS:
		cmd = CSTR("AT+CPBS=\"ON\"");
		break;
	case PBSTORE_EMERGENCY:
		cmd = CSTR("AT+CPBS=\"EN\"");
		break;
	case PBSTORE_LASTNUMDIAL:
		cmd = CSTR("AT+CPBS=\"LD\"");
		break;
	case PBSTORE_UNANSWERED:
		cmd = CSTR("AT+CPBS=\"MC\"");
		break;
	case PBSTORE_ME:
		cmd = CSTR("AT+CPBS=\"ME\"");
		break;
	case PBSTORE_ME_SIM:
		cmd = CSTR("AT+CPBS=\"MT\"");
		break;
	case PBSTORE_RECEIVED_CALL:
		cmd = CSTR("AT+CPBS=\"RC\"");
		break;
	case PBSTORE_SERVICE_DIALING_NUMBERS:
		cmd = CSTR("AT+CPBS=\"SN\"");
		break;
	case PBSTORE_UNKNOWN:
	default:
		return false;
	}
	return this->SendBoolCommandC(cmd.v, cmd.leng);
}

Bool IO::GSMModemController::PBGetStorage(OptOut<PBStorage> storage, OptOut<Int32> usedEntry, OptOut<Int32> freeEntry)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Text::PString buffs[4];
	if (!this->SendStringCommand(sbuff, UTF8STRC("AT+CPBS?"), 3000).SetTo(sptr))
		return false;
	if (!Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("+CPBS: ")))
		return false;
	if (Text::StrSplitP(buffs, 4, {&sbuff[7], (UOSInt)(sptr - &sbuff[7])}, ',') != 3)
		return false;

	if (storage.IsNotNull())
	{
		if (Text::StrEqualsC(buffs[0].v, buffs[0].leng, UTF8STRC("\"SM\"")))
			storage.SetNoCheck(IO::GSMModemController::PBSTORE_SIM);
		else if (Text::StrEqualsC(buffs[0].v, buffs[0].leng, UTF8STRC("\"FD\"")))
			storage.SetNoCheck(IO::GSMModemController::PBSTORE_SIM_RESTRICTED);
		else if (Text::StrEqualsC(buffs[0].v, buffs[0].leng, UTF8STRC("\"ON\"")))
			storage.SetNoCheck(IO::GSMModemController::PBSTORE_SIM_OWN_NUMBERS);
		else if (Text::StrEqualsC(buffs[0].v, buffs[0].leng, UTF8STRC("\"EN\"")))
			storage.SetNoCheck(IO::GSMModemController::PBSTORE_EMERGENCY);
		else if (Text::StrEqualsC(buffs[0].v, buffs[0].leng, UTF8STRC("\"LD\"")))
			storage.SetNoCheck(IO::GSMModemController::PBSTORE_LASTNUMDIAL);
		else if (Text::StrEqualsC(buffs[0].v, buffs[0].leng, UTF8STRC("\"MC\"")))
			storage.SetNoCheck(IO::GSMModemController::PBSTORE_UNANSWERED);
		else if (Text::StrEqualsC(buffs[0].v, buffs[0].leng, UTF8STRC("\"ME\"")))
			storage.SetNoCheck(IO::GSMModemController::PBSTORE_ME);
		else if (Text::StrEqualsC(buffs[0].v, buffs[0].leng, UTF8STRC("\"MT\"")))
			storage.SetNoCheck(IO::GSMModemController::PBSTORE_ME_SIM);
		else if (Text::StrEqualsC(buffs[0].v, buffs[0].leng, UTF8STRC("\"RC\"")))
			storage.SetNoCheck(IO::GSMModemController::PBSTORE_RECEIVED_CALL);
		else if (Text::StrEqualsC(buffs[0].v, buffs[0].leng, UTF8STRC("\"SN\"")))
			storage.SetNoCheck(IO::GSMModemController::PBSTORE_SERVICE_DIALING_NUMBERS);
		else
			storage.SetNoCheck(IO::GSMModemController::PBSTORE_UNKNOWN);
	}
	if (usedEntry.IsNotNull())
	{
		usedEntry.SetNoCheck(Text::StrToInt32(buffs[1].v));
	}
	if (freeEntry.IsNotNull())
	{
		freeEntry.SetNoCheck(Text::StrToInt32(buffs[2].v));
	}
	return true;
}

Bool IO::GSMModemController::PBGetStorageStatus(OptOut<Int32> startEntry, OptOut<Int32> endEntry, OptOut<Int32> maxNumberLen, OptOut<Int32> maxTextLen)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Text::PString sarr[4];
	Text::PString sarr2[3];
	if (this->SendStringCommand(sbuff, UTF8STRC("AT+CPBR=?"), 3000).SetTo(sptr) && Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("+CPBR: ")) && Text::StrSplitP(sarr, 4, {&sbuff[7], (UOSInt)(sptr - &sbuff[7])}, ',') == 3)
	{
		if (sarr[0].v[0] == '(' && sarr[0].EndsWith(')') && Text::StrSplitP(sarr2, 3, sarr[0].Substring(1), '-') == 2)
		{
			sarr2[1].v[sarr2[1].leng - 1] = 0;
			sarr2[1].leng -= 1;
			if (startEntry.IsNotNull())
			{
				startEntry.SetNoCheck(Text::StrToInt32(sarr2[0].v));
			}
			if (endEntry.IsNotNull())
			{
				endEntry.SetNoCheck(Text::StrToInt32(sarr2[1].v));
			}
		}
		if (maxNumberLen.IsNotNull())
		{
			maxNumberLen.SetNoCheck(Text::StrToInt32(sarr[1].v));
		}
		if (maxTextLen.IsNotNull())
		{
			maxTextLen.SetNoCheck(Text::StrToInt32(sarr[2].v));
		}
		return true;
	}
	else
	{
		return false;
	}
}

Bool IO::GSMModemController::PBReadEntries(NN<Data::ArrayListNN<PBEntry>> phoneList, Int32 startEntry, Int32 endEntry)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	Text::PString sbuffs[5];
	if (startEntry == endEntry)
	{
		sptr = Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("AT+CPBR=")), startEntry);
	}
	else
	{
		sptr = Text::StrInt32(Text::StrConcatC(Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("AT+CPBR=")), startEntry), UTF8STRC(",")), endEntry);
	}
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, sbuff, (UOSInt)(sptr - sbuff), 10000);
	UOSInt i;
	UOSInt j = this->cmdResults.GetCount();
	NN<Text::String> val;
	NN<IO::GSMModemController::PBEntry> ent;
	if (j > 1)
	{
		if (this->cmdResults.GetItem(j - 1).SetTo(val) && val->Equals(UTF8STRC("OK")))
		{
			j -= 1;
			i = 1;
			while (i < j)
			{
				if (this->cmdResults.GetItem(i).SetTo(val) && val->Equals(UTF8STRC("+CPBR: ")))
				{
					sptr = Text::StrConcatC(sbuff, &val->v[7], val->leng - 7);
					if (Text::StrCSVSplitP(sbuffs, 5, sbuff) >= 4)
					{
						ent = MemAllocNN(PBEntry);
						ent->number = Text::String::New(sbuffs[1].ToCString());
						ent->name = Text::String::New(sbuffs[3].ToCString());
						ent->index = Text::StrToInt32(sbuffs[0].v);
						phoneList->Add(ent);
					}
				}

				i++;
			}
			
			ClearCmdResult();
			mutUsage.EndUse();
			return true;
		}
		else
		{
			ClearCmdResult();
			mutUsage.EndUse();
			return false;
		}
	}
	else
	{
		ClearCmdResult();
		mutUsage.EndUse();
		return false;
	}
	return false;
}

Bool IO::GSMModemController::PBReadAllEntries(NN<Data::ArrayListNN<PBEntry>> phoneList)
{
	Int32 startEntry;
	Int32 endEntry;
	if (!PBGetStorageStatus(startEntry, endEntry, 0, 0))
		return false;
	return PBReadEntries(phoneList, startEntry, endEntry);
}

void __stdcall IO::GSMModemController::PBFreeEntry(NN<PBEntry> entry)
{
	entry->name->Release();
	entry->number->Release();
	MemFreeNN(entry);
}

void IO::GSMModemController::PBFreeEntries(NN<Data::ArrayListNN<PBEntry>> phoneList)
{
	phoneList->FreeAll(PBFreeEntry);
}

UnsafeArrayOpt<UTF8Char> IO::GSMModemController::GetICCID(UnsafeArray<UTF8Char> sbuff)
{
	return 0;
}

UOSInt IO::GSMModemController::QueryCells(NN<Data::ArrayListNN<CellSignal>> cells)
{
	return 0;
}

Int32 IO::GSMModemController::RSSIGetdBm(RSSI rssi)
{
	switch (rssi)
	{
	case RSSI_113:
		return -113;
	case RSSI_111:
		return -111;
	case RSSI_109:
		return -109;
	case RSSI_107:
		return -107;
	case RSSI_105:
		return -105;
	case RSSI_103:
		return -103;
	case RSSI_101:
		return -101;
	case RSSI_99:
		return -99;
	case RSSI_97:
		return -97;
	case RSSI_95:
		return -95;
	case RSSI_93:
		return -93;
	case RSSI_91:
		return -91;
	case RSSI_89:
		return -89;
	case RSSI_87:
		return -87;
	case RSSI_85:
		return -85;
	case RSSI_83:
		return -83;
	case RSSI_81:
		return -81;
	case RSSI_79:
		return -79;
	case RSSI_77:
		return -77;
	case RSSI_75:
		return -75;
	case RSSI_73:
		return -73;
	case RSSI_71:
		return -71;
	case RSSI_69:
		return -69;
	case RSSI_67:
		return -67;
	case RSSI_65:
		return -65;
	case RSSI_63:
		return -63;
	case RSSI_61:
		return -61;
	case RSSI_59:
		return -59;
	case RSSI_57:
		return -57;
	case RSSI_55:
		return -55;
	case RSSI_53:
		return -53;
	case RSSI_51:
		return -51;
	case RSSI_UNKNOWN:
	default:
		return 0;
	}
}

UnsafeArray<UTF8Char> IO::GSMModemController::RSSIGetName(UnsafeArray<UTF8Char> buff, RSSI rssi)
{
	switch (rssi)
	{
	case RSSI_113:
		return Text::StrConcatC(buff, UTF8STRC("-113 dBm"));
	case RSSI_111:
		return Text::StrConcatC(buff, UTF8STRC("-111 dBm"));
	case RSSI_109:
		return Text::StrConcatC(buff, UTF8STRC("-109 dBm"));
	case RSSI_107:
		return Text::StrConcatC(buff, UTF8STRC("-107 dBm"));
	case RSSI_105:
		return Text::StrConcatC(buff, UTF8STRC("-105 dBm"));
	case RSSI_103:
		return Text::StrConcatC(buff, UTF8STRC("-103 dBm"));
	case RSSI_101:
		return Text::StrConcatC(buff, UTF8STRC("-101 dBm"));
	case RSSI_99:
		return Text::StrConcatC(buff, UTF8STRC("-99 dBm"));
	case RSSI_97:
		return Text::StrConcatC(buff, UTF8STRC("-97 dBm"));
	case RSSI_95:
		return Text::StrConcatC(buff, UTF8STRC("-95 dBm"));
	case RSSI_93:
		return Text::StrConcatC(buff, UTF8STRC("-93 dBm"));
	case RSSI_91:
		return Text::StrConcatC(buff, UTF8STRC("-91 dBm"));
	case RSSI_89:
		return Text::StrConcatC(buff, UTF8STRC("-89 dBm"));
	case RSSI_87:
		return Text::StrConcatC(buff, UTF8STRC("-87 dBm"));
	case RSSI_85:
		return Text::StrConcatC(buff, UTF8STRC("-85 dBm"));
	case RSSI_83:
		return Text::StrConcatC(buff, UTF8STRC("-83 dBm"));
	case RSSI_81:
		return Text::StrConcatC(buff, UTF8STRC("-81 dBm"));
	case RSSI_79:
		return Text::StrConcatC(buff, UTF8STRC("-79 dBm"));
	case RSSI_77:
		return Text::StrConcatC(buff, UTF8STRC("-77 dBm"));
	case RSSI_75:
		return Text::StrConcatC(buff, UTF8STRC("-75 dBm"));
	case RSSI_73:
		return Text::StrConcatC(buff, UTF8STRC("-73 dBm"));
	case RSSI_71:
		return Text::StrConcatC(buff, UTF8STRC("-71 dBm"));
	case RSSI_69:
		return Text::StrConcatC(buff, UTF8STRC("-69 dBm"));
	case RSSI_67:
		return Text::StrConcatC(buff, UTF8STRC("-67 dBm"));
	case RSSI_65:
		return Text::StrConcatC(buff, UTF8STRC("-65 dBm"));
	case RSSI_63:
		return Text::StrConcatC(buff, UTF8STRC("-63 dBm"));
	case RSSI_61:
		return Text::StrConcatC(buff, UTF8STRC("-61 dBm"));
	case RSSI_59:
		return Text::StrConcatC(buff, UTF8STRC("-59 dBm"));
	case RSSI_57:
		return Text::StrConcatC(buff, UTF8STRC("-57 dBm"));
	case RSSI_55:
		return Text::StrConcatC(buff, UTF8STRC("-55 dBm"));
	case RSSI_53:
		return Text::StrConcatC(buff, UTF8STRC("-53 dBm"));
	case RSSI_51:
		return Text::StrConcatC(buff, UTF8STRC("-51 dBm"));
	case RSSI_UNKNOWN:
	default:
		return Text::StrConcatC(buff, UTF8STRC("Unknown"));
	}
}

UnsafeArray<UTF8Char> IO::GSMModemController::BERGetName(UnsafeArray<UTF8Char> buff, BER ber)
{
	switch (ber)
	{
	case BER_RXQUAL_0:
		return Text::StrConcatC(buff, UTF8STRC("< 0.2%"));
	case BER_RXQUAL_1:
		return Text::StrConcatC(buff, UTF8STRC("0.2% - 0.4%"));
	case BER_RXQUAL_2:
		return Text::StrConcatC(buff, UTF8STRC("0.4% - 0.8%"));
	case BER_RXQUAL_3:
		return Text::StrConcatC(buff, UTF8STRC("0.8% - 1.6%"));
	case BER_RXQUAL_4:
		return Text::StrConcatC(buff, UTF8STRC("1.6% - 3.2%"));
	case BER_RXQUAL_5:
		return Text::StrConcatC(buff, UTF8STRC("3.2% - 6.4%"));
	case BER_RXQUAL_6:
		return Text::StrConcatC(buff, UTF8STRC("6.4% - 12.8%"));
	case BER_RXQUAL_7:
		return Text::StrConcatC(buff, UTF8STRC("12.8%"));
	case BER_UNKNWON:
	default:
		return Text::StrConcatC(buff, UTF8STRC("Unknown"));
	}
}

Text::CStringNN IO::GSMModemController::OperStatusGetName(OperStatus operStatus)
{
	switch (operStatus)
	{
	case OSTAT_AVAILABLE:
		return CSTR("Available");
	case OSTAT_CURRENT:
		return CSTR("Current");
	case OSTAT_FORBIDDEN:
		return CSTR("Forbidden");
	case OSTAT_UNKNOWN:
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN IO::GSMModemController::SIMStatusGetName(SIMStatus simStatus)
{
	switch (simStatus)
	{
	case SIMS_READY:
		return CSTR("Ready");
	case SIMS_ABSENT:
		return CSTR("Absent");
	case SIMS_UNKNOWN:
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN IO::GSMModemController::NetworkResultGetName(NetworkResult n)
{
	switch (n)
	{
	case NetworkResult::Disable:
		return CSTR("Disable network registration");
	case NetworkResult::Enable:
		return CSTR("Enable network registration");
	case NetworkResult::Enable_w_Location:
		return CSTR("Enable network registration and location information");
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN IO::GSMModemController::RegisterStatusGetName(RegisterStatus stat)
{
	switch (stat)
	{
	case RegisterStatus::NotRegistered:
		return CSTR("Not registered, MS is not currently searching for a new operator to register with");
	case RegisterStatus::RegisteredHomeNetwork:
		return CSTR("Registered, home network");
	case RegisterStatus::NotRegisteredSearch:
		return CSTR("Not registered, but MS is currently searching for a new operator to register with");
	case RegisterStatus::RegistrationDenied:
		return CSTR("Registration denied");
	case RegisterStatus::Unknown:
		return CSTR("Unknown");
	case RegisterStatus::RegisteredRoaming:
		return CSTR("Registered, roaming");
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN IO::GSMModemController::AccessTechGetName(AccessTech act)
{
	switch (act)
	{
	case AccessTech::GSM:
		return CSTR("GSM");
	case AccessTech::GSMCompact:
		return CSTR("GSM Compact");
	case AccessTech::UTRAN:
		return CSTR("UTRAN");
	case AccessTech::GSM_w_EGPRS:
		return CSTR("GSM w/EGPRS");
	case AccessTech::UTRAN_w_HSDPA:
		return CSTR("UTRAN w/HSDPA");
	case AccessTech::UTRAN_w_HSUPA:
		return CSTR("UTRAN w/HSUPA");
	case AccessTech::UTRAN_w_HSDPA_HSUPA:
		return CSTR("UTRAN w/HSDPA and HSUPA");
	case AccessTech::EUTRAN:
		return CSTR("E-UTRAN");
	default:
		return CSTR("Unknown");
	}
}

Text::CStringNN IO::GSMModemController::SysModeGetName(SysMode sysMode)
{
	switch (sysMode)
	{
	default:
	case SysMode::NoService:
		return CSTR("NoService");
	case SysMode::GSM:
		return CSTR("GSM");
	case SysMode::WCDMA:
		return CSTR("WCDMA");
	case SysMode::TD_SCDMA:
		return CSTR("TD-SCDMA");
	case SysMode::LTE:
		return CSTR("LTE");
	case SysMode::CDMA:
		return CSTR("CDMA");
	case SysMode::EVDO:
		return CSTR("EVDO");
	case SysMode::CDMA_EVDO:
		return CSTR("CDMA-EVDO");
	case SysMode::WIMAX:
		return CSTR("WiMAX");
	}
}
