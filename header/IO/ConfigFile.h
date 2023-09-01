#ifndef _SM_IO_CONFIGFILE
#define _SM_IO_CONFIGFILE
#include "Data/ArrayListNN.h"
#include "Data/FastStringMap.h"

namespace IO
{
	class ConfigFile
	{
	protected:
		NotNullPtr<Text::String> defCate;
	private:
		Data::FastStringMap<Data::FastStringMap<Text::String *>*> cfgVals;

		void MergeCate(Data::FastStringMap<Text::String *> *myCate, Data::FastStringMap<Text::String *> *cateToMerge);
	public:
		ConfigFile();
		virtual ~ConfigFile();

		Text::String *GetValue(NotNullPtr<Text::String> name);
		Text::String *GetValue(Text::CString name);
		virtual Text::String *GetCateValue(Text::String *category, NotNullPtr<Text::String> name);
		virtual Text::String *GetCateValue(NotNullPtr<Text::String> category, NotNullPtr<Text::String> name);
		virtual Text::String *GetCateValue(Text::CStringNN category, Text::CString name);
		Bool SetValue(Text::String *category, NotNullPtr<Text::String> name, Text::String *value);
		Bool SetValue(Text::CString category, Text::CString name, Text::CString value);
		Bool RemoveValue(Text::CString category, Text::CString name);
		UOSInt GetCateCount();
		UOSInt GetCateList(Data::ArrayListNN<Text::String> *cateList, Bool withEmpty);
		UOSInt GetKeys(Text::String *category, Data::ArrayListNN<Text::String> *keyList);
		UOSInt GetKeys(Text::CString category, Data::ArrayListNN<Text::String> *keyList);
		Bool HasCategory(Text::CString category);
		IO::ConfigFile *CloneCate(Text::CString category);

		void MergeConfig(IO::ConfigFile *cfg);
	};
}
#endif
