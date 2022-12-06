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

		virtual IO::ParsedObject *ParseFileHdr(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr) = 0;
		IO::ParsedObject *ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType);

		IO::ParsedObject *ParseFilePath(Text::CString filePath);

		static const UOSInt hdrSize;
	};
}
#endif

