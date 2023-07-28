#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/BufferedOutputStream.h"
#include "IO/FileStream.h"
#include "Net/WiFiLogFile.h"
#include "Net/WirelessLAN.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"

UOSInt Net::WiFiLogFile::DirectInsert(LogFileEntry *newLog)
{
	Net::WiFiLogFile::LogFileEntry *log;
	OSInt i;
	OSInt j;
	OSInt k;
	i = 0;
	j = (OSInt)this->logList.GetCount() - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		log = this->logList.GetItem((UOSInt)k);
		if (newLog->macInt > log->macInt)
		{
			i = k + 1;
		}
		else if (newLog->macInt < log->macInt)
		{
			j = k - 1;
		}
		else
		{
			this->logList.Insert((UOSInt)k, newLog);
			return (UOSInt)k;
		}
	}
	this->logList.Insert((UOSInt)i, newLog);
	return (UOSInt)i;
}

Net::WiFiLogFile::WiFiLogFile()
{
}

Net::WiFiLogFile::~WiFiLogFile()
{
	this->Clear();
}

void Net::WiFiLogFile::LoadFile(Text::CString fileName)
{
	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		Text::PString sarr[12];
		Text::PString sarr2[7];
		UInt8 buff[8];
		UOSInt i;
		UOSInt j;
		OSInt k;
		Net::WiFiLogFile::LogFileEntry *log;
		Text::StringBuilderUTF8 sb;
		UInt64 iMAC;
		Text::UTF8Reader reader(fs);
		sb.ClearStr();
		buff[0] = 0;
		buff[1] = 0;
		while (reader.ReadLine(sb, 4096))
		{
			i = Text::StrSplitP(sarr, 12, sb, '\t');
			if (i == 4 || i == 7 || i == 9 || i == 10 || i == 11)
			{
				if (Text::StrSplitP(sarr2, 7, sarr[0], ':') == 6)
				{
					buff[2] = Text::StrHex2UInt8C(sarr2[0].v);
					buff[3] = Text::StrHex2UInt8C(sarr2[1].v);
					buff[4] = Text::StrHex2UInt8C(sarr2[2].v);
					buff[5] = Text::StrHex2UInt8C(sarr2[3].v);
					buff[6] = Text::StrHex2UInt8C(sarr2[4].v);
					buff[7] = Text::StrHex2UInt8C(sarr2[5].v);
					iMAC = ReadMUInt64(buff);
					log = (Net::WiFiLogFile::LogFileEntry*)this->Get(iMAC);
					if (log)
					{
						if (i >= 7)
						{
							if ((log->manuf == 0 || log->manuf->v[0] == 0) && sarr[4].v[0] != 0)
							{
								SDEL_STRING(log->manuf);
								log->manuf = Text::String::New(sarr[4].v, sarr[4].leng).Ptr();
							}
							if ((log->model == 0 || log->model->v[0] == 0) && sarr[5].v[0] != 0)
							{
								SDEL_STRING(log->model);
								log->model = Text::String::New(sarr[5].v, sarr[5].leng).Ptr();
							}
							if ((log->serialNum == 0 || log->serialNum->v[0] == 0) && sarr[6].v[0] != 0)
							{
								SDEL_STRING(log->serialNum);
								log->serialNum = Text::String::New(sarr[6].v, sarr[6].leng).Ptr();
							}
						}
						if (i >= 9)
						{
							if ((log->country == 0 || log->country->v[0] == 0) && sarr[8].v[0] != 0)
							{
								SDEL_STRING(log->country);
								log->country = Text::String::New(sarr[8].v, sarr[8].leng).Ptr();
							}
							j = Text::StrSplitP(sarr2, 3, sarr[7], ',');
							while (j-- > 0)
							{
								if (sarr2[j].leng == 6)
								{
									Text::StrHex2Bytes(sarr2[j].v, &buff[2]);
									k = 0;
									while (k < 3)
									{
										if (buff[2] == log->ouis[k][0] && buff[3] == log->ouis[k][1] && buff[4] == log->ouis[k][2])
										{
											break;
										}
										else if (log->ouis[k][0] == 0 && log->ouis[k][1] == 0 && log->ouis[k][2] == 2)
										{
											log->ouis[k][0] = buff[2];
											log->ouis[k][1] = buff[3];
											log->ouis[k][2] = buff[4];
											break;
										}
										k++;
									}
								}
							}
						}
						if (i >= 10)
						{
							sarr2[1] = sarr[9];
							if (sarr2[1].v[0])
							{
								UInt64 iMAC;
								while (true)
								{
									j = Text::StrSplitP(sarr2, 2, sarr2[1], ',');
									iMAC = Text::StrHex2UInt64C(sarr2[0].v);
									k = 0;
									while (k < 20)
									{
										if (log->neighbour[k] == 0)
										{
											log->neighbour[k] = iMAC;
											break;
										}
										else if ((log->neighbour[k] & 0xFFFFFFFFFFFFLL) == (iMAC & 0xFFFFFFFFFFFFLL))
										{
											if ((Int8)((log->neighbour[k] >> 48) & 0xff) < (Int8)((iMAC >> 48) & 0xff))
											{
												log->neighbour[k] = iMAC;
											}
											break;
										}
										k++;
									}
									if (j != 2)
									{
										break;
									}
								}
							}
						}
						if (i >= 11)
						{
							UInt32 ieLen = (UInt32)(sarr[10].leng >> 1);
							if (ieLen > log->ieLen)
							{
								log->ieLen = ieLen;
								if (log->ieBuff)
								{
									MemFree(log->ieBuff);
								}
								log->ieBuff = MemAlloc(UInt8, log->ieLen);
								Text::StrHex2Bytes(sarr[10].v, log->ieBuff);
							}
						}
					}
					else
					{
						log = MemAlloc(Net::WiFiLogFile::LogFileEntry, 1);
						MemClear(log->neighbour, sizeof(log->neighbour));
						log->lastScanTime = 0;
						log->lastRSSI = 0;
						log->mac[0] = buff[2];
						log->mac[1] = buff[3];
						log->mac[2] = buff[4];
						log->mac[3] = buff[5];
						log->mac[4] = buff[6];
						log->mac[5] = buff[7];
						log->macInt = iMAC;
						log->ssid = Text::String::New(sarr[1].v, sarr[1].leng);
						log->phyType = Text::StrToInt32(sarr[2].v);
						log->freq = Text::StrToDouble(sarr[3].v);
						if (i >= 7)
						{
							log->manuf = Text::String::New(sarr[4].v, sarr[4].leng).Ptr();
							log->model = Text::String::New(sarr[5].v, sarr[5].leng).Ptr();
							log->serialNum = Text::String::New(sarr[6].v, sarr[6].leng).Ptr();
						}
						else
						{
							log->manuf = 0;
							log->model = 0;
							log->serialNum = 0;
						}
						j = 3;
						while (j-- > 0)
						{
							log->ouis[j][0] = 0;
							log->ouis[j][1] = 0;
							log->ouis[j][2] = 0;
						}
						if (i >= 9)
						{
							log->country = Text::String::New(sarr[8].v, sarr[8].leng).Ptr();
							j = Text::StrSplitP(sarr2, 3, sarr[7], ',');
							while (j-- > 0)
							{
								if (sarr2[j].leng == 6)
								{
									Text::StrHex2Bytes(sarr2[j].v, log->ouis[j]);
								}
							}
						}
						else
						{
							log->country = 0;
						}
						if (i >= 10)
						{
							sarr2[1] = sarr[9];
							if (sarr2[1].v[0])
							{
								j = 0;
								while (Text::StrSplitP(sarr2, 2, sarr2[1], ',') == 2)
								{
									log->neighbour[j] = Text::StrHex2UInt64C(sarr2[0].v);
									j++;
								}
								log->neighbour[j] = Text::StrHex2UInt64C(sarr2[0].v);
							}
						}
						if (i >= 11)
						{
							log->ieLen = (UInt32)(sarr[10].leng >> 1);
							if (log->ieLen > 0)
							{
								log->ieBuff = MemAlloc(UInt8, log->ieLen);
								Text::StrHex2Bytes(sarr[10].v, log->ieBuff);
							}
							else
							{
								log->ieBuff = 0;
							}
						}
						else
						{
							log->ieBuff = 0;
							log->ieLen = 0;
						}
						
						this->DirectInsert(log);
					}
				}
			}
			sb.ClearStr();
		}
	}
}

