#ifndef _SM_IO_FILEPARSER
#define _SM_IO_FILEPARSER
#include "IO/ParserBase.h"

namespace IO
{
	class PackageFile;

	class FileParser : public IO::ParserBase
	{
	public:
		virtual ~FileParser(){};

		virtual IO::ParsedObject *ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr) = 0;
		IO::ParsedObject *ParseFile(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType);

		IO::ParsedObject *ParseFilePath(Text::CString filePath);

		static const UOSInt hdrSize;
	};
}
#endif

