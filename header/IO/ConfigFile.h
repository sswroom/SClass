#ifndef _SM_IO_CONFIGFILE
#define _SM_IO_CONFIGFILE
#include "Data/ArrayListStringNN.h"
#include "Data/FastStringMapNN.hpp"
#include "IO/ParsedObject.h"

namespace IO
{
	class ConfigFile : public IO::ParsedObject
	{
	protected:
		NN<Text::String> defCate;
	private:
		Data::FastStringMapNN<Data::FastStringMapNN<Text::String>> cfgVals;

		void MergeCate(NN<Data::FastStringMapNN<Text::String>> myCate, NN<Data::FastStringMapNN<Text::String>> cateToMerge);
	public:
		ConfigFile(NN<Text::String> sourceName);
		ConfigFile(Text::CStringNN sourceName);
		virtual ~ConfigFile();

		virtual IO::ParserType GetParserType() const;
		Optional<Text::String> GetValue(NN<Text::String> name);
		Optional<Text::String> GetValue(Text::CStringNN name);
		virtual Optional<Text::String> GetCateValue(NN<Text::String> category, NN<Text::String> name);
		virtual Optional<Text::String> GetCateValue(Text::CStringNN category, Text::CStringNN name);
		virtual Bool SetValue(NN<Text::String> category, NN<Text::String> name, Optional<Text::String> value);
		virtual Bool SetValue(Text::CStringNN category, Text::CStringNN name, Text::CString value);
		virtual Bool RemoveValue(Text::CString category, Text::CStringNN name);
		virtual UIntOS GetCateCount() const;
		virtual UIntOS GetCateList(NN<Data::ArrayListStringNN> cateList, Bool withEmpty);
		virtual UIntOS GetKeys(NN<Text::String> category, NN<Data::ArrayListStringNN> keyList);
		virtual UIntOS GetKeys(Text::CStringNN category, NN<Data::ArrayListStringNN> keyList);
		virtual UIntOS GetCount(Text::CString category) const;
		virtual Optional<Text::String> GetKey(Text::CString category, UIntOS index) const;
		virtual Bool HasCategory(Text::CString category) const;
		Optional<IO::ConfigFile> CloneCate(Text::CString category);

		void MergeConfig(NN<IO::ConfigFile> cfg);
	};
}
#endif
