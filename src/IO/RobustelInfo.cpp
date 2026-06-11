#include "Stdafx.h"
#include "IO/RobustelInfo.h"
#include "IO/RobustelStatus.h"
#include "Text/MyString.h"

Optional<Text::String> IO::RobustelInfo::GetIMEI()
{
	return IO::RobustelStatus::GetStatus("cellular.status.imei");
}

Optional<Text::String> IO::RobustelInfo::GetCellID()
{
	return IO::RobustelStatus::GetStatus("cellular.status.cell_id");
}

Bool IO::RobustelInfo::GetRSSI(OutParam<Int8> val)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	Int16 ival;
	NN<Text::String> s;
	if (IO::RobustelStatus::GetStatus("cellular.status.csq").SetTo(s))
	{
		Bool succ = false;
		UIntOS i = s->IndexOf('(');
		if (i != INVALID_INDEX)
		{
			sptr = Text::StrConcatC(sbuff, &s->v[i + 1], s->leng - i - 1);
			i = Text::StrIndexOfC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("dBm"));
			if (i != INVALID_INDEX)
			{
				sbuff[i] = 0;
				if (Text::StrToInt16(sbuff, ival))
				{
					val.Set((Int8)ival);
				}
				succ = true;
			}
		}
		s->Release();
		return succ;
	}
	return false;
}

Bool IO::RobustelInfo::GetRSRP(OutParam<Int8> val)
{
	Int16 ival;
	NN<Text::String> s;
	if (IO::RobustelStatus::GetStatus("cellular.status.rsrp").SetTo(s))
	{
		Bool succ = false;
		UIntOS i = s->IndexOf(UTF8STRC(" dB"));
		if (i != INVALID_INDEX)
		{
			s->v[i] = 0;
			if (s->ToInt16(ival))
			{
				val.Set((Int8)ival);
				succ = true;
			}
		}
		s->Release();
		return succ;
	}
	return false;
}

Bool IO::RobustelInfo::GetRSRQ(OutParam<Int8> val)
{
	Int16 ival;
	NN<Text::String> s;
	if (IO::RobustelStatus::GetStatus("cellular.status.rsrq").SetTo(s))
	{
		Bool succ = false;
		UIntOS i = s->IndexOf(UTF8STRC(" dB"));
		if (i != INVALID_INDEX)
		{
			s->v[i] = 0;
			if (s->ToInt16(ival))
			{
				val.Set((Int8)ival);
				succ = true;
			}
		}
		s->Release();
		return succ;
	}
	return false;
}

Bool IO::RobustelInfo::GetPosition(OutParam<Double> lat, OutParam<Double> lon)
{
	Bool succ = true;
	NN<Text::String> s;
	if (!IO::RobustelStatus::GetStatus("gps.latitude").SetTo(s))
	{
		succ = false;
	}
	else
	{
		if (!s->ToDouble(lat))
		{
			succ = false;
		}
		s->Release();
	}
	if (!IO::RobustelStatus::GetStatus("gps.longitude").SetTo(s))
	{
		succ = false;
	}
	else
	{
		if (!s->ToDouble(lon))
		{
			succ = false;
		}
		s->Release();
	}
	return succ;
}
