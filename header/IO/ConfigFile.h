#ifndef _SM_IO_CONFIGFILE
#define _SM_IO_CONFIGFILE
#include "Data/StringUTF8Map.h"

namespace IO
{
	class ConfigFile
	{
	private:
		Data::StringUTF8Map<Data::StringUTF8Map<const UTF8Char *>*> *cfgVals;

	public:
		ConfigFile();
		virtual ~ConfigFile();

		virtual const UTF8Char *GetValue(const UTF8Char *name);
		virtual const UTF8Char *GetValue(const UTF8Char *category, const UTF8Char *name);
		Bool SetValue(const UTF8Char *category, const UTF8Char *name, const UTF8Char *value);
		Bool RemoveValue(const UTF8Char *category, const UTF8Char *name);
		UOSInt GetCateCount();
		UOSInt GetCateList(Data::ArrayList<const UTF8Char *> *cateList);
		UOSInt GetKeys(const UTF8Char *category, Data::ArrayList<const UTF8Char *> *keyList);
		Bool HasCategory(const UTF8Char *category);
		IO::ConfigFile *CloneCate(const UTF8Char *category);
	};
}
#endif
