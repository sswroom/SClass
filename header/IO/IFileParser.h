#ifndef _SM_IO_IFILEPARSER
#define _SM_IO_IFILEPARSER
#include "IO/IParser.h"

namespace IO
{
	class PackageFile;

	class IFileParser : public IO::IParser
	{
	public:
		virtual ~IFileParser(){};

		virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType) = 0;
	};
};
#endif

