#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Net/ASN1Util.h"
#include "Net/SNMPManager.h"
#include "Sync/MutexUsage.h"
#include "Text/StringBuilderUTF8.h"

void Net::SNMPManager::FreeAllItems(Data::ArrayList<Net::SNMPUtil::BindingItem*> *itemList)
{
	UOSInt i = itemList->GetCount();
	while (i-- > 0)
	{
		Net::SNMPUtil::FreeBindingItem(itemList->GetItem(i));
	}
	itemList->Clear();
}

Net::SNMPManager::SNMPManager(NotNullPtr<Net::SocketFactory> sockf)
{
	NEW_CLASS(this->cli, Net::SNMPClient(sockf));
}

Net::SNMPManager::~SNMPManager()
{
	UOSInt i;
	UOSInt j;
	AgentInfo *agent;
	ReadingInfo *reading;
	DEL_CLASS(this->cli);
	i = this->agentList.GetCount();
	while (i-- > 0)
	{
		agent = this->agentList.GetItem(i);
		agent->community->Release();
		agent->descr->Release();
		SDEL_STRING(agent->name);
		SDEL_STRING(agent->contact);
		SDEL_STRING(agent->location);
		j = agent->readingList->GetCount();
		while (j-- > 0)
		{
			reading = agent->readingList->GetItem(j);
			reading->name->Release();
			MemFree(reading);
		}
		DEL_CLASS(agent->readingList);
		SDEL_STRING(agent->model);
		SDEL_STRING(agent->vendor);
		SDEL_STRING(agent->cpuName);
		MemFree(agent);
	}
}

Bool Net::SNMPManager::IsError()
{
	return this->cli->IsError();
}

void Net::SNMPManager::UpdateValues()
{
	UOSInt i = this->agentList.GetCount();
	UOSInt j;
	AgentInfo *agent;
	ReadingInfo *reading;
	Int32 iVal;
	Data::ArrayList<Net::SNMPUtil::BindingItem *> itemList;
	Net::SNMPUtil::BindingItem *item;

	Net::SNMPUtil::ErrorStatus err;
	while (i-- > 0)
	{
		agent = this->agentList.GetItem(i);
		j = agent->readingList->GetCount();
		while (j-- > 0)
		{
			reading = agent->readingList->GetItem(j);
			err = this->cli->V1GetRequestPDU(&agent->addr, agent->community, reading->objId, reading->objIdLen, &itemList);
			if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
			{
				item = itemList.GetItem(0);
				if (Net::SNMPUtil::ValueToInt32(item->valType, item->valBuff, item->valLen, &iVal))
				{
					reading->currVal = iVal * reading->mulVal;
					reading->valValid = iVal != reading->invVal;
				}
				else
				{
					reading->valValid = false;
				}
			}
			FreeAllItems(&itemList);
		}
	}
}

UOSInt Net::SNMPManager::GetAgentList(Data::ArrayList<AgentInfo*> *agentList)
{
	UOSInt ret;
	Sync::MutexUsage mutUsage(this->agentMut);
	ret = this->agentList.GetCount();
	agentList->AddAll(this->agentList);
	return ret;
}

