#include "Stdafx.h"
#include "IO/DataRateCalc.h"

IO::DataRateCalc::DataRateCalc(NN<Text::String> sourceName) : ParsedObject(sourceName)
{
}

IO::DataRateCalc::~DataRateCalc()
{
	this->dataMap.MemFreeAll();
}

void IO::DataRateCalc::AddData(NN<Data::DateTime> dt, UInt64 dataSize)
{
	Int64 t = dt->ToUnixTimestamp();
	NN<DataStat> stat;
	if (dataMap.Get(t).SetTo(stat))
	{
		stat->dataSize += dataSize;
	}
	else
	{
		stat = MemAllocNN(DataStat);
		stat->sec = t;
		stat->dataSize = dataSize;
		dataMap.Put(t, stat);
	}
}

UIntOS IO::DataRateCalc::GetCount() const
{
	return this->dataMap.GetCount();
}

Optional<IO::DataRateCalc::DataStat> IO::DataRateCalc::GetItem(UIntOS index) const
{
	return this->dataMap.GetItem(index);
}
