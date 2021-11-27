#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/IChart.h"
#include "Math/Math.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Data::IChart::IChart()
{
	this->title = 0;
	this->xAxisName = 0;
	this->yAxisName = 0;

	this->timeFormat = Text::StrCopyNew("HH:mm");
	this->dateFormat = Text::StrCopyNew("yyyy/MM/dd");
	this->dblFormat = Text::StrCopyNew("0.00");
	this->minDblVal = 0.01;
}

Data::IChart::~IChart()
{
	SDEL_TEXT(this->title);
	SDEL_TEXT(this->xAxisName);
	SDEL_TEXT(this->yAxisName);

	SDEL_TEXT(this->dateFormat);
	SDEL_TEXT(this->timeFormat);
	SDEL_TEXT(this->dblFormat);
}

void Data::IChart::SetTitle(const UTF8Char *title)
{
	SDEL_TEXT(this->title);
	this->title = SCOPY_TEXT(title);
}

const UTF8Char *Data::IChart::GetTitle()
{
	return this->title;
}

void Data::IChart::SetDateFormat(const Char *format)
{
	if (this->dateFormat)
	{
		Text::StrDelNew(this->dateFormat);
	}
	this->dateFormat = Text::StrCopyNew(format);
}

const Char *Data::IChart::GetDateFormat()
{
	return this->dateFormat;
}

void Data::IChart::SetTimeFormat(const Char *format)
{
	SDEL_TEXT(this->timeFormat);
	if (format)
	{
		this->timeFormat = Text::StrCopyNew(format);
	}
}

const Char *Data::IChart::GetTimeFormat()
{
	return this->timeFormat;
}

void Data::IChart::SetDblFormat(const Char *format)
{
	if (this->dblFormat)
	{
		Text::StrDelNew(this->dblFormat);
	}
	this->dblFormat = Text::StrCopyNew(format);
	UOSInt i = Text::StrIndexOf(format, ".");
	if (i == INVALID_INDEX)
	{
		this->minDblVal = 1.0;
	}
	else
	{
		i = Text::StrCharCnt(format) - i - 1;
		this->minDblVal = 1.0;
		while (i-- > 0)
		{
			this->minDblVal = this->minDblVal * 0.1;
		}
	}
}

const Char *Data::IChart::GetDblFormat()
{
	return this->dblFormat;
}

void Data::IChart::SetXAxisName(const UTF8Char *xAxisName)
{
	SDEL_TEXT(this->xAxisName);
	this->xAxisName = SCOPY_TEXT(xAxisName);
}

const UTF8Char *Data::IChart::GetXAxisName()
{
	return this->xAxisName;
}

void Data::IChart::SetYAxisName(const UTF8Char *yAxisName)
{
	SDEL_TEXT(yAxisName);
	this->yAxisName = SCOPY_TEXT(yAxisName);
}

const UTF8Char *Data::IChart::GetYAxisName()
{
	return this->yAxisName;
}

UOSInt Data::IChart::CalScaleMarkDbl(Data::ArrayListDbl *locations, Data::ArrayList<const UTF8Char*> *labels, Double min, Double max, Double leng, Double minLeng, const Char *dblFormat, Double minDblVal, const UTF8Char *unit)
{
	UOSInt retCnt = 2;
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	Double scale;
	Double lScale;
	Double dScale;
	Double pos;

	sptr = Text::StrDoubleFmt(sbuff, min, dblFormat);
	locations->Add(0);
	if (unit)
		sptr = Text::StrConcat(sptr, unit);
	labels->Add(Text::StrCopyNew(sbuff));

	scale = minLeng * (max - min) / leng;
	lScale = (Int32)(Math::Log10(scale));
	dScale = Math::Pow(10, lScale);
	if (scale / dScale <= 2)
		dScale = Math::Pow(10, lScale) * 2;
	else if (scale / dScale <= 5)
		dScale = Math::Pow(10, lScale) * 5;
	else
		dScale = Math::Pow(10, lScale) * 10;

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
				if (unit)
					sptr = Text::StrConcat(sptr, unit);
				labels->Add(Text::StrCopyNew(sbuff));
				retCnt++;
			}
			scale += dScale;
		}
	}

	sptr = Text::StrDoubleFmt(sbuff, max, dblFormat);
	locations->Add(leng);
	if (unit)
		sptr = Text::StrConcat(sptr, unit);
	labels->Add(Text::StrCopyNew(sbuff));
	return retCnt;
}

