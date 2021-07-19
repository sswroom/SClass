#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/GSMModemController.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"

Bool IO::GSMModemController::SetSMSFormat(IO::GSMModemController::SMSFormat smsFormat)
{
	if (smsFormat == IO::GSMModemController::SMSF_PDU)
	{
		Bool succ = this->SendBoolCommand("AT+CMGF=0");
		if (succ)
			this->smsFormat = 0;
		return succ;
	}
	else if (smsFormat == IO::GSMModemController::SMSF_TEXT)
	{
		Bool succ = this->SendBoolCommand("AT+CMGF=1");
		if (succ)
			this->smsFormat = 1;
		return succ;
	}
	return false;
}

Bool IO::GSMModemController::GetSMSFormat(SMSFormat *smsFormat)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr = SendStringCommand(sbuff, "AT+CMGF?", 3000);
	if (sptr == 0)
		return false;
	if (Text::StrCompare(sbuff, (const UTF8Char*)"+CMGF: 1") == 0)
	{
		*smsFormat = IO::GSMModemController::SMSF_TEXT;
		return true;
	}
	else if (Text::StrCompare(sbuff, (const UTF8Char*)"+CMGF: 0") == 0)
	{
		*smsFormat = IO::GSMModemController::SMSF_PDU;
		return true;
	}
	else
	{
		return false;
	}
}

void IO::GSMModemController::FreeOperator(Operator *oper)
{
	Text::StrDelNew(oper->longName);
	Text::StrDelNew(oper->shortName);
	MemFree(oper);
}

IO::GSMModemController::GSMModemController(IO::ATCommandChannel *channel, Bool needRelease) : IO::ModemController(channel, needRelease)
{
	this->smsFormat = -1;
}

IO::GSMModemController::~GSMModemController()
{
}

UTF8Char *IO::GSMModemController::GSMGetManufacturer(UTF8Char *manu)
{
	return this->SendStringCommand(manu, "AT+CGMI", 3000);
}

UTF8Char *IO::GSMModemController::GSMGetModelIdent(UTF8Char *model)
{
	return this->SendStringCommand(model, "AT+CGMM", 3000);
}

UTF8Char *IO::GSMModemController::GSMGetModemVer(UTF8Char *ver)
{
	return this->SendStringCommand(ver, "AT+CGMR", 3000);
}

UTF8Char *IO::GSMModemController::GSMGetIMEI(UTF8Char *imei)
{
	return this->SendStringCommand(imei, "AT+CGSN", 3000);
}

UTF8Char *IO::GSMModemController::GSMGetTECharset(UTF8Char *cs)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = this->SendStringCommand(sbuff, "AT+CSCS?", 3000);
	if (sptr && Text::StrStartsWith(sbuff, (const UTF8Char*)"+CSCS: "))
	{
		return Text::StrConcat(cs, &sbuff[7]);
	}
	return 0;
}

Bool IO::GSMModemController::GSMSetTECharset(const UTF8Char *cs)
{
	Char sbuff[16];
	Char *sptr = Text::StrConcat(sbuff, "AT+CSCS=");
	sptr = Text::StrConcat(sptr, (const Char*)cs);
	return this->SendBoolCommand(sbuff);
}

UTF8Char *IO::GSMModemController::GSMGetIMSI(UTF8Char *imsi)
{
	return this->SendStringCommand(imsi, "AT+CIMI", 3000);
}

