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

		virtual Optional<IO::ParsedObject> ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr) = 0;
		Optional<IO::ParsedObject> ParseFile(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType);

		Optional<IO::ParsedObject> ParseFilePath(Text::CStringNN filePath);

		static const UIntOS hdrSize;
	};
}
#endif