UOSInt Data::IChart::CalScaleMarkInt(Data::ArrayListDbl *locations, Data::ArrayList<const UTF8Char*> *labels, Int32 min, Int32 max, Double leng, Double minLeng, const UTF8Char *unit)
{
	UOSInt retCnt = 2;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	Double scale;
	Double lScale;
	Double dScale;
	Single pos;

	sptr = Text::StrInt32(sbuff, min);
	locations->Add(0);
	if (unit)
		sptr = Text::StrConcat(sptr, unit);
	labels->Add(Text::StrCopyNew(sbuff));

	scale = minLeng * (Double)(max - min) / leng;
	lScale = (Int32)(Math::Log10(scale));
	if (scale < 1)
		dScale = 1;
	else
	{
		dScale = Math::Pow(10, lScale);
		if (scale / dScale <= 2)
			dScale = Math::Pow(10, lScale) * 2;
		else if (scale / dScale <= 5)
			dScale = Math::Pow(10, lScale) * 5;
		else
			dScale = Math::Pow(10, lScale) * 10;
	}

	scale = (((Int32)(min / dScale)) + 1) * dScale;
	while (scale < max)
	{
		pos = (Single)((scale - min) * leng / (Single)(max - min));
		if ((pos > minLeng) && (pos < leng - minLeng))
		{
			sptr = Text::StrInt32(sbuff, Math::Double2Int32(scale));
			locations->Add(pos);
			if (unit)
				sptr = Text::StrConcat(sptr, unit);
			labels->Add(Text::StrCopyNew(sbuff));
			retCnt++;
		}
		scale += dScale;
	}

	sptr = Text::StrInt32(sbuff, max);
	locations->Add(leng);
	if (unit)
		sptr = Text::StrConcat(sptr, unit);
	labels->Add(Text::StrCopyNew(sbuff));
	return retCnt;
}

