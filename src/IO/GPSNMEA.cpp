#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/GPSNMEA.h"
#include "Math/Math.h"
#include "Sync/RWMutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"

void IO::GPSNMEA::ParseUnknownCmd(const UTF8Char *cmd)
{
}

IO::GPSNMEA::ParseStatus IO::GPSNMEA::ParseNMEALine(UTF8Char *line, UOSInt lineLen, Map::GPSTrack::GPSRecord3 *record, SateRecord *sateRec)
{
	UTF8Char *sarr[32];
	UOSInt scnt;
	if (lineLen <= 3)
	{
		return ParseStatus::NotNMEA;
	}
	if (line[lineLen - 3] != '*' || line[0] != '$')
	{
		return ParseStatus::NotNMEA;
	}
	else
	{
		UTF8Char c = 0;
		UTF8Char *sptr2 = &line[1];
		while (sptr2 < &line[lineLen - 3])
		{
			c = c ^ *sptr2++;
		}
		if (c != Text::StrHex2UInt8C(&line[lineLen - 2]))
		{
			return ParseStatus::NotNMEA;
		}
	}

	if (Text::StrStartsWithC(line, lineLen, UTF8STRC("$GPGGA")))
	{
		scnt = Text::StrSplit(sarr, 32, line, ',');
		if (scnt >= 15)
		{
			record->valid = (Text::StrToInt32(sarr[6]) != 0);
			record->nSateUsed = (UInt8)Text::StrToUInt32(sarr[7]);
			if (record->valid)
			{
				record->altitude = Text::StrToDouble(sarr[9]);
			}
		}
		return ParseStatus::Handled;
	}
	else if (Text::StrStartsWithC(line, lineLen, UTF8STRC("$GPGSA")))
	{
		scnt = Text::StrSplit(sarr, 32, line, ',');
		if (scnt >= 18)
		{
			UInt8 nSateGPS = 0;
			UInt8 nSateSBAS = 0;
			UInt8 nSateGLO = 0;
			UInt32 prn;
			UOSInt i = 15;
			while (i-- > 3)
			{
				if (sarr[i][0] != 0)
				{
					prn = Text::StrToUInt32(sarr[i]);
					if (prn <= 32)
					{
						nSateGPS++;
					}
					else if (prn <= 64)
					{
						nSateSBAS++;
					}
					else if (prn <= 96)
					{
						nSateGLO++;
					}
				}
			}
			record->nSateUsedGPS = nSateGPS;
			record->nSateUsedSBAS = nSateSBAS;
			record->nSateUsedGLO = nSateGLO;
		}
		return ParseStatus::Handled;
	}
	else if (Text::StrStartsWithC(line, lineLen, UTF8STRC("$GPGSV")))
	{
		scnt = Text::StrSplit(sarr, 32, line, ',');
		if (scnt >= 4)
		{
			UInt32 nSateView = Text::StrToUInt32(sarr[3]);
			if (nSateView > record->nSateViewGPS)
			{
				record->nSateViewGPS = (UInt8)nSateView;
			}
			if (sateRec && nSateView == record->nSateViewGPS)
			{
				UOSInt i = 4;
				UOSInt j;
				while (i + 4 < scnt)
				{
					j = sateRec->sateCnt;
					sateRec->sates[j].sateType = SateType::GPS;
					sateRec->sates[j].prn = (UInt8)Text::StrToUInt32(sarr[i]);
					sateRec->sates[j].elev = (UInt8)Text::StrToUInt32(sarr[i + 1]);
					Text::StrToUInt16S(sarr[i + 2], &sateRec->sates[j].azimuth, 0);
					if (sarr[i + 3][0])
					{
						sateRec->sates[j].snr = (Int8)Text::StrToInt32(sarr[i + 3]);
					}
					else
					{
						sateRec->sates[j].snr = -1;
					}

					sateRec->sateCnt++;
					i += 4;
				}
			}
		}
		return ParseStatus::Handled;
	}
	else if (Text::StrStartsWithC(line, lineLen, UTF8STRC("$GLGSV")))
	{
		scnt = Text::StrSplit(sarr, 32, line, ',');
		if (scnt >= 4)
		{
			record->nSateViewGLO = (UInt8)Text::StrToUInt32(sarr[3]);
		}
		return ParseStatus::Handled;
	}
	else if (Text::StrStartsWithC(line, lineLen, UTF8STRC("$GAGSV")))
	{
		scnt = Text::StrSplit(sarr, 32, line, ',');
		if (scnt >= 4)
		{
			record->nSateViewGA = (UInt8)Text::StrToUInt32(sarr[3]);
		}
		return ParseStatus::Handled;
	}
	else if (Text::StrStartsWithC(line, lineLen, UTF8STRC("$GQGSV")))
	{
		scnt = Text::StrSplit(sarr, 32, line, ',');
		if (scnt >= 4)
		{
			record->nSateViewQZSS = (UInt8)Text::StrToUInt32(sarr[3]);
		}
		return ParseStatus::Handled;
	}
	else if (Text::StrStartsWithC(line, lineLen, UTF8STRC("$GBGSV")))
	{
		scnt = Text::StrSplit(sarr, 32, line, ',');
		if (scnt >= 4)
		{
			record->nSateViewBD = (UInt8)Text::StrToUInt32(sarr[3]);
		}
		return ParseStatus::Handled;
	}
	else if (Text::StrStartsWithC(line, lineLen, UTF8STRC("$BDGSV")))
	{
		scnt = Text::StrSplit(sarr, 32, line, ',');
		if (scnt >= 4)
		{
			record->nSateViewBD = (UInt8)Text::StrToUInt32(sarr[3]);
		}
		return ParseStatus::Handled;
	}
	else if (Text::StrStartsWithC(line, lineLen, UTF8STRC("$GPVTG")))
	{
		scnt = Text::StrSplit(sarr, 32, line, ',');
		return ParseStatus::Handled;
	}
	else if (Text::StrStartsWithC(line, lineLen, UTF8STRC("$GPRMC")))
	{
		scnt = Text::StrSplit(sarr, 32, line, ',');
		if (scnt >= 12)
		{
			Int32 d;
			Int32 d2;
			Int32 d3;
			Double t;
			Int32 it;
			Int32 t2;
			Int32 t3;
			Int32 deg;
			Double ddeg;
			record->speed = Text::StrToDouble(sarr[7]);
			record->heading = Text::StrToDouble(sarr[8]);
			record->valid = (sarr[2][0] == 'A');
			d = Text::StrToInt32(sarr[9]);
			t = Text::StrToDouble(sarr[1]);
			it = (Int32)t;
			d2 = d / 100;
			d3 = d2 / 100;
			t2 = it / 100;
			t3 = t2 / 100;

			Data::DateTime dt;
			dt.SetCurrTimeUTC();
			dt.SetYear((UInt16)(dt.GetYear() - 5));
			Data::TimeInstant minTime = dt.ToInstant();
			dt.SetCurrTimeUTC();
			dt.SetYear((UInt16)(dt.GetYear() + 1));
			Data::TimeInstant maxTime = dt.ToInstant();

			dt.SetValue((UInt16)((d % 100) + 2000), d2 % 100, d3, t3, t2 % 100, it % 100, Double2Int32((t - it) * 1000));
			record->recTime = dt.ToInstant();
			while (record->recTime < minTime)
			{
				record->recTime.sec += 619315200; //7168 days
			}
			while (record->recTime > maxTime)
			{
				record->recTime.sec -= 619315200; //7168 days
			}

			if ((sarr[4][0] == 'N' || sarr[4][0] == 'S') && (sarr[6][0] == 'E' || sarr[6][0] == 'W'))
			{
				ddeg = Text::StrToDouble(sarr[3]);
				deg = (Int32)(ddeg / 100);
				ddeg = deg + (ddeg - deg * 100) / 60;
				if (sarr[4][0] == 'S')
				{
					ddeg = -ddeg;
				}
				record->pos.SetLat(ddeg);

				ddeg = Text::StrToDouble(sarr[5]);
				deg = (Int32)(ddeg / 100);
				ddeg = deg + (ddeg - deg * 100) / 60;
				if (sarr[6][0] == 'W')
				{
					ddeg = -ddeg;
				}
				record->pos.SetLon(ddeg);
			}
			return ParseStatus::NewRecord;
		}
		else
		{
			return ParseStatus::Unsupported;
		}
	}
	else
	{
		return ParseStatus::Unsupported;
	}
}

