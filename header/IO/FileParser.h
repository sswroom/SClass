#ifndef _SM_IO_FILEPARSER
#define _SM_IO_FILEPARSER
#include "IO/IParser.h"

namespace IO
{
	class PackageFile;

	class FileParser : public IO::IParser
	{
	public:
		virtual ~FileParser(){};

		virtual IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType) = 0;

		IO::ParsedObject *ParseFilePath(const UTF8Char *filePath);
	};
}
#endif

