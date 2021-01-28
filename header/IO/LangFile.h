#ifndef _SM_IO_LANGFILE
#define _SM_IO_LANGFILE
#include "IO/ConfigFile.h"

namespace IO
{
	class LangFile : public ConfigFile
	{
	private:
		IO::ConfigFile *cfg;

	public:
		LangFile(const UTF8Char *fileName, Int32 codePage);
		virtual ~LangFile();

		virtual const UTF8Char *GetValue(const UTF8Char *name);
		virtual const UTF8Char *GetValue(const UTF8Char *category, const UTF8Char *name);
	};
}
#endif