UInt32 __stdcall IO::GPSNMEA::NMEAThread(void *userObj)
{
	IO::GPSNMEA *me = (IO::GPSNMEA*)userObj;
	UTF8Char sbuff[8200];
	UTF8Char *sptr;
	Map::GPSTrack::GPSRecord3 record;
	SateRecord sateRec;

	MemClear(&record, sizeof(record));
	MemClear(&sateRec, sizeof(sateRec));
	me->threadRunning = true;
	{
		Text::UTF8Reader reader(me->stm);

		while (!me->threadToStop)
		{
			sptr = reader.ReadLine(sbuff, 8192);
			if (sptr && (sptr - sbuff) > 3)
			{
				if (me->cmdHdlr)
				{
					me->cmdHdlr(me->cmdHdlrObj, sbuff, (UOSInt)(sptr - sbuff));
				}
				ParseStatus ps = ParseNMEALine(sbuff, (UOSInt)(sptr - sbuff), &record, &sateRec);
				switch (ps)
				{
				case ParseStatus::NotNMEA:
					break;
				case ParseStatus::Unsupported:
					me->ParseUnknownCmd(sbuff);
					break;
				case ParseStatus::NewRecord:
					{
						Sync::RWMutexUsage mutUsage(&me->hdlrMut, false);
						UOSInt i = me->hdlrList.GetCount();
						while (i-- > 0)
						{
							me->hdlrList.GetItem(i)(me->hdlrObjs.GetItem(i), &record, sateRec.sateCnt, sateRec.sates);
						}
						mutUsage.EndUse();
						MemClear(&record, sizeof(record));
						sateRec.sateCnt = 0;
						break;
					}
				case ParseStatus::Handled:
				default:
					break;
				}
			}
			else
			{
				Sync::SimpleThread::Sleep(10);
			}
		}
	}
	me->threadRunning = false;
	return 0;
}

