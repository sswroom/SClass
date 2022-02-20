#ifndef _SM_IO_CONFIGFILE
#define _SM_IO_CONFIGFILE
#include "Data/ArrayList.h"
#include "Data/FastStringMap.h"

namespace IO
{
	class ConfigFile
	{
	private:
		Data::FastStringMap<Data::FastStringMap<Text::String *>*> *cfgVals;

	public:
		ConfigFile();
		virtual ~ConfigFile();

		virtual Text::String *GetValue(Text::String *name);
		virtual Text::String *GetValue(Text::CString name);
		virtual Text::String *GetValue(Text::String *category, Text::String *name);
		virtual Text::String *GetValue(Text::CString category, Text::CString name);
		Bool SetValue(Text::String *category, Text::String *name, Text::String *value);
		Bool SetValue(const UTF8Char *category, UOSInt categoryLen, const UTF8Char *name, UOSInt nameLen, const UTF8Char *value, UOSInt valueLen);
		Bool RemoveValue(const UTF8Char *category, UOSInt categoryLen, const UTF8Char *name, UOSInt nameLen);
		UOSInt GetCateCount();
		UOSInt GetCateList(Data::ArrayList<Text::String *> *cateList, Bool withEmpty);
		UOSInt GetKeys(Text::String *category, Data::ArrayList<Text::String *> *keyList);
		UOSInt GetKeys(const UTF8Char *category, UOSInt categoryLen, Data::ArrayList<Text::String *> *keyList);
		Bool HasCategory(const UTF8Char *category, UOSInt categoryLen);
		IO::ConfigFile *CloneCate(const UTF8Char *category, UOSInt categoryLen);
	};
}
#endif