Net::SNMPManager::AgentInfo *Net::SNMPManager::AddAgent(const Net::SocketUtil::AddressInfo *addr, NotNullPtr<Text::String> community)
{
	Data::ArrayList<Net::SNMPUtil::BindingItem *> itemList;
	Net::SNMPUtil::ErrorStatus err;
	Net::SNMPManager::AgentInfo *agent = 0;
	Net::SNMPUtil::BindingItem *item;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	UOSInt i;
	if (addr->addrType == Net::AddrType::IPv4)
	{
		UInt32 ipv4 = ReadMUInt32(addr->addr);
		if (this->ipv4Agents.Get(ipv4))
		{
			return 0;
		}
	}
	err = this->cli->V1GetRequest(addr, community, UTF8STRC("1.3.6.1.2.1.1.1.0"), &itemList); //sysDescr
	i = itemList.GetCount();
	if (err == Net::SNMPUtil::ES_NOERROR && i == 1)
	{
		item = itemList.GetItem(0);
		if (item->valType == 4 && item->valLen > 0)
		{
			agent = MemAlloc(AgentInfo, 1);
			agent->addr = *addr;
			agent->community = community->Clone();
			agent->descr = Text::String::New(item->valBuff, item->valLen);
			agent->objIdLen = 0;
			agent->name = 0;
			agent->location = 0;
			agent->contact = 0;
			agent->model = 0;
			agent->vendor = 0;
			agent->cpuName = 0;
			MemClear(agent->mac, 6);
			NEW_CLASS(agent->readingList, Data::ArrayList<ReadingInfo*>());
			Sync::MutexUsage mutUsage(this->agentMut);
			this->agentList.Add(agent);
			if (addr->addrType == Net::AddrType::IPv4)
			{
				UInt32 ipv4 = ReadMUInt32(addr->addr);
				this->ipv4Agents.Put(ipv4, agent);
			}
		}
	}
	FreeAllItems(&itemList);
	if (agent)
	{
		err = this->cli->V1GetRequest(addr, community, UTF8STRC("1.3.6.1.2.1.1.2.0"), &itemList); //sysObjectID
		if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
		{
			item = itemList.GetItem(0);
			if (item->valType == 6 && item->valLen > 0)
			{
				MemCopyNO(agent->objId, item->valBuff, item->valLen);
				agent->objIdLen = item->valLen;
			}
		}
		FreeAllItems(&itemList);

		err = this->cli->V1GetRequest(addr, community, UTF8STRC("1.3.6.1.2.1.1.4.0"), &itemList); //sysContact
		if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
		{
			item = itemList.GetItem(0);
			if (item->valType == 4 && item->valLen > 0)
			{
				agent->contact = Text::String::New(item->valBuff, item->valLen).Ptr();
			}
		}
		FreeAllItems(&itemList);

		err = this->cli->V1GetRequest(addr, community, UTF8STRC("1.3.6.1.2.1.1.5.0"), &itemList); //sysName
		if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
		{
			item = itemList.GetItem(0);
			if (item->valType == 4 && item->valLen > 0)
			{
				agent->name = Text::String::New(item->valBuff, item->valLen).Ptr();
			}
		}
		FreeAllItems(&itemList);

		err = this->cli->V1GetRequest(addr, community, UTF8STRC("1.3.6.1.2.1.1.6.0"), &itemList); //sysLocation
		if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
		{
			item = itemList.GetItem(0);
			if (item->valType == 4 && item->valLen > 0)
			{
				agent->location = Text::String::New(item->valBuff, item->valLen).Ptr();
			}
		}
		FreeAllItems(&itemList);

		UOSInt i = 1;
		while (true)
		{
			sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("1.3.6.1.2.1.2.2.1.6.")), i);
			err = this->cli->V1GetRequest(addr, community, sbuff, (UOSInt)(sptr - sbuff), &itemList); //sysLocation
			if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
			{
				item = itemList.GetItem(0);
				if (item->valType == 4 && item->valLen == 6)
				{
					MemCopyNO(agent->mac, item->valBuff, 6);
				}
			}
			else if (err == Net::SNMPUtil::ES_NOSUCHNAME)
			{
				FreeAllItems(&itemList);
				break;
			}
			FreeAllItems(&itemList);

			if (agent->mac[5] != 0 || agent->mac[4] != 0 || agent->mac[3] != 0)
			{
				break;
			}
			i++;
		}

		if (agent->objIdLen > 0)
		{
			Bool found = false;
			UOSInt pduSize;
			UInt8 oidPDU[64];
			if (!found)
			{
				pduSize = Net::ASN1Util::OIDText2PDU(UTF8STRC("1.3.6.1.4.1.24681"), oidPDU); //QNAP
				if (Net::ASN1Util::OIDCompare(oidPDU, pduSize, agent->objId, agent->objIdLen) == 0)
				{
					ReadingInfo *reading;
					UInt32 slotCnt = 0;
					Int32 iVal;
					Text::StringBuilderUTF8 sb;
					found = true;
					agent->vendor = Text::String::New(UTF8STRC("QNAP")).Ptr();

					err = this->cli->V1GetRequest(addr, community, UTF8STRC("1.3.6.1.4.1.24681.1.4.1.1.1.1.1.2.1.5.1"), &itemList); //enclosureSlot.1
					if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
					{
						item = itemList.GetItem(0);
						Net::SNMPUtil::ValueToInt32(item->valType, item->valBuff, item->valLen, (Int32*)&slotCnt);
					}
					FreeAllItems(&itemList);

					err = this->cli->V1GetRequest(addr, community, UTF8STRC("1.3.6.1.4.1.24681.1.4.1.1.1.1.1.2.1.3.1"), &itemList); //enclosureModel.1
					if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
					{
						item = itemList.GetItem(0);
						if (item->valType == 4 && item->valLen > 0)
						{
							agent->model = Text::String::New(item->valBuff, item->valLen).Ptr();
						}
					}
					FreeAllItems(&itemList);

					err = this->cli->V1GetRequest(addr, community, UTF8STRC("1.3.6.1.4.1.24681.1.4.1.1.1.1.1.2.1.7.1"), &itemList); //enclosureSystemTemp.1
					if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
					{
						item = itemList.GetItem(0);
						if (Net::SNMPUtil::ValueToInt32(item->valType, item->valBuff, item->valLen, &iVal) && iVal > 0)
						{
							reading = MemAlloc(ReadingInfo, 1);
							reading->name = Text::String::New(UTF8STRC("System Temp"));
							reading->index = 0;
							reading->objIdLen = Net::ASN1Util::OIDText2PDU(UTF8STRC("1.3.6.1.4.1.24681.1.4.1.1.1.1.1.2.1.7.1"), reading->objId);
							reading->mulVal = 1.0;
							reading->invVal = -1;
							reading->readingType = SSWR::SMonitor::SAnalogSensor::RT_TEMPERATURE;
							reading->valValid = true;
							reading->currVal = iVal;
							agent->readingList->Add(reading);
						}
					}
					FreeAllItems(&itemList);

					err = this->cli->V1GetRequest(addr, community, UTF8STRC("1.3.6.1.4.1.24681.1.4.1.1.1.1.2.2.1.5.1"), &itemList); //systemFanSpeed.1
					if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
					{
						item = itemList.GetItem(0);
						if (Net::SNMPUtil::ValueToInt32(item->valType, item->valBuff, item->valLen, &iVal) && iVal > 0)
						{
							reading = MemAlloc(ReadingInfo, 1);
							reading->name = Text::String::New(UTF8STRC("System Fan Speed"));
							reading->index = 1;
							reading->objIdLen = Net::ASN1Util::OIDText2PDU(UTF8STRC("1.3.6.1.4.1.24681.1.4.1.1.1.1.2.2.1.5.1"), reading->objId);
							reading->mulVal = 1.0;
							reading->invVal = -1;
							reading->readingType = SSWR::SMonitor::SAnalogSensor::RT_ENGINERPM;
							reading->valValid = true;
							reading->currVal = iVal;
							agent->readingList->Add(reading);
						}
					}
					FreeAllItems(&itemList);

					err = this->cli->V1GetRequest(addr, community, UTF8STRC("1.3.6.1.4.1.24681.1.4.1.1.1.1.3.2.1.5.1"), &itemList); //systemPowerFanSpeed.1
					if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
					{
						item = itemList.GetItem(0);
						if (Net::SNMPUtil::ValueToInt32(item->valType, item->valBuff, item->valLen, &iVal) && iVal > 0)
						{
							reading = MemAlloc(ReadingInfo, 1);
							reading->name = Text::String::New(UTF8STRC("Power Fan Speed"));
							reading->index = 2;
							reading->objIdLen = Net::ASN1Util::OIDText2PDU(UTF8STRC("1.3.6.1.4.1.24681.1.4.1.1.1.1.3.2.1.5.1"), reading->objId);
							reading->mulVal = 1.0;
							reading->invVal = -1;
							reading->readingType = SSWR::SMonitor::SAnalogSensor::RT_ENGINERPM;
							reading->valValid = true;
							reading->currVal = iVal;
							agent->readingList->Add(reading);
						}
					}
					FreeAllItems(&itemList);

					err = this->cli->V1GetRequest(addr, community, UTF8STRC("1.3.6.1.4.1.24681.1.4.1.1.1.1.3.2.1.6.1"), &itemList); //systemPowerTemp.1
					if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
					{
						item = itemList.GetItem(0);
						if (Net::SNMPUtil::ValueToInt32(item->valType, item->valBuff, item->valLen, &iVal) && iVal > 0)
						{
							reading = MemAlloc(ReadingInfo, 1);
							reading->name = Text::String::New(UTF8STRC("Power Temp"));
							reading->index = 3;
							reading->objIdLen = Net::ASN1Util::OIDText2PDU(UTF8STRC("1.3.6.1.4.1.24681.1.4.1.1.1.1.3.2.1.6.1"), reading->objId);
							reading->mulVal = 1.0;
							reading->invVal = -1;
							reading->readingType = SSWR::SMonitor::SAnalogSensor::RT_TEMPERATURE;
							reading->valValid = true;
							reading->currVal = iVal;
							agent->readingList->Add(reading);
						}
					}
					FreeAllItems(&itemList);

					if (slotCnt == 0)
					{
						slotCnt = 4;
					}
					i = 0;
					while (i < slotCnt)
					{
						sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("1.3.6.1.4.1.24681.1.4.1.1.1.1.5.2.1.6.")), i);
						err = this->cli->V1GetRequest(addr, community, sbuff, (UOSInt)(sptr - sbuff), &itemList); //diskTemperature
						if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
						{
							item = itemList.GetItem(0);
							if (Net::SNMPUtil::ValueToInt32(item->valType, item->valBuff, item->valLen, &iVal) && iVal > 0)
							{
								reading = MemAlloc(ReadingInfo, 1);
								reading->name = Text::String::NewEmpty();
								reading->index = 4 + i;
								reading->objIdLen = Net::ASN1Util::OIDText2PDU(sbuff, (UOSInt)(sptr - sbuff), reading->objId);
								reading->mulVal = 1.0;
								reading->invVal = -1;
								reading->readingType = SSWR::SMonitor::SAnalogSensor::RT_TEMPERATURE;
								reading->valValid = true;
								reading->currVal = iVal;
								agent->readingList->Add(reading);
								FreeAllItems(&itemList);

								sb.ClearStr();
								sb.AppendC(UTF8STRC("Disk "));
								sb.AppendUOSInt(i);
								sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("1.3.6.1.4.1.24681.1.4.1.1.1.1.5.2.1.8.")), i);
								err = this->cli->V1GetRequest(addr, community, sbuff, (UOSInt)(sptr - sbuff), &itemList); //diskModel
								if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
								{
									item = itemList.GetItem(0);
									if (item->valType == 4 && item->valLen > 0)
									{
										sb.AppendUTF8Char(' ');
										sb.AppendC(item->valBuff, item->valLen);
									}
								}
								reading->name->Release();
								reading->name = Text::String::New(sb.ToString(), sb.GetLength());

								sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("1.3.6.1.4.1.24681.1.4.1.1.1.1.5.2.1.2.")), i);
								err = this->cli->V1GetRequest(addr, community, sbuff, (UOSInt)(sptr - sbuff), &itemList); //diskID
								if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
								{
									item = itemList.GetItem(0);
									if (Net::SNMPUtil::ValueToInt32(item->valType, item->valBuff, item->valLen, &iVal) && iVal > 0)
									{
										reading->index = 3 + (UInt32)iVal;
									}
								}
							}
						}
						FreeAllItems(&itemList);

						i++;
					}
				}
			}
			if (!found)
			{
				pduSize = Net::ASN1Util::OIDText2PDU(UTF8STRC("1.3.6.1.4.1.8072.3.2.10"), oidPDU); //Linux
				if (Net::ASN1Util::OIDCompare(oidPDU, pduSize, agent->objId, agent->objIdLen) == 0)
				{
					if (agent->mac[0] == 0x00 && agent->mac[1] == 0x11 && agent->mac[2] == 0x32) //Synology
					{
						ReadingInfo *reading;
						Int32 iVal;
						Text::StringBuilderUTF8 sb;
						found = true;
						agent->vendor = Text::String::New(UTF8STRC("Synology")).Ptr();

						err = this->cli->V1GetRequest(addr, community, UTF8STRC("1.3.6.1.4.1.6574.1.5.1.0"), &itemList); //modelName.1
						if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
						{
							item = itemList.GetItem(0);
							if (item->valType == 4 && item->valLen > 0)
							{
								agent->model = Text::String::New(item->valBuff, item->valLen).Ptr();
							}
						}
						FreeAllItems(&itemList);

						err = this->cli->V1GetRequest(addr, community, UTF8STRC("1.3.6.1.4.1.6574.1.2.0"), &itemList); //temperature.0
						if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
						{
							item = itemList.GetItem(0);
							if (Net::SNMPUtil::ValueToInt32(item->valType, item->valBuff, item->valLen, &iVal) && iVal > 0)
							{
								reading = MemAlloc(ReadingInfo, 1);
								reading->name = Text::String::New(UTF8STRC("System Temp"));
								reading->index = 0;
								reading->objIdLen = Net::ASN1Util::OIDText2PDU(UTF8STRC("1.3.6.1.4.1.6574.1.2.0"), reading->objId);
								reading->mulVal = 1.0;
								reading->invVal = 0;
								reading->readingType = SSWR::SMonitor::SAnalogSensor::RT_TEMPERATURE;
								reading->valValid = true;
								reading->currVal = iVal;
								agent->readingList->Add(reading);
							}
						}
						FreeAllItems(&itemList);

						i = 0;
						while (true)
						{
							sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("1.3.6.1.4.1.6574.2.1.1.6.")), i);
							err = this->cli->V1GetRequest(addr, community, sbuff, (UOSInt)(sptr - sbuff), &itemList); //diskTemperature
							if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
							{
								item = itemList.GetItem(0);
								if (Net::SNMPUtil::ValueToInt32(item->valType, item->valBuff, item->valLen, &iVal))
								{
									reading = MemAlloc(ReadingInfo, 1);
									reading->name = Text::String::NewEmpty();
									reading->index = 4 + i;
									reading->objIdLen = Net::ASN1Util::OIDText2PDU(sbuff, (UOSInt)(sptr - sbuff), reading->objId);
									reading->mulVal = 1.0;
									reading->invVal = 0;
									reading->readingType = SSWR::SMonitor::SAnalogSensor::RT_TEMPERATURE;
									reading->valValid = iVal > 0;
									reading->currVal = iVal;
									agent->readingList->Add(reading);
									FreeAllItems(&itemList);

									sb.ClearStr();
									sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("1.3.6.1.4.1.6574.2.1.1.2.")), i);
									err = this->cli->V1GetRequest(addr, community, sbuff, (UOSInt)(sptr - sbuff), &itemList); //diskID
									if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
									{
										item = itemList.GetItem(0);
										if (item->valType == 4 && item->valLen > 0)
										{
											sb.AppendC(item->valBuff, item->valLen);
											if (item->valLen >= 6)
											{
												if (item->valBuff[5] >= '1' && item->valBuff[5] <= '9')
												{
													reading->index = 3 + (UOSInt)item->valBuff[5] - 0x30;
												}
											}
										}
									}
									if (sb.GetLength() == 0)
									{
										sb.AppendC(UTF8STRC("Disk "));
										sb.AppendUOSInt(i + 1);
									}
									FreeAllItems(&itemList);

									sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("1.3.6.1.4.1.6574.2.1.1.3.")), i);
									err = this->cli->V1GetRequest(addr, community, sbuff, (UOSInt)(sptr - sbuff), &itemList); //diskModel
									if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
									{
										item = itemList.GetItem(0);
										if (item->valType == 4 && item->valLen > 0)
										{
											sb.AppendUTF8Char(' ');
											sb.AppendC(item->valBuff, item->valLen);
											sb.RTrim();
										}
									}
									reading->name->Release();
									reading->name = Text::String::New(sb.ToString(), sb.GetLength());
								}
							}
							else
							{
								FreeAllItems(&itemList);
								break;
							}
							FreeAllItems(&itemList);
							i++;
						}
					}
				}
			}
			if (!found)
			{
				pduSize = Net::ASN1Util::OIDText2PDU(UTF8STRC("1.3.6.1.4.1.1602"), oidPDU); //Canon
				if (Net::ASN1Util::OIDStartsWith(agent->objId, agent->objIdLen, oidPDU, pduSize))
				{
					Int32 iVal;
					ReadingInfo *reading;

					found = true;
					agent->vendor = Text::String::New(UTF8STRC("Canon")).Ptr();

					err = this->cli->V1GetRequest(addr, community, UTF8STRC("1.3.6.1.4.1.1602.1.1.1.2.0"), &itemList); //enclosureSlot.1
					if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
					{
						item = itemList.GetItem(0);
						if (item->valType == 4 && item->valLen > 0)
						{
							agent->model = Text::String::New(item->valBuff, item->valLen).Ptr();
						}
					}
					FreeAllItems(&itemList);

					i = 1;
					while (true)
					{
						sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("1.3.6.1.4.1.1602.1.11.2.1.1.3.")), i);
						err = this->cli->V1GetRequest(addr, community, sbuff, (UOSInt)(sptr - sbuff), &itemList); //
						if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
						{
							item = itemList.GetItem(0);
							if (Net::SNMPUtil::ValueToInt32(item->valType, item->valBuff, item->valLen, &iVal))
							{
								reading = MemAlloc(ReadingInfo, 1);
								reading->name = Text::String::NewEmpty();
								reading->index = i - 1;
								reading->objIdLen = Net::ASN1Util::OIDText2PDU(sbuff, (UOSInt)(sptr - sbuff), reading->objId);
								reading->mulVal = 1.0;
								reading->invVal = -1;
								reading->readingType = SSWR::SMonitor::SAnalogSensor::RT_COUNT;
								reading->valValid = iVal >= 0;
								reading->currVal = iVal;
								agent->readingList->Add(reading);
								FreeAllItems(&itemList);

								sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("1.3.6.1.4.1.1602.1.11.2.1.1.2.")), i);
								err = this->cli->V1GetRequest(addr, community, sbuff, (UOSInt)(sptr - sbuff), &itemList);
								if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
								{
									item = itemList.GetItem(0);
									if (item->valType == 4 && item->valLen > 0)
									{
										reading->name->Release();
										reading->name = Text::String::New(item->valBuff, item->valLen);
									}
								}
								if (reading->name->leng == 0)
								{
									sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Count ")), i);
									reading->name->Release();
									reading->name = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
								}
							}
							FreeAllItems(&itemList);
						}
						else
						{
							FreeAllItems(&itemList);
							break;
						}
						i++;
					}
				}
			}
			if (!found)
			{
				pduSize = Net::ASN1Util::OIDText2PDU(UTF8STRC("1.3.6.1.4.1.26696"), oidPDU); //HP
				if (Net::ASN1Util::OIDStartsWith(agent->objId, agent->objIdLen, oidPDU, pduSize))
				{
					found = true;
					agent->vendor = Text::String::New(UTF8STRC("HP")).Ptr();

					err = this->cli->V1GetRequest(addr, community, UTF8STRC("1.3.6.1.4.1.11.2.4.3.1.10.0"), &itemList); //npSysModelNumber.0
					if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
					{
						item = itemList.GetItem(0);
						if (item->valType == 4 && item->valLen > 0)
						{
							agent->model = Text::String::New(item->valBuff, item->valLen).Ptr();
						}
					}
					FreeAllItems(&itemList);
				}
			}
			if (!found)
			{
				pduSize = Net::ASN1Util::OIDText2PDU(UTF8STRC("1.3.6.1.4.1.3854.1"), oidPDU); //AKCP sensorProbe
				if (Net::ASN1Util::OIDStartsWith(agent->objId, agent->objIdLen, oidPDU, pduSize))
				{
					ReadingInfo *reading;
					Int32 iVal;
					found = true;
					agent->vendor = Text::String::New(UTF8STRC("AKCP")).Ptr();

					err = this->cli->V1GetRequest(addr, community, UTF8STRC("1.3.6.1.4.1.3854.1.1.8.0"), &itemList); //npSysModelNumber.0
					if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
					{
						item = itemList.GetItem(0);
						if (item->valType == 4 && item->valLen > 0)
						{
							agent->model = Text::String::New(item->valBuff, item->valLen).Ptr();
						}
					}
					FreeAllItems(&itemList);

					i = 0;
					while (true)
					{
						sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("1.3.6.1.4.1.3854.1.2.2.1.16.1.5.")), i);
						err = this->cli->V1GetRequest(addr, community, sbuff, (UOSInt)(sptr - sbuff), &itemList); //
						if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
						{
							item = itemList.GetItem(0);
							if (Net::SNMPUtil::ValueToInt32(item->valType, item->valBuff, item->valLen, &iVal))
							{
								if (iVal == 1)
								{
									sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("1.3.6.1.4.1.3854.1.2.2.1.16.1.14.")), i);
									reading = MemAlloc(ReadingInfo, 1);
									reading->name = Text::String::NewEmpty();
									reading->index = i;
									reading->objIdLen = Net::ASN1Util::OIDText2PDU(sbuff, (UOSInt)(sptr - sbuff), reading->objId);
									reading->mulVal = 0.1;
									reading->invVal = -512;
									reading->readingType = SSWR::SMonitor::SAnalogSensor::RT_TEMPERATURE;
									reading->valValid = false;
									reading->currVal = 0;
									agent->readingList->Add(reading);
									FreeAllItems(&itemList);

									err = this->cli->V1GetRequest(addr, community, sbuff, (UOSInt)(sptr - sbuff), &itemList);
									if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
									{
										item = itemList.GetItem(0);
										if (Net::SNMPUtil::ValueToInt32(item->valType, item->valBuff, item->valLen, &iVal))
										{
											reading->currVal = iVal * reading->mulVal;
											reading->valValid = true;
										}
									}
									FreeAllItems(&itemList);

									sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("1.3.6.1.4.1.3854.1.2.2.1.16.1.1.")), i);
									err = this->cli->V1GetRequest(addr, community, sbuff, (UOSInt)(sptr - sbuff), &itemList);
									if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
									{
										item = itemList.GetItem(0);
										if (item->valType == 4 && item->valLen > 0)
										{
											reading->name->Release();
											reading->name = Text::String::New(item->valBuff, item->valLen);
										}
									}
									if (reading->name->leng == 0)
									{
										sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Temperature")), i + 1);
										reading->name->Release();
										reading->name = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
									}
								}
							}
							FreeAllItems(&itemList);
						}
						else
						{
							FreeAllItems(&itemList);
							break;
						}
						i++;
					}

					i = 0;
					while (true)
					{
						sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("1.3.6.1.4.1.3854.1.2.2.1.17.1.5.")), i);
						err = this->cli->V1GetRequest(addr, community, sbuff, (UOSInt)(sptr - sbuff), &itemList); //
						if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
						{
							item = itemList.GetItem(0);
							if (Net::SNMPUtil::ValueToInt32(item->valType, item->valBuff, item->valLen, &iVal))
							{
								if (iVal == 1)
								{
									sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("1.3.6.1.4.1.3854.1.2.2.1.17.1.3.")), i);
									reading = MemAlloc(ReadingInfo, 1);
									reading->name = Text::String::NewEmpty();
									reading->index = i;
									reading->objIdLen = Net::ASN1Util::OIDText2PDU(sbuff, (UOSInt)(sptr - sbuff), reading->objId);
									reading->mulVal = 1.0;
									reading->invVal = -1;
									reading->readingType = SSWR::SMonitor::SAnalogSensor::RT_RHUMIDITY;
									reading->valValid = false;
									reading->currVal = 0;
									agent->readingList->Add(reading);
									FreeAllItems(&itemList);

									err = this->cli->V1GetRequest(addr, community, sbuff, (UOSInt)(sptr - sbuff), &itemList);
									if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
									{
										item = itemList.GetItem(0);
										if (Net::SNMPUtil::ValueToInt32(item->valType, item->valBuff, item->valLen, &iVal))
										{
											reading->currVal = iVal * reading->mulVal;
											reading->valValid = true;
										}
									}
									FreeAllItems(&itemList);

									sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("1.3.6.1.4.1.3854.1.2.2.1.17.1.1.")), i);
									err = this->cli->V1GetRequest(addr, community, sbuff, (UOSInt)(sptr - sbuff), &itemList);
									if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
									{
										item = itemList.GetItem(0);
										if (item->valType == 4 && item->valLen > 0)
										{
											reading->name->Release();
											reading->name = Text::String::New(item->valBuff, item->valLen);
										}
									}
									if (reading->name->leng == 0)
									{
										sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Humidity")), i + 1);
										reading->name->Release();
										reading->name = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
									}
								}
							}
							FreeAllItems(&itemList);
						}
						else
						{
							FreeAllItems(&itemList);
							break;
						}
						i++;
					}

					i = 0;
					while (true)
					{
						sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("1.3.6.1.4.1.3854.1.2.2.1.18.1.4.")), i);
						err = this->cli->V1GetRequest(addr, community, sbuff, (UOSInt)(sptr - sbuff), &itemList); //
						if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
						{
							item = itemList.GetItem(0);
							if (Net::SNMPUtil::ValueToInt32(item->valType, item->valBuff, item->valLen, &iVal))
							{
								if (iVal == 1)
								{
									sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("1.3.6.1.4.1.3854.1.2.2.1.18.1.3.")), i);
									reading = MemAlloc(ReadingInfo, 1);
									reading->name = Text::String::NewEmpty();
									reading->index = i;
									reading->objIdLen = Net::ASN1Util::OIDText2PDU(sbuff, (UOSInt)(sptr - sbuff), reading->objId);
									reading->mulVal = 1.0;
									reading->invVal = -1;
									reading->readingType = SSWR::SMonitor::SAnalogSensor::RT_ONOFF;
									reading->valValid = false;
									reading->currVal = 0;
									agent->readingList->Add(reading);
									FreeAllItems(&itemList);

									err = this->cli->V1GetRequest(addr, community, sbuff, (UOSInt)(sptr - sbuff), &itemList);
									if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
									{
										item = itemList.GetItem(0);
										if (Net::SNMPUtil::ValueToInt32(item->valType, item->valBuff, item->valLen, &iVal))
										{
											reading->currVal = iVal * reading->mulVal;
											reading->valValid = true;
										}
									}
									FreeAllItems(&itemList);

									sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("1.3.6.1.4.1.3854.1.2.2.1.18.1.1.")), i);
									err = this->cli->V1GetRequest(addr, community, sbuff, (UOSInt)(sptr - sbuff), &itemList);
									if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
									{
										item = itemList.GetItem(0);
										if (item->valType == 4 && item->valLen > 0)
										{
											reading->name->Release();
											reading->name = Text::String::New(item->valBuff, item->valLen);
										}
									}
									if (reading->name->leng == 0)
									{
										sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("Dry Contact Switch")), i + 1);
										reading->name->Release();
										reading->name = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
									}
								}
							}
							FreeAllItems(&itemList);
						}
						else
						{
							FreeAllItems(&itemList);
							break;
						}
						i++;
					}
				}
			}
			if (!found)
			{
				pduSize = Net::ASN1Util::OIDText2PDU(UTF8STRC("1.3.6.1.4.1.311.1.1.3.1.1"), oidPDU); //workstation (Windows NT)
				if (Net::ASN1Util::OIDStartsWith(agent->objId, agent->objIdLen, oidPDU, pduSize))
				{
					found = true;
					agent->vendor = Text::String::New(UTF8STRC("Microsoft")).Ptr();

					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("WindowsNT"));

					err = this->cli->V1GetRequest(addr, community, UTF8STRC("1.3.6.1.4.1.77.1.1.1.0"), &itemList); //comVersionMaj.0
					if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
					{
						item = itemList.GetItem(0);
						if (item->valType == 4 && item->valLen > 0)
						{
							sb.AppendUTF8Char(' ');
							sb.AppendC(item->valBuff, item->valLen);
						}
					}
					FreeAllItems(&itemList);

					err = this->cli->V1GetRequest(addr, community, UTF8STRC("1.3.6.1.4.1.77.1.1.2.0"), &itemList); //comVersionMin.0
					if (err == Net::SNMPUtil::ES_NOERROR && itemList.GetCount() == 1)
					{
						item = itemList.GetItem(0);
						if (item->valType == 4 && item->valLen > 0)
						{
							sb.AppendUTF8Char('.');
							sb.AppendC(item->valBuff, item->valLen);
						}
					}
					FreeAllItems(&itemList);
					agent->model = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
				}
			}
		}
	}
	return agent;
}

