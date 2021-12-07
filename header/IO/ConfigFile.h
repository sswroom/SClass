#ifndef _SM_IO_CONFIGFILE
#define _SM_IO_CONFIGFILE
#include "Data/StringMap.h"

namespace IO
{
	class ConfigFile
	{
	private:
		Data::StringMap<Data::StringMap<Text::String *>*> *cfgVals;

	public:
		ConfigFile();
		virtual ~ConfigFile();

		virtual Text::String *GetValue(Text::String *name);
		virtual Text::String *GetValue(const UTF8Char *name);
		virtual Text::String *GetValue(Text::String *category, Text::String *name);
		virtual Text::String *GetValue(const UTF8Char *category, const UTF8Char *name);
		Bool SetValue(Text::String *category, Text::String *name, Text::String *value);
		Bool SetValue(const UTF8Char *category, const UTF8Char *name, const UTF8Char *value);
		Bool RemoveValue(const UTF8Char *category, const UTF8Char *name);
		UOSInt GetCateCount();
		UOSInt GetCateList(Data::ArrayList<Text::String *> *cateList, Bool withEmpty);
		UOSInt GetKeys(Text::String *category, Data::ArrayList<Text::String *> *keyList);
		UOSInt GetKeys(const UTF8Char *category, Data::ArrayList<Text::String *> *keyList);
		Bool HasCategory(const UTF8Char *category);
		IO::ConfigFile *CloneCate(const UTF8Char *category);
	};
}
#endif
