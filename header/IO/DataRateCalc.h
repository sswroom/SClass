#ifndef _SM_IO_DATARATECALC
#define _SM_IO_DATARATECALC
#include "AnyType.h"
#include "Data/DateTime.h"
#include "Data/FastMapNN.hpp"
#include "IO/ParsedObject.h"

namespace IO
{
	class DataRateCalc : public ParsedObject
	{
	public:
		struct DataStat
		{
			Int64 sec;
			UInt64 dataSize;
		};
	private:
		Data::Int64FastMapNN<DataStat> dataMap;

	public:
		DataRateCalc(NN<Text::String> sourceName);
		virtual ~DataRateCalc();

		IO::ParserType GetParserType() const { return IO::ParserType::DataRateCalc; }
		void AddData(NN<Data::DateTime> dt, UInt64 dataSize);
		UIntOS GetCount() const;
		Optional<DataStat> GetItem(UIntOS index) const;
	};
}
#endif
