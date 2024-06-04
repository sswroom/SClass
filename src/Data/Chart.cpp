#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Chart.h"
#include "Math/Math.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Data::Chart::Chart()
{
	this->title = 0;
	this->xAxisName = 0;
	this->yAxisName = 0;

	this->timeFormat = Text::String::New(UTF8STRC("HH:mm"));
	this->dateFormat = Text::String::New(UTF8STRC("yyyy/MM/dd"));
	this->dblFormat = Text::String::New(UTF8STRC("0.00"));
	this->minDblVal = 0.01;
}

Data::Chart::~Chart()
{
	OPTSTR_DEL(this->title);
	OPTSTR_DEL(this->xAxisName);
	OPTSTR_DEL(this->yAxisName);

	this->dateFormat->Release();
	this->timeFormat->Release();
	this->dblFormat->Release();
}

void Data::Chart::SetTitle(Text::CString title)
{
	OPTSTR_DEL(this->title);
	this->title = Text::String::NewOrNull(title);
}

Optional<Text::String> Data::Chart::GetTitle() const
{
	return this->title;
}

void Data::Chart::SetDateFormat(Text::CStringNN format)
{
	this->dateFormat->Release();
	this->dateFormat = Text::String::New(format);
}

NN<Text::String> Data::Chart::GetDateFormat() const
{
	return this->dateFormat;
}

void Data::Chart::SetTimeFormat(Text::CStringNN format)
{
	this->timeFormat->Release();
	this->timeFormat = Text::String::New(format);
}

NN<Text::String> Data::Chart::GetTimeFormat() const
{
	return this->timeFormat;
}

void Data::Chart::SetDblFormat(Text::CStringNN format)
{
	this->dblFormat->Release();
	this->dblFormat = Text::String::New(format);
	UOSInt i = format.IndexOf('.');
	if (i == INVALID_INDEX)
	{
		this->minDblVal = 1.0;
	}
	else
	{
		i = format.leng - i - 1;
		this->minDblVal = 1.0;
		while (i-- > 0)
		{
			this->minDblVal = this->minDblVal * 0.1;
		}
	}
}

NN<Text::String> Data::Chart::GetDblFormat() const
{
	return this->dblFormat;
}

void Data::Chart::SetXAxisName(Text::CString xAxisName)
{
	OPTSTR_DEL(this->xAxisName);
	this->xAxisName = Text::String::NewOrNull(xAxisName);
}

Optional<Text::String> Data::Chart::GetXAxisName() const
{
	return this->xAxisName;
}

void Data::Chart::SetYAxisName(Text::CString yAxisName)
{
	OPTSTR_DEL(this->yAxisName);
	this->yAxisName = Text::String::NewOrNull(yAxisName);
}

Optional<Text::String> Data::Chart::GetYAxisName() const
{
	return this->yAxisName;
}

UOSInt Data::Chart::CalScaleMarkDbl(Data::ArrayListDbl *locations, Data::ArrayListStringNN *labels, Double min, Double max, Double leng, Double minLeng, UnsafeArray<const Char> dblFormat, Double minDblVal, Optional<Text::String> unit)
{
	UOSInt retCnt = 2;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	Double scale;
	Double lScale;
	Double dScale;
	Double pos;
	NN<Text::String> s;

	sptr = Text::StrDoubleFmt(sbuff, min, dblFormat);
	locations->Add(0);
	if (unit.SetTo(s))
		sptr = s->ConcatTo(sptr);
	labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));

	scale = minLeng * (max - min) / leng;
	lScale = (Int32)(Math_Log10(scale));
	dScale = Math_Pow(10, lScale);
	if (scale / dScale <= 2)
		dScale = Math_Pow(10, lScale) * 2;
	else if (scale / dScale <= 5)
		dScale = Math_Pow(10, lScale) * 5;
	else
		dScale = Math_Pow(10, lScale) * 10;

	if (dScale <= 0 || lScale <= -10)
	{
	}
	else
	{
		scale = (((Int32)(min / dScale)) + 1) * dScale;
		while (scale < max)
		{
			pos = ((scale - min) * leng / (max - min));
			if ((pos > minLeng) && (pos < leng - minLeng))
			{
				sptr = Text::StrDoubleFmt(sbuff, scale, dblFormat);
				locations->Add(pos);
				if (unit.SetTo(s))
					sptr = s->ConcatTo(sptr);
				labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				retCnt++;
			}
			scale += dScale;
		}
	}

	sptr = Text::StrDoubleFmt(sbuff, max, dblFormat);
	locations->Add(leng);
	if (unit.SetTo(s))
		sptr = s->ConcatTo(sptr);
	labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
	return retCnt;
}