UOSInt Net::SNMPManager::AddAgents(const Net::SocketUtil::AddressInfo *addr, NotNullPtr<Text::String> community, Data::ArrayList<AgentInfo*> *agentList, Bool scanIP)
{
	AgentInfo *agent;
	UOSInt ret = 0;
	if (addr->addrType == Net::AddrType::IPv4)
	{
		Net::IPType ipType = Net::SocketUtil::GetIPv4Type(ReadNUInt32(addr->addr));
		if (ipType == Net::IPType::Broadcast)
		{
			Net::SocketUtil::AddressInfo *agentAddr;
			Data::ArrayList<Net::SocketUtil::AddressInfo *> addrList;
			UOSInt i;
			UOSInt j;
			this->cli->V1ScanGetRequest(addr, community, UTF8STRC("1.3.6.1.2.1.1.1.0"), &addrList, 3000, scanIP);
			i = 0;
			j = addrList.GetCount();
			while (i < j)
			{
				agentAddr = addrList.GetItem(i);
				agent = this->AddAgent(agentAddr, community);
				if (agent)
				{
					agentList->Add(agent);
					ret++;
				}
				MemFree(agentAddr);
				i++;
			}
			return ret;
		}
	}
	agent = this->AddAgent(addr, community);
	if (agent)
	{
		agentList->Add(agent);
		ret++;
	}
	return ret;
}

