#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/GPSNMEA.h"
#include "Math/Math.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"

void IO::GPSNMEA::ParseUnknownCmd(const UTF8Char *cmd)
{
}

IO::GPSNMEA::ParseStatus IO::GPSNMEA::ParseNMEALine(UTF8Char *line, Map::GPSTrack::GPSRecord *record)
{
	UTF8Char *sarr[32];
	UOSInt scnt;
	UOSInt slen = Text::StrCharCnt(line);
	if (slen <= 3)
	{
		return PS_NOT_NMEA;
	}
	if (line[slen - 3] != '*' || line[0] != '$')
	{
		return PS_NOT_NMEA;
	}
	else
	{
		UTF8Char c = 0;
		UTF8Char *sptr2 = &line[1];
		while (sptr2 < &line[slen - 3])
		{
			c = c ^ *sptr2++;
		}
		if (c != Text::StrHex2UInt8C(&line[slen - 2]))
		{
			return PS_NOT_NMEA;
		}
	}

	if (Text::StrStartsWith(line, (const UTF8Char*)"$GPGGA"))
	{
		scnt = Text::StrSplit(sarr, 32, line, ',');
		if (scnt >= 15)
		{
			record->valid = (Text::StrToInt32(sarr[6]) != 0);
			record->nSateView = Text::StrToInt32(sarr[7]);
			if (record->valid)
			{
				record->altitude = Text::StrToDouble(sarr[9]);
			}
		}
		return PS_HANDLED;
	}
	else if (Text::StrStartsWith(line, (const UTF8Char*)"$GPGSA"))
	{
		scnt = Text::StrSplit(sarr, 32, line, ',');
		if (scnt >= 18)
		{
			Int32 nSate = 0;
			OSInt i = 15;
			while (i-- > 3)
			{
				if (sarr[i][0] != 0)
					nSate++;
			}
			record->nSateUsed = nSate;
		}
		return PS_HANDLED;
	}
	else if (Text::StrStartsWith(line, (const UTF8Char*)"$GPGSV"))
	{
		scnt = Text::StrSplit(sarr, 32, line, ',');
		return PS_HANDLED;
	}
	else if (Text::StrStartsWith(line, (const UTF8Char*)"$GPRMC"))
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
			dt.SetValue((UInt16)((d % 100) + 2000), d2 % 100, d3, t3, t2 % 100, it % 100, Math::Double2Int32((t - it) * 1000));
			record->utcTimeTicks = dt.ToTicks();

			if ((sarr[4][0] == 'N' || sarr[4][0] == 'S') && (sarr[6][0] == 'E' || sarr[6][0] == 'W'))
			{
				ddeg = Text::StrToDouble(sarr[3]);
				deg = (Int32)(ddeg / 100);
				ddeg = deg + (ddeg - deg * 100) / 60;
				if (sarr[4][0] == 'S')
				{
					ddeg = -ddeg;
				}
				record->lat = ddeg;

				ddeg = Text::StrToDouble(sarr[5]);
				deg = (Int32)(ddeg / 100);
				ddeg = deg + (ddeg - deg * 100) / 60;
				if (sarr[6][0] == 'W')
				{
					ddeg = -ddeg;
				}
				record->lon = ddeg;
			}
			return PS_NEW_RECORD;
		}
		else
		{
			return PS_UNSUPPORTED;
		}
	}
	else
	{
		return PS_UNSUPPORTED;
	}
}

UInt32 __stdcall IO::GPSNMEA::NMEAThread(void *userObj)
{
	IO::GPSNMEA *me = (IO::GPSNMEA*)userObj;
	Text::UTF8Reader *reader;
	UTF8Char sbuff[8200];
	UTF8Char *sptr;
	Map::GPSTrack::GPSRecord record;

	record.lat = 0;
	record.lon = 0;
	record.altitude = 0;
	record.nSateUsed = 0;
	record.nSateView = 0;
	me->threadRunning = true;
	NEW_CLASS(reader, Text::UTF8Reader(me->stm));

	while (!me->threadToStop)
	{
		sptr = reader->ReadLine(sbuff, 8192);
		if (sptr && (sptr - sbuff) > 3)
		{
			ParseStatus ps = ParseNMEALine(sbuff, &record);
			if (ps == PS_NOT_NMEA)
			{

			}
			else if (ps == PS_UNSUPPORTED)
			{
				me->ParseUnknownCmd(sbuff);
			}
			else if (ps == PS_NEW_RECORD)
			{
				me->hdlrMut->LockRead();
				UOSInt i = me->hdlrList->GetCount();
				while (i-- > 0)
				{
					me->hdlrList->GetItem(i)(me->hdlrObjs->GetItem(i), &record);
				}
				me->hdlrMut->UnlockRead();
			}
		}
		else
		{
			Sync::Thread::Sleep(10);
		}
	}
	DEL_CLASS(reader);
	me->threadRunning = false;
	return 0;
}

