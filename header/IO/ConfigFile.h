#ifndef _SM_IO_CONFIGFILE
#define _SM_IO_CONFIGFILE
#include "Data/ArrayListStringNN.h"
#include "Data/FastStringMapNN.h"

namespace IO
{
	class ConfigFile
	{
	protected:
		NotNullPtr<Text::String> defCate;
	private:
		Data::FastStringMapNN<Data::FastStringMapNN<Text::String>> cfgVals;

		void MergeCate(NotNullPtr<Data::FastStringMapNN<Text::String>> myCate, NotNullPtr<Data::FastStringMapNN<Text::String>> cateToMerge);
	public:
		ConfigFile();
		virtual ~ConfigFile();

		Optional<Text::String> GetValue(NotNullPtr<Text::String> name);
		Optional<Text::String> GetValue(Text::CStringNN name);
		virtual Optional<Text::String> GetCateValue(NotNullPtr<Text::String> category, NotNullPtr<Text::String> name);
		virtual Optional<Text::String> GetCateValue(Text::CStringNN category, Text::CStringNN name);
		Bool SetValue(NotNullPtr<Text::String> category, NotNullPtr<Text::String> name, Optional<Text::String> value);
		Bool SetValue(Text::CStringNN category, Text::CStringNN name, Text::CString value);
		Bool RemoveValue(Text::CString category, Text::CStringNN name);
		UOSInt GetCateCount() const;
		UOSInt GetCateList(NotNullPtr<Data::ArrayListStringNN> cateList, Bool withEmpty);
		UOSInt GetKeys(NotNullPtr<Text::String> category, NotNullPtr<Data::ArrayListStringNN> keyList);
		UOSInt GetKeys(Text::CStringNN category, NotNullPtr<Data::ArrayListStringNN> keyList);
		UOSInt GetCount(Text::CString category) const;
		Optional<Text::String> GetKey(Text::CString category, UOSInt index) const;
		Bool HasCategory(Text::CString category) const;
		Optional<IO::ConfigFile> CloneCate(Text::CString category);

		void MergeConfig(NotNullPtr<IO::ConfigFile> cfg);
	};
}
#endif
