#ifndef _SM_IO_CONFIGFILE
#define _SM_IO_CONFIGFILE
#include "Data/ArrayListStringNN.h"
#include "Data/FastStringMapNN.h"

namespace IO
{
	class ConfigFile
	{
	protected:
		NN<Text::String> defCate;
	private:
		Data::FastStringMapNN<Data::FastStringMapNN<Text::String>> cfgVals;

		void MergeCate(NN<Data::FastStringMapNN<Text::String>> myCate, NN<Data::FastStringMapNN<Text::String>> cateToMerge);
	public:
		ConfigFile();
		virtual ~ConfigFile();

		Optional<Text::String> GetValue(NN<Text::String> name);
		Optional<Text::String> GetValue(Text::CStringNN name);
		virtual Optional<Text::String> GetCateValue(NN<Text::String> category, NN<Text::String> name);
		virtual Optional<Text::String> GetCateValue(Text::CStringNN category, Text::CStringNN name);
		Bool SetValue(NN<Text::String> category, NN<Text::String> name, Optional<Text::String> value);
		Bool SetValue(Text::CStringNN category, Text::CStringNN name, Text::CString value);
		Bool RemoveValue(Text::CString category, Text::CStringNN name);
		UOSInt GetCateCount() const;
		UOSInt GetCateList(NN<Data::ArrayListStringNN> cateList, Bool withEmpty);
		UOSInt GetKeys(NN<Text::String> category, NN<Data::ArrayListStringNN> keyList);
		UOSInt GetKeys(Text::CStringNN category, NN<Data::ArrayListStringNN> keyList);
		UOSInt GetCount(Text::CString category) const;
		Optional<Text::String> GetKey(Text::CString category, UOSInt index) const;
		Bool HasCategory(Text::CString category) const;
		Optional<IO::ConfigFile> CloneCate(Text::CString category);

		void MergeConfig(NN<IO::ConfigFile> cfg);
	};
}
#endif