Bool Net::WiFiLogFile::StoreFile(Text::CString fileName)
{
	Text::StringBuilderUTF8 sb;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Net::WiFiLogFile::LogFileEntry *log;
	Bool succ = false;
	IO::FileStream fs(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	{
		IO::BufferedOutputStream cstm(fs, 8192);
		Text::UTF8Writer writer(cstm);
		succ = true;
		i = 0;
		j = this->logList.GetCount();
		while (i < j)
		{
			log = this->logList.GetItem(i);
			sb.ClearStr();
			sb.AppendHexBuff(log->mac, 6, ':', Text::LineBreakType::None);
			sb.AppendC(UTF8STRC("\t"));
			sb.Append(log->ssid);
			sb.AppendC(UTF8STRC("\t"));
			sb.AppendI32(log->phyType);
			sb.AppendC(UTF8STRC("\t"));
			Text::SBAppendF64(&sb, log->freq);
			sb.AppendC(UTF8STRC("\t"));
			if (log->manuf)
			{
				sb.Append(log->manuf);
			}
			sb.AppendC(UTF8STRC("\t"));
			if (log->model)
			{
				sb.Append(log->model);
			}
			sb.AppendC(UTF8STRC("\t"));
			if (log->serialNum)
			{
				sb.Append(log->serialNum);
			}
			sb.AppendC(UTF8STRC("\t"));
			sb.AppendHexBuff(log->ouis[0], 3, 0, Text::LineBreakType::None);
			sb.AppendUTF8Char(',');
			sb.AppendHexBuff(log->ouis[1], 3, 0, Text::LineBreakType::None);
			sb.AppendUTF8Char(',');
			sb.AppendHexBuff(log->ouis[2], 3, 0, Text::LineBreakType::None);
			sb.AppendC(UTF8STRC("\t"));
			if (log->country)
			{
				sb.Append(log->country);
			}
			sb.AppendUTF8Char('\t');
			k = 0;
			while (k < 20)
			{
				if (log->neighbour[k] == 0)
				{
					break;
				}
				if (k > 0)
				{
					sb.AppendUTF8Char(',');
				}
				sb.AppendHex64(log->neighbour[k]);
				k++;
			}
			sb.AppendUTF8Char('\t');
			if (log->ieLen > 0)
			{
				sb.AppendHexBuff(log->ieBuff, log->ieLen, 0, Text::LineBreakType::None);
			}
			if (!writer.WriteLineC(sb.ToString(), sb.GetLength()))
			{
				succ = false;
			}
			i++;
		}
	}
	return succ;
}

void Net::WiFiLogFile::Clear()
{
	UOSInt i = this->logList.GetCount();
	Net::WiFiLogFile::LogFileEntry *log;
	while (i-- > 0)
	{
		log = this->logList.GetItem(i);
		log->ssid->Release();
		SDEL_STRING(log->manuf);
		SDEL_STRING(log->model);
		SDEL_STRING(log->serialNum);
		SDEL_STRING(log->country);
		if (log->ieBuff)
		{
			MemFree(log->ieBuff);
		}
		MemFree(log);
	}
	this->logList.Clear();
}

Net::WiFiLogFile::LogFileEntry *Net::WiFiLogFile::Get(UInt64 iMAC)
{
	Net::WiFiLogFile::LogFileEntry *log;
	OSInt i;
	OSInt j;
	OSInt k;
	i = 0;
	j = (OSInt)this->logList.GetCount() - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		log = this->logList.GetItem((UOSInt)k);
		if (iMAC > log->macInt)
		{
			i = k + 1;
		}
		else if (iMAC < log->macInt)
		{
			j = k - 1;
		}
		else
		{
			return log;
		}
	}
	return 0;
}