void Net::SNMPManager::Agent2Record(const AgentInfo *agent, SSWR::SMonitor::ISMonitorCore::DevRecord2 *rec, Int64 *cliId)
{
	*cliId = Agent2CliId(agent);

	Data::FastMap<UInt32, Int32> readingIdMap;
	Int32 currId;
	ReadingInfo *reading;
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	rec->recTime = dt.ToTicks();
	rec->recvTime = rec->recTime;
	rec->ndigital = 0;
	rec->nreading = agent->readingList->GetCount();
	rec->nOutput = 0;
	rec->profileId = 5;
	rec->digitalVals = 0;
	MemClear(rec->readings, sizeof(SSWR::SMonitor::ISMonitorCore::ReadingInfo) * SMONITORCORE_DEVREADINGCNT);
	UOSInt i = 0;
	UOSInt j = agent->readingList->GetCount();
	while (i < j)
	{
		reading = agent->readingList->GetItem(i);
		WriteInt16(&rec->readings[i].status[0], reading->index);
		WriteInt16(&rec->readings[i].status[2], SSWR::SMonitor::SAnalogSensor::ST_SNMP);
		currId = readingIdMap.Get((UInt32)reading->index);
		WriteInt16(&rec->readings[i].status[4], currId);
		readingIdMap.Put((UInt32)reading->index, currId + 1);
		if (reading->valValid)
		{
			WriteInt16(&rec->readings[i].status[6], reading->readingType);
		}
		else
		{
			WriteInt16(&rec->readings[i].status[6], 0);
		}
		rec->readings[i].reading = reading->currVal;
		i++;
	}
}

Int64 Net::SNMPManager::Agent2CliId(const AgentInfo *agent)
{
	UInt8 ibuff[8];
	WriteMInt16(ibuff, 161);
	ibuff[2] = agent->mac[0];
	ibuff[3] = agent->mac[1];
	ibuff[4] = agent->mac[2];
	ibuff[5] = agent->mac[3];
	ibuff[6] = agent->mac[4];
	ibuff[7] = agent->mac[5];
	return ReadMInt64(ibuff);	
}