UOSInt Data::Chart::CalScaleMarkInt(Data::ArrayListDbl *locations, Data::ArrayListStringNN *labels, Int32 min, Int32 max, Double leng, Double minLeng, Optional<Text::String> unit)
{
	UOSInt retCnt = 2;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	Double scale;
	Double lScale;
	Double dScale;
	Single pos;
	NN<Text::String> s;

	sptr = Text::StrInt32(sbuff, min);
	locations->Add(0);
	if (unit.SetTo(s))
		sptr = s->ConcatTo(sptr);
	labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));

	scale = minLeng * (Double)(max - min) / leng;
	lScale = (Int32)(Math_Log10(scale));
	if (scale < 1)
		dScale = 1;
	else
	{
		dScale = Math_Pow(10, lScale);
		if (scale / dScale <= 2)
			dScale = Math_Pow(10, lScale) * 2;
		else if (scale / dScale <= 5)
			dScale = Math_Pow(10, lScale) * 5;
		else
			dScale = Math_Pow(10, lScale) * 10;
	}

	scale = (((Int32)(min / dScale)) + 1) * dScale;
	while (scale < max)
	{
		pos = (Single)((scale - min) * leng / (Single)(max - min));
		if ((pos > minLeng) && (pos < leng - minLeng))
		{
			sptr = Text::StrInt32(sbuff, Double2Int32(scale));
			locations->Add(pos);
			if (unit.SetTo(s))
				sptr = s->ConcatTo(sptr);
			labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
			retCnt++;
		}
		scale += dScale;
	}

	sptr = Text::StrInt32(sbuff, max);
	locations->Add(leng);
	if (unit.SetTo(s))
		sptr = s->ConcatTo(sptr);
	labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
	return retCnt;
}