IO::GPSNMEA::GPSNMEA(NotNullPtr<IO::Stream> stm, Bool relStm)
{
	this->stm = stm;
	this->relStm = relStm;
	this->cmdHdlr = 0;
	this->cmdHdlrObj = 0;
	this->threadRunning = false;
	this->threadToStop = false;

	Sync::ThreadUtil::Create(NMEAThread, this);
	while (!this->threadRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
}

IO::GPSNMEA::~GPSNMEA()
{
	this->threadToStop = true;
	this->stm->Close();
	while (this->threadRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
	if (this->relStm)
	{
		this->stm.Delete();
		this->relStm = false;
	}
}

Bool IO::GPSNMEA::IsDown()
{
	return this->stm->IsDown();
}

void IO::GPSNMEA::RegisterLocationHandler(LocationHandler hdlr, void *userObj)
{
	Sync::RWMutexUsage mutUsage(&this->hdlrMut, true);
	this->hdlrList.Add(hdlr);
	this->hdlrObjs.Add(userObj);
}

void IO::GPSNMEA::UnregisterLocationHandler(LocationHandler hdlr, void *userObj)
{
	Sync::RWMutexUsage mutUsage(&this->hdlrMut, true);
	UOSInt i = this->hdlrList.GetCount();
	while (i-- > 0)
	{
		if (this->hdlrList.GetItem(i) == hdlr && this->hdlrObjs.GetItem(i) == userObj)
		{
			this->hdlrList.RemoveAt(i);
			this->hdlrObjs.RemoveAt(i);
			break;
		}
	}
}

void IO::GPSNMEA::ErrorRecover()
{
	this->stm->Recover();
}

Map::ILocationService::ServiceType IO::GPSNMEA::GetServiceType()
{
	return Map::ILocationService::ST_NMEA;
}

void IO::GPSNMEA::HandleCommand(CommandHandler cmdHdlr, void *userObj)
{
	this->cmdHdlrObj = userObj;
	this->cmdHdlr = cmdHdlr;
}

UOSInt IO::GPSNMEA::GenNMEACommand(const UTF8Char *cmd, UOSInt cmdLen, UInt8 *buff)
{
	UOSInt size;
	UInt8 chk;
	UOSInt i;
	if (cmd[0] != '$')
		return 0;
	size = (UOSInt)(Text::StrConcatC(buff, cmd, cmdLen) - buff);
	i = size;
	chk = 0;
	while (i-- > 1)
	{
		chk ^= buff[i];
	}
	buff[size] = '*';
	Text::StrHexByte((Char*)&buff[size + 1], chk);
	buff[size + 3] = 13;
	buff[size + 4] = 10;
	return size + 5;
}

Map::GPSTrack *IO::GPSNMEA::NMEA2Track(NotNullPtr<IO::Stream> stm, Text::CString sourceName)
{
	Map::GPSTrack *trk;
	Map::GPSTrack::GPSRecord3 record;
	SateRecord sateRec;
	Text::StringBuilderUTF8 sb;
	ParseStatus ps;
	MemClear(&record, sizeof(record));
	MemClear(&sateRec, sizeof(sateRec));
	NEW_CLASS(trk, Map::GPSTrack(sourceName, true, 65001, sourceName));
	Text::UTF8Reader reader(stm);
	while (true)
	{
		sb.ClearStr();
		if (!reader.ReadLine(&sb, 1024))
		{
			break;
		}
		ps = ParseNMEALine(sb.v, sb.GetLength(), &record, &sateRec);
		if (ps == ParseStatus::NewRecord)
		{
			trk->AddRecord(&record);
			MemClear(&record, sizeof(record));
		}
	}
	return trk;
}