UTF8Char *IO::GSMModemController::GSMGetCurrOperator(UTF8Char *oper)
{
	UTF8Char sbuff[256];
	UTF8Char *sbuffs[5];
	UTF8Char *sptr;
	sptr = this->SendStringCommand(sbuff, "AT+COPS=3,0", 3000);
	sptr = this->SendStringCommand(sbuff, "AT+COPS?", 3000);
	if (sptr == 0)
		return 0;
	if (Text::StrStartsWith(sbuff, (const UTF8Char*)"+COPS: "))
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

UTF8Char *IO::GSMModemController::GSMGetCurrPLMN(UTF8Char *plmn)
{
	UTF8Char sbuff[256];
	UTF8Char *sbuffs[5];
	UTF8Char *sptr;
	sptr = this->SendStringCommand(sbuff, "AT+COPS=3,2", 3000);
	sptr = this->SendStringCommand(sbuff, "AT+COPS?", 3000);
	if (sptr == 0)
		return 0;
	if (Text::StrStartsWith(sbuff, (const UTF8Char*)"+COPS: "))
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
	Char sbuff[256];
	Char *sptr = Text::StrConcat(sbuff, "AT+COPS=1,2,\"");
	sptr = Text::StrInt32(sptr, plmn);
	*sptr++ = '"';
	*sptr = 0;
	return this->SendBoolCommand(sbuff, 300000);
}

Bool IO::GSMModemController::GSMGetAllowedOperators(Data::ArrayList<Operator*> *operList)
{
	UTF8Char sbuff[256];
	UTF8Char lbuff[256];
	UTF8Char tmpBuff[256];
	const Char *sptr;
	Char c;
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, "AT+COPS=?", 60000);
	UOSInt i;
	UOSInt j = this->cmdResults->GetCount();
	UOSInt k = 0;
	UOSInt l = 0;
	Bool operStarted;
	Bool quoteStarted;
	Bool operExist;
	Bool operError;
	const Char *val;
	if (j > 1)
	{
		val = this->cmdResults->GetItem(j - 1);
		if (Text::StrCompare(val, "OK") == 0)
		{
			Int32 status;
			Int32 plmn;
			Operator *newOper;
			j -= 1;
			i = 1;
			while (i < j)
			{
				val = this->cmdResults->GetItem(i);
				if (Text::StrStartsWith(val, "+COPS: "))
				{
					sptr = &val[7];
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
										newOper = MemAlloc(Operator, 1);
										newOper->status = (OperStatus)status;
										newOper->plmn = plmn;
										newOper->longName = Text::StrCopyNew(lbuff);
										newOper->shortName = Text::StrCopyNew(sbuff);
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
										k = 2;
										l = 0;
									}
									else if (k == 2)
									{
										sbuff[l] = 0;
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

void IO::GSMModemController::GSMFreeOperators(Data::ArrayList<Operator*> *operList)
{
	UOSInt i = operList->GetCount();
	while (i-- > 0)
	{
		FreeOperator(operList->RemoveAt(i));
	}
}

Int32 IO::GSMModemController::GSMSearchOperatorPLMN(Int32 netact)
{
	Data::ArrayList<IO::GSMModemController::Operator *> operList;
	IO::GSMModemController::Operator *oper;
	Int32 plmn = 0;
	UOSInt i;
	UOSInt j;
	this->GSMGetAllowedOperators(&operList);
	i = 0;
	j = operList.GetCount();
	while (i < j)
	{
		oper = operList.GetItem(i);
		if (oper->status == IO::GSMModemController::OSTAT_AVAILABLE || oper->status == IO::GSMModemController::OSTAT_CURRENT)
		{
			plmn = oper->plmn;
			break;
		}
		i++;
	}
	this->GSMFreeOperators(&operList);
	return plmn;
}

IO::GSMModemController::SIMStatus IO::GSMModemController::GSMGetSIMStatus()
{
	IO::GSMModemController::SIMStatus sims = IO::GSMModemController::SIMS_UNKNOWN;

	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, "AT+CPIN?", 3000);
	UOSInt i = this->cmdResults->GetCount();
	const Char *val;
	while (i-- > 0)
	{
		val = this->cmdResults->GetItem(i);
		if (Text::StrCompare(val, "+CPIN: READY") == 0)
		{
			sims = IO::GSMModemController::SIMS_READY;
			break;
		}
		else if (Text::StrCompare(val, "+CME ERROR: SIM not inserted") == 0)
		{
			sims = IO::GSMModemController::SIMS_ABSENT;
			break;
		}
		else if (Text::StrStartsWith(val, "+CME ERROR: "))
		{
			break;
		}
	}
	ClearCmdResult();
	mutUsage.EndUse();
	return sims;
}

Bool IO::GSMModemController::GSMGetSignalQuality(RSSI *rssi, BER *ber)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr = this->SendStringCommand(sbuff, "AT+CSQ", 3000);
	UTF8Char *sbuffs[3];
	if (sptr == 0)
		return false;
	if (Text::StrStartsWith(sbuff, (const UTF8Char*)"+CSQ: "))
	{
		if (Text::StrSplitTrim(sbuffs, 3, &sbuff[6], ',') == 2)
		{
			*rssi = (RSSI)Text::StrToInt32(sbuffs[0]);
			*ber = (BER)Text::StrToInt32(sbuffs[1]);
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
	return this->SendBoolCommand("AT+CFUN=0");
}

Bool IO::GSMModemController::GSMSetFunctionalityFull()
{
	return this->SendBoolCommand("AT+CFUN=1");
}

Bool IO::GSMModemController::GSMSetFunctionalityReset()
{
	return this->SendBoolCommand("AT+CFUN=6");
}

Bool IO::GSMModemController::GSMGetModemTime(Data::DateTime *date)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr = this->SendStringCommand(sbuff, "AT+CCLK?", 3000);
	if (sptr == 0)
		return false;
	if (Text::StrStartsWith(sbuff, (const UTF8Char*)"+CCLK: \""))
	{
		OSInt i = Text::StrIndexOf(&sbuff[8], '\"');
		if (i >= 0)
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
			date->SetValue(&sbuff[6]);
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

Bool IO::GSMModemController::GSMSetModemTime(Data::DateTime *date)
{
	Char sbuff[256];
	Char *sptr = Text::StrConcat(sbuff, "AT+CCLK=\"");
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
	return SendBoolCommand(sbuff);
}

Int32 IO::GSMModemController::GSMGetSIMPLMN()
{
	UTF8Char sbuff[32];
	if (this->GSMGetIMSI(sbuff) == 0)
	{
		return 0;
	}
	Int32 digitCnt = 5;
	if (Text::StrStartsWith(sbuff, (const UTF8Char*)"405"))
	{
		/////////////////////////
		digitCnt = 6;
	}
	else if (Text::StrStartsWith(sbuff, (const UTF8Char*)"50215"))
	{
		digitCnt = 6;
	}
	else if (Text::StrStartsWith(sbuff, (const UTF8Char*)"3"))
	{
		/////////////////////////
		digitCnt = 6;
	}
	else if (Text::StrStartsWith(sbuff, (const UTF8Char*)"722"))
	{
		digitCnt = 6;
	}
	else if (Text::StrStartsWith(sbuff, (const UTF8Char*)"732"))
	{
		digitCnt = 6;
	}
	sbuff[digitCnt] = 0;
	return Text::StrToInt32(sbuff);
}

Bool IO::GSMModemController::GPRSEPSReg()
{
	return this->SendBoolCommand("AT+CEREG=1");
}

Bool IO::GSMModemController::GPRSNetworkReg()
{
	return this->SendBoolCommand("AT+CGREG=1");
}

Bool IO::GSMModemController::GPRSServiceIsAttached(Bool *attached)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr = this->SendStringCommand(sbuff, "AT+CGATT?", 3000);
	if (sptr == 0 || !Text::StrStartsWith(sbuff, (const UTF8Char*)"+CGATT: "))
	{
		return false;
	}
	return Text::StrToInt32(&sbuff[8]) != 0;
}

Bool IO::GSMModemController::GPRSServiceSetAttached(Bool attached)
{
	if (attached)
	{
		return SendBoolCommand("AT+CGATT=1", 300000);
	}
	else
	{
		return SendBoolCommand("AT+CGATT=0", 300000);
	}
}

Bool IO::GSMModemController::GPRSSetAPN(const UTF8Char *apn)
{
	Char sbuff[256];
	Char *sptr = Text::StrConcat(sbuff, "AT+CGDCONT=1,\"IP\",\"");
	sptr = Text::StrConcat(sptr, (const Char*)apn);
	*sptr++ = '"';
	*sptr = 0;
	return SendBoolCommand(sbuff);
}

Bool IO::GSMModemController::GPRSSetPDPActive(Bool active)
{
	if (active)
	{
		return SendBoolCommand("AT+CGACT=1", 3000);
	}
	else
	{
		return SendBoolCommand("AT+CGACT=0", 3000);
	}
}

Bool IO::GSMModemController::SMSListMessages(Data::ArrayList<IO::GSMModemController::SMSMessage*> *msgList, IO::GSMModemController::SMSStatus status)
{
	Char sbuff[256];
	Char *sbuffs[5];
	UOSInt strLen;
	if (this->smsFormat != 0)
	{
		this->SetSMSFormat(IO::GSMModemController::SMSF_PDU);
	}
	Text::StrInt32(Text::StrConcat(sbuff, "AT+CMGL="), (Int32)status);
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, sbuff, 10000);
	UOSInt i;
	UOSInt j = this->cmdResults->GetCount();
	Int32 lastIndex = 0;
	const Char *val;
	const Char *val2;
	IO::GSMModemController::SMSMessage *msg;
	if (j > 1)
	{
		val = this->cmdResults->GetItem(j - 1);
		if (Text::StrCompare(val, "OK") == 0)
		{
			j -= 2;
			i = 1;
			while (i < j)
			{
				val = this->cmdResults->GetItem(i);
				val2 = this->cmdResults->GetItem(i + 1);

				if (Text::StrStartsWith(val, "+CMGL: "))
				{
					Text::StrConcat(sbuff, &val[7]);
					if (Text::StrSplit(sbuffs, 5, sbuff, ',') == 4)
					{
						strLen = Text::StrCharCnt(val2);
						msg = MemAlloc(IO::GSMModemController::SMSMessage, 1);
						lastIndex = msg->index = Text::StrToInt32(sbuffs[0]);
						msg->status = (SMSStatus)Text::StrToInt32(sbuffs[1]);
						msg->pduLeng = (strLen >> 1);
						msg->pduMessage = MemAlloc(UInt8, strLen >> 1);
						Text::StrHex2Bytes(val2, msg->pduMessage);
						msgList->Add(msg);
					}
				}
				else
				{
					if (Text::StrIndexOf(val, ',') == -1)
					{
						strLen = Text::StrCharCnt(val);
						lastIndex++;
						msg = MemAlloc(IO::GSMModemController::SMSMessage, 1);
						msg->index = lastIndex + 1;
						msg->status = (SMSStatus)0;
						msg->pduLeng = (strLen >> 1);
						msg->pduMessage = MemAlloc(UInt8, strLen >> 1);
						Text::StrHex2Bytes(val, msg->pduMessage);
						msgList->Add(msg);
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

void IO::GSMModemController::SMSFreeMessages(Data::ArrayList<SMSMessage*> *msgList)
{
	UOSInt i = msgList->GetCount();
	while (i-- > 0)
	{
		SMSFreeMessage(msgList->RemoveAt(i));
	}
}

void IO::GSMModemController::SMSFreeMessage(SMSMessage *msg)
{
	MemFree(msg->pduMessage);
	MemFree(msg);
}

Bool IO::GSMModemController::SMSDeleteMessage(Int32 index)
{
	Char sbuff[32];
	Text::StrInt32(Text::StrConcat(sbuff, "AT+CMGD="), index);
	return this->SendBoolCommand(sbuff);
}

Bool IO::GSMModemController::SMSSendMessage(Text::SMSMessage *msg)
{
	Char sbuff1[32];
	Char sbuff2[512];
	UInt8 dataBuff[256];
	Bool isSucc;
	UOSInt buffSize = msg->ToSubmitPDU(dataBuff);
	Text::StrHexBytes(sbuff2, dataBuff, buffSize, 0);
	Text::StrInt32(Text::StrConcat(sbuff1, "AT+CMGS="), (Int32)(buffSize - 1));

	if (this->smsFormat != 0)
	{
		this->SetSMSFormat(IO::GSMModemController::SMSF_PDU);
	}
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommands(this->cmdResults, sbuff1, sbuff2, 10000);
	isSucc = this->IsCmdSucceed();
	ClearCmdResult();
	mutUsage.EndUse();
	return isSucc;
}

Bool IO::GSMModemController::SMSSetStorage(SMSStorage reading, SMSStorage writing, SMSStorage store)
{
	Char sbuff[64];
	Char *sptr;

	sptr = Text::StrConcat(sbuff, "AT+CPMS=");
	if (reading == IO::GSMModemController::SMSSTORE_SIM)
	{
		sptr = Text::StrConcat(sptr, "\"SM\"");
	}
	else if (reading == IO::GSMModemController::SMSSTORE_FLASH)
	{
		sptr = Text::StrConcat(sptr, "\"ME\"");
	}
	else if (reading == IO::GSMModemController::SMSSTORE_STATUSREPORT)
	{
		sptr = Text::StrConcat(sptr, "\"SR\"");
	}
	else if (reading == IO::GSMModemController::SMSSTORE_CBMMESSAGE)
	{
		sptr = Text::StrConcat(sptr, "\"BM\"");
	}

	if (store != IO::GSMModemController::SMSSTORE_UNCHANGE || writing != IO::GSMModemController::SMSSTORE_UNCHANGE)
	{
		sptr = Text::StrConcat(sptr, ",");
		if (writing == IO::GSMModemController::SMSSTORE_SIM)
		{
			sptr = Text::StrConcat(sptr, "\"SM\"");
		}
		else if (writing == IO::GSMModemController::SMSSTORE_FLASH)
		{
			sptr = Text::StrConcat(sptr, "\"ME\"");
		}
		else if (writing == IO::GSMModemController::SMSSTORE_STATUSREPORT)
		{
			sptr = Text::StrConcat(sptr, "\"SR\"");
		}
		else if (writing == IO::GSMModemController::SMSSTORE_CBMMESSAGE)
		{
			sptr = Text::StrConcat(sptr, "\"BM\"");
		}
		if (store != IO::GSMModemController::SMSSTORE_UNCHANGE)
		{
			sptr = Text::StrConcat(sptr, ",");
			if (store == IO::GSMModemController::SMSSTORE_SIM)
			{
				sptr = Text::StrConcat(sptr, "\"SM\"");
			}
			else if (store == IO::GSMModemController::SMSSTORE_FLASH)
			{
				sptr = Text::StrConcat(sptr, "\"ME\"");
			}
			else if (store == IO::GSMModemController::SMSSTORE_STATUSREPORT)
			{
				sptr = Text::StrConcat(sptr, "\"SR\"");
			}
			else if (store == IO::GSMModemController::SMSSTORE_CBMMESSAGE)
			{
				sptr = Text::StrConcat(sptr, "\"BM\"");
			}
		}
	}
	return this->SendBoolCommand(sbuff);
}

Bool IO::GSMModemController::SMSGetStorageInfo(SMSStorageInfo *reading, SMSStorageInfo *writing, SMSStorageInfo *store)
{
	UTF8Char sbuff[256];
	UTF8Char *buffs[10];
	if (this->SendStringCommand(sbuff, "AT+CPMS?", 3000) == 0)
		return false;
	if (!Text::StrStartsWith(sbuff, (const UTF8Char*)"+CPMS: "))
		return false;
	if (Text::StrSplit(buffs, 10, &sbuff[7], ',') != 9)
		return false;
	if (reading)
	{
		if (Text::StrCompare(buffs[0], (const UTF8Char*)"\"SM\"") == 0)
			reading->storage = IO::GSMModemController::SMSSTORE_SIM;
		else if (Text::StrCompare(buffs[0], (const UTF8Char*)"\"ME\"") == 0)
			reading->storage = IO::GSMModemController::SMSSTORE_FLASH;
		else if (Text::StrCompare(buffs[0], (const UTF8Char*)"\"SR\"") == 0)
			reading->storage = IO::GSMModemController::SMSSTORE_STATUSREPORT;
		else if (Text::StrCompare(buffs[0], (const UTF8Char*)"\"BM\"") == 0)
			reading->storage = IO::GSMModemController::SMSSTORE_CBMMESSAGE;
		else
			reading->storage = IO::GSMModemController::SMSSTORE_UNCHANGE;
		reading->used = Text::StrToInt32(buffs[1]);
		reading->available = Text::StrToInt32(buffs[2]);
	}
	if (writing)
	{
		if (Text::StrCompare(buffs[3], (const UTF8Char*)"\"SM\"") == 0)
			writing->storage = IO::GSMModemController::SMSSTORE_SIM;
		else if (Text::StrCompare(buffs[3], (const UTF8Char*)"\"ME\"") == 0)
			writing->storage = IO::GSMModemController::SMSSTORE_FLASH;
		else if (Text::StrCompare(buffs[3], (const UTF8Char*)"\"SR\"") == 0)
			writing->storage = IO::GSMModemController::SMSSTORE_STATUSREPORT;
		else if (Text::StrCompare(buffs[3], (const UTF8Char*)"\"BM\"") == 0)
			writing->storage = IO::GSMModemController::SMSSTORE_CBMMESSAGE;
		else
			writing->storage = IO::GSMModemController::SMSSTORE_UNCHANGE;
		writing->used = Text::StrToInt32(buffs[4]);
		writing->available = Text::StrToInt32(buffs[5]);
	}
	if (store)
	{
		if (Text::StrCompare(buffs[6], (const UTF8Char*)"\"SM\"") == 0)
			store->storage = IO::GSMModemController::SMSSTORE_SIM;
		else if (Text::StrCompare(buffs[6], (const UTF8Char*)"\"ME\"") == 0)
			store->storage = IO::GSMModemController::SMSSTORE_FLASH;
		else if (Text::StrCompare(buffs[6], (const UTF8Char*)"\"SR\"") == 0)
			store->storage = IO::GSMModemController::SMSSTORE_STATUSREPORT;
		else if (Text::StrCompare(buffs[6], (const UTF8Char*)"\"BM\"") == 0)
			store->storage = IO::GSMModemController::SMSSTORE_CBMMESSAGE;
		else
			store->storage = IO::GSMModemController::SMSSTORE_UNCHANGE;
		store->used = Text::StrToInt32(buffs[7]);
		store->available = Text::StrToInt32(buffs[8]);
	}
	return true;
}

UTF8Char *IO::GSMModemController::SMSGetSMSC(UTF8Char *buff)
{
	UTF8Char sbuff[256];
	UTF8Char *sarr[3];
	UTF8Char *sptr = 0;
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, "AT+CSCA?", 3000);
	UOSInt i = this->cmdResults->GetCount();
	const Char *val;
	while (i-- > 0)
	{
		val = this->cmdResults->GetItem(i);
		if (Text::StrStartsWith(val, "+CSCA: "))
		{
			Text::StrConcat(sbuff, (const UTF8Char*)&val[7]);
			Text::StrCSVSplit(sarr, 3, sbuff);
			sptr = Text::StrConcat(buff, sarr[0]);
			break;
		}
	}
	ClearCmdResult();
	mutUsage.EndUse();
	return sptr;
}

UTF8Char *IO::GSMModemController::PBGetCharset(UTF8Char *cs)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr = this->SendStringCommand(sbuff, "AT+WPCS?", 3000);
	if (sptr && Text::StrStartsWith(sbuff, (const UTF8Char*)"+WPCS: "))
	{
		return Text::StrConcat(cs, &sbuff[7]);
	}
	return 0;
}


Bool IO::GSMModemController::PBSetCharset(const Char *cs)
{
	Char sbuff[16];
	Char *sptr = Text::StrConcat(sbuff, "AT+WPCS=");
	sptr = Text::StrConcat(sptr, cs);
	return this->SendBoolCommand(sbuff);
}

Bool IO::GSMModemController::PBSetStorage(PBStorage storage)
{
	const Char *cmd;
	switch (storage)
	{
	case PBSTORE_SIM:
		cmd = "AT+CPBS=\"SM\"";
		break;
	case PBSTORE_SIM_RESTRICTED:
		cmd = "AT+CPBS=\"FD\"";
		break;
	case PBSTORE_SIM_OWN_NUMBERS:
		cmd = "AT+CPBS=\"ON\"";
		break;
	case PBSTORE_EMERGENCY:
		cmd = "AT+CPBS=\"EN\"";
		break;
	case PBSTORE_LASTNUMDIAL:
		cmd = "AT+CPBS=\"LD\"";
		break;
	case PBSTORE_UNANSWERED:
		cmd = "AT+CPBS=\"MC\"";
		break;
	case PBSTORE_ME:
		cmd = "AT+CPBS=\"ME\"";
		break;
	case PBSTORE_ME_SIM:
		cmd = "AT+CPBS=\"MT\"";
		break;
	case PBSTORE_RECEIVED_CALL:
		cmd = "AT+CPBS=\"RC\"";
		break;
	case PBSTORE_SERVICE_DIALING_NUMBERS:
		cmd = "AT+CPBS=\"SN\"";
		break;
	case PBSTORE_UNKNOWN:
	default:
		return false;
	}
	return this->SendBoolCommand(cmd);
}

Bool IO::GSMModemController::PBGetStorage(PBStorage *storage, Int32 *usedEntry, Int32 *freeEntry)
{
	UTF8Char sbuff[256];
	UTF8Char *buffs[10];
	if (this->SendStringCommand(sbuff, "AT+CPBS?", 3000) == 0)
		return false;
	if (!Text::StrStartsWith(sbuff, (const UTF8Char*)"+CPBS: "))
		return false;
	if (Text::StrSplit(buffs, 4, &sbuff[7], ',') != 3)
		return false;
	if (storage)
	{
		if (Text::StrCompare(buffs[0], (const UTF8Char*)"\"SM\"") == 0)
			*storage = IO::GSMModemController::PBSTORE_SIM;
		else if (Text::StrCompare(buffs[0], (const UTF8Char*)"\"FD\"") == 0)
			*storage = IO::GSMModemController::PBSTORE_SIM_RESTRICTED;
		else if (Text::StrCompare(buffs[0], (const UTF8Char*)"\"ON\"") == 0)
			*storage = IO::GSMModemController::PBSTORE_SIM_OWN_NUMBERS;
		else if (Text::StrCompare(buffs[0], (const UTF8Char*)"\"EN\"") == 0)
			*storage = IO::GSMModemController::PBSTORE_EMERGENCY;
		else if (Text::StrCompare(buffs[0], (const UTF8Char*)"\"LD\"") == 0)
			*storage = IO::GSMModemController::PBSTORE_LASTNUMDIAL;
		else if (Text::StrCompare(buffs[0], (const UTF8Char*)"\"MC\"") == 0)
			*storage = IO::GSMModemController::PBSTORE_UNANSWERED;
		else if (Text::StrCompare(buffs[0], (const UTF8Char*)"\"ME\"") == 0)
			*storage = IO::GSMModemController::PBSTORE_ME;
		else if (Text::StrCompare(buffs[0], (const UTF8Char*)"\"MT\"") == 0)
			*storage = IO::GSMModemController::PBSTORE_ME_SIM;
		else if (Text::StrCompare(buffs[0], (const UTF8Char*)"\"RC\"") == 0)
			*storage = IO::GSMModemController::PBSTORE_RECEIVED_CALL;
		else if (Text::StrCompare(buffs[0], (const UTF8Char*)"\"SN\"") == 0)
			*storage = IO::GSMModemController::PBSTORE_SERVICE_DIALING_NUMBERS;
		else
			*storage = IO::GSMModemController::PBSTORE_UNKNOWN;
	}
	if (usedEntry)
	{
		*usedEntry = Text::StrToInt32(buffs[1]);
	}
	if (freeEntry)
	{
		*freeEntry = Text::StrToInt32(buffs[2]);
	}
	return true;
}

Bool IO::GSMModemController::PBGetStorageStatus(Int32 *startEntry, Int32 *endEntry, Int32 *maxNumberLen, Int32 *maxTextLen)
{
	UTF8Char sbuff[256];
	UTF8Char *sarr[4];
	UTF8Char *sarr2[3];
	this->SendStringCommand(sbuff, "AT+CPBR=?", 3000);
	if (Text::StrStartsWith(sbuff, (const UTF8Char*)"+CPBR: ") && Text::StrSplit(sarr, 4, &sbuff[7], ',') == 3)
	{
		if (sarr[0][0] == '(' && Text::StrEndsWith(sarr[0], (const UTF8Char*)")") && Text::StrSplit(sarr2, 3, &sarr[0][1], '-') == 2)
		{
			sarr2[1][Text::StrCharCnt(sarr2[1]) - 1] = 0;
			if (startEntry)
			{
				*startEntry = Text::StrToInt32(sarr2[0]);
			}
			if (endEntry)
			{
				*endEntry = Text::StrToInt32(sarr2[1]);
			}
		}
		if (maxNumberLen)
		{
			*maxNumberLen = Text::StrToInt32(sarr[1]);
		}
		if (maxTextLen)
		{
			*maxTextLen = Text::StrToInt32(sarr[2]);
		}
		return true;
	}
	else
	{
		return false;
	}
}

Bool IO::GSMModemController::PBReadEntries(Data::ArrayList<PBEntry*> *phoneList, Int32 startEntry, Int32 endEntry)
{
	Char sbuff[256];
	Char *sbuffs[5];
	if (startEntry == endEntry)
	{
		Text::StrInt32(Text::StrConcat(sbuff, "AT+CPBR="), startEntry);
	}
	else
	{
		Text::StrInt32(Text::StrConcat(Text::StrInt32(Text::StrConcat(sbuff, "AT+CPBR="), startEntry), ","), endEntry);
	}
	Sync::MutexUsage mutUsage(this->cmdMut);
	this->channel->SendATCommand(this->cmdResults, sbuff, 10000);
	UOSInt i;
	UOSInt j = this->cmdResults->GetCount();
	const Char *val;
	IO::GSMModemController::PBEntry *ent;
	if (j > 1)
	{
		val = this->cmdResults->GetItem(j - 1);
		if (Text::StrCompare(val, "OK") == 0)
		{
			j -= 1;
			i = 1;
			while (i < j)
			{
				val = this->cmdResults->GetItem(i);
				if (Text::StrStartsWith(val, "+CPBR: "))
				{
					Text::StrConcat(sbuff, &val[7]);
					if (Text::StrCSVSplit(sbuffs, 5, sbuff) >= 4)
					{
						ent = MemAlloc(PBEntry, 1);
						ent->number = Text::StrCopyNew((const UTF8Char*)sbuffs[1]);
						ent->name = Text::StrCopyNew((const UTF8Char*)sbuffs[3]);
						ent->index = Text::StrToInt32(sbuffs[0]);
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

Bool IO::GSMModemController::PBReadAllEntries(Data::ArrayList<PBEntry*> *phoneList)
{
	Int32 startEntry;
	Int32 endEntry;
	Int32 maxNumberLen;
	Int32 maxTextLen;
	if (!PBGetStorageStatus(&startEntry, &endEntry, &maxNumberLen, &maxTextLen))
		return false;
	return PBReadEntries(phoneList, startEntry, endEntry);
}

void IO::GSMModemController::PBFreeEntry(PBEntry *entry)
{
	SDEL_TEXT(entry->name);
	SDEL_TEXT(entry->number);
	MemFree(entry);
}

void IO::GSMModemController::PBFreeEntries(Data::ArrayList<PBEntry*> *phoneList)
{
	UOSInt i = phoneList->GetCount();
	while (i-- > 0)
	{
		PBFreeEntry(phoneList->RemoveAt(i));
	}
}

Int32 IO::GSMModemController::GetRSSIdBm(RSSI rssi)
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

UTF8Char *IO::GSMModemController::GetRSSIString(UTF8Char *buff, RSSI rssi)
{
	switch (rssi)
	{
	case RSSI_113:
		return Text::StrConcat(buff, (const UTF8Char*)"-113 dBm");
	case RSSI_111:
		return Text::StrConcat(buff, (const UTF8Char*)"-111 dBm");
	case RSSI_109:
		return Text::StrConcat(buff, (const UTF8Char*)"-109 dBm");
	case RSSI_107:
		return Text::StrConcat(buff, (const UTF8Char*)"-107 dBm");
	case RSSI_105:
		return Text::StrConcat(buff, (const UTF8Char*)"-105 dBm");
	case RSSI_103:
		return Text::StrConcat(buff, (const UTF8Char*)"-103 dBm");
	case RSSI_101:
		return Text::StrConcat(buff, (const UTF8Char*)"-101 dBm");
	case RSSI_99:
		return Text::StrConcat(buff, (const UTF8Char*)"-99 dBm");
	case RSSI_97:
		return Text::StrConcat(buff, (const UTF8Char*)"-97 dBm");
	case RSSI_95:
		return Text::StrConcat(buff, (const UTF8Char*)"-95 dBm");
	case RSSI_93:
		return Text::StrConcat(buff, (const UTF8Char*)"-93 dBm");
	case RSSI_91:
		return Text::StrConcat(buff, (const UTF8Char*)"-91 dBm");
	case RSSI_89:
		return Text::StrConcat(buff, (const UTF8Char*)"-89 dBm");
	case RSSI_87:
		return Text::StrConcat(buff, (const UTF8Char*)"-87 dBm");
	case RSSI_85:
		return Text::StrConcat(buff, (const UTF8Char*)"-85 dBm");
	case RSSI_83:
		return Text::StrConcat(buff, (const UTF8Char*)"-83 dBm");
	case RSSI_81:
		return Text::StrConcat(buff, (const UTF8Char*)"-81 dBm");
	case RSSI_79:
		return Text::StrConcat(buff, (const UTF8Char*)"-79 dBm");
	case RSSI_77:
		return Text::StrConcat(buff, (const UTF8Char*)"-77 dBm");
	case RSSI_75:
		return Text::StrConcat(buff, (const UTF8Char*)"-75 dBm");
	case RSSI_73:
		return Text::StrConcat(buff, (const UTF8Char*)"-73 dBm");
	case RSSI_71:
		return Text::StrConcat(buff, (const UTF8Char*)"-71 dBm");
	case RSSI_69:
		return Text::StrConcat(buff, (const UTF8Char*)"-69 dBm");
	case RSSI_67:
		return Text::StrConcat(buff, (const UTF8Char*)"-67 dBm");
	case RSSI_65:
		return Text::StrConcat(buff, (const UTF8Char*)"-65 dBm");
	case RSSI_63:
		return Text::StrConcat(buff, (const UTF8Char*)"-63 dBm");
	case RSSI_61:
		return Text::StrConcat(buff, (const UTF8Char*)"-61 dBm");
	case RSSI_59:
		return Text::StrConcat(buff, (const UTF8Char*)"-59 dBm");
	case RSSI_57:
		return Text::StrConcat(buff, (const UTF8Char*)"-57 dBm");
	case RSSI_55:
		return Text::StrConcat(buff, (const UTF8Char*)"-55 dBm");
	case RSSI_53:
		return Text::StrConcat(buff, (const UTF8Char*)"-53 dBm");
	case RSSI_51:
		return Text::StrConcat(buff, (const UTF8Char*)"-51 dBm");
	case RSSI_UNKNOWN:
	default:
		return Text::StrConcat(buff, (const UTF8Char*)"Unknown");
	}
}

UTF8Char *IO::GSMModemController::GetBERString(UTF8Char *buff, BER ber)
{
	switch (ber)
	{
	case BER_RXQUAL_0:
		return Text::StrConcat(buff, (const UTF8Char*)"< 0.2%");
	case BER_RXQUAL_1:
		return Text::StrConcat(buff, (const UTF8Char*)"0.2% - 0.4%");
	case BER_RXQUAL_2:
		return Text::StrConcat(buff, (const UTF8Char*)"0.4% - 0.8%");
	case BER_RXQUAL_3:
		return Text::StrConcat(buff, (const UTF8Char*)"0.8% - 1.6%");
	case BER_RXQUAL_4:
		return Text::StrConcat(buff, (const UTF8Char*)"1.6% - 3.2%");
	case BER_RXQUAL_5:
		return Text::StrConcat(buff, (const UTF8Char*)"3.2% - 6.4%");
	case BER_RXQUAL_6:
		return Text::StrConcat(buff, (const UTF8Char*)"6.4% - 12.8%");
	case BER_RXQUAL_7:
		return Text::StrConcat(buff, (const UTF8Char*)"12.8%");
	case BER_UNKNWON:
	default:
		return Text::StrConcat(buff, (const UTF8Char*)"Unknown");
	}
}

const UTF8Char *IO::GSMModemController::OperStatusGetName(OperStatus operStatus)
{
	switch (operStatus)
	{
	case OSTAT_AVAILABLE:
		return (const UTF8Char*)"Available";
	case OSTAT_CURRENT:
		return (const UTF8Char*)"Current";
	case OSTAT_FORBIDDEN:
		return (const UTF8Char*)"Forbidden";
	case OSTAT_UNKNOWN:
	default:
		return (const UTF8Char*)"Unknown";
	}
}

const UTF8Char *IO::GSMModemController::SIMStatusGetName(SIMStatus simStatus)
{
	switch (simStatus)
	{
	case SIMS_READY:
		return (const UTF8Char*)"Ready";
	case SIMS_ABSENT:
		return (const UTF8Char*)"Absent";
	case SIMS_UNKNOWN:
	default:
		return (const UTF8Char*)"Unknown";
	}
}
