#ifndef _SM_IO_IOBJECTPARSER
#define _SM_IO_IOBJECTPARSER
#include "IO/IParser.h"

namespace IO
{
	class PackageFile;

	class IObjectParser : public IO::IParser
	{
	public:
		virtual ~IObjectParser(){};

		virtual IO::ParsedObject *ParseObject(IO::ParsedObject *pobj, IO::PackageFile *pkgFile, IO::ParserType targetType) = 0;
	};
};
#endif

