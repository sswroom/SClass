#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "IO/BufferedOutputStream.h"
#include "IO/FileStream.h"
#include "Net/WiFiLogFile.h"
#include "Net/WirelessLAN.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"

UIntOS Net::WiFiLogFile::DirectInsert(NN<LogFileEntry> newLog)
{
	NN<Net::WiFiLogFile::LogFileEntry> log;
	IntOS i;
	IntOS j;
	IntOS k;
	i = 0;
	j = (IntOS)this->logList.GetCount() - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		log = this->logList.GetItemNoCheck((UIntOS)k);
		if (newLog->mac64Int > log->mac64Int)
		{
			i = k + 1;
		}
		else if (newLog->mac64Int < log->mac64Int)
		{
			j = k - 1;
		}
		else
		{
			this->logList.Insert((UIntOS)k, newLog);
			return (UIntOS)k;
		}
	}
	this->logList.Insert((UIntOS)i, newLog);
	return (UIntOS)i;
}

Net::WiFiLogFile::WiFiLogFile()
{
}

Net::WiFiLogFile::~WiFiLogFile()
{
	this->Clear();
}

void Net::WiFiLogFile::LoadFile(Text::CStringNN fileName)
{
	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		Text::PString sarr[12];
		Text::PString sarr2[7];
		UInt8 buff[8];
		UIntOS i;
		UIntOS j;
		IntOS k;
		NN<Net::WiFiLogFile::LogFileEntry> log;
		Text::StringBuilderUTF8 sb;
		UInt64 iMAC;
		Text::UTF8Reader reader(fs);
		NN<Text::String> s;
		UnsafeArray<UInt8> wieBuff;
		sb.ClearStr();
		buff[6] = 0;
		buff[7] = 0;
		while (reader.ReadLine(sb, 4096))
		{
			i = Text::StrSplitP(sarr, 12, sb, '\t');
			if (i == 4 || i == 7 || i == 9 || i == 10 || i == 11)
			{
				if (Text::StrSplitP(sarr2, 7, sarr[0], ':') == 6)
				{
					buff[0] = Text::StrHex2UInt8C(sarr2[0].v);
					buff[1] = Text::StrHex2UInt8C(sarr2[1].v);
					buff[2] = Text::StrHex2UInt8C(sarr2[2].v);
					buff[3] = Text::StrHex2UInt8C(sarr2[3].v);
					buff[4] = Text::StrHex2UInt8C(sarr2[4].v);
					buff[5] = Text::StrHex2UInt8C(sarr2[5].v);
					iMAC = ReadMUInt64(buff);
					if (this->Get(iMAC).SetTo(log))
					{
						if (i >= 7)
						{
							if ((!log->manuf.SetTo(s) || s->v[0] == 0) && sarr[4].v[0] != 0)
							{
								OPTSTR_DEL(log->manuf);
								log->manuf = Text::String::New(sarr[4].v, sarr[4].leng);
							}
							if ((!log->model.SetTo(s) || s->v[0] == 0) && sarr[5].v[0] != 0)
							{
								OPTSTR_DEL(log->model);
								log->model = Text::String::New(sarr[5].v, sarr[5].leng);
							}
							if ((!log->serialNum.SetTo(s) || s->v[0] == 0) && sarr[6].v[0] != 0)
							{
								OPTSTR_DEL(log->serialNum);
								log->serialNum = Text::String::New(sarr[6].v, sarr[6].leng);
							}
						}
						if (i >= 9)
						{
							if (!log->country.SetTo(s) && sarr[8].v[0] != 0)
							{
								log->country = Text::String::New(sarr[8].v, sarr[8].leng);
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
										else if ((log->neighbour[k] & 0xFFFFFFFFFFFF0000LL) == (iMAC & 0xFFFFFFFFFFFF0000LL))
										{
											if ((Int8)(log->neighbour[k] & 0xff) < (Int8)(iMAC & 0xff))
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
								if (log->ieBuff.SetTo(wieBuff))
								{
									MemFreeArr(wieBuff);
								}
								log->ieBuff = wieBuff = MemAllocArr(UInt8, log->ieLen);
								Text::StrHex2Bytes(sarr[10].v, wieBuff);
							}
						}
					}
					else
					{
						log = MemAllocNN(Net::WiFiLogFile::LogFileEntry);
						MemClear(log->neighbour, sizeof(log->neighbour));
						log->lastScanTime = 0;
						log->lastRSSI = 0;
						log->mac[0] = buff[0];
						log->mac[1] = buff[1];
						log->mac[2] = buff[2];
						log->mac[3] = buff[3];
						log->mac[4] = buff[4];
						log->mac[5] = buff[5];
						log->mac64Int = iMAC;
						log->ssid = Text::String::New(sarr[1].v, sarr[1].leng);
						log->phyType = Text::StrToInt32(sarr[2].v);
						log->freq = Text::StrToDoubleOrNAN(sarr[3].v);
						if (i >= 7)
						{
							log->manuf = Text::String::New(sarr[4].v, sarr[4].leng).Ptr();
							log->model = Text::String::New(sarr[5].v, sarr[5].leng).Ptr();
							log->serialNum = Text::String::New(sarr[6].v, sarr[6].leng).Ptr();
						}
						else
						{
							log->manuf = nullptr;
							log->model = nullptr;
							log->serialNum = nullptr;
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
							log->country = nullptr;
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
								log->ieBuff = wieBuff = MemAllocArr(UInt8, log->ieLen);
								Text::StrHex2Bytes(sarr[10].v, wieBuff);
							}
							else
							{
								log->ieBuff = nullptr;
							}
						}
						else
						{
							log->ieBuff = nullptr;
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

Bool Net::WiFiLogFile::StoreFile(Text::CStringNN fileName)
{
	Text::StringBuilderUTF8 sb;
	UIntOS i;
	UIntOS j;
	UIntOS k;
	NN<Net::WiFiLogFile::LogFileEntry> log;
	UnsafeArray<UInt8> wieBuff;
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
			log = this->logList.GetItemNoCheck(i);
			sb.ClearStr();
			sb.AppendHexBuff(log->mac, 6, ':', Text::LineBreakType::None);
			sb.AppendC(UTF8STRC("\t"));
			sb.Append(log->ssid);
			sb.AppendC(UTF8STRC("\t"));
			sb.AppendI32(log->phyType);
			sb.AppendC(UTF8STRC("\t"));
			sb.AppendDouble(log->freq);
			sb.AppendC(UTF8STRC("\t"));
			sb.AppendOpt(log->manuf);
			sb.AppendC(UTF8STRC("\t"));
			sb.AppendOpt(log->model);
			sb.AppendC(UTF8STRC("\t"));
			sb.AppendOpt(log->serialNum);
			sb.AppendC(UTF8STRC("\t"));
			sb.AppendHexBuff(log->ouis[0], 3, 0, Text::LineBreakType::None);
			sb.AppendUTF8Char(',');
			sb.AppendHexBuff(log->ouis[1], 3, 0, Text::LineBreakType::None);
			sb.AppendUTF8Char(',');
			sb.AppendHexBuff(log->ouis[2], 3, 0, Text::LineBreakType::None);
			sb.AppendC(UTF8STRC("\t"));
			sb.AppendOpt(log->country);
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
			if (log->ieLen > 0 && log->ieBuff.SetTo(wieBuff))
			{
				sb.AppendHexBuff(wieBuff, log->ieLen, 0, Text::LineBreakType::None);
			}
			if (!writer.WriteLine(sb.ToCString()))
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
	UIntOS i = this->logList.GetCount();
	UnsafeArray<UInt8> wieBuff;
	NN<Net::WiFiLogFile::LogFileEntry> log;
	while (i-- > 0)
	{
		log = this->logList.GetItemNoCheck(i);
		log->ssid->Release();
		OPTSTR_DEL(log->manuf);
		OPTSTR_DEL(log->model);
		OPTSTR_DEL(log->serialNum);
		OPTSTR_DEL(log->country);
		if (log->ieBuff.SetTo(wieBuff))
		{
			MemFreeArr(wieBuff);
		}
		MemFreeNN(log);
	}
	this->logList.Clear();
}

Optional<Net::WiFiLogFile::LogFileEntry> Net::WiFiLogFile::Get(UInt64 iMAC)
{
	NN<Net::WiFiLogFile::LogFileEntry> log;
	IntOS i;
	IntOS j;
	IntOS k;
	i = 0;
	j = (IntOS)this->logList.GetCount() - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		log = this->logList.GetItemNoCheck((UIntOS)k);
		if (iMAC > log->mac64Int)
		{
			i = k + 1;
		}
		else if (iMAC < log->mac64Int)
		{
			j = k - 1;
		}
		else
		{
			return log;
		}
	}
	return nullptr;
}

IntOS Net::WiFiLogFile::GetIndex(UInt64 iMAC)
{
	NN<Net::WiFiLogFile::LogFileEntry> log;
	IntOS i;
	IntOS j;
	IntOS k;
	i = 0;
	j = (IntOS)this->logList.GetCount() - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		log = this->logList.GetItemNoCheck((UIntOS)k);
		if (iMAC > log->mac64Int)
		{
			i = k + 1;
		}
		else if (iMAC < log->mac64Int)
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

NN<Data::ArrayListNN<Net::WiFiLogFile::LogFileEntry>> Net::WiFiLogFile::GetLogList()
{
	return this->logList;
}

Optional<const Net::WiFiLogFile::LogFileEntry> Net::WiFiLogFile::GetItem(UIntOS index)
{
	return this->logList.GetItem(index);
}

NN<Net::WiFiLogFile::LogFileEntry> Net::WiFiLogFile::AddBSSInfo(NN<Net::WirelessLAN::BSSInfo> bss, OutParam<IntOS> lastIndex)
{
	UInt8 buff[8];
	UInt64 imac;
	UIntOS k;
	UIntOS l;
	UIntOS m;
	UIntOS ieLen;
	UnsafeArray<const UInt8> ieBuff;
	UnsafeArray<UInt8> wieBuff;
	NN<Net::WirelessLANIE> ie;
	NN<Text::String> s;
	MemCopyNO(&buff[0], bss->GetMAC().Ptr(), 6);
	buff[6] = 0;
	buff[7] = 0;
	imac = ReadMUInt64(buff);
	NN<Net::WiFiLogFile::LogFileEntry> log;
	const UInt8 tmpOUI[] = {0, 0, 0};
	UnsafeArray<const UInt8> oui1 = bss->GetChipsetOUI(0).Or(tmpOUI);
	UnsafeArray<const UInt8> oui2 = bss->GetChipsetOUI(1).Or(tmpOUI);
	UnsafeArray<const UInt8> oui3 = bss->GetChipsetOUI(2).Or(tmpOUI);
	ieLen = 0;
	k = bss->GetIECount();
	while (k-- > 0)
	{
		if (bss->GetIE(k).SetTo(ie))
		{
			ieLen += (UIntOS)ie->GetIEBuff()[1] + 2;
		}
	}
	if (!this->Get(imac).SetTo(log))
	{
		log = MemAllocNN(Net::WiFiLogFile::LogFileEntry);
		log->lastScanTime = 0;
		MemClear(log->neighbour, sizeof(log->neighbour));
		MemCopyNO(log->mac, &buff[0], 6);
		log->mac64Int = imac;
		log->ssid = bss->GetSSID()->Clone();
		log->phyType = bss->GetPHYType();
		log->freq = bss->GetFreq();
		log->manuf = Text::String::CopyOrNull(bss->GetManuf());
		log->model =  Text::String::CopyOrNull(bss->GetModel());
		log->serialNum =  Text::String::CopyOrNull(bss->GetSN());
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
			log->ieBuff = wieBuff = MemAllocArr(UInt8, ieLen);
			k = 0;
			l = bss->GetIECount();
			m = 0;
			while (k < l)
			{
				if (bss->GetIE(k).SetTo(ie))
				{
					ieBuff = ie->GetIEBuff();
					MemCopyNO(&wieBuff[m], ieBuff.Ptr(), (UIntOS)ieBuff[1] + 2);
					m += (UIntOS)ieBuff[1] + 2;
				}
				k++;
			}
		}
		else
		{
			log->ieBuff = nullptr;
		}
		DirectInsert(log);
		lastIndex.Set(-1);
		return log;
	}
	else
	{
		IntOS sk = this->GetIndex(imac);
		if (sk >= 0)
		{
			if (log->manuf.IsNull() && bss->GetManuf().SetTo(s))
			{
				log->manuf = s->Clone();
			}
			if (log->model.IsNull() && bss->GetModel().SetTo(s))
			{
				log->model = s->Clone();
			}
			if (log->serialNum.IsNull() && bss->GetSN().SetTo(s))
			{
				log->serialNum = s->Clone();
			}
			if (log->country.IsNull() && bss->GetCountry().SetTo(ieBuff))
			{
				log->country = Text::String::NewNotNullSlow(ieBuff);
			}
		}
		UIntOS l;
		UnsafeArray<const UInt8> oui;
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
			if (log->ieBuff.SetTo(wieBuff))
			{
				MemFreeArr(wieBuff);
			}
			log->ieBuff = wieBuff = MemAllocArr(UInt8, ieLen);
			k = 0;
			l = bss->GetIECount();
			m = 0;
			while (k < l)
			{
				if (bss->GetIE(k).SetTo(ie))
				{
					ieBuff = ie->GetIEBuff();
					MemCopyNO(&wieBuff[m], ieBuff.Ptr(), (UIntOS)ieBuff[1] + 2);
					m += (UIntOS)ieBuff[1] + 2;
				}
				k++;
			}
		}
		lastIndex.Set(sk);
		return log;
	}
}
