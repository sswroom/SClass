#ifndef _SM_IO_OBJECTPARSER
#define _SM_IO_OBJECTPARSER
#include "IO/ParserBase.h"

namespace IO
{
	class PackageFile;

	class ObjectParser : public IO::ParserBase
	{
	public:
		virtual ~ObjectParser(){};

		virtual IO::ParsedObject *ParseObject(NotNullPtr<IO::ParsedObject> pobj, IO::PackageFile *pkgFile, IO::ParserType targetType) = 0;
	};
}
#endif