IO::GPSNMEA::GPSNMEA(IO::Stream *stm, Bool relStm)
{
	this->stm = stm;
	this->relStm = relStm;
	NEW_CLASS(this->hdlrMut, Sync::RWMutex());
	NEW_CLASS(this->hdlrList, Data::ArrayList<LocationHandler>());
	NEW_CLASS(this->hdlrObjs, Data::ArrayList<void *>());
	this->threadRunning = false;
	this->threadToStop = false;
	Sync::Thread::Create(NMEAThread, this);
	while (!this->threadRunning)
	{
		Sync::Thread::Sleep(10);
	}
}

IO::GPSNMEA::~GPSNMEA()
{
	this->threadToStop = true;
	this->stm->Close();
	while (this->threadRunning)
	{
		Sync::Thread::Sleep(10);
	}
	if (this->relStm)
	{
		DEL_CLASS(this->stm);
		this->stm = 0;
		this->relStm = false;
	}
	DEL_CLASS(this->hdlrList);
	DEL_CLASS(this->hdlrObjs);
	DEL_CLASS(this->hdlrMut);
}

void IO::GPSNMEA::RegisterLocationHandler(LocationHandler hdlr, void *userObj)
{
	this->hdlrMut->LockWrite();
	this->hdlrList->Add(hdlr);
	this->hdlrObjs->Add(userObj);
	this->hdlrMut->UnlockWrite();
}

void IO::GPSNMEA::UnregisterLocationHandler(LocationHandler hdlr, void *userObj)
{
	this->hdlrMut->LockWrite();
	UOSInt i = this->hdlrList->GetCount();
	while (i-- > 0)
	{
		if (this->hdlrList->GetItem(i) == hdlr && this->hdlrObjs->GetItem(i) == userObj)
		{
			this->hdlrList->RemoveAt(i);
			this->hdlrObjs->RemoveAt(i);
			break;
		}
	}
	this->hdlrMut->UnlockWrite();
}

void IO::GPSNMEA::ErrorRecover()
{
	this->stm->Recover();
}

Map::ILocationService::ServiceType IO::GPSNMEA::GetServiceType()
{
	return Map::ILocationService::ST_NMEA;
}

UOSInt IO::GPSNMEA::GenNMEACommand(const UTF8Char *cmd, UInt8 *buff)
{
	UOSInt size;
	UInt8 chk;
	UOSInt i;
	if (cmd[0] != '$')
		return 0;
	size = (UOSInt)(Text::StrConcat(buff, cmd) - buff);
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

Map::GPSTrack *IO::GPSNMEA::NMEA2Track(IO::Stream *stm, const UTF8Char *sourceName)
{
	Map::GPSTrack *trk;
	Map::GPSTrack::GPSRecord record;
	Text::StringBuilderUTF8 sb;
	Text::UTF8Reader *reader;
	ParseStatus ps;
	record.lat = 0;
	record.lon = 0;
	record.altitude = 0;
	record.nSateUsed = 0;
	record.nSateView = 0;
	NEW_CLASS(trk, Map::GPSTrack(sourceName, true, 65001, sourceName));
	NEW_CLASS(reader, Text::UTF8Reader(stm));
	while (true)
	{
		sb.ClearStr();
		if (!reader->ReadLine(&sb, 1024))
		{
			break;
		}
		ps = ParseNMEALine(sb.ToString(), &record);
		if (ps == PS_NEW_RECORD)
		{
			trk->AddRecord(&record);
		}
	}
	DEL_CLASS(reader);
	return trk;
}
