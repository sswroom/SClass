#include "Stdafx.h"
#include "IO/RobustelInfo.h"
#include "IO/RobustelStatus.h"
#include "Text/MyString.h"

Text::String *IO::RobustelInfo::GetIMEI()
{
	return IO::RobustelStatus::GetStatus("cellular.status.imei");
}

Text::String *IO::RobustelInfo::GetCellID()
{
	return IO::RobustelStatus::GetStatus("cellular.status.cell_id");
}

Bool IO::RobustelInfo::GetRSSI(Int8 *val)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	Int16 ival;
	Text::String *s = IO::RobustelStatus::GetStatus("cellular.status.csq");
	if (s)
	{
		Bool succ = false;
		UOSInt i = s->IndexOf('(');
		if (s >= 0)
		{
			sptr = Text::StrConcatC(sbuff, &s->v[i + 1], s->leng - i - 1);
			i = Text::StrIndexOfC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("dBm"));
			if (i != INVALID_INDEX)
			{
				sbuff[i] = 0;
				if (Text::StrToInt16(sbuff, &ival))
				{
					*val = (Int8)ival;
				}
				succ = true;
			}
		}
		s->Release();
		return succ;
	}
	return false;
}

Bool IO::RobustelInfo::GetRSRP(Int8 *val)
{
	Text::String *s = IO::RobustelStatus::GetStatus("cellular.status.rsrp");
	Int16 ival;
	if (s)
	{
		Bool succ = false;
		UOSInt i = s->IndexOf(UTF8STRC(" dB"));
		if (i != INVALID_INDEX)
		{
			s->v[i] = 0;
			if (s->ToInt16(&ival))
			{
				*val = (Int8)ival;
				succ = true;
			}
		}
		s->Release();
		return succ;
	}
	return false;
}

Bool IO::RobustelInfo::GetRSRQ(Int8 *val)
{
	Text::String *s = IO::RobustelStatus::GetStatus("cellular.status.rsrq");
	Int16 ival;
	if (s)
	{
		Bool succ = false;
		UOSInt i = s->IndexOf(UTF8STRC(" dB"));
		if (i != INVALID_INDEX)
		{
			s->v[i] = 0;
			if (s->ToInt16(&ival))
			{
				*val = (Int8)ival;
				succ = true;
			}
		}
		s->Release();
		return succ;
	}
	return false;
}

Bool IO::RobustelInfo::GetPosition(Double *lat, Double *lon)
{
	Bool succ = true;
	Text::String *s;
	s = IO::RobustelStatus::GetStatus("gps.latitude");
	if (s == 0 || !s->ToDouble(lat))
	{
		succ = false;
	}
	SDEL_STRING(s);
	s = IO::RobustelStatus::GetStatus("gps.longitude");
	if (s == 0 || !s->ToDouble(lon))
	{
		succ = false;
	}
	SDEL_STRING(s);
	return succ;
}