UOSInt Data::Chart::CalScaleMarkDate(Data::ArrayListDbl *locations, Data::ArrayListStringNN *labels, NN<Data::DateTime> min, NN<Data::DateTime> max, Double leng, Double minLeng, UnsafeArray<const Char> dateFormat, UnsafeArrayOpt<const Char> timeFormat)
{
	UOSInt retCnt = 2;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	Int64 timeDif;
	Double scale;
	Double lScale;
	Int32 iScale;
//	Double dScale;
	Data::DateTime currDate;
	Single pos;
	Bool hasSecond = true;
	UnsafeArray<const Char> nntimeFormat;
	if (timeFormat.SetTo(nntimeFormat))
	{
		if (Text::StrIndexOfCharCh(nntimeFormat, 's') == INVALID_INDEX)
		{
			hasSecond = false;
		}
	}
    
	timeDif = max->DiffMS(min);
	if (!timeFormat.SetTo(nntimeFormat) || Data::DateTimeUtil::MS2Days(timeDif) * minLeng / leng >= 1)
	{
		sptr = min->ToString(sbuff, dateFormat);
		locations->Add(0);
		labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));

		scale = Data::DateTimeUtil::MS2Days(timeDif) * minLeng / leng;
		lScale = (Int32)(Math_Log10(scale));
		iScale = Double2Int32(Math_Pow(10, lScale));
		if (scale / iScale <= 2)
			iScale = Double2Int32(Math_Pow(10, lScale) * 2);
		else if (scale / iScale <= 5)
			iScale = Double2Int32(Math_Pow(10, lScale) * 5);
		else
			iScale = Double2Int32(Math_Pow(10, lScale) * 10);

		currDate = min;
		currDate.ClearTime();
		currDate.AddDay(iScale - (currDate.GetDay() % (iScale)));
		if (((Double)currDate.DiffMS(min) / (Double)timeDif) < minLeng / leng)
		{
			currDate.AddDay(iScale);
		}
		while (currDate < max)
		{
			pos = (Single)(Data::DateTimeUtil::MS2Minutes(currDate.DiffMS(min)) * leng / Data::DateTimeUtil::MS2Minutes(max->DiffMS(min)));
			if ((pos > minLeng) && (pos < leng - minLeng))
			{
				sptr = currDate.ToString(sbuff, dateFormat);
				locations->Add(pos);
				labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				retCnt++;
			}
			currDate.AddDay(iScale);
		}

		sptr = max->ToString(sbuff, dateFormat);
		locations->Add(leng);
		labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
	}
	else if (Data::DateTimeUtil::MS2Hours(timeDif) * minLeng / leng >= 1)
	{
		if (min->GetMSPassedDate() == 0)
		{
			sptr = min->ToString(sbuff, dateFormat);
			locations->Add(0);
			labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
		}
		else
		{
			sptr = min->ToString(sbuff, nntimeFormat);
			locations->Add(0);
			labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
		}
		
		scale = Data::DateTimeUtil::MS2Hours(timeDif) * minLeng / leng;
		if (scale <= 2)
			iScale = 2;
		else if (scale <= 3)
			iScale = 3;
		else if (scale <= 6)
			iScale = 6;
		else if (scale <= 12)
			iScale = 12;
		else
			iScale = 24;

		currDate = min;
		currDate.ClearTime();
		currDate.AddHour(iScale + (Int32)(Data::DateTimeUtil::MS2Hours(min->GetMSPassedDate()) / iScale) * iScale);
		while (currDate < max)
		{
			pos = (Single)(Data::DateTimeUtil::MS2Minutes(currDate.DiffMS(min)) * leng / Data::DateTimeUtil::MS2Minutes(max->DiffMS(min)));
			if ((pos > minLeng) && (pos < leng - minLeng))
			{
				if (currDate.GetMSPassedDate() == 0)
				{
					sptr = currDate.ToString(sbuff, dateFormat);
					locations->Add(pos);
					labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				}
				else
				{
					sptr = currDate.ToString(sbuff, nntimeFormat);
					locations->Add(pos);
					labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				}
				retCnt++;
			}
			currDate.AddHour(iScale);
		}

		if (max->GetMSPassedDate() == 0)
		{
			sptr = max->ToString(sbuff, dateFormat);
			locations->Add(leng);
			labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
		}
		else
		{
			sptr = max->ToString(sbuff, nntimeFormat);
			locations->Add(leng);
			labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
		}
	}
	else if (!hasSecond || Data::DateTimeUtil::MS2Minutes(timeDif) * minLeng / leng >= 1)
	{
		if (min->GetMSPassedDate() == 0)
		{
			sptr = min->ToString(sbuff, dateFormat);
			locations->Add(0);
			labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
		}
		else
		{
			sptr = min->ToString(sbuff, nntimeFormat);
			locations->Add(0);
			labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
		}

		scale = Data::DateTimeUtil::MS2Minutes(timeDif) * minLeng / leng;
		if (scale <= 1)
			iScale = 1;
		else if (scale <= 2)
			iScale = 2;
		else if (scale <= 5)
			iScale = 5;
		else if (scale <= 10)
			iScale = 10;
		else if (scale <= 20)
			iScale = 20;
		else if (scale <= 30)
			iScale = 30;
		else
			iScale = 60;

		currDate = min;
		currDate.ClearTime();
		currDate.AddMinute(iScale + (Int32)(Data::DateTimeUtil::MS2Minutes(min->GetMSPassedDate()) / iScale) * iScale);
		while (currDate < max)
		{
			pos = (Single)(Data::DateTimeUtil::MS2Minutes(currDate.DiffMS(min)) * leng / Data::DateTimeUtil::MS2Minutes(max->DiffMS(min)));
			if ((pos > minLeng) && (pos < leng - minLeng))
			{
				if (currDate.GetMSPassedDate() == 0)
				{
					sptr = currDate.ToString(sbuff, dateFormat);
					locations->Add(pos);
					labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				}
				else
				{
					sptr = currDate.ToString(sbuff, nntimeFormat);
					locations->Add(pos);
					labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				}
				retCnt++;
			}
			currDate.AddMinute(iScale);
		}

		if (max->GetMSPassedDate() == 0)
		{
			sptr = max->ToString(sbuff, dateFormat);
			locations->Add(leng);
			labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
		}
		else
		{
			sptr = max->ToString(sbuff, nntimeFormat);
			locations->Add(leng);
			labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
		}
	}
	else if (Data::DateTimeUtil::MS2Seconds(timeDif) >= 1)
	{
		if (min->GetMSPassedDate() == 0)
		{
			sptr = min->ToString(sbuff, dateFormat);
			locations->Add(0);
			labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
		}
		else
		{
			sptr = min->ToString(sbuff, nntimeFormat);
			locations->Add(0);
			labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
		}

		scale = Data::DateTimeUtil::MS2Seconds(timeDif) * minLeng / leng;
		if (scale <= 1)
			iScale = 1;
		else if (scale <= 2)
			iScale = 2;
		else if (scale <= 5)
			iScale = 5;
		else if (scale <= 10)
			iScale = 10;
		else if (scale <= 20)
			iScale = 20;
		else if (scale <= 30)
			iScale = 30;
		else
			iScale = 60;

		currDate = min;
		currDate.ClearTime();
		currDate.AddSecond(iScale + (Int32)(Data::DateTimeUtil::MS2Seconds(min->GetMSPassedDate()) / iScale) * iScale);
		while (currDate < max)
		{
			pos = (Single)(Data::DateTimeUtil::MS2Minutes(currDate.DiffMS(min)) * leng / Data::DateTimeUtil::MS2Minutes(max->DiffMS(min)));
			if ((pos > minLeng) && (pos < leng - minLeng))
			{
				if (currDate.GetMSPassedDate() == 0)
				{
					sptr = currDate.ToString(sbuff, dateFormat);
					locations->Add(pos);
					labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				}
				else
				{
					sptr = currDate.ToString(sbuff, nntimeFormat);
					locations->Add(pos);
					labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				}
				retCnt++;
			}
			currDate.AddSecond(iScale);
		}

		if (max->GetMSPassedDate() == 0)
		{
			sptr = max->ToString(sbuff, dateFormat);
			locations->Add(leng);
			labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
		}
		else
		{
			sptr = max->ToString(sbuff, nntimeFormat);
			locations->Add(leng);
			labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
		}
	}
	else
	{
		sptr = min->ToString(sbuff, nntimeFormat);
		locations->Add(0);
		labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));

		sptr = max->ToString(sbuff, nntimeFormat);
		locations->Add(leng);
		labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
	}
	return retCnt;
}