UOSInt Data::IChart::CalScaleMarkDate(Data::ArrayListDbl *locations, Data::ArrayList<const UTF8Char*> *labels, Data::DateTime *min, Data::DateTime *max, Double leng, Double minLeng, const Char *dateFormat, const Char *timeFormat)
{
	UOSInt retCnt = 2;
	UTF8Char sbuff[64];
	Int64 timeDif;
	Double scale;
	Double lScale;
	Int32 iScale;
//	Double dScale;
	Data::DateTime currDate;
	Single pos;
	Bool hasSecond = true;
	if (timeFormat)
	{
		if (Text::StrIndexOf(timeFormat, 's') == INVALID_INDEX)
		{
			hasSecond = false;
		}
	}
    
	timeDif = max->DiffMS(min);
	if (timeFormat == 0 || Data::DateTime::MS2Days(timeDif) * minLeng / leng >= 1)
	{
		min->ToString(sbuff, dateFormat);
		locations->Add(0);
		labels->Add(Text::StrCopyNew(sbuff));

		scale = Data::DateTime::MS2Days(timeDif) * minLeng / leng;
		lScale = (Int32)(Math::Log10(scale));
		iScale = Math::Double2Int32(Math::Pow(10, lScale));
		if (scale / iScale <= 2)
			iScale = Math::Double2Int32(Math::Pow(10, lScale) * 2);
		else if (scale / iScale <= 5)
			iScale = Math::Double2Int32(Math::Pow(10, lScale) * 5);
		else
			iScale = Math::Double2Int32(Math::Pow(10, lScale) * 10);

		currDate = min;
		currDate.ClearTime();
		currDate.AddDay(iScale - (currDate.GetDay() % (iScale)));
		if (((Double)currDate.DiffMS(min) / (Double)timeDif) < minLeng / leng)
		{
			currDate.AddDay(iScale);
		}
		while (currDate < max)
		{
			pos = (Single)(Data::DateTime::MS2Minutes(currDate.DiffMS(min)) * leng / Data::DateTime::MS2Minutes(max->DiffMS(min)));
			if ((pos > minLeng) && (pos < leng - minLeng))
			{
				currDate.ToString(sbuff, dateFormat);
				locations->Add(pos);
				labels->Add(Text::StrCopyNew(sbuff));
				retCnt++;
			}
			currDate.AddDay(iScale);
		}

		max->ToString(sbuff, dateFormat);
		locations->Add(leng);
		labels->Add(Text::StrCopyNew(sbuff));
	}
	else if (Data::DateTime::MS2Hours(timeDif) * minLeng / leng >= 1)
	{
		if (min->GetMSPassedDate() == 0)
		{
			min->ToString(sbuff, dateFormat);
			locations->Add(0);
			labels->Add(Text::StrCopyNew(sbuff));
		}
		else
		{
			min->ToString(sbuff, timeFormat);
			locations->Add(0);
			labels->Add(Text::StrCopyNew(sbuff));
		}
		
		scale = Data::DateTime::MS2Hours(timeDif) * minLeng / leng;
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
		currDate.AddHour(iScale + (Int32)(Data::DateTime::MS2Hours(min->GetMSPassedDate()) / iScale) * iScale);
		while (currDate < max)
		{
			pos = (Single)(Data::DateTime::MS2Minutes(currDate.DiffMS(min)) * leng / Data::DateTime::MS2Minutes(max->DiffMS(min)));
			if ((pos > minLeng) && (pos < leng - minLeng))
			{
				if (currDate.GetMSPassedDate() == 0)
				{
					currDate.ToString(sbuff, dateFormat);
					locations->Add(pos);
					labels->Add(Text::StrCopyNew(sbuff));
				}
				else
				{
					currDate.ToString(sbuff, timeFormat);
					locations->Add(pos);
					labels->Add(Text::StrCopyNew(sbuff));
				}
				retCnt++;
			}
			currDate.AddHour(iScale);
		}

		if (max->GetMSPassedDate() == 0)
		{
			max->ToString(sbuff, dateFormat);
			locations->Add(leng);
			labels->Add(Text::StrCopyNew(sbuff));
		}
		else
		{
			max->ToString(sbuff, timeFormat);
			locations->Add(leng);
			labels->Add(Text::StrCopyNew(sbuff));
		}
	}
	else if (!hasSecond || Data::DateTime::MS2Minutes(timeDif) * minLeng / leng >= 1)
	{
		if (min->GetMSPassedDate() == 0)
		{
			min->ToString(sbuff, dateFormat);
			locations->Add(0);
			labels->Add(Text::StrCopyNew(sbuff));
		}
		else
		{
			min->ToString(sbuff, timeFormat);
			locations->Add(0);
			labels->Add(Text::StrCopyNew(sbuff));
		}

		scale = Data::DateTime::MS2Minutes(timeDif) * minLeng / leng;
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
		currDate.AddMinute(iScale + (Int32)(Data::DateTime::MS2Minutes(min->GetMSPassedDate()) / iScale) * iScale);
		while (currDate < max)
		{
			pos = (Single)(Data::DateTime::MS2Minutes(currDate.DiffMS(min)) * leng / Data::DateTime::MS2Minutes(max->DiffMS(min)));
			if ((pos > minLeng) && (pos < leng - minLeng))
			{
				if (currDate.GetMSPassedDate() == 0)
				{
					currDate.ToString(sbuff, dateFormat);
					locations->Add(pos);
					labels->Add(Text::StrCopyNew(sbuff));
				}
				else
				{
					currDate.ToString(sbuff, timeFormat);
					locations->Add(pos);
					labels->Add(Text::StrCopyNew(sbuff));
				}
				retCnt++;
			}
			currDate.AddMinute(iScale);
		}

		if (max->GetMSPassedDate() == 0)
		{
			max->ToString(sbuff, dateFormat);
			locations->Add(leng);
			labels->Add(Text::StrCopyNew(sbuff));
		}
		else
		{
			max->ToString(sbuff, timeFormat);
			locations->Add(leng);
			labels->Add(Text::StrCopyNew(sbuff));
		}
	}
	else if (Data::DateTime::MS2Seconds(timeDif) >= 1)
	{
		if (min->GetMSPassedDate() == 0)
		{
			min->ToString(sbuff, dateFormat);
			locations->Add(0);
			labels->Add(Text::StrCopyNew(sbuff));
		}
		else
		{
			min->ToString(sbuff, timeFormat);
			locations->Add(0);
			labels->Add(Text::StrCopyNew(sbuff));
		}

		scale = Data::DateTime::MS2Seconds(timeDif) * minLeng / leng;
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
		currDate.AddSecond(iScale + (Int32)(Data::DateTime::MS2Seconds(min->GetMSPassedDate()) / iScale) * iScale);
		while (currDate < max)
		{
			pos = (Single)(Data::DateTime::MS2Minutes(currDate.DiffMS(min)) * leng / Data::DateTime::MS2Minutes(max->DiffMS(min)));
			if ((pos > minLeng) && (pos < leng - minLeng))
			{
				if (currDate.GetMSPassedDate() == 0)
				{
					currDate.ToString(sbuff, dateFormat);
					locations->Add(pos);
					labels->Add(Text::StrCopyNew(sbuff));
				}
				else
				{
					currDate.ToString(sbuff, timeFormat);
					locations->Add(pos);
					labels->Add(Text::StrCopyNew(sbuff));
				}
				retCnt++;
			}
			currDate.AddSecond(iScale);
		}

		if (max->GetMSPassedDate() == 0)
		{
			max->ToString(sbuff, dateFormat);
			locations->Add(leng);
			labels->Add(Text::StrCopyNew(sbuff));
		}
		else
		{
			max->ToString(sbuff, timeFormat);
			locations->Add(leng);
			labels->Add(Text::StrCopyNew(sbuff));
		}
	}
	else
	{
		min->ToString(sbuff, timeFormat);
		locations->Add(0);
		labels->Add(Text::StrCopyNew(sbuff));

		max->ToString(sbuff, timeFormat);
		locations->Add(leng);
		labels->Add(Text::StrCopyNew(sbuff));
	}
	return retCnt;
}