OSInt Net::WiFiLogFile::GetIndex(UInt64 iMAC)
{
	Net::WiFiLogFile::LogFileEntry *log;
	OSInt i;
	OSInt j;
	OSInt k;
	i = 0;
	j = (OSInt)this->logList.GetCount() - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		log = this->logList.GetItem((UOSInt)k);
		if (iMAC > log->macInt)
		{
			i = k + 1;
		}
		else if (iMAC < log->macInt)
		{
			j = k - 1;
		}
		else
		{
			return k;
		}
	}
	return ~i;
}

NotNullPtr<Data::ArrayList<Net::WiFiLogFile::LogFileEntry*>> Net::WiFiLogFile::GetLogList()
{
	return this->logList;
}

const Net::WiFiLogFile::LogFileEntry *Net::WiFiLogFile::GetItem(UOSInt index)
{
	return this->logList.GetItem(index);
}

Net::WiFiLogFile::LogFileEntry *Net::WiFiLogFile::AddBSSInfo(Net::WirelessLAN::BSSInfo *bss, OSInt *lastIndex)
{
	UInt8 buff[8];
	UInt64 imac;
	UOSInt k;
	UOSInt l;
	UOSInt m;
	UOSInt ieLen;
	const UInt8 *ieBuff;
	Net::WirelessLANIE *ie;
	MemCopyNO(&buff[2], bss->GetMAC(), 6);
	buff[0] = 0;
	buff[1] = 0;
	imac = ReadMUInt64(buff);
	Net::WiFiLogFile::LogFileEntry *log = (Net::WiFiLogFile::LogFileEntry*)this->Get(imac);
	const UInt8 *oui1 = bss->GetChipsetOUI(0);
	const UInt8 *oui2 = bss->GetChipsetOUI(1);
	const UInt8 *oui3 = bss->GetChipsetOUI(2);
	ieLen = 0;
	k = bss->GetIECount();
	while (k-- > 0)
	{
		ie = bss->GetIE(k);
		ieLen += (UOSInt)ie->GetIEBuff()[1] + 2;
	}
	if (log == 0)
	{
		log = MemAlloc(Net::WiFiLogFile::LogFileEntry, 1);
		log->lastScanTime = 0;
		MemClear(log->neighbour, sizeof(log->neighbour));
		MemCopyNO(log->mac, &buff[2], 6);
		log->macInt = imac;
		log->ssid = bss->GetSSID()->Clone();
		log->phyType = bss->GetPHYType();
		log->freq = bss->GetFreq();
		log->manuf = SCOPY_STRING(bss->GetManuf());
		log->model =  SCOPY_STRING(bss->GetModel());
		log->serialNum =  SCOPY_STRING(bss->GetSN());
		log->country =  Text::String::NewOrNullSlow(bss->GetCountry());
		log->lastRSSI = bss->GetRSSI();
		log->ouis[0][0] = oui1[0];
		log->ouis[0][1] = oui1[1];
		log->ouis[0][2] = oui1[2];
		log->ouis[1][0] = oui2[0];
		log->ouis[1][1] = oui2[1];
		log->ouis[1][2] = oui2[2];
		log->ouis[2][0] = oui3[0];
		log->ouis[2][1] = oui3[1];
		log->ouis[2][2] = oui3[2];
		log->ieLen = (UInt32)ieLen;
		if (ieLen > 0)
		{
			log->ieBuff = MemAlloc(UInt8, ieLen);
			k = 0;
			l = bss->GetIECount();
			m = 0;
			while (k < l)
			{
				ie = bss->GetIE(k);
				ieBuff = ie->GetIEBuff();
				MemCopyNO(&log->ieBuff[m], ieBuff, (UOSInt)ieBuff[1] + 2);
				m += (UOSInt)ieBuff[1] + 2;
				k++;
			}
		}
		else
		{
			log->ieBuff = 0;
		}
		DirectInsert(log);
		*lastIndex = -1;
		return log;
	}
	else
	{
		OSInt sk = this->GetIndex(imac);
		if (sk >= 0)
		{
			if (log->manuf == 0 && bss->GetManuf())
			{
				log->manuf = bss->GetManuf()->Clone().Ptr();
			}
			if (log->model == 0 && bss->GetModel())
			{
				log->model = bss->GetModel()->Clone().Ptr();
			}
			if (log->serialNum == 0 && bss->GetSN())
			{
				log->serialNum = bss->GetSN()->Clone().Ptr();
			}
			if (log->country == 0 && bss->GetCountry())
			{
				log->country = Text::String::NewNotNullSlow(bss->GetCountry()).Ptr();
			}
		}
		UOSInt l;
		const UInt8 *oui;
		oui = oui1;
		if (oui[0] != 0 || oui[1] != 0 || oui[2] != 0)
		{
			l = 0;
			while (l < 3)
			{
				if (log->ouis[l][0] == oui[0] && log->ouis[l][1] == oui[1] && log->ouis[l][2] == oui[2])
				{
					break;
				}
				else if (log->ouis[l][0] == 0 && log->ouis[l][1] == 0 && log->ouis[l][2] == 0)
				{
					log->ouis[l][0] = oui[0];
					log->ouis[l][1] = oui[1];
					log->ouis[l][2] = oui[2];
				}
				l++;
			}
		}

		oui = oui2;
		if (oui[0] != 0 || oui[1] != 0 || oui[2] != 0)
		{
			l = 0;
			while (l < 3)
			{
				if (log->ouis[l][0] == oui[0] && log->ouis[l][1] == oui[1] && log->ouis[l][2] == oui[2])
				{
					break;
				}
				else if (log->ouis[l][0] == 0 && log->ouis[l][1] == 0 && log->ouis[l][2] == 0)
				{
					log->ouis[l][0] = oui[0];
					log->ouis[l][1] = oui[1];
					log->ouis[l][2] = oui[2];
				}
				l++;
			}
		}

		oui = oui3;
		if (oui[0] != 0 || oui[1] != 0 || oui[2] != 0)
		{
			l = 0;
			while (l < 3)
			{
				if (log->ouis[l][0] == oui[0] && log->ouis[l][1] == oui[1] && log->ouis[l][2] == oui[2])
				{
					break;
				}
				else if (log->ouis[l][0] == 0 && log->ouis[l][1] == 0 && log->ouis[l][2] == 0)
				{
					log->ouis[l][0] = oui[0];
					log->ouis[l][1] = oui[1];
					log->ouis[l][2] = oui[2];
				}
				l++;
			}
		}

		if (ieLen > log->ieLen)
		{
			if (log->ieBuff)
			{
				MemFree(log->ieBuff);
			}
			log->ieBuff = MemAlloc(UInt8, ieLen);
			k = 0;
			l = bss->GetIECount();
			m = 0;
			while (k < l)
			{
				ie = bss->GetIE(k);
				ieBuff = ie->GetIEBuff();
				MemCopyNO(&log->ieBuff[m], ieBuff, (UOSInt)ieBuff[1] + 2);
				m += (UOSInt)ieBuff[1] + 2;
				k++;
			}
		}
		*lastIndex = sk;
		return log;
	}
}
